/*
 * Copyright © 2014 Intel Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

/**
 * DOC: Panel Self Refresh (PSR/SRD)
 *
 * Since Haswell Display controller supports Panel Self-Refresh on display
 * panels witch have a remote frame buffer (RFB) implemented according to PSR
 * spec in eDP1.3. PSR feature allows the display to go to lower standby states
 * when system is idle but display is on as it eliminates display refresh
 * request to DDR memory completely as long as the frame buffer for that
 * display is unchanged.
 *
 * Panel Self Refresh must be supported by both Hardware (source) and
 * Panel (sink).
 *
 * PSR saves power by caching the framebuffer in the panel RFB, which allows us
 * to power down the link and memory controller. For DSI panels the same idea
 * is called "manual mode".
 *
 * The implementation uses the hardware-based PSR support which automatically
 * enters/exits self-refresh mode. The hardware takes care of sending the
 * required DP aux message and could even retrain the link (that part isn't
 * enabled yet though). The hardware also keeps track of any frontbuffer
 * changes to know when to exit self-refresh mode again. Unfortunately that
 * part doesn't work too well, hence why the i915 PSR support uses the
 * software frontbuffer tracking to make sure it doesn't miss a screen
 * update. For this integration intel_psr_invalidate() and intel_psr_flush()
 * get called by the frontbuffer tracking code. Note that because of locking
 * issues the self-refresh re-enable code is done from a work queue, which
 * must be correctly synchronized/cancelled when shutting down the pipe."
 */

#include <drm/drmP.h>

#include "intel_drv.h"
#include "i915_drv.h"

static inline enum intel_display_power_domain
psr_aux_domain(struct intel_dp *intel_dp)
{
	/* CNL HW requires corresponding AUX IOs to be powered up for PSR.
	 * However, for non-A AUX ports the corresponding non-EDP transcoders
	 * would have already enabled power well 2 and DC_OFF. This means we can
	 * acquire a wider POWER_DOMAIN_AUX_{B,C,D,F} reference instead of a
	 * specific AUX_IO reference without powering up any extra wells.
	 * Note that PSR is enabled only on Port A even though this function
	 * returns the correct domain for other ports too.
	 */
	return intel_dp->aux_ch == AUX_CH_A ? POWER_DOMAIN_AUX_IO_A :
					      intel_dp->aux_power_domain;
}

static void psr_aux_io_power_get(struct intel_dp *intel_dp)
{
	struct intel_digital_port *intel_dig_port = dp_to_dig_port(intel_dp);
	struct drm_i915_private *dev_priv = to_i915(intel_dig_port->base.base.dev);

	if (INTEL_GEN(dev_priv) < 10)
		return;

	intel_display_power_get(dev_priv, psr_aux_domain(intel_dp));
}

static void psr_aux_io_power_put(struct intel_dp *intel_dp)
{
	struct intel_digital_port *intel_dig_port = dp_to_dig_port(intel_dp);
	struct drm_i915_private *dev_priv = to_i915(intel_dig_port->base.base.dev);

	if (INTEL_GEN(dev_priv) < 10)
		return;

	intel_display_power_put(dev_priv, psr_aux_domain(intel_dp));
}

void intel_psr_irq_control(struct drm_i915_private *dev_priv, bool debug)
{
	u32 debug_mask, mask;

	/* No PSR interrupts on VLV/CHV */
	if (IS_VALLEYVIEW(dev_priv) || IS_CHERRYVIEW(dev_priv))
		return;

	mask = EDP_PSR_ERROR(TRANSCODER_EDP);
	debug_mask = EDP_PSR_POST_EXIT(TRANSCODER_EDP) |
		     EDP_PSR_PRE_ENTRY(TRANSCODER_EDP);

	if (INTEL_GEN(dev_priv) >= 8) {
		mask |= EDP_PSR_ERROR(TRANSCODER_A) |
			EDP_PSR_ERROR(TRANSCODER_B) |
			EDP_PSR_ERROR(TRANSCODER_C);

		debug_mask |= EDP_PSR_POST_EXIT(TRANSCODER_A) |
			      EDP_PSR_PRE_ENTRY(TRANSCODER_A) |
			      EDP_PSR_POST_EXIT(TRANSCODER_B) |
			      EDP_PSR_PRE_ENTRY(TRANSCODER_B) |
			      EDP_PSR_POST_EXIT(TRANSCODER_C) |
			      EDP_PSR_PRE_ENTRY(TRANSCODER_C);
	}

	if (debug)
		mask |= debug_mask;

	WRITE_ONCE(dev_priv->psr.debug, debug);
	I915_WRITE(EDP_PSR_IMR, ~mask);
}

static void psr_event_print(u32 val, bool psr2_enabled)
{
	DRM_DEBUG_KMS("PSR exit events: 0x%x\n", val);
	if (val & PSR_EVENT_PSR2_WD_TIMER_EXPIRE)
		DRM_DEBUG_KMS("\tPSR2 watchdog timer expired\n");
	if ((val & PSR_EVENT_PSR2_DISABLED) && psr2_enabled)
		DRM_DEBUG_KMS("\tPSR2 disabled\n");
	if (val & PSR_EVENT_SU_DIRTY_FIFO_UNDERRUN)
		DRM_DEBUG_KMS("\tSU dirty FIFO underrun\n");
	if (val & PSR_EVENT_SU_CRC_FIFO_UNDERRUN)
		DRM_DEBUG_KMS("\tSU CRC FIFO underrun\n");
	if (val & PSR_EVENT_GRAPHICS_RESET)
		DRM_DEBUG_KMS("\tGraphics reset\n");
	if (val & PSR_EVENT_PCH_INTERRUPT)
		DRM_DEBUG_KMS("\tPCH interrupt\n");
	if (val & PSR_EVENT_MEMORY_UP)
		DRM_DEBUG_KMS("\tMemory up\n");
	if (val & PSR_EVENT_FRONT_BUFFER_MODIFY)
		DRM_DEBUG_KMS("\tFront buffer modification\n");
	if (val & PSR_EVENT_WD_TIMER_EXPIRE)
		DRM_DEBUG_KMS("\tPSR watchdog timer expired\n");
	if (val & PSR_EVENT_PIPE_REGISTERS_UPDATE)
		DRM_DEBUG_KMS("\tPIPE registers updated\n");
	if (val & PSR_EVENT_REGISTER_UPDATE)
		DRM_DEBUG_KMS("\tRegister updated\n");
	if (val & PSR_EVENT_HDCP_ENABLE)
		DRM_DEBUG_KMS("\tHDCP enabled\n");
	if (val & PSR_EVENT_KVMR_SESSION_ENABLE)
		DRM_DEBUG_KMS("\tKVMR session enabled\n");
	if (val & PSR_EVENT_VBI_ENABLE)
		DRM_DEBUG_KMS("\tVBI enabled\n");
	if (val & PSR_EVENT_LPSP_MODE_EXIT)
		DRM_DEBUG_KMS("\tLPSP mode exited\n");
	if ((val & PSR_EVENT_PSR_DISABLE) && !psr2_enabled)
		DRM_DEBUG_KMS("\tPSR disabled\n");
}

void intel_psr_irq_handler(struct drm_i915_private *dev_priv, u32 psr_iir)
{
	u32 transcoders = BIT(TRANSCODER_EDP);
	enum transcoder cpu_transcoder;
	ktime_t time_ns =  ktime_get();

	if (INTEL_GEN(dev_priv) >= 8)
		transcoders |= BIT(TRANSCODER_A) |
			       BIT(TRANSCODER_B) |
			       BIT(TRANSCODER_C);

	for_each_cpu_transcoder_masked(dev_priv, cpu_transcoder, transcoders) {
		/* FIXME: Exit PSR and link train manually when this happens. */
		if (psr_iir & EDP_PSR_ERROR(cpu_transcoder))
			DRM_DEBUG_KMS("[transcoder %s] PSR aux error\n",
				      transcoder_name(cpu_transcoder));

		if (psr_iir & EDP_PSR_PRE_ENTRY(cpu_transcoder)) {
			dev_priv->psr.last_entry_attempt = time_ns;
			DRM_DEBUG_KMS("[transcoder %s] PSR entry attempt in 2 vblanks\n",
				      transcoder_name(cpu_transcoder));
		}

		if (psr_iir & EDP_PSR_POST_EXIT(cpu_transcoder)) {
			dev_priv->psr.last_exit = time_ns;
			DRM_DEBUG_KMS("[transcoder %s] PSR exit completed\n",
				      transcoder_name(cpu_transcoder));

			if (INTEL_GEN(dev_priv) >= 9) {
				u32 val = I915_READ(PSR_EVENT(cpu_transcoder));
				bool psr2_enabled = dev_priv->psr.psr2_enabled;

				I915_WRITE(PSR_EVENT(cpu_transcoder), val);
				psr_event_print(val, psr2_enabled);
			}
		}
	}
}

static bool intel_dp_get_y_coord_required(struct intel_dp *intel_dp)
{
	uint8_t psr_caps = 0;

	if (drm_dp_dpcd_readb(&intel_dp->aux, DP_PSR_CAPS, &psr_caps) != 1)
		return false;
	return psr_caps & DP_PSR2_SU_Y_COORDINATE_REQUIRED;
}

static bool intel_dp_get_colorimetry_status(struct intel_dp *intel_dp)
{
	uint8_t dprx = 0;

	if (drm_dp_dpcd_readb(&intel_dp->aux, DP_DPRX_FEATURE_ENUMERATION_LIST,
			      &dprx) != 1)
		return false;
	return dprx & DP_VSC_SDP_EXT_FOR_COLORIMETRY_SUPPORTED;
}

static bool intel_dp_get_alpm_status(struct intel_dp *intel_dp)
{
	uint8_t alpm_caps = 0;

	if (drm_dp_dpcd_readb(&intel_dp->aux, DP_RECEIVER_ALPM_CAP,
			      &alpm_caps) != 1)
		return false;
	return alpm_caps & DP_ALPM_CAP;
}

static u8 intel_dp_get_sink_sync_latency(struct intel_dp *intel_dp)
{
	u8 val = 0;

	if (drm_dp_dpcd_readb(&intel_dp->aux,
			      DP_SYNCHRONIZATION_LATENCY_IN_SINK, &val) == 1)
		val &= DP_MAX_RESYNC_FRAME_COUNT_MASK;
	else
		DRM_ERROR("Unable to get sink synchronization latency\n");
	return val;
}

void intel_psr_init_dpcd(struct intel_dp *intel_dp)
{
	struct drm_i915_private *dev_priv =
		to_i915(dp_to_dig_port(intel_dp)->base.base.dev);

	drm_dp_dpcd_read(&intel_dp->aux, DP_PSR_SUPPORT, intel_dp->psr_dpcd,
			 sizeof(intel_dp->psr_dpcd));

	if (intel_dp->psr_dpcd[0]) {
		dev_priv->psr.sink_support = true;
		DRM_DEBUG_KMS("Detected EDP PSR Panel.\n");
	}

	if (INTEL_GEN(dev_priv) >= 9 &&
	    (intel_dp->psr_dpcd[0] == DP_PSR2_WITH_Y_COORD_IS_SUPPORTED)) {
		/*
		 * All panels that supports PSR version 03h (PSR2 +
		 * Y-coordinate) can handle Y-coordinates in VSC but we are
		 * only sure that it is going to be used when required by the
		 * panel. This way panel is capable to do selective update
		 * without a aux frame sync.
		 *
		 * To support PSR version 02h and PSR version 03h without
		 * Y-coordinate requirement panels we would need to enable
		 * GTC first.
		 */
		dev_priv->psr.sink_psr2_support =
				intel_dp_get_y_coord_required(intel_dp);
		DRM_DEBUG_KMS("PSR2 %s on sink", dev_priv->psr.sink_psr2_support
			      ? "supported" : "not supported");

		if (dev_priv->psr.sink_psr2_support) {
			dev_priv->psr.colorimetry_support =
				intel_dp_get_colorimetry_status(intel_dp);
			dev_priv->psr.alpm =
				intel_dp_get_alpm_status(intel_dp);
			dev_priv->psr.sink_sync_latency =
				intel_dp_get_sink_sync_latency(intel_dp);
		}
	}
}

static bool vlv_is_psr_active_on_pipe(struct drm_device *dev, int pipe)
{
	struct drm_i915_private *dev_priv = to_i915(dev);
	uint32_t val;

	val = I915_READ(VLV_PSRSTAT(pipe)) &
	      VLV_EDP_PSR_CURR_STATE_MASK;
	return (val == VLV_EDP_PSR_ACTIVE_NORFB_UP) ||
	       (val == VLV_EDP_PSR_ACTIVE_SF_UPDATE);
}

static void vlv_psr_setup_vsc(struct intel_dp *intel_dp,
			      const struct intel_crtc_state *crtc_state)
{
	struct intel_crtc *crtc = to_intel_crtc(crtc_state->base.crtc);
	struct drm_i915_private *dev_priv = to_i915(crtc->base.dev);
	uint32_t val;

	/* VLV auto-generate VSC package as per EDP 1.3 spec, Table 3.10 */
	val  = I915_READ(VLV_VSCSDP(crtc->pipe));
	val &= ~VLV_EDP_PSR_SDP_FREQ_MASK;
	val |= VLV_EDP_PSR_SDP_FREQ_EVFRAME;
	I915_WRITE(VLV_VSCSDP(crtc->pipe), val);
}

static void hsw_psr_setup_vsc(struct intel_dp *intel_dp,
			      const struct intel_crtc_state *crtc_state)
{
	struct intel_digital_port *intel_dig_port = dp_to_dig_port(intel_dp);
	struct drm_i915_private *dev_priv = to_i915(intel_dig_port->base.base.dev);
	struct edp_vsc_psr psr_vsc;

	if (dev_priv->psr.psr2_enabled) {
		/* Prepare VSC Header for SU as per EDP 1.4 spec, Table 6.11 */
		memset(&psr_vsc, 0, sizeof(psr_vsc));
		psr_vsc.sdp_header.HB0 = 0;
		psr_vsc.sdp_header.HB1 = 0x7;
		if (dev_priv->psr.colorimetry_support) {
			psr_vsc.sdp_header.HB2 = 0x5;
			psr_vsc.sdp_header.HB3 = 0x13;
		} else {
			psr_vsc.sdp_header.HB2 = 0x4;
			psr_vsc.sdp_header.HB3 = 0xe;
		}
	} else {
		/* Prepare VSC packet as per EDP 1.3 spec, Table 3.10 */
		memset(&psr_vsc, 0, sizeof(psr_vsc));
		psr_vsc.sdp_header.HB0 = 0;
		psr_vsc.sdp_header.HB1 = 0x7;
		psr_vsc.sdp_header.HB2 = 0x2;
		psr_vsc.sdp_header.HB3 = 0x8;
	}

	intel_dig_port->write_infoframe(&intel_dig_port->base.base, crtc_state,
					DP_SDP_VSC, &psr_vsc, sizeof(psr_vsc));
}

static void vlv_psr_enable_sink(struct intel_dp *intel_dp)
{
	drm_dp_dpcd_writeb(&intel_dp->aux, DP_PSR_EN_CFG,
			   DP_PSR_ENABLE | DP_PSR_MAIN_LINK_ACTIVE);
}

static void hsw_psr_setup_aux(struct intel_dp *intel_dp)
{
	struct intel_digital_port *dig_port = dp_to_dig_port(intel_dp);
	struct drm_i915_private *dev_priv = to_i915(dig_port->base.base.dev);
	u32 aux_clock_divider, aux_ctl;
	int i;
	static const uint8_t aux_msg[] = {
		[0] = DP_AUX_NATIVE_WRITE << 4,
		[1] = DP_SET_POWER >> 8,
		[2] = DP_SET_POWER & 0xff,
		[3] = 1 - 1,
		[4] = DP_SET_POWER_D0,
	};
	u32 psr_aux_mask = EDP_PSR_AUX_CTL_TIME_OUT_MASK |
			   EDP_PSR_AUX_CTL_MESSAGE_SIZE_MASK |
			   EDP_PSR_AUX_CTL_PRECHARGE_2US_MASK |
			   EDP_PSR_AUX_CTL_BIT_CLOCK_2X_MASK;

	BUILD_BUG_ON(sizeof(aux_msg) > 20);
	for (i = 0; i < sizeof(aux_msg); i += 4)
		I915_WRITE(EDP_PSR_AUX_DATA(i >> 2),
			   intel_dp_pack_aux(&aux_msg[i], sizeof(aux_msg) - i));

	aux_clock_divider = intel_dp->get_aux_clock_divider(intel_dp, 0);

	/* Start with bits set for DDI_AUX_CTL register */
	aux_ctl = intel_dp->get_aux_send_ctl(intel_dp, 0, sizeof(aux_msg),
					     aux_clock_divider);

	/* Select only valid bits for SRD_AUX_CTL */
	aux_ctl &= psr_aux_mask;
	I915_WRITE(EDP_PSR_AUX_CTL, aux_ctl);
}

static void hsw_psr_enable_sink(struct intel_dp *intel_dp)
{
	struct intel_digital_port *dig_port = dp_to_dig_port(intel_dp);
	struct drm_device *dev = dig_port->base.base.dev;
	struct drm_i915_private *dev_priv = to_i915(dev);
	u8 dpcd_val = DP_PSR_ENABLE;

	/* Enable ALPM at sink for psr2 */
	if (dev_priv->psr.psr2_enabled && dev_priv->psr.alpm)
		drm_dp_dpcd_writeb(&intel_dp->aux,
				DP_RECEIVER_ALPM_CONFIG,
				DP_ALPM_ENABLE);

	if (dev_priv->psr.psr2_enabled)
		dpcd_val |= DP_PSR_ENABLE_PSR2;
	if (dev_priv->psr.link_standby)
		dpcd_val |= DP_PSR_MAIN_LINK_ACTIVE;
	drm_dp_dpcd_writeb(&intel_dp->aux, DP_PSR_EN_CFG, dpcd_val);

	drm_dp_dpcd_writeb(&intel_dp->aux, DP_SET_POWER, DP_SET_POWER_D0);
}

static void vlv_psr_enable_source(struct intel_dp *intel_dp,
				  const struct intel_crtc_state *crtc_state)
{
	struct intel_digital_port *dig_port = dp_to_dig_port(intel_dp);
	struct drm_i915_private *dev_priv = to_i915(dig_port->base.base.dev);
	struct intel_crtc *crtc = to_intel_crtc(crtc_state->base.crtc);

	/* Transition from PSR_state 0 (disabled) to PSR_state 1 (inactive) */
	I915_WRITE(VLV_PSRCTL(crtc->pipe),
		   VLV_EDP_PSR_MODE_SW_TIMER |
		   VLV_EDP_PSR_SRC_TRANSMITTER_STATE |
		   VLV_EDP_PSR_ENABLE);
}

static void vlv_psr_activate(struct intel_dp *intel_dp)
{
	struct intel_digital_port *dig_port = dp_to_dig_port(intel_dp);
	struct drm_device *dev = dig_port->base.base.dev;
	struct drm_i915_private *dev_priv = to_i915(dev);
	struct drm_crtc *crtc = dig_port->base.base.crtc;
	enum pipe pipe = to_intel_crtc(crtc)->pipe;

	/*
	 * Let's do the transition from PSR_state 1 (inactive) to
	 * PSR_state 2 (transition to active - static frame transmission).
	 * Then Hardware is responsible for the transition to
	 * PSR_state 3 (active - no Remote Frame Buffer (RFB) update).
	 */
	I915_WRITE(VLV_PSRCTL(pipe), I915_READ(VLV_PSRCTL(pipe)) |
		   VLV_EDP_PSR_ACTIVE_ENTRY);
}

static void hsw_activate_psr1(struct intel_dp *intel_dp)
{
	struct intel_digital_port *dig_port = dp_to_dig_port(intel_dp);
	struct drm_device *dev = dig_port->base.base.dev;
	struct drm_i915_private *dev_priv = to_i915(dev);

	uint32_t max_sleep_time = 0x1f;
	/*
	 * Let's respect VBT in case VBT asks a higher idle_frame value.
	 * Let's use 6 as the minimum to cover all known cases including
	 * the off-by-one issue that HW has in some cases. Also there are
	 * cases where sink should be able to train
	 * with the 5 or 6 idle patterns.
	 */
	uint32_t idle_frames = max(6, dev_priv->vbt.psr.idle_frames);
	uint32_t val = EDP_PSR_ENABLE;

	val |= max_sleep_time << EDP_PSR_MAX_SLEEP_TIME_SHIFT;
	val |= idle_frames << EDP_PSR_IDLE_FRAME_SHIFT;

	if (IS_HASWELL(dev_priv))
		val |= EDP_PSR_MIN_LINK_ENTRY_TIME_8_LINES;

	if (dev_priv->psr.link_standby)
		val |= EDP_PSR_LINK_STANDBY;

	if (dev_priv->vbt.psr.tp1_wakeup_time > 5)
		val |= EDP_PSR_TP1_TIME_2500us;
	else if (dev_priv->vbt.psr.tp1_wakeup_time > 1)
		val |= EDP_PSR_TP1_TIME_500us;
	else if (dev_priv->vbt.psr.tp1_wakeup_time > 0)
		val |= EDP_PSR_TP1_TIME_100us;
	else
		val |= EDP_PSR_TP1_TIME_0us;

	if (dev_priv->vbt.psr.tp2_tp3_wakeup_time > 5)
		val |= EDP_PSR_TP2_TP3_TIME_2500us;
	else if (dev_priv->vbt.psr.tp2_tp3_wakeup_time > 1)
		val |= EDP_PSR_TP2_TP3_TIME_500us;
	else if (dev_priv->vbt.psr.tp2_tp3_wakeup_time > 0)
		val |= EDP_PSR_TP2_TP3_TIME_100us;
	else
		val |= EDP_PSR_TP2_TP3_TIME_0us;

	if (intel_dp_source_supports_hbr2(intel_dp) &&
	    drm_dp_tps3_supported(intel_dp->dpcd))
		val |= EDP_PSR_TP1_TP3_SEL;
	else
		val |= EDP_PSR_TP1_TP2_SEL;

	val |= I915_READ(EDP_PSR_CTL) & EDP_PSR_RESTORE_PSR_ACTIVE_CTX_MASK;
	I915_WRITE(EDP_PSR_CTL, val);
}

static void hsw_activate_psr2(struct intel_dp *intel_dp)
{
	struct intel_digital_port *dig_port = dp_to_dig_port(intel_dp);
	struct drm_device *dev = dig_port->base.base.dev;
	struct drm_i915_private *dev_priv = to_i915(dev);
	/*
	 * Let's respect VBT in case VBT asks a higher idle_frame value.
	 * Let's use 6 as the minimum to cover all known cases including
	 * the off-by-one issue that HW has in some cases. Also there are
	 * cases where sink should be able to train
	 * with the 5 or 6 idle patterns.
	 */
	uint32_t idle_frames = max(6, dev_priv->vbt.psr.idle_frames);
	u32 val = idle_frames << EDP_PSR2_IDLE_FRAME_SHIFT;

	/* FIXME: selective update is probably totally broken because it doesn't
	 * mesh at all with our frontbuffer tracking. And the hw alone isn't
	 * good enough. */
	val |= EDP_PSR2_ENABLE | EDP_SU_TRACK_ENABLE;
	if (INTEL_GEN(dev_priv) >= 10 || IS_GEMINILAKE(dev_priv))
		val |= EDP_Y_COORDINATE_ENABLE;

	val |= EDP_PSR2_FRAME_BEFORE_SU(dev_priv->psr.sink_sync_latency + 1);

	if (dev_priv->vbt.psr.tp2_tp3_wakeup_time > 5)
		val |= EDP_PSR2_TP2_TIME_2500;
	else if (dev_priv->vbt.psr.tp2_tp3_wakeup_time > 1)
		val |= EDP_PSR2_TP2_TIME_500;
	else if (dev_priv->vbt.psr.tp2_tp3_wakeup_time > 0)
		val |= EDP_PSR2_TP2_TIME_100;
	else
		val |= EDP_PSR2_TP2_TIME_50;

	I915_WRITE(EDP_PSR2_CTL, val);
}

static void hsw_psr_activate(struct intel_dp *intel_dp)
{
	struct intel_digital_port *dig_port = dp_to_dig_port(intel_dp);
	struct drm_device *dev = dig_port->base.base.dev;
	struct drm_i915_private *dev_priv = to_i915(dev);

	/* On HSW+ after we enable PSR on source it will activate it
	 * as soon as it match configure idle_frame count. So
	 * we just actually enable it here on activation time.
	 */

	/* psr1 and psr2 are mutually exclusive.*/
	if (dev_priv->psr.psr2_enabled)
		hsw_activate_psr2(intel_dp);
	else
		hsw_activate_psr1(intel_dp);
}

static bool intel_psr2_config_valid(struct intel_dp *intel_dp,
				    struct intel_crtc_state *crtc_state)
{
	struct intel_digital_port *dig_port = dp_to_dig_port(intel_dp);
	struct drm_i915_private *dev_priv = to_i915(dig_port->base.base.dev);
	int crtc_hdisplay = crtc_state->base.adjusted_mode.crtc_hdisplay;
	int crtc_vdisplay = crtc_state->base.adjusted_mode.crtc_vdisplay;
	int psr_max_h = 0, psr_max_v = 0;

	/*
	 * FIXME psr2_support is messed up. It's both computed
	 * dynamically during PSR enable, and extracted from sink
	 * caps during eDP detection.
	 */
	if (!dev_priv->psr.sink_psr2_support)
		return false;

	if (INTEL_GEN(dev_priv) >= 10 || IS_GEMINILAKE(dev_priv)) {
		psr_max_h = 4096;
		psr_max_v = 2304;
	} else if (IS_GEN9(dev_priv)) {
		psr_max_h = 3640;
		psr_max_v = 2304;
	}

	if (crtc_hdisplay > psr_max_h || crtc_vdisplay > psr_max_v) {
		DRM_DEBUG_KMS("PSR2 not enabled, resolution %dx%d > max supported %dx%d\n",
			      crtc_hdisplay, crtc_vdisplay,
			      psr_max_h, psr_max_v);
		return false;
	}

	return true;
}

void intel_psr_compute_config(struct intel_dp *intel_dp,
			      struct intel_crtc_state *crtc_state)
{
	struct intel_digital_port *dig_port = dp_to_dig_port(intel_dp);
	struct drm_i915_private *dev_priv = to_i915(dig_port->base.base.dev);
	const struct drm_display_mode *adjusted_mode =
		&crtc_state->base.adjusted_mode;
	int psr_setup_time;

	if (!CAN_PSR(dev_priv))
		return;

	if (!i915_modparams.enable_psr) {
		DRM_DEBUG_KMS("PSR disable by flag\n");
		return;
	}

	/*
	 * HSW spec explicitly says PSR is tied to port A.
	 * BDW+ platforms with DDI implementation of PSR have different
	 * PSR registers per transcoder and we only implement transcoder EDP
	 * ones. Since by Display design transcoder EDP is tied to port A
	 * we can safely escape based on the port A.
	 */
	if (HAS_DDI(dev_priv) && dig_port->base.port != PORT_A) {
		DRM_DEBUG_KMS("PSR condition failed: Port not supported\n");
		return;
	}

	if ((IS_VALLEYVIEW(dev_priv) || IS_CHERRYVIEW(dev_priv)) &&
	    !dev_priv->psr.link_standby) {
		DRM_ERROR("PSR condition failed: Link off requested but not supported on this platform\n");
		return;
	}

	if (IS_HASWELL(dev_priv) &&
	    I915_READ(HSW_STEREO_3D_CTL(crtc_state->cpu_transcoder)) &
		      S3D_ENABLE) {
		DRM_DEBUG_KMS("PSR condition failed: Stereo 3D is Enabled\n");
		return;
	}

	if (IS_HASWELL(dev_priv) &&
	    adjusted_mode->flags & DRM_MODE_FLAG_INTERLACE) {
		DRM_DEBUG_KMS("PSR condition failed: Interlaced is Enabled\n");
		return;
	}

	psr_setup_time = drm_dp_psr_setup_time(intel_dp->psr_dpcd);
	if (psr_setup_time < 0) {
		DRM_DEBUG_KMS("PSR condition failed: Invalid PSR setup time (0x%02x)\n",
			      intel_dp->psr_dpcd[1]);
		return;
	}

	if (intel_usecs_to_scanlines(adjusted_mode, psr_setup_time) >
	    adjusted_mode->crtc_vtotal - adjusted_mode->crtc_vdisplay - 1) {
		DRM_DEBUG_KMS("PSR condition failed: PSR setup time (%d us) too long\n",
			      psr_setup_time);
		return;
	}

	if (!(intel_dp->edp_dpcd[1] & DP_EDP_SET_POWER_CAP)) {
		DRM_DEBUG_KMS("PSR condition failed: panel lacks power state control\n");
		return;
	}

	crtc_state->has_psr = true;
	crtc_state->has_psr2 = intel_psr2_config_valid(intel_dp, crtc_state);
	DRM_DEBUG_KMS("Enabling PSR%s\n", crtc_state->has_psr2 ? "2" : "");
}

static void intel_psr_activate(struct intel_dp *intel_dp)
{
	struct intel_digital_port *intel_dig_port = dp_to_dig_port(intel_dp);
	struct drm_device *dev = intel_dig_port->base.base.dev;
	struct drm_i915_private *dev_priv = to_i915(dev);

	if (dev_priv->psr.psr2_enabled)
		WARN_ON(I915_READ(EDP_PSR2_CTL) & EDP_PSR2_ENABLE);
	else
		WARN_ON(I915_READ(EDP_PSR_CTL) & EDP_PSR_ENABLE);
	WARN_ON(dev_priv->psr.active);
	lockdep_assert_held(&dev_priv->psr.lock);

	dev_priv->psr.activate(intel_dp);
	dev_priv->psr.active = true;
}

static void hsw_psr_enable_source(struct intel_dp *intel_dp,
				  const struct intel_crtc_state *crtc_state)
{
	struct intel_digital_port *dig_port = dp_to_dig_port(intel_dp);
	struct drm_device *dev = dig_port->base.base.dev;
	struct drm_i915_private *dev_priv = to_i915(dev);
	enum transcoder cpu_transcoder = crtc_state->cpu_transcoder;

	psr_aux_io_power_get(intel_dp);

	/* Only HSW and BDW have PSR AUX registers that need to be setup. SKL+
	 * use hardcoded values PSR AUX transactions
	 */
	if (IS_HASWELL(dev_priv) || IS_BROADWELL(dev_priv))
		hsw_psr_setup_aux(intel_dp);

	if (dev_priv->psr.psr2_enabled) {
		u32 chicken = I915_READ(CHICKEN_TRANS(cpu_transcoder));

		if (INTEL_GEN(dev_priv) == 9 && !IS_GEMINILAKE(dev_priv))
			chicken |= (PSR2_VSC_ENABLE_PROG_HEADER
				   | PSR2_ADD_VERTICAL_LINE_COUNT);

		else
			chicken &= ~VSC_DATA_SEL_SOFTWARE_CONTROL;
		I915_WRITE(CHICKEN_TRANS(cpu_transcoder), chicken);

		I915_WRITE(EDP_PSR_DEBUG,
			   EDP_PSR_DEBUG_MASK_MEMUP |
			   EDP_PSR_DEBUG_MASK_HPD |
			   EDP_PSR_DEBUG_MASK_LPSP |
			   EDP_PSR_DEBUG_MASK_MAX_SLEEP |
			   EDP_PSR_DEBUG_MASK_DISP_REG_WRITE);
	} else {
		/*
		 * Per Spec: Avoid continuous PSR exit by masking MEMUP
		 * and HPD. also mask LPSP to avoid dependency on other
		 * drivers that might block runtime_pm besides
		 * preventing  other hw tracking issues now we can rely
		 * on frontbuffer tracking.
		 */
		I915_WRITE(EDP_PSR_DEBUG,
			   EDP_PSR_DEBUG_MASK_MEMUP |
			   EDP_PSR_DEBUG_MASK_HPD |
			   EDP_PSR_DEBUG_MASK_LPSP |
			   EDP_PSR_DEBUG_MASK_DISP_REG_WRITE);
	}
}

/**
 * intel_psr_enable - Enable PSR
 * @intel_dp: Intel DP
 * @crtc_state: new CRTC state
 *
 * This function can only be called after the pipe is fully trained and enabled.
 */
void intel_psr_enable(struct intel_dp *intel_dp,
		      const struct intel_crtc_state *crtc_state)
{
	struct intel_digital_port *intel_dig_port = dp_to_dig_port(intel_dp);
	struct drm_device *dev = intel_dig_port->base.base.dev;
	struct drm_i915_private *dev_priv = to_i915(dev);

	if (!crtc_state->has_psr)
		return;

	if (WARN_ON(!CAN_PSR(dev_priv)))
		return;

	WARN_ON(dev_priv->drrs.dp);
	mutex_lock(&dev_priv->psr.lock);
	if (dev_priv->psr.enabled) {
		DRM_DEBUG_KMS("PSR already in use\n");
		goto unlock;
	}

	dev_priv->psr.psr2_enabled = crtc_state->has_psr2;
	dev_priv->psr.busy_frontbuffer_bits = 0;

	dev_priv->psr.setup_vsc(intel_dp, crtc_state);
	dev_priv->psr.enable_sink(intel_dp);
	dev_priv->psr.enable_source(intel_dp, crtc_state);
	dev_priv->psr.enabled = intel_dp;

	if (INTEL_GEN(dev_priv) >= 9) {
		intel_psr_activate(intel_dp);
	} else {
		/*
		 * FIXME: Activation should happen immediately since this
		 * function is just called after pipe is fully trained and
		 * enabled.
		 * However on some platforms we face issues when first
		 * activation follows a modeset so quickly.
		 *     - On VLV/CHV we get bank screen on first activation
		 *     - On HSW/BDW we get a recoverable frozen screen until
		 *       next exit-activate sequence.
		 */
		schedule_delayed_work(&dev_priv->psr.work,
				      msecs_to_jiffies(intel_dp->panel_power_cycle_delay * 5));
	}

unlock:
	mutex_unlock(&dev_priv->psr.lock);
}

static void vlv_psr_disable(struct intel_dp *intel_dp,
			    const struct intel_crtc_state *old_crtc_state)
{
	struct intel_digital_port *intel_dig_port = dp_to_dig_port(intel_dp);
	struct drm_device *dev = intel_dig_port->base.base.dev;
	struct drm_i915_private *dev_priv = to_i915(dev);
	struct intel_crtc *crtc = to_intel_crtc(old_crtc_state->base.crtc);
	uint32_t val;

	if (dev_priv->psr.active) {
		/* Put VLV PSR back to PSR_state 0 (disabled). */
		if (intel_wait_for_register(dev_priv,
					    VLV_PSRSTAT(crtc->pipe),
					    VLV_EDP_PSR_IN_TRANS,
					    0,
					    1))
			WARN(1, "PSR transition took longer than expected\n");

		val = I915_READ(VLV_PSRCTL(crtc->pipe));
		val &= ~VLV_EDP_PSR_ACTIVE_ENTRY;
		val &= ~VLV_EDP_PSR_ENABLE;
		val &= ~VLV_EDP_PSR_MODE_MASK;
		I915_WRITE(VLV_PSRCTL(crtc->pipe), val);

		dev_priv->psr.active = false;
	} else {
		WARN_ON(vlv_is_psr_active_on_pipe(dev, crtc->pipe));
	}
}

static void hsw_psr_disable(struct intel_dp *intel_dp,
			    const struct intel_crtc_state *old_crtc_state)
{
	struct intel_digital_port *intel_dig_port = dp_to_dig_port(intel_dp);
	struct drm_device *dev = intel_dig_port->base.base.dev;
	struct drm_i915_private *dev_priv = to_i915(dev);

	if (dev_priv->psr.active) {
		i915_reg_t psr_status;
		u32 psr_status_mask;

		if (dev_priv->psr.psr2_enabled) {
			psr_status = EDP_PSR2_STATUS;
			psr_status_mask = EDP_PSR2_STATUS_STATE_MASK;

			I915_WRITE(EDP_PSR2_CTL,
				   I915_READ(EDP_PSR2_CTL) &
				   ~(EDP_PSR2_ENABLE | EDP_SU_TRACK_ENABLE));

		} else {
			psr_status = EDP_PSR_STATUS;
			psr_status_mask = EDP_PSR_STATUS_STATE_MASK;

			I915_WRITE(EDP_PSR_CTL,
				   I915_READ(EDP_PSR_CTL) & ~EDP_PSR_ENABLE);
		}

		/* Wait till PSR is idle */
		if (intel_wait_for_register(dev_priv,
					    psr_status, psr_status_mask, 0,
					    2000))
			DRM_ERROR("Timed out waiting for PSR Idle State\n");

		dev_priv->psr.active = false;
	} else {
		if (dev_priv->psr.psr2_enabled)
			WARN_ON(I915_READ(EDP_PSR2_CTL) & EDP_PSR2_ENABLE);
		else
			WARN_ON(I915_READ(EDP_PSR_CTL) & EDP_PSR_ENABLE);
	}

	psr_aux_io_power_put(intel_dp);
}

/**
 * intel_psr_disable - Disable PSR
 * @intel_dp: Intel DP
 * @old_crtc_state: old CRTC state
 *
 * This function needs to be called before disabling pipe.
 */
void intel_psr_disable(struct intel_dp *intel_dp,
		       const struct intel_crtc_state *old_crtc_state)
{
	struct intel_digital_port *intel_dig_port = dp_to_dig_port(intel_dp);
	struct drm_device *dev = intel_dig_port->base.base.dev;
	struct drm_i915_private *dev_priv = to_i915(dev);

	if (!old_crtc_state->has_psr)
		return;

	if (WARN_ON(!CAN_PSR(dev_priv)))
		return;

	mutex_lock(&dev_priv->psr.lock);
	if (!dev_priv->psr.enabled) {
		mutex_unlock(&dev_priv->psr.lock);
		return;
	}

	dev_priv->psr.disable_source(intel_dp, old_crtc_state);

	/* Disable PSR on Sink */
	drm_dp_dpcd_writeb(&intel_dp->aux, DP_PSR_EN_CFG, 0);

	dev_priv->psr.enabled = NULL;
	mutex_unlock(&dev_priv->psr.lock);

	cancel_delayed_work_sync(&dev_priv->psr.work);
}

static bool psr_wait_for_idle(struct drm_i915_private *dev_priv)
{
	struct intel_dp *intel_dp;
	i915_reg_t reg;
	u32 mask;
	int err;

	intel_dp = dev_priv->psr.enabled;
	if (!intel_dp)
		return false;

	if (HAS_DDI(dev_priv)) {
		if (dev_priv->psr.psr2_enabled) {
			reg = EDP_PSR2_STATUS;
			mask = EDP_PSR2_STATUS_STATE_MASK;
		} else {
			reg = EDP_PSR_STATUS;
			mask = EDP_PSR_STATUS_STATE_MASK;
		}
	} else {
		struct drm_crtc *crtc =
			dp_to_dig_port(intel_dp)->base.base.crtc;
		enum pipe pipe = to_intel_crtc(crtc)->pipe;

		reg = VLV_PSRSTAT(pipe);
		mask = VLV_EDP_PSR_IN_TRANS;
	}

	mutex_unlock(&dev_priv->psr.lock);

	err = intel_wait_for_register(dev_priv, reg, mask, 0, 50);
	if (err)
		DRM_ERROR("Timed out waiting for PSR Idle for re-enable\n");

	/* After the unlocked wait, verify that PSR is still wanted! */
	mutex_lock(&dev_priv->psr.lock);
	return err == 0 && dev_priv->psr.enabled;
}

static void intel_psr_work(struct work_struct *work)
{
	struct drm_i915_private *dev_priv =
		container_of(work, typeof(*dev_priv), psr.work.work);

	mutex_lock(&dev_priv->psr.lock);

	/*
	 * We have to make sure PSR is ready for re-enable
	 * otherwise it keeps disabled until next full enable/disable cycle.
	 * PSR might take some time to get fully disabled
	 * and be ready for re-enable.
	 */
	if (!psr_wait_for_idle(dev_priv))
		goto unlock;

	/*
	 * The delayed work can race with an invalidate hence we need to
	 * recheck. Since psr_flush first clears this and then reschedules we
	 * won't ever miss a flush when bailing out here.
	 */
	if (dev_priv->psr.busy_frontbuffer_bits)
		goto unlock;

	intel_psr_activate(dev_priv->psr.enabled);
unlock:
	mutex_unlock(&dev_priv->psr.lock);
}

static void intel_psr_exit(struct drm_i915_private *dev_priv)
{
	struct intel_dp *intel_dp = dev_priv->psr.enabled;
	struct drm_crtc *crtc = dp_to_dig_port(intel_dp)->base.base.crtc;
	enum pipe pipe = to_intel_crtc(crtc)->pipe;
	u32 val;

	if (!dev_priv->psr.active)
		return;

	if (HAS_DDI(dev_priv)) {
		if (dev_priv->psr.psr2_enabled) {
			val = I915_READ(EDP_PSR2_CTL);
			WARN_ON(!(val & EDP_PSR2_ENABLE));
			I915_WRITE(EDP_PSR2_CTL, val & ~EDP_PSR2_ENABLE);
		} else {
			val = I915_READ(EDP_PSR_CTL);
			WARN_ON(!(val & EDP_PSR_ENABLE));
			I915_WRITE(EDP_PSR_CTL, val & ~EDP_PSR_ENABLE);
		}
	} else {
		val = I915_READ(VLV_PSRCTL(pipe));

		/*
		 * Here we do the transition drirectly from
		 * PSR_state 3 (active - no Remote Frame Buffer (RFB) update) to
		 * PSR_state 5 (exit).
		 * PSR State 4 (active with single frame update) can be skipped.
		 * On PSR_state 5 (exit) Hardware is responsible to transition
		 * back to PSR_state 1 (inactive).
		 * Now we are at Same state after vlv_psr_enable_source.
		 */
		val &= ~VLV_EDP_PSR_ACTIVE_ENTRY;
		I915_WRITE(VLV_PSRCTL(pipe), val);

		/*
		 * Send AUX wake up - Spec says after transitioning to PSR
		 * active we have to send AUX wake up by writing 01h in DPCD
		 * 600h of sink device.
		 * XXX: This might slow down the transition, but without this
		 * HW doesn't complete the transition to PSR_state 1 and we
		 * never get the screen updated.
		 */
		drm_dp_dpcd_writeb(&intel_dp->aux, DP_SET_POWER,
				   DP_SET_POWER_D0);
	}

	dev_priv->psr.active = false;
}

/**
 * intel_psr_single_frame_update - Single Frame Update
 * @dev_priv: i915 device
 * @frontbuffer_bits: frontbuffer plane tracking bits
 *
 * Some platforms support a single frame update feature that is used to
 * send and update only one frame on Remote Frame Buffer.
 * So far it is only implemented for Valleyview and Cherryview because
 * hardware requires this to be done before a page flip.
 */
void intel_psr_single_frame_update(struct drm_i915_private *dev_priv,
				   unsigned frontbuffer_bits)
{
	struct drm_crtc *crtc;
	enum pipe pipe;
	u32 val;

	if (!CAN_PSR(dev_priv))
		return;

	/*
	 * Single frame update is already supported on BDW+ but it requires
	 * many W/A and it isn't really needed.
	 */
	if (!IS_VALLEYVIEW(dev_priv) && !IS_CHERRYVIEW(dev_priv))
		return;

	mutex_lock(&dev_priv->psr.lock);
	if (!dev_priv->psr.enabled) {
		mutex_unlock(&dev_priv->psr.lock);
		return;
	}

	crtc = dp_to_dig_port(dev_priv->psr.enabled)->base.base.crtc;
	pipe = to_intel_crtc(crtc)->pipe;

	if (frontbuffer_bits & INTEL_FRONTBUFFER_ALL_MASK(pipe)) {
		val = I915_READ(VLV_PSRCTL(pipe));

		/*
		 * We need to set this bit before writing registers for a flip.
		 * This bit will be self-clear when it gets to the PSR active state.
		 */
		I915_WRITE(VLV_PSRCTL(pipe), val | VLV_EDP_PSR_SINGLE_FRAME_UPDATE);
	}
	mutex_unlock(&dev_priv->psr.lock);
}

/**
 * intel_psr_invalidate - Invalidade PSR
 * @dev_priv: i915 device
 * @frontbuffer_bits: frontbuffer plane tracking bits
 * @origin: which operation caused the invalidate
 *
 * Since the hardware frontbuffer tracking has gaps we need to integrate
 * with the software frontbuffer tracking. This function gets called every
 * time frontbuffer rendering starts and a buffer gets dirtied. PSR must be
 * disabled if the frontbuffer mask contains a buffer relevant to PSR.
 *
 * Dirty frontbuffers relevant to PSR are tracked in busy_frontbuffer_bits."
 */
void intel_psr_invalidate(struct drm_i915_private *dev_priv,
			  unsigned frontbuffer_bits, enum fb_op_origin origin)
{
	struct drm_crtc *crtc;
	enum pipe pipe;

	if (!CAN_PSR(dev_priv))
		return;

	if (dev_priv->psr.has_hw_tracking && origin == ORIGIN_FLIP)
		return;

	mutex_lock(&dev_priv->psr.lock);
	if (!dev_priv->psr.enabled) {
		mutex_unlock(&dev_priv->psr.lock);
		return;
	}

	crtc = dp_to_dig_port(dev_priv->psr.enabled)->base.base.crtc;
	pipe = to_intel_crtc(crtc)->pipe;

	frontbuffer_bits &= INTEL_FRONTBUFFER_ALL_MASK(pipe);
	dev_priv->psr.busy_frontbuffer_bits |= frontbuffer_bits;

	if (frontbuffer_bits)
		intel_psr_exit(dev_priv);

	mutex_unlock(&dev_priv->psr.lock);
}

/**
 * intel_psr_flush - Flush PSR
 * @dev_priv: i915 device
 * @frontbuffer_bits: frontbuffer plane tracking bits
 * @origin: which operation caused the flush
 *
 * Since the hardware frontbuffer tracking has gaps we need to integrate
 * with the software frontbuffer tracking. This function gets called every
 * time frontbuffer rendering has completed and flushed out to memory. PSR
 * can be enabled again if no other frontbuffer relevant to PSR is dirty.
 *
 * Dirty frontbuffers relevant to PSR are tracked in busy_frontbuffer_bits.
 */
void intel_psr_flush(struct drm_i915_private *dev_priv,
		     unsigned frontbuffer_bits, enum fb_op_origin origin)
{
	struct drm_crtc *crtc;
	enum pipe pipe;

	if (!CAN_PSR(dev_priv))
		return;

	if (dev_priv->psr.has_hw_tracking && origin == ORIGIN_FLIP)
		return;

	mutex_lock(&dev_priv->psr.lock);
	if (!dev_priv->psr.enabled) {
		mutex_unlock(&dev_priv->psr.lock);
		return;
	}

	crtc = dp_to_dig_port(dev_priv->psr.enabled)->base.base.crtc;
	pipe = to_intel_crtc(crtc)->pipe;

	frontbuffer_bits &= INTEL_FRONTBUFFER_ALL_MASK(pipe);
	dev_priv->psr.busy_frontbuffer_bits &= ~frontbuffer_bits;

	/* By definition flush = invalidate + flush */
	if (frontbuffer_bits) {
		if (dev_priv->psr.psr2_enabled ||
		    IS_VALLEYVIEW(dev_priv) || IS_CHERRYVIEW(dev_priv)) {
			intel_psr_exit(dev_priv);
		} else {
			/*
			 * Display WA #0884: all
			 * This documented WA for bxt can be safely applied
			 * broadly so we can force HW tracking to exit PSR
			 * instead of disabling and re-enabling.
			 * Workaround tells us to write 0 to CUR_SURFLIVE_A,
			 * but it makes more sense write to the current active
			 * pipe.
			 */
			I915_WRITE(CURSURFLIVE(pipe), 0);
		}
	}

	if (!dev_priv->psr.active && !dev_priv->psr.busy_frontbuffer_bits)
		if (!work_busy(&dev_priv->psr.work.work))
			schedule_delayed_work(&dev_priv->psr.work,
					      msecs_to_jiffies(100));
	mutex_unlock(&dev_priv->psr.lock);
}

/**
 * intel_psr_init - Init basic PSR work and mutex.
 * @dev_priv: i915 device private
 *
 * This function is  called only once at driver load to initialize basic
 * PSR stuff.
 */
void intel_psr_init(struct drm_i915_private *dev_priv)
{
	if (!HAS_PSR(dev_priv))
		return;

	dev_priv->psr_mmio_base = IS_HASWELL(dev_priv) ?
		HSW_EDP_PSR_BASE : BDW_EDP_PSR_BASE;

	if (!dev_priv->psr.sink_support)
		return;

	if (i915_modparams.enable_psr == -1) {
		i915_modparams.enable_psr = dev_priv->vbt.psr.enable;

		/* Per platform default: all disabled. */
		i915_modparams.enable_psr = 0;
	}

	/* Set link_standby x link_off defaults */
	if (IS_HASWELL(dev_priv) || IS_BROADWELL(dev_priv))
		/* HSW and BDW require workarounds that we don't implement. */
		dev_priv->psr.link_standby = false;
	else if (IS_VALLEYVIEW(dev_priv) || IS_CHERRYVIEW(dev_priv))
		/* On VLV and CHV only standby mode is supported. */
		dev_priv->psr.link_standby = true;
	else
		/* For new platforms let's respect VBT back again */
		dev_priv->psr.link_standby = dev_priv->vbt.psr.full_link;

	/* Override link_standby x link_off defaults */
	if (i915_modparams.enable_psr == 2 && !dev_priv->psr.link_standby) {
		DRM_DEBUG_KMS("PSR: Forcing link standby\n");
		dev_priv->psr.link_standby = true;
	}
	if (i915_modparams.enable_psr == 3 && dev_priv->psr.link_standby) {
		DRM_DEBUG_KMS("PSR: Forcing main link off\n");
		dev_priv->psr.link_standby = false;
	}

	INIT_DELAYED_WORK(&dev_priv->psr.work, intel_psr_work);
	mutex_init(&dev_priv->psr.lock);

	if (IS_VALLEYVIEW(dev_priv) || IS_CHERRYVIEW(dev_priv)) {
		dev_priv->psr.enable_source = vlv_psr_enable_source;
		dev_priv->psr.disable_source = vlv_psr_disable;
		dev_priv->psr.enable_sink = vlv_psr_enable_sink;
		dev_priv->psr.activate = vlv_psr_activate;
		dev_priv->psr.setup_vsc = vlv_psr_setup_vsc;
	} else {
		dev_priv->psr.has_hw_tracking = true;
		dev_priv->psr.enable_source = hsw_psr_enable_source;
		dev_priv->psr.disable_source = hsw_psr_disable;
		dev_priv->psr.enable_sink = hsw_psr_enable_sink;
		dev_priv->psr.activate = hsw_psr_activate;
		dev_priv->psr.setup_vsc = hsw_psr_setup_vsc;
	}
}