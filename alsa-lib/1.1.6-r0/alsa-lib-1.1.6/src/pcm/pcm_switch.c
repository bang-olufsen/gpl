/**
 * \file pcm/pcm_switch.c
 * \ingroup PCM_Plugins
 * \brief PCM Switch Plugin Interface
 * \author Simon Mikuda <simon.mikuda@streamunlimited.com>
 * \date 2016
 */

#include <time.h>
#include <stdio.h>
#include <pthread.h>

#include "pcm_local.h"
#include "pcm_generic.h"

#define LOG(...) SNDERR(__VA_ARGS__)
//#define DEBUG(...) SNDERR(__VA_ARGS__)
#define DEBUG(...)
//#define TRACE(...) SNDERR(__VA_ARGS__)
#define TRACE(...)

// Kernel supported rates
static unsigned int pcm_rates[] = { 5512, 8000, 11025, 16000, 22050, 32000, 44100,
                                 48000, 64000, 88200, 96000, 176400, 192000,
                                 352800, 384000, 705600, 768000, 1411200,
                                 1536000, 2822400, 3072000 };
unsigned pcm_rates_size = 21;

#define SWITCH_CTL_OFF 0
#define SWITCH_CTL_ON 1

static void apply_switch_value(snd_pcm_t *pcm);
static const char *const _switch_ctl_names[] = { "Off", "On" };
static pthread_mutex_t _switch_ctl_lock = PTHREAD_MUTEX_INITIALIZER;

static snd_pcm_sframes_t snd_pcm_switch_writei(snd_pcm_t *pcm, const void *buffer, snd_pcm_uframes_t size);

#ifndef PIC
/* entry for static linking */
const char *_snd_module_pcm_switch = "";
#endif

#ifndef DOC_HIDDEN
typedef struct {
	/* This field need to be the first */
	snd_pcm_generic_t gen;
	snd_ctl_t *ctl;
	snd_ctl_elem_value_t elem;
	snd_ctl_elem_value_t drift_elem;
	double drift_mult;
	int cchannels;
	int cur_switch;
	int active_switch;
	int needs_update;
	int restrict_rate;
	snd_async_handler_t *async_handler;
	int has_sw_params;
	snd_pcm_sw_params_t sw_params;
	int has_hw_params;
	snd_pcm_hw_params_t hw_params;
	int has_hw_params_any;
	snd_pcm_hw_params_t hw_params_any;
	snd_config_t *root;
	snd_pcm_stream_t stream;
	snd_config_t *conf;
	snd_config_t *sconf;
	int stream_mode;
	snd_htimestamp_t trigger_tstamp;
	snd_pcm_state_t state;
	snd_pcm_uframes_t appl_ptr;
	snd_pcm_uframes_t hw_ptr;
	snd_pcm_chmap_query_t **chmap;
	struct timespec clock;
	struct timespec clock_offset;
	unsigned clock_diff_usec;
	int poll_fd;
} snd_pcm_switch_t;
#endif

static int update_softvare_clock(snd_pcm_t *pcm)
{
	TRACE("");
	snd_pcm_uframes_t forward = 0;
	snd_pcm_switch_t *swtch = pcm->private_data;

	struct timespec clock;
	gettimestamp(&clock, pcm->tstamp_type);

	if (swtch->clock.tv_sec > 0) {
		u_int64_t forward_usec, diff_nsec;
		signed long long comp = 0, drift;

		if (swtch->drift_mult != 0 &&
		    snd_ctl_elem_read(swtch->ctl, &swtch->drift_elem) >= 0) {

			/* Handle drift compensator */
			drift = swtch->drift_elem.value.integer.value[0] * swtch->drift_mult;
			comp = (((signed long long) pcm->rate) * drift ) / 1000000;
		}

		/* Calculate software clock */
		diff_nsec = 1000000000ULL * (clock.tv_sec - swtch->clock.tv_sec) + (clock.tv_nsec - swtch->clock.tv_nsec);
		forward_usec = (((pcm->rate + comp) * diff_nsec) / 1000ULL) + swtch->clock_diff_usec;
		forward = forward_usec / 1000000ULL;
		swtch->clock_diff_usec = forward_usec % 1000000ULL;
	}

	if (!swtch->gen.slave && swtch->state == SND_PCM_STATE_RUNNING) {
		snd_pcm_uframes_t hw_avail = snd_pcm_mmap_playback_hw_avail(pcm);
		if (forward > hw_avail) {
			DEBUG("UNDERRUN %lu frames!", (forward - hw_avail));
			swtch->state = SND_PCM_STATE_XRUN;
			swtch->trigger_tstamp = clock;
			swtch->clock_diff_usec = 0;
			forward = hw_avail;
		}

		snd_pcm_mmap_hw_forward(pcm, forward);
	}

	swtch->clock = clock;
	return forward;
}

static void update_switch_value(snd_pcm_switch_t *swtch)
{
	TRACE("");

	if (!swtch->ctl)
		return;

	if (snd_ctl_elem_read(swtch->ctl, &swtch->elem) < 0)
		return;

	swtch->cur_switch = swtch->elem.value.integer.value[0] ? 1 : 0;
}

static void check_switch_value(snd_pcm_t *pcm)
{
	snd_pcm_switch_t *swtch = pcm->private_data;

	TRACE("needs_update=%d", swtch->needs_update);
	if (swtch->needs_update) {
		apply_switch_value(pcm);
	}
}

inline void switch_free(snd_pcm_switch_t *swtch)
{
	DEBUG("");
	if (swtch->async_handler)
		snd_async_del_handler(swtch->async_handler);
	if (swtch->gen.close_slave && swtch->gen.slave)
		snd_pcm_close(swtch->gen.slave);
	if (swtch->ctl)
		snd_ctl_close(swtch->ctl);
	if (swtch->sconf)
		snd_config_delete(swtch->sconf);
	if (swtch->conf)
		snd_config_delete(swtch->conf);
	if (swtch->root)
		snd_config_delete(swtch->root);
	if (swtch->poll_fd)
		close(swtch->poll_fd);
	free(swtch);
}

static int snd_pcm_switch_hw_refine_cprepare(snd_pcm_t *pcm ATTRIBUTE_UNUSED, snd_pcm_hw_params_t *params)
{
	DEBUG("");
	int err;
	snd_pcm_access_mask_t access_mask = { SND_PCM_ACCBIT_SHM };
	err = _snd_pcm_hw_param_set_mask(params, SND_PCM_HW_PARAM_ACCESS,
	                                 &access_mask);
	if (err < 0)
		return err;
	params->info &= ~(SND_PCM_INFO_MMAP | SND_PCM_INFO_MMAP_VALID);
	return 0;
}

static int snd_pcm_switch_hw_refine_sprepare(snd_pcm_t *pcm ATTRIBUTE_UNUSED, snd_pcm_hw_params_t *sparams)
{
	DEBUG("");
	snd_pcm_access_mask_t saccess_mask = { SND_PCM_ACCBIT_MMAP };
	_snd_pcm_hw_params_any(sparams);
	_snd_pcm_hw_param_set_mask(sparams, SND_PCM_HW_PARAM_ACCESS,
	                           &saccess_mask);
	return 0;
}

static int snd_pcm_switch_hw_refine_schange(snd_pcm_t *pcm ATTRIBUTE_UNUSED, snd_pcm_hw_params_t *params,
                                            snd_pcm_hw_params_t *sparams)
{
	DEBUG("");
	int err;
	unsigned int links = ~SND_PCM_HW_PARBIT_ACCESS;
	err = _snd_pcm_hw_params_refine(sparams, links, params);
	if (err < 0)
		return err;
	return 0;
}

static int snd_pcm_switch_hw_refine_cchange(snd_pcm_t *pcm ATTRIBUTE_UNUSED, snd_pcm_hw_params_t *params,
                                            snd_pcm_hw_params_t *sparams)
{
	DEBUG("");
	int err;
	unsigned int links = ~SND_PCM_HW_PARBIT_ACCESS;
	err = _snd_pcm_hw_params_refine(params, links, sparams);
	if (err < 0)
		return err;
	return 0;
}

static int snd_pcm_switch_close(snd_pcm_t *pcm)
{
	DEBUG("");
	snd_pcm_switch_t *swtch = pcm->private_data;

	switch_free(swtch);
	return 0;
}

static int snd_pcm_switch_info(snd_pcm_t *pcm, snd_pcm_info_t * info)
{
	DEBUG("");
	snd_pcm_switch_t *swtch = pcm->private_data;

	if (swtch->gen.slave) {
		return snd_pcm_generic_info(pcm, info);
	}
	else {
		memset(info, 0, sizeof(*info));
		info->stream = pcm->stream;
		info->card = -1;
		if (pcm->name) {
			strncpy((char *)info->id, pcm->name, sizeof(info->id));
			strncpy((char *)info->name, pcm->name, sizeof(info->name));
			strncpy((char *)info->subname, pcm->name, sizeof(info->subname));
		}
		info->subdevices_count = 1;
		return 0;
	}
}

#define hw_param_mask(params,var) \
	&((params)->masks[(var) - SND_PCM_HW_PARAM_FIRST_MASK])

#define hw_param_interval(params,var) \
	&((params)->intervals[(var) - SND_PCM_HW_PARAM_FIRST_INTERVAL])

static int snd_pcm_switch_hw_refine(snd_pcm_t *pcm, snd_pcm_hw_params_t *params)
{
	snd_pcm_switch_t *swtch = pcm->private_data;

	if (swtch->restrict_rate)
		snd_interval_refine_set(hw_param_interval(params, SND_PCM_HW_PARAM_RATE), swtch->restrict_rate);

	if (swtch->gen.slave) {
		DEBUG("copy");
		return snd_pcm_hw_refine_slave(pcm, params,
		                               snd_pcm_switch_hw_refine_cprepare,
		                               snd_pcm_switch_hw_refine_cchange,
		                               snd_pcm_switch_hw_refine_sprepare,
		                               snd_pcm_switch_hw_refine_schange,
		                               snd_pcm_generic_hw_refine);
	}
	else {
		DEBUG("null");

		int err = 0;
		unsigned int k, i, rate;

		// If we have stored real hw params refine desired hw params also according to them
		if (swtch->has_hw_params_any) {

			for (k = SND_PCM_HW_PARAM_FIRST_MASK; k <= SND_PCM_HW_PARAM_LAST_MASK && err >= 0; k++) {
				if (!(params->rmask & (1 << k)))
					continue;
				err = snd_mask_refine(hw_param_mask(params, k), hw_param_mask(&swtch->hw_params_any, k));
				if (err) // error has negative values, change has positive value
					params->cmask |= 1 << k;
			}

			for (k = SND_PCM_HW_PARAM_FIRST_INTERVAL; k <= SND_PCM_HW_PARAM_LAST_INTERVAL && err >= 0; k++) {
				if (!(params->rmask & (1 << k)))
					continue;
				err = snd_interval_refine(hw_param_interval(params, k), hw_param_interval(&swtch->hw_params_any, k));

				// Refine to kernel supported values
				if (k == SND_PCM_HW_PARAM_RATE) {
					rate = params->intervals[k - SND_PCM_HW_PARAM_FIRST_INTERVAL].min;
					if (rate == params->intervals[k - SND_PCM_HW_PARAM_FIRST_INTERVAL].max) {
						for (i = 0; i < pcm_rates_size; ++i)
							if (rate == pcm_rates[i])
								break;
						if (i == pcm_rates_size)
							err = -1;
					}
				}

				if (err) // error has negative values, change has positive value
					params->cmask |= 1 << k;
			}

			if (err >= 0) {
				err = snd_pcm_hw_refine_soft(pcm, params);

				params->info = swtch->hw_params_any.info;
				params->fifo_size = swtch->hw_params_any.fifo_size;
			}

		} else {
			err = snd_pcm_hw_refine_soft(pcm, params);
			params->info = SND_PCM_INFO_MMAP | SND_PCM_INFO_MMAP_VALID |
						   SND_PCM_INFO_RESUME | SND_PCM_INFO_PAUSE;
			params->fifo_size = 0;
		}
		return err;
	}
}

static int snd_pcm_switch_hw_params(snd_pcm_t *pcm, snd_pcm_hw_params_t *params)
{
	TRACE("");
	snd_pcm_switch_t *swtch = pcm->private_data;
/*
	snd_output_t *out;
	char* buff;
	snd_output_buffer_open(&out);
	snd_pcm_hw_params_dump(params, out);
	DEBUG("%.*s", snd_output_buffer_string(out, &buff), buff);
	snd_output_close(out);
*/
	swtch->has_hw_params = 1;
	snd_pcm_hw_params_copy(&swtch->hw_params, params);

	if (swtch->gen.slave) {
		DEBUG("copy");
		return snd_pcm_hw_params_slave(pcm, params,
		                               snd_pcm_switch_hw_refine_cchange,
		                               snd_pcm_switch_hw_refine_sprepare,
		                               snd_pcm_switch_hw_refine_schange,
		                               snd_pcm_generic_hw_params);
	}
	else {
		DEBUG("null");
		return 0;
	}
}

static int snd_pcm_switch_hw_free(snd_pcm_t *pcm)
{
	DEBUG("");
	snd_pcm_switch_t *swtch = pcm->private_data;

	swtch->has_hw_params = 0;

	if (swtch->gen.slave) {
		return snd_pcm_generic_hw_free(pcm);
	}
	else {
		return 0;
	}
}

static int snd_pcm_switch_sw_params(snd_pcm_t *pcm, snd_pcm_sw_params_t * params)
{
	TRACE("");
	snd_pcm_switch_t *swtch = pcm->private_data;
/*
	snd_output_t *out;
	char* buff;
	snd_output_buffer_open(&out);
	snd_pcm_sw_params_dump(params, out);
	DEBUG("%.*s", snd_output_buffer_string(out, &buff), buff);
	snd_output_close(out);
*/
	swtch->has_sw_params = 1;
	snd_pcm_sw_params_copy(&swtch->sw_params, params);

	if (swtch->gen.slave) {
		DEBUG("copy");
		snd_pcm_generic_sw_params(pcm, params);
		return 0;
	}
	else {
		DEBUG("null");
		return 0;
	}
}

static int snd_pcm_switch_nonblock(snd_pcm_t *pcm, int nonblock)
{
	DEBUG("");
	snd_pcm_switch_t *swtch = pcm->private_data;

	if (swtch->gen.slave) {
		return snd_pcm_generic_nonblock(pcm, nonblock);
	}
	else {
		return 0;
	}
}

static int snd_pcm_switch_async(snd_pcm_t *pcm, int sig, pid_t pid)
{
	DEBUG("");
	snd_pcm_switch_t *swtch = pcm->private_data;

	if (swtch->gen.slave) {
		return snd_pcm_generic_async(pcm, sig, pid);
	}
	else {
		return -ENOSYS;
	}
}

static snd_pcm_chmap_query_t **snd_pcm_switch_query_chmaps(snd_pcm_t *pcm)
{
	DEBUG("");
	snd_pcm_switch_t *swtch = pcm->private_data;

	if (swtch->gen.slave) {
		return snd_pcm_generic_query_chmaps(pcm);
	}
	else {
		if (swtch->chmap)
			return _snd_pcm_copy_chmap_query(swtch->chmap);
		return NULL;
	}

}

static snd_pcm_chmap_t *snd_pcm_switch_get_chmap(snd_pcm_t *pcm)
{
	DEBUG("");
	snd_pcm_switch_t *swtch = pcm->private_data;

	if (swtch->gen.slave) {
		return snd_pcm_generic_get_chmap(pcm);
	}
	else {
		if (swtch->chmap)
			return _snd_pcm_choose_fixed_chmap(pcm, swtch->chmap);
		return NULL;
	}

}

static snd_pcm_uframes_t snd_pcm_switch_write_areas(snd_pcm_t *pcm,
                                                    const snd_pcm_channel_area_t *areas,
                                                    snd_pcm_uframes_t offset,
                                                    snd_pcm_uframes_t size,
                                                    const snd_pcm_channel_area_t *slave_areas,
                                                    snd_pcm_uframes_t slave_offset,
                                                    snd_pcm_uframes_t *slave_sizep)
{
	TRACE("");
	if (size > *slave_sizep)
		size = *slave_sizep;

	snd_pcm_areas_copy(slave_areas, slave_offset,
	                   areas, offset,
	                   pcm->channels, size, pcm->format);

	*slave_sizep = size;
	return size;
}

static void snd_pcm_switch_dump(snd_pcm_t *pcm, snd_output_t *out)
{
	snd_pcm_switch_t *copy = pcm->private_data;
	snd_output_printf(out, "Copy conversion PCM\n");
	if (pcm->setup) {
		snd_output_printf(out, "Its setup is:\n");
		snd_pcm_dump_setup(pcm, out);
	}
	snd_output_printf(out, "Slave: ");
	snd_pcm_dump(copy->gen.slave, out);
}

static snd_pcm_sframes_t snd_pcm_switch_avail_update(snd_pcm_t *pcm)
{
	snd_pcm_switch_t *swtch = pcm->private_data;
	snd_pcm_sframes_t avail;

	check_switch_value(pcm);
	update_softvare_clock(pcm);

	if (swtch->gen.slave) {
		avail = snd_pcm_avail_update(swtch->gen.slave);
		TRACE("copy avail=%ld ", avail);

		*pcm->hw.ptr = *swtch->gen.slave->hw.ptr;
		return avail;
	}
	else {
		if (swtch->state == SND_PCM_STATE_XRUN) {
			TRACE("null XRUN");
			return -EPIPE;
		}

		avail = snd_pcm_mmap_avail(pcm);
		TRACE("null avail=%ld", avail);
		return avail;
	}
}

static int snd_pcm_switch_status(snd_pcm_t *pcm, snd_pcm_status_t * status)
{
	TRACE("");
	snd_pcm_switch_t *swtch = pcm->private_data;

	check_switch_value(pcm);

	if (swtch->gen.slave) {
		TRACE("copy");
		int err;
		/* sync with the latest hw and appl ptrs */
		snd_pcm_switch_avail_update(pcm);

		err = snd_pcm_status(swtch->gen.slave, status);
		if (err < 0)
			return err;
		status->appl_ptr = *pcm->appl.ptr;
		status->hw_ptr = *pcm->hw.ptr;
		if (!status->tstamp.tv_sec && !status->tstamp.tv_nsec)
			/* if we don't have clock from hw use system clock instead */
			status->tstamp = swtch->clock;
		else {
			/* save offset to synchronize hw clock timestamps and system clock timestamp */
			swtch->clock_offset.tv_sec = status->tstamp.tv_sec - swtch->clock.tv_sec;
			swtch->clock_offset.tv_nsec = status->tstamp.tv_nsec - swtch->clock.tv_nsec;
		}
	}
	else {
		TRACE("null");
		memset(status, 0, sizeof(*status));
		status->state = swtch->state;
		status->trigger_tstamp = swtch->trigger_tstamp;
		status->avail = snd_pcm_switch_avail_update(pcm);
		status->avail_max = pcm->buffer_size;
		status->appl_ptr = *pcm->appl.ptr;
		status->hw_ptr = *pcm->hw.ptr;
		status->delay = snd_pcm_mmap_playback_hw_avail(pcm);

		/* add clock offset and normalize nanoseconds */
		status->tstamp.tv_sec = swtch->clock.tv_sec + swtch->clock_offset.tv_sec;
		status->tstamp.tv_nsec = swtch->clock.tv_nsec + swtch->clock_offset.tv_nsec;
		while (status->tstamp.tv_nsec >= 1000000000) {
			status->tstamp.tv_nsec -= 1000000000;
			status->tstamp.tv_sec += 1;
		}
		while (status->tstamp.tv_nsec < 0) {
			status->tstamp.tv_nsec += 1000000000;
			status->tstamp.tv_sec -= 1;
		}
	}
/*
	DEBUG ("%s:\n"
	       "state=%d\n"
	       "trigger_tstamp=%lld.%.9ld\n"
	       "avail=%lu\n""avail_max=%lu\n"
	       "tstamp=%lld.%.9ld\n"
	       "appl_ptr=%lu\n"
	       "hw_ptr=%lu\n"
	       "delay=%ld\n"
	       "overrange=%lu"
	       , (swtch->gen.slave ? "copy" : "null")
	       , status->state
	       , status->trigger_tstamp.tv_sec, status->trigger_tstamp.tv_nsec
	       , status->avail
	       , status->avail_max
	       , status->tstamp.tv_sec, status->tstamp.tv_nsec
	       , status->appl_ptr
	       , status->hw_ptr
	       , status->delay
	       , status->overrange);
*/
	return 0;
}

static snd_pcm_state_t snd_pcm_switch_state(snd_pcm_t *pcm)
{
	snd_pcm_switch_t *swtch = pcm->private_data;

	if (swtch->gen.slave) {
		TRACE("copy state=%d", snd_pcm_state(swtch->gen.slave));
		return snd_pcm_generic_state(pcm);
	}
	else {
		TRACE("null state=%d", swtch->state);
		return swtch->state;
	}
}

static int snd_pcm_switch_hwsync(snd_pcm_t *pcm)
{
	TRACE("");
	snd_pcm_switch_t *swtch = pcm->private_data;

	if (swtch->gen.slave) {
		return snd_pcm_generic_hwsync(pcm);
	}
	else {
		return 0;
	}
}

static int snd_pcm_switch_delay(snd_pcm_t *pcm, snd_pcm_sframes_t *delayp)
{
	DEBUG("");
	snd_pcm_switch_t *swtch = pcm->private_data;

	if (swtch->gen.slave) {
		snd_pcm_sframes_t sd;
		int err = snd_pcm_delay(swtch->gen.slave, &sd);
		if (err < 0)
			return err;
		*delayp = sd;
		return 0;
	}
	else {
		*delayp = 0;
		return 0;
	}
}

static int snd_pcm_switch_reset(snd_pcm_t *pcm)
{
	snd_pcm_switch_t *swtch = pcm->private_data;
	int err;

	if (swtch->gen.slave) {
		DEBUG("copy");
		err = snd_pcm_reset(swtch->gen.slave);
		if (err < 0) {
			return err;
		}
		swtch->appl_ptr = 0;
		swtch->hw_ptr = 0;
		return 0;
	}
	else {
		DEBUG("null");
		swtch->appl_ptr = 0;
		swtch->hw_ptr = 0;
		return 0;
	}
}

static int snd_pcm_switch_prepare(snd_pcm_t *pcm)
{
	snd_pcm_switch_t *swtch = pcm->private_data;
	int err;

	if (swtch->gen.slave) {
		DEBUG("copy");
		err = snd_pcm_prepare(swtch->gen.slave);
		if (err < 0) {
			return err;
		}
		swtch->appl_ptr = 0;
		swtch->hw_ptr = 0;
		return 0;
	}
	else {
		DEBUG("null");
		swtch->state = SND_PCM_STATE_PREPARED;
		return snd_pcm_switch_reset(pcm);
	}
}

static int snd_pcm_switch_start(snd_pcm_t *pcm)
{
	snd_pcm_switch_t *swtch = pcm->private_data;

	if (swtch->gen.slave) {
		DEBUG("copy");
		return snd_pcm_generic_start(pcm);
	}
	else {
		DEBUG("null");
		swtch->state = SND_PCM_STATE_RUNNING;
		gettimestamp(&swtch->trigger_tstamp, pcm->tstamp_type);
		swtch->clock = swtch->trigger_tstamp;
		return 0;
	}
}

static int snd_pcm_switch_drop(snd_pcm_t *pcm)
{
	snd_pcm_switch_t *swtch = pcm->private_data;

	if (swtch->gen.slave) {
		DEBUG("copy");
		return snd_pcm_generic_drop(pcm);
	}
	else {
		DEBUG("null state=%d", swtch->state);
		swtch->state = SND_PCM_STATE_SETUP;
		return 0;
	}
}

static int snd_pcm_switch_drain(snd_pcm_t *pcm)
{
	snd_pcm_switch_t *swtch = pcm->private_data;

	if (swtch->gen.slave) {
		DEBUG("null");
		return snd_pcm_generic_drain(pcm);
	}
	else {
		DEBUG("copy");
		swtch->state = SND_PCM_STATE_SETUP;
		return 0;
	}

}

static int snd_pcm_switch_pause(snd_pcm_t *pcm, int enable)
{
	DEBUG("");
	snd_pcm_switch_t *swtch = pcm->private_data;

	if (swtch->gen.slave) {
		return snd_pcm_generic_pause(pcm, enable);
	}
	else {
		if (enable) {
			if (swtch->state != SND_PCM_STATE_RUNNING)
				return -EBADFD;
			swtch->state = SND_PCM_STATE_PAUSED;
			gettimestamp(&swtch->trigger_tstamp, pcm->tstamp_type);
		} else {
			if (swtch->state != SND_PCM_STATE_PAUSED)
				return -EBADFD;
			swtch->state = SND_PCM_STATE_RUNNING;
			gettimestamp(&swtch->trigger_tstamp, pcm->tstamp_type);
			swtch->clock = swtch->trigger_tstamp;
		}
		return 0;
	}
}

static snd_pcm_sframes_t snd_pcm_switch_rewindable(snd_pcm_t *pcm)
{
	DEBUG("");
	snd_pcm_switch_t *swtch = pcm->private_data;

	if (swtch->gen.slave) {
		return snd_pcm_mmap_hw_rewindable(pcm);
	}
	else {
		return pcm->buffer_size;
	}
}

static snd_pcm_sframes_t snd_pcm_switch_rewind(snd_pcm_t *pcm, snd_pcm_uframes_t frames)
{
	DEBUG("");
	snd_pcm_switch_t *swtch = pcm->private_data;

	if (swtch->gen.slave) {
		snd_pcm_sframes_t n = snd_pcm_mmap_hw_rewindable(pcm);
		snd_pcm_sframes_t sframes;

		if ((snd_pcm_uframes_t)n < frames)
			frames = n;
		if (frames == 0)
			return 0;

		sframes = frames;
		sframes = snd_pcm_rewind(swtch->gen.slave, sframes);
		if (sframes < 0) {
			return sframes;
		}
		snd_pcm_mmap_appl_backward(pcm, (snd_pcm_uframes_t) sframes);
		return (snd_pcm_sframes_t) sframes;
	}
	else {
		switch (swtch->state) {
		case SND_PCM_STATE_RUNNING:
			snd_pcm_mmap_hw_backward(pcm, frames);
			/* Fall through */
		case SND_PCM_STATE_PREPARED:
			snd_pcm_mmap_appl_backward(pcm, frames);
			return frames;
		default:
			return -EBADFD;
		}
	}
}

static snd_pcm_sframes_t snd_pcm_switch_forwardable(snd_pcm_t *pcm)
{
	DEBUG("");
	snd_pcm_switch_t *swtch = pcm->private_data;

	if (swtch->gen.slave) {
		return snd_pcm_mmap_avail(pcm);
	}
	else {
		return 0;
	}
}

static snd_pcm_sframes_t snd_pcm_switch_forward(snd_pcm_t *pcm, snd_pcm_uframes_t frames)
{
	DEBUG("");
	snd_pcm_switch_t *swtch = pcm->private_data;

	if (swtch->gen.slave) {
		snd_pcm_sframes_t n = snd_pcm_switch_forwardable(pcm);
		snd_pcm_sframes_t sframes;

		if ((snd_pcm_uframes_t)n < frames)
			frames = n;
		if (frames == 0)
			return 0;

		sframes = frames;
		sframes = INTERNAL(snd_pcm_forward)(swtch->gen.slave, sframes);
		if (sframes < 0) {
			return sframes;
		}
		snd_pcm_mmap_appl_forward(pcm, (snd_pcm_uframes_t) frames);
		return (snd_pcm_sframes_t) frames;
	}
	else {
		switch (swtch->state) {
		case SND_PCM_STATE_RUNNING:
			snd_pcm_mmap_hw_forward(pcm, frames);
			/* Fall through */
		case SND_PCM_STATE_PREPARED:
			snd_pcm_mmap_appl_forward(pcm, frames);
			return frames;
		default:
			return -EBADFD;
		}
	}
}

static int snd_pcm_switch_resume(snd_pcm_t *pcm)
{
	DEBUG("");
	snd_pcm_switch_t *swtch = pcm->private_data;

	if (swtch->gen.slave) {
		return snd_pcm_generic_resume(pcm);
	}
	else {
		return 0;
	}
}

static void apply_switch_value(snd_pcm_t *pcm)
{
	snd_pcm_switch_t *swtch = pcm->private_data;
	snd_pcm_t *slave;
	snd_pcm_state_t pcm_state;
	snd_pcm_uframes_t avail_playback, waiting_playback;
	int err;

	TRACE("swtch->cur_switch=%d swtch->active_switch=%d swtch->gen.slave=%p", swtch->cur_switch, swtch->active_switch, swtch->gen.slave);

	swtch->needs_update = 0;

	// sanity checks
	if ((swtch->cur_switch == SWITCH_CTL_ON && swtch->gen.slave) ||
	    (swtch->cur_switch == SWITCH_CTL_OFF && !swtch->gen.slave) ||
	    (swtch->active_switch == swtch->cur_switch)) {
		return;
	}
	swtch->active_switch = swtch->cur_switch;

	pcm_state = snd_pcm_state(pcm);
	avail_playback = snd_pcm_avail_update(pcm);

	if (swtch->cur_switch == SWITCH_CTL_ON)	{
		DEBUG("~~~ OPEN SLAVE");

		err = snd_pcm_open_slave(&slave, swtch->root,
		                         swtch->sconf, swtch->stream,
		                         swtch->stream_mode, swtch->conf);
		if (err) {
			DEBUG("Failed to open slave %d", err);
			swtch->needs_update = 1;
			swtch->active_switch = !swtch->active_switch;
			return;
		}

		swtch->gen.slave = slave;

		if (swtch->has_hw_params)
			snd_pcm_hw_params_slave(pcm, &swtch->hw_params,
			                        snd_pcm_switch_hw_refine_cchange,
			                        snd_pcm_switch_hw_refine_sprepare,
			                        snd_pcm_switch_hw_refine_schange,
			                        snd_pcm_generic_hw_params);
		if (swtch->has_sw_params)
			snd_pcm_generic_sw_params(pcm, &swtch->sw_params);
	}
	else {
		DEBUG("~~~ CLOSE SLAVE");
		slave = swtch->gen.slave;
		swtch->gen.slave = NULL;
		swtch->state = SND_PCM_STATE_OPEN;

		if (!swtch->has_hw_params_any &&
			snd_pcm_hw_params_any(slave, &swtch->hw_params_any) >= 0) {
			swtch->has_hw_params_any = 1;
		}
	}

	if (pcm_state == SND_PCM_STATE_PREPARED ||
	    pcm_state == SND_PCM_STATE_RUNNING) {
		snd_pcm_switch_prepare(pcm);
		waiting_playback = pcm->buffer_size - avail_playback;
	}

	if (pcm_state == SND_PCM_STATE_RUNNING) {

		/* Don't drop audio when on switch value change */
		if (swtch->cur_switch != SWITCH_CTL_ON) {
			swtch->state = SND_PCM_STATE_RUNNING;
			if (waiting_playback > 0)
				snd_pcm_mmap_appl_forward(pcm, waiting_playback);
		} else {
			/* Forward frames that were processed when we were opening hw pcm */
			waiting_playback -= update_softvare_clock(pcm);

			if (waiting_playback > 0) {
				void *buffer = calloc(waiting_playback, pcm->frame_bits / 8);
				snd_pcm_switch_writei(pcm, buffer, waiting_playback);
				free(buffer);
			}
			snd_pcm_switch_start(pcm);
		}
	}

	if (pcm_state == SND_PCM_STATE_PREPARED) {
		/* Don't drop audio when on switch value change */
		if (swtch->cur_switch == SWITCH_CTL_ON) {
			if (waiting_playback > 0) {
				void *buffer = calloc(waiting_playback, pcm->frame_bits / 8);
				snd_pcm_switch_writei(pcm, buffer, waiting_playback);
				free(buffer);
			}
		}
	}

	if (swtch->cur_switch != SWITCH_CTL_ON)
		snd_pcm_close(slave);
}

static snd_pcm_sframes_t snd_pcm_switch_xfer_write_areas(snd_pcm_t *pcm,
                                                         const snd_pcm_channel_area_t *areas,
                                                         snd_pcm_uframes_t offset,
                                                         snd_pcm_uframes_t size)
{
	TRACE("");
	snd_pcm_switch_t *swtch = pcm->private_data;
	snd_pcm_t *slave = swtch->gen.slave;
	snd_pcm_uframes_t xfer = 0;
	snd_pcm_sframes_t result;
	int err;

	if (!slave) {
		snd_pcm_mmap_appl_forward(pcm, size);

		return size;
	}

	while (size > 0) {
		snd_pcm_uframes_t frames = size;
		const snd_pcm_channel_area_t *slave_areas;
		snd_pcm_uframes_t slave_offset;
		snd_pcm_uframes_t slave_frames = ULONG_MAX;

		err = snd_pcm_mmap_begin(slave, &slave_areas, &slave_offset, &slave_frames);
		if (err < 0 || slave_frames == 0)
			break;
		frames = snd_pcm_switch_write_areas(pcm, areas, offset, frames,
		                                    slave_areas, slave_offset, &slave_frames);
		if (CHECK_SANITY(slave_frames > snd_pcm_mmap_playback_avail(slave))) {
			SNDMSG("write overflow %ld > %ld", slave_frames,
			snd_pcm_mmap_playback_avail(slave));
			return -EPIPE;
		}
		snd_pcm_mmap_appl_forward(pcm, frames);
		result = snd_pcm_mmap_commit(slave, slave_offset, slave_frames);
		if (result > 0 && (snd_pcm_uframes_t)result != slave_frames) {
			snd_pcm_sframes_t res;
			res = slave_frames - result; // undo_write
			if (res < 0)
				return xfer > 0 ? (snd_pcm_sframes_t)xfer : res;
			frames -= res;
		}
		if (result <= 0)
			return xfer > 0 ? (snd_pcm_sframes_t)xfer : result;
		offset += frames;
		xfer += frames;
		size -= frames;
	}
	return (snd_pcm_sframes_t)xfer;
}

static snd_pcm_sframes_t snd_pcm_switch_writei(snd_pcm_t *pcm, const void *buffer, snd_pcm_uframes_t size)
{
	snd_pcm_sframes_t frames;
	snd_pcm_channel_area_t areas[pcm->channels];
	snd_pcm_areas_from_buf(pcm, areas, (void*)buffer);

	frames = snd_pcm_write_areas(pcm, areas, 0, size, snd_pcm_switch_xfer_write_areas);
	TRACE("frames=%ld size=%ld", frames, size);

	return frames;
}

static snd_pcm_sframes_t snd_pcm_switch_writen(snd_pcm_t *pcm, void **bufs, snd_pcm_uframes_t size)
{
	DEBUG("");
	snd_pcm_channel_area_t areas[pcm->channels];
	snd_pcm_areas_from_bufs(pcm, areas, bufs);
	return snd_pcm_write_areas(pcm, areas, 0, size, snd_pcm_switch_xfer_write_areas);
}

static snd_pcm_sframes_t snd_pcm_switch_mmap_commit(snd_pcm_t *pcm, snd_pcm_uframes_t offset ATTRIBUTE_UNUSED, snd_pcm_uframes_t size)
{
	TRACE("");
	snd_pcm_switch_t *swtch = pcm->private_data;

	if (swtch->gen.slave) {
		snd_pcm_t *slave = swtch->gen.slave;
		const snd_pcm_channel_area_t *areas;
		snd_pcm_uframes_t appl_offset;
		snd_pcm_sframes_t slave_size;
		snd_pcm_sframes_t xfer;

		slave_size = snd_pcm_avail_update(slave);
		if (slave_size < 0)
			return slave_size;
		areas = snd_pcm_mmap_areas(pcm);
		appl_offset = snd_pcm_mmap_offset(pcm);
		xfer = 0;
		while (size > 0 && slave_size > 0) {
			snd_pcm_uframes_t frames = size;
			snd_pcm_uframes_t cont = pcm->buffer_size - appl_offset;
			const snd_pcm_channel_area_t *slave_areas;
			snd_pcm_uframes_t slave_offset;
			snd_pcm_uframes_t slave_frames = ULONG_MAX;
			snd_pcm_sframes_t result;
			int err;

			err = snd_pcm_mmap_begin(slave, &slave_areas, &slave_offset, &slave_frames);
			if (err < 0)
				return xfer > 0 ? xfer : err;
			if (frames > cont)
				frames = cont;
			frames = snd_pcm_switch_write_areas(pcm, areas, appl_offset, frames,
			                                    slave_areas, slave_offset, &slave_frames);
			snd_pcm_mmap_appl_forward(pcm, frames);
			result = snd_pcm_mmap_commit(slave, slave_offset, slave_frames);
			if (result > 0 && (snd_pcm_uframes_t)result != slave_frames) {
				snd_pcm_sframes_t res;

				res = slave_frames - result; // undo_write
				if (res < 0)
					return xfer > 0 ? xfer : res;
				frames -= res;
			}
			if (result <= 0)
				return xfer > 0 ? xfer : result;
			if (frames == cont)
				appl_offset = 0;
			else
				appl_offset += result;
			size -= frames;
			slave_size -= frames;
			xfer += frames;
		}
		if (CHECK_SANITY(size)) {
			SNDMSG("short commit: %ld", size);
			return -EPIPE;
		}
		return xfer;
	}
	else {
		return snd_pcm_switch_forward(pcm, size);
	}
}

static int snd_pcm_switch_htimestamp(snd_pcm_t *pcm, snd_pcm_uframes_t *avail, snd_htimestamp_t *tstamp)
{
	DEBUG("");
	snd_pcm_switch_t *swtch = pcm->private_data;

	if (swtch->gen.slave) {
		return snd_pcm_generic_htimestamp(pcm, avail, tstamp);
	}
	else {
		return snd_pcm_generic_real_htimestamp(pcm, avail, tstamp);
	}
}

static int snd_pcm_switch_channel_info(snd_pcm_t *pcm, snd_pcm_channel_info_t *info)
{
	DEBUG("");
	snd_pcm_switch_t *swtch = pcm->private_data;

	if (swtch->gen.slave) {
		return snd_pcm_generic_channel_info(pcm, info);
	}
	else {
		return 0;
	}
}

static int snd_pcm_switch_mmap(snd_pcm_t *pcm)
{
	DEBUG("");
	snd_pcm_switch_t *swtch = pcm->private_data;

	if (swtch->gen.slave) {
		return snd_pcm_generic_mmap(pcm);
	}
	else {
		return 0;
	}
}

static int snd_pcm_switch_munmap(snd_pcm_t *pcm)
{
	DEBUG("");
	snd_pcm_switch_t *swtch = pcm->private_data;

	if (swtch->gen.slave) {
		return snd_pcm_generic_munmap(pcm);
	}
	else {
		return 0;
	}
}

static int snd_pcm_switch_set_chmap(snd_pcm_t *pcm, const snd_pcm_chmap_t *map)
{
	DEBUG("");
	snd_pcm_switch_t *swtch = pcm->private_data;

	if (swtch->gen.slave) {
		return snd_pcm_generic_set_chmap(pcm, map);
	}
	else {
		return 0;
	}
}

static int snd_pcm_switch_link(snd_pcm_t *pcm1, snd_pcm_t *pcm2)
{
	DEBUG("");
	snd_pcm_switch_t *swtch = pcm1->private_data;

	if (swtch->gen.slave) {
		return snd_pcm_generic_link(pcm1, pcm2);
	}
	else {
		return 0;
	}
}

static int snd_pcm_switch_link_slaves(snd_pcm_t *pcm, snd_pcm_t *master)
{
	DEBUG("");
	snd_pcm_switch_t *swtch = pcm->private_data;

	if (swtch->gen.slave) {
		return snd_pcm_generic_link_slaves(pcm, master);
	}
	else {
		return 0;
	}
}

static int snd_pcm_switch_unlink(snd_pcm_t *pcm)
{
	DEBUG("");
	snd_pcm_switch_t *swtch = pcm->private_data;

	if (swtch->gen.slave) {
		return snd_pcm_generic_unlink(pcm);
	}
	else {
		return 0;
	}
}

static int snd_pcm_switch_may_wait_for_avail_min(snd_pcm_t *pcm, snd_pcm_uframes_t avail)
{
	TRACE("");
	snd_pcm_switch_t *swtch = pcm->private_data;

	if (swtch->gen.slave) {
		return snd_pcm_generic_may_wait_for_avail_min(pcm, avail);
	}
	else {
		return avail >= pcm->avail_min ? 0 : 1;
	}
}

int snd_pcm_switch_poll_descriptors_count(snd_pcm_t *pcm)
{
	TRACE("");
	snd_pcm_switch_t *swtch = pcm->private_data;

	if (swtch->gen.slave) {
		return snd_pcm_generic_poll_descriptors_count(pcm);
	}
	else {
		return 1;
	}
}

static int snd_pcm_switch_poll_descriptors(snd_pcm_t *pcm, struct pollfd *pfds, unsigned int space)
{
	TRACE("");
	snd_pcm_switch_t *swtch = pcm->private_data;

	if (swtch->gen.slave) {
		return snd_pcm_generic_poll_descriptors(pcm, pfds, space);
	}
	else {
		if (space < 1)
			return -ENOMEM;
		pfds[0].fd = swtch->poll_fd;
		pfds[0].events = POLLOUT | POLLERR | POLLNVAL;
		return 1;
	}
}

static int snd_pcm_switch_poll_revents(snd_pcm_t *pcm, struct pollfd *pfds, unsigned int nfds, unsigned short *revents)
{
	snd_pcm_switch_t *swtch = pcm->private_data;
	unsigned short events;

	if (swtch->gen.slave) {
		TRACE("copy");
		return snd_pcm_generic_poll_revents(pcm, pfds, nfds, revents);
	}
	else {
		TRACE("null");
		if (nfds != 1 || pfds[0].fd != swtch->poll_fd )
			return -EINVAL;

		events = pfds[0].revents;
		update_softvare_clock(pcm);
		if (snd_pcm_mmap_avail(pcm) > pcm->avail_min)
			events |= POLLOUT;
		else {
			/* We must wait some time because /dev/null fd is always "write ready"
			 * and this causes forever loop when alsa buffer is full.
			 */
			usleep(((__useconds_t)(pcm->avail_min - snd_pcm_mmap_avail(pcm)) * (1000000 / pcm->rate)));

			update_softvare_clock(pcm);
			if (snd_pcm_mmap_avail(pcm) > pcm->avail_min)
				events |= POLLOUT;
			else
				events &= ~POLLOUT;
		}

		*revents = events;
		return 0;
	}
}

static void switch_ctl_async_handler(snd_async_handler_t *handler)
{
	TRACE("");
	snd_pcm_t *pcm;
	snd_pcm_switch_t *swtch;
	int old_value, new_value;

	pcm = snd_async_handler_get_callback_private(handler);
	swtch = pcm->private_data;

	// async handler can be called from multiple threads
	pthread_mutex_lock(&_switch_ctl_lock);

	old_value = swtch->cur_switch;
	update_switch_value(swtch);
	new_value = swtch->cur_switch;

	pthread_mutex_unlock(&_switch_ctl_lock);

	if (old_value != new_value) {
		DEBUG("~~~ Switch changed (needs_update=%d)", swtch->needs_update);
		if (snd_pcm_switch_state(pcm) == SND_PCM_STATE_RUNNING)
			swtch->needs_update = !swtch->needs_update;
		else
			apply_switch_value(pcm);
	}
}

static int add_user_ctl(snd_pcm_switch_t *swtch, snd_ctl_elem_info_t *cinfo)
{
	int err = snd_ctl_elem_add_boolean(swtch->ctl, &cinfo->id, 1);
	if (err < 0) {
		return err;
	}

	int def_val = SWITCH_CTL_ON;
	swtch->elem.value.integer.value[0] = def_val;
	swtch->cur_switch = def_val;

	return snd_ctl_elem_write(swtch->ctl, &swtch->elem);
}

/*
 * load and set up user-control
 * returns 0 if the user-control is found or created,
 * or a negative error code
 */
static int switch_load_control(snd_pcm_t *pcm, snd_pcm_switch_t *swtch,
                               int ctl_card, snd_ctl_elem_id_t *ctl_id,
                               const char *drift_elem_name, double drift_mult)
{
	DEBUG("");
	char tmp_name[32];
	snd_pcm_info_t *info;
	snd_ctl_elem_info_t *cinfo;
	snd_ctl_elem_id_t *drift_ctl_id;
	snd_ctl_elem_list_t *elem_list;
	int err, numid = -1;
	unsigned n;

	if (ctl_card < 0) {
		snd_pcm_info_alloca(&info);
		err = snd_pcm_info(pcm, info);
		if (err < 0)
			return err;
		ctl_card = snd_pcm_info_get_card(info);
		if (ctl_card < 0) {
			SNDERR("No card defined for switch control");
			return -EINVAL;
		}
	}
	sprintf(tmp_name, "hw:%d", ctl_card);

	err = snd_ctl_open(&swtch->ctl, tmp_name, 0);
	if (err < 0) {
		SNDERR("Cannot open CTL %s", tmp_name);
		return err;
	}

	swtch->elem.id = *ctl_id;

	snd_ctl_elem_info_alloca(&cinfo);
	snd_ctl_elem_info_set_id(cinfo, ctl_id);

	err = snd_ctl_elem_info(swtch->ctl, cinfo);
	if (err < 0) {
		if (err != -ENOENT) {
			SNDERR("Cannot get info for CTL %s", tmp_name);
			return err;
		}
		err = add_user_ctl(swtch, cinfo);
		if (err < 0) {
			SNDERR("Cannot add a control");
			return err;
		}
	}
	else if (cinfo->type != SND_CTL_ELEM_TYPE_BOOLEAN ||
	         cinfo->count != 1) {
		err = snd_ctl_elem_remove(swtch->ctl, &cinfo->id);
		if (err < 0) {
			SNDERR("Control %s mismatch", tmp_name);
			return err;
		}
		snd_ctl_elem_info_set_id(cinfo, ctl_id); /* reset numid */
		err = add_user_ctl(swtch, cinfo);
		if (err < 0) {
			SNDERR("Cannot add a control");
			return err;
		}
	}
	else {
		update_switch_value(swtch);
	}

	err = snd_ctl_subscribe_events(swtch->ctl, 1);
	if (err < 0) {
		SNDERR("Cannot suscribe to control events");
		return err;
	}

	err = snd_async_add_ctl_handler(&swtch->async_handler, swtch->ctl, switch_ctl_async_handler, pcm);
	if (err < 0) {
		SNDERR("Cannot add a control handler");
		return err;
	}

	if (drift_elem_name && drift_mult != 0) {
		/* Get drift compensator element */

		snd_ctl_elem_list_alloca(&elem_list);
		snd_ctl_elem_list_alloc_space(elem_list, 1);

		for (n = 0, elem_list->count = 1; n < snd_ctl_elem_list_get_count(elem_list); ++n) {
			snd_ctl_elem_list(swtch->ctl, elem_list);
			if (snd_ctl_elem_list_get_used(elem_list) &&
				!strcmp(snd_ctl_elem_list_get_name(elem_list, 0), drift_elem_name)) {
				numid = snd_ctl_elem_list_get_numid(elem_list, 0);
				break;
			}
			++elem_list->offset;
		}

		snd_ctl_elem_list_free_space(elem_list);

		if (numid < 0) {
			SNDERR("Drift compensator element not found");
			return numid;
		}

		snd_ctl_elem_id_alloca(&drift_ctl_id);
		snd_ctl_elem_id_set_numid(drift_ctl_id, numid);

		snd_ctl_elem_info_alloca(&cinfo);
		snd_ctl_elem_info_set_id(cinfo, drift_ctl_id);

		err = snd_ctl_elem_info(swtch->ctl, cinfo);
		if (err < 0) {
			SNDERR("Cannot open drift compensator element");
			return err;
		}
		swtch->drift_elem.id = *drift_ctl_id;
	}

	return 0;
}


int snd_pcm_switch_parse_drift_config(snd_config_t *conf, const char **drift_elem_name, double *drift_mult)
{
	snd_config_iterator_t i, next;
	int err, found_mult = 0, found_name = 0;

	if (snd_config_get_type(conf) != SND_CONFIG_TYPE_COMPOUND)
		return -EINVAL;

	snd_config_for_each(i, next, conf) {
		snd_config_t *n = snd_config_iterator_entry(i);
		const char *id;
		if (snd_config_get_id(n, &id) < 0)
			continue;
		if (snd_pcm_conf_generic_id(id))
			continue;
		if (strcmp(id, "name") == 0) {
			err = snd_config_get_string(n, drift_elem_name);
			if (err < 0) {
				SNDERR("Invalid type for %s", id);
				return err;
			}
			found_name = 1;
			continue;
		}
		if (strcmp(id, "mult") == 0) {
			switch (snd_config_get_type(n)) {
			case SND_CONFIG_TYPE_INTEGER: {
				long value;
				err = snd_config_get_integer(n, &value);
				*drift_mult = value;
				break;
			}
			case SND_CONFIG_TYPE_REAL:
				err = snd_config_get_real(n, drift_mult);
				break;
			default:
				err = -EINVAL;
				break;
			}
			if (err < 0) {
				SNDERR("Invalid type for %s", id);
				return err;
			}
			found_mult = 1;
			continue;
		}
	}

	// Set default values if not found
	if (!found_name)
		*drift_elem_name = "Drift compensator";
	if (!found_mult)
		*drift_mult = 1;

	DEBUG("Drift compensator config parsed: name=\"%s\" mult=%lf", *drift_elem_name, *drift_mult);
	return 0;
}

static const snd_pcm_ops_t snd_pcm_switch_ops = {
	.close = snd_pcm_switch_close,
	.info = snd_pcm_switch_info,
	.hw_refine = snd_pcm_switch_hw_refine,
	.hw_params = snd_pcm_switch_hw_params,
	.hw_free = snd_pcm_switch_hw_free,
	.sw_params = snd_pcm_switch_sw_params,
	.channel_info = snd_pcm_switch_channel_info,
	.dump = snd_pcm_switch_dump,
	.nonblock = snd_pcm_switch_nonblock,
	.async = snd_pcm_switch_async,
	.mmap = snd_pcm_switch_mmap,
	.munmap = snd_pcm_switch_munmap,
	.query_chmaps = snd_pcm_switch_query_chmaps,
	.get_chmap = snd_pcm_switch_get_chmap,
	.set_chmap = snd_pcm_switch_set_chmap,
};

static const snd_pcm_fast_ops_t snd_pcm_switch_fast_ops = {
	.status = snd_pcm_switch_status,
	.state = snd_pcm_switch_state,
	.hwsync = snd_pcm_switch_hwsync,
	.delay = snd_pcm_switch_delay,
	.prepare = snd_pcm_switch_prepare,
	.reset = snd_pcm_switch_reset,
	.start = snd_pcm_switch_start,
	.drop = snd_pcm_switch_drop,
	.drain = snd_pcm_switch_drain,
	.pause = snd_pcm_switch_pause,
	.rewindable = snd_pcm_switch_rewindable,
	.rewind = snd_pcm_switch_rewind,
	.forwardable = snd_pcm_switch_forwardable,
	.forward = snd_pcm_switch_forward,
	.resume = snd_pcm_switch_resume,
	.link = snd_pcm_switch_link,
	.link_slaves = snd_pcm_switch_link_slaves,
	.unlink = snd_pcm_switch_unlink,
	.writei = snd_pcm_switch_writei,
	.writen = snd_pcm_switch_writen,
	.avail_update = snd_pcm_switch_avail_update,
	.mmap_commit = snd_pcm_switch_mmap_commit,
	.htimestamp = snd_pcm_switch_htimestamp,
	.poll_descriptors_count = snd_pcm_switch_poll_descriptors_count,
	.poll_descriptors = snd_pcm_switch_poll_descriptors,
	.poll_revents = snd_pcm_switch_poll_revents,
	.may_wait_for_avail_min = snd_pcm_switch_may_wait_for_avail_min,
};

/**
 * \brief Creates a new copy PCM
 * \param pcmp Returns created PCM handle
 * \param name Name of PCM
 * \param stream Stream type
 * \param ctl_card card index of the control
 * \param ctl_id The control element
 * \param drift_elem_name Drift element compensator control name
 * \param drift_mult Multiplier of drift element control value
 * \retval zero on success otherwise a negative error code
 * \warning Using of this function might be dangerous in the sense
 *          of compatibility reasons. The prototype might be freely
 *          changed in future.
 */
int snd_pcm_switch_open(snd_pcm_t **pcmp, const char *name,
                        snd_pcm_stream_t stream, int mode ATTRIBUTE_UNUSED,
                        int ctl_card, snd_ctl_elem_id_t *ctl_id,
                        int cchannels,
                        const char *drift_elem_name, double drift_mult)
{
	DEBUG("");
	snd_pcm_t *pcm;
	snd_pcm_switch_t *swtch;
	int fd;
	int err;

	assert(pcmp);

	if (stream != SND_PCM_STREAM_PLAYBACK) {
		SYSERR("Only PLAYBACK mode is supported");
		return -EINVAL;
	}

	fd = open("/dev/null", O_WRONLY);
	if (fd < 0) {
		SYSERR("Cannot open /dev/null");
		return -errno;
	}

	swtch = calloc(1, sizeof(snd_pcm_switch_t));
	if (!swtch) {
		return -ENOMEM;
	}
	swtch->state = SND_PCM_STATE_OPEN;
	swtch->active_switch = swtch->cur_switch;
	swtch->cchannels = cchannels;
	swtch->poll_fd = fd;

	err = snd_pcm_new(&pcm, SND_PCM_TYPE_SWITCH, name,
	                  stream, mode);
	if (err < 0) {
		switch_free(swtch);
		return err;
	}
	pcm->ops = &snd_pcm_switch_ops;
	pcm->fast_ops = &snd_pcm_switch_fast_ops;
	pcm->private_data = swtch;
	pcm->poll_events = POLLOUT;
	pcm->tstamp_type = SND_PCM_TSTAMP_TYPE_MONOTONIC_RAW; // TODO: check if correct

	err = switch_load_control(pcm, swtch, ctl_card, ctl_id, drift_elem_name, drift_mult);
	if (err < 0) {
		switch_free(swtch);
		return err;
	}

	snd_pcm_set_hw_ptr(pcm, &swtch->hw_ptr, -1, 0);
	snd_pcm_set_appl_ptr(pcm, &swtch->appl_ptr, -1, 0);

	*pcmp = pcm;

	return 0;
}

/* in pcm_misc.c */
int snd_pcm_parse_control_id(snd_config_t *conf, snd_ctl_elem_id_t *ctl_id, int *cardp,
                             int *cchannelsp, int *hwctlp);

/*! \page pcm_plugins

\section pcm_plugins_switch Plugin: switch

This plugin can disconnect its slave when control is set.

When switch is on this plugin copies samples from master copy PCM to given
slave PCM.

When switch is off this plugin discards contents of a PCM stream synchronized
with softvare clock.

\code
pcm.name {
	type switch		# Switch PCM
	slave STR		# Slave name
	# or
	slave {			# Slave definition
		pcm STR		# Slave PCM name
		# or
		pcm { }		# Slave PCM definition
	}
}
\endcode

\subsection pcm_plugins_switch_funcref Function reference

<UL>
  <LI>snd_pcm_switch_open()
  <LI>_snd_pcm_switch_open()
</UL>

*/

/**
 * \brief Creates a new switch PCM
 * \param pcmp Returns created PCM handle
 * \param name Name of PCM
 * \param root Root configuration node
 * \param conf Configuration node with copy PCM description
 * \param stream Stream type
 * \param mode Stream mode
 * \retval zero on success otherwise a negative error code
 * \warning Using of this function might be dangerous in the sense
 *          of compatibility reasons. The prototype might be freely
 *          changed in future.
 */
int _snd_pcm_switch_open(snd_pcm_t **pcmp, const char *name,
                         snd_config_t *root, snd_config_t *conf,
                         snd_pcm_stream_t stream, int mode)
{
	DEBUG("");

	snd_config_iterator_t i, next;
	snd_pcm_t *spcm = NULL;
	snd_config_t *slave = NULL, *sconf;
	snd_config_t *control = NULL;
	snd_ctl_elem_id_t *ctl_id;
	snd_pcm_chmap_query_t **chmap = NULL;
	int ctl_card = -1, cchannels = 2, err;
	double drift_mult = 0;
	long rate = 0;
	const char *drift_elem_name = NULL;

	snd_config_for_each(i, next, conf) {
		snd_config_t *n = snd_config_iterator_entry(i);
		const char *id;
		if (snd_config_get_id(n, &id) < 0)
			continue;
		if (snd_pcm_conf_generic_id(id))
			continue;
		if (strcmp(id, "slave") == 0) {
			slave = n;
			continue;
		}
		if (strcmp(id, "control") == 0) {
			control = n;
			continue;
		}
		if (strcmp(id, "rate") == 0) {
			err = snd_config_get_integer(n, &rate);
			if (err < 0) {
				SNDERR("Invalid rate config");
				return err;
			}
			continue;
		}
		if (strcmp(id, "drift") == 0) {
			err = snd_pcm_switch_parse_drift_config(n, &drift_elem_name, &drift_mult);
			if (err < 0) {
				SNDERR("Invalid drift compensator config for %s", id);
				return err;
			}
			continue;
		}
		if (strcmp(id, "chmap") == 0) {
			snd_pcm_free_chmaps(chmap);
			chmap = _snd_pcm_parse_config_chmaps(n);
			if (!chmap) {
				SNDERR("Invalid channel map for %s", id);
				return -EINVAL;
			}
			continue;
		}
		SNDERR("Unknown field %s", id);
		err = -EINVAL;
		goto failed_open;
	}
	if (!slave) {
		SNDERR("slave is not defined");
		err = -EINVAL;
		goto failed_open;
	}
	if (!control) {
		SNDERR("control is not defined");
		err = -EINVAL;
		goto failed_open;
	}

	err = snd_pcm_slave_conf(root, slave, &sconf, 0);
	if (err < 0)
		goto failed_open;

	snd_ctl_elem_id_alloca(&ctl_id);
	err = snd_pcm_parse_control_id(control, ctl_id, &ctl_card, &cchannels, NULL);
	if (err < 0)
		goto failed_open;

	err = snd_pcm_switch_open(pcmp, name, stream, mode, ctl_card, ctl_id, cchannels, drift_elem_name, drift_mult);
	if (err < 0)
		goto failed_open;

	snd_pcm_switch_t *swtch = (*pcmp)->private_data;
	snd_config_copy(&swtch->conf, conf);
	snd_config_copy(&swtch->root, root);
	swtch->sconf = sconf;
	swtch->stream = stream;
	swtch->stream_mode = mode;
	swtch->active_switch = swtch->cur_switch;
	swtch->gen.close_slave = 1;
	swtch->gen.slave = NULL;
	swtch->drift_mult = drift_mult;
	swtch->restrict_rate = rate;

	/* Store hw configuration to handle use cases when we are opening
	 * swith plugin while it is disabled.
	 */
	const char *hw_params_path = "/tmp/alsa_switch_hw_params";

	if (swtch->cur_switch == SWITCH_CTL_ON) {
		/* open slave if switch is enabled */
		err = snd_pcm_open_slave(&spcm, root, sconf, stream, mode, conf);
		if (err == -EBUSY) {
			swtch->needs_update = 1;
			swtch->active_switch = SWITCH_CTL_OFF;
		} else if (err < 0)
			goto failed_open;
		else
			swtch->gen.slave = spcm;

		/* save hw_params to file */
		FILE * pFile;
		if (access(hw_params_path, 0)) {
			DEBUG("Write hw params %s", name);
			pFile = fopen(hw_params_path, "w");
			if (pFile != NULL) {
				err = snd_pcm_hw_params_any(spcm, &swtch->hw_params_any);
				if (err >= 0) {
					swtch->has_hw_params_any = 1;
					fwrite(&swtch->hw_params_any, sizeof(snd_pcm_hw_params_t), 1, pFile);
				}
				fclose (pFile);
			}
		} else {
			DEBUG("Have hw params");
		}
	}

	if (!swtch->has_hw_params_any) {
		/* read hw_params from file */
		FILE * pFile;
		pFile = fopen (hw_params_path, "r");
		if (pFile != NULL) {
			DEBUG("Read hw params");
			swtch->has_hw_params_any = 1;
			fread(&swtch->hw_params_any, sizeof(snd_pcm_hw_params_t), 1, pFile);
			fclose (pFile);
		}
	}

	DEBUG("Switch opened");

	return 0;

failed_open:
	snd_pcm_free_chmaps(chmap);
	if (spcm)
		snd_pcm_close(spcm);
	return err;
}

#ifndef DOC_HIDDEN
SND_DLSYM_BUILD_VERSION(_snd_pcm_switch_open, SND_PCM_DLSYM_VERSION);
#endif
