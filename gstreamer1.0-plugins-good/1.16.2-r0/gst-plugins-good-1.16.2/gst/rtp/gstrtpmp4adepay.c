/* GStreamer
 * Copyright (C) <2007> Nokia Corporation (contact <stefan.kost@nokia.com>)
 *               <2007> Wim Taymans <wim.taymans@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License version 2 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#include <gst/base/gstbitreader.h>
#include <gst/rtp/gstrtpbuffer.h>
#include <gst/audio/audio.h>

#include <string.h>
#include "gstrtpmp4adepay.h"
#include "gstrtputils.h"

GST_DEBUG_CATEGORY_STATIC (rtpmp4adepay_debug);
#define GST_CAT_DEFAULT (rtpmp4adepay_debug)

static GstStaticPadTemplate gst_rtp_mp4a_depay_src_template =
GST_STATIC_PAD_TEMPLATE ("src",
    GST_PAD_SRC,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS ("audio/mpeg,"
        "mpegversion = (int) { 2, 4 },"
        "framed = (boolean) true, "
        "stream-format = (string) raw,"
        "clock-rate = (int) [ 1, MAX ], "
        "channels = (int) [ 1, 2 ]," "base-profile = (string) { lc, ltp, ssr }")
    );

static GstStaticPadTemplate gst_rtp_mp4a_depay_sink_template =
GST_STATIC_PAD_TEMPLATE ("sink",
    GST_PAD_SINK,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS ("application/x-rtp, "
        "media = (string) \"audio\", "
        "clock-rate = (int) [1, MAX ], "
        "encoding-name = (string) \"MP4A-LATM\""
        /* All optional parameters
         *
         * "profile-level-id=[1,MAX]"
         * "config="
         */
    )
    );

#define gst_rtp_mp4a_depay_parent_class parent_class
G_DEFINE_TYPE (GstRtpMP4ADepay, gst_rtp_mp4a_depay,
    GST_TYPE_RTP_BASE_DEPAYLOAD);

static void gst_rtp_mp4a_depay_finalize (GObject * object);

static gboolean gst_rtp_mp4a_depay_setcaps (GstRTPBaseDepayload * depayload,
    GstCaps * caps);
static GstBuffer *gst_rtp_mp4a_depay_process (GstRTPBaseDepayload * depayload,
    GstRTPBuffer * rtp);

static GstStateChangeReturn gst_rtp_mp4a_depay_change_state (GstElement *
    element, GstStateChange transition);


static void
gst_rtp_mp4a_depay_class_init (GstRtpMP4ADepayClass * klass)
{
  GObjectClass *gobject_class;
  GstElementClass *gstelement_class;
  GstRTPBaseDepayloadClass *gstrtpbasedepayload_class;

  gobject_class = (GObjectClass *) klass;
  gstelement_class = (GstElementClass *) klass;
  gstrtpbasedepayload_class = (GstRTPBaseDepayloadClass *) klass;

  gobject_class->finalize = gst_rtp_mp4a_depay_finalize;

  gstelement_class->change_state = gst_rtp_mp4a_depay_change_state;

  gstrtpbasedepayload_class->process_rtp_packet = gst_rtp_mp4a_depay_process;
  gstrtpbasedepayload_class->set_caps = gst_rtp_mp4a_depay_setcaps;

  gst_element_class_add_static_pad_template (gstelement_class,
      &gst_rtp_mp4a_depay_src_template);
  gst_element_class_add_static_pad_template (gstelement_class,
      &gst_rtp_mp4a_depay_sink_template);

  gst_element_class_set_static_metadata (gstelement_class,
      "RTP MPEG4 audio depayloader", "Codec/Depayloader/Network/RTP",
      "Extracts MPEG4 audio from RTP packets (RFC 3016)",
      "Nokia Corporation (contact <stefan.kost@nokia.com>), "
      "Wim Taymans <wim.taymans@gmail.com>");

  GST_DEBUG_CATEGORY_INIT (rtpmp4adepay_debug, "rtpmp4adepay", 0,
      "MPEG4 audio RTP Depayloader");
}

static void
gst_rtp_mp4a_depay_init (GstRtpMP4ADepay * rtpmp4adepay)
{
  rtpmp4adepay->adapter = gst_adapter_new ();
}

static void
gst_rtp_mp4a_depay_finalize (GObject * object)
{
  GstRtpMP4ADepay *rtpmp4adepay;

  rtpmp4adepay = GST_RTP_MP4A_DEPAY (object);

  g_object_unref (rtpmp4adepay->adapter);
  rtpmp4adepay->adapter = NULL;

  G_OBJECT_CLASS (parent_class)->finalize (object);
}

static const guint aac_sample_rates[] = { 96000, 88200, 64000, 48000,
  44100, 32000, 24000, 22050, 16000, 12000, 11025, 8000, 7350
};


struct _GstAudioMuxElement
{
  guint8 audio_mux_version;
  guint8 audio_mux_version_a;
  guint32 tara_buffer_fullness;
  guint8 all_streams_same_time_framing;

  // contains only data for the first layer of the first program of the first frame

  guint16 audio_object_type;
  guint8 sampling_frequency_index;
  guint32 sampling_frequency;
  guint8 channel_configuration;

  guint8 frame_length_type;
  guint8 latm_buffer_fullness;

  guint32 other_data_length;
  guint8 crc;

  guint32 payload_pos;
  guint32 payload_length;
};

static void
gst_audio_mux_element_print (GstAudioMuxElement * element)
{
  GST_INFO
      ("StreamMuxConfig version=%u versionA=%u sameTime=%u taraBufferF=%u objectType=%u freqIdx=%u freq=%u channelConf=%u frameLenType=%u otherLen=%u payloadPos=%u payloadLen=%u",
      element->audio_mux_version, element->audio_mux_version_a,
      element->all_streams_same_time_framing, element->tara_buffer_fullness,
      element->audio_object_type, element->sampling_frequency_index,
      element->sampling_frequency, element->channel_configuration,
      element->frame_length_type, element->other_data_length,
      element->payload_pos, element->payload_length);
}

static guint32
gst_audio_mux_element_latm_get_value (GstBitReader * br)
{
  guint8 bytes_for_value;
  guint8 i;
  guint8 tmp;
  guint32 value = 0;

  gst_bit_reader_get_bits_uint8 (br, &bytes_for_value, 2);
  for (i = 0; i <= bytes_for_value; ++i) {
    value <<= 8;
    gst_bit_reader_get_bits_uint8 (br, &tmp, 8);
    value += tmp;
  }

  return value;
}

static guint32
gst_audio_mux_element_audio_specific_config (GstAudioMuxElement * element,
    GstBitReader * br)
{
  guint32 bit_start_pos = gst_bit_reader_get_pos (br);

  gst_bit_reader_get_bits_uint16 (br, &element->audio_object_type, 5);
  if (element->audio_object_type == 31) {
    gst_bit_reader_get_bits_uint16 (br, &element->audio_object_type, 6);
    element->audio_object_type += 32;
  }

  gst_bit_reader_get_bits_uint8 (br, &element->sampling_frequency_index, 4);
  if (element->sampling_frequency_index == 0xF) {
    gst_bit_reader_get_bits_uint32 (br, &element->sampling_frequency, 24);
  }

  gst_bit_reader_get_bits_uint8 (br, &element->channel_configuration, 4);

  // other bits of audio specific config are skipped

  return gst_bit_reader_get_pos (br) - bit_start_pos;
}

static gboolean
gst_audio_mux_element_init (GstAudioMuxElement * element, guint8 * data,
    gsize size)
{
  GstBitReader br;
  guint8 tmp8;
  guint32 tmp32;

  gst_bit_reader_init (&br, data, size);

  // check useSameStreamMux
  gst_bit_reader_get_bits_uint8 (&br, &tmp8, 1);
  if (!tmp8) {
    gst_bit_reader_get_bits_uint8 (&br, &element->audio_mux_version, 1);
    if (element->audio_mux_version == 1) {
      gst_bit_reader_get_bits_uint8 (&br, &element->audio_mux_version_a, 1);
    } else {
      element->audio_mux_version_a = 0;
    }

    if (element->audio_mux_version_a == 0) {
      if (element->audio_mux_version == 1) {
        element->tara_buffer_fullness =
            gst_audio_mux_element_latm_get_value (&br);
      }

      gst_bit_reader_get_bits_uint8 (&br,
          &element->all_streams_same_time_framing, 1);

      // check numSubFrames == 0
      gst_bit_reader_get_bits_uint8 (&br, &tmp8, 6);
      if (tmp8 != 0) {
        GST_ERROR ("Unsupported AudioMuxElement numSubFrames=%u", tmp8);
        return FALSE;
      }
      // check numProgram == 0
      gst_bit_reader_get_bits_uint8 (&br, &tmp8, 4);
      if (tmp8 != 0) {
        GST_ERROR ("Unsuported AudioMuxElement numProgram=%u", tmp8);
        return FALSE;
      }
      // check numLayer == 0
      gst_bit_reader_get_bits_uint8 (&br, &tmp8, 3);
      if (tmp8 != 0) {
        GST_ERROR ("Unsupported AudioMuxElement numLayer=%u", tmp8);
        return FALSE;
      }

      if (element->audio_mux_version == 0) {
        gst_audio_mux_element_audio_specific_config (element, &br);
      } else {
        // tmp32 = ascLen
        tmp32 = gst_audio_mux_element_latm_get_value (&br);
        tmp32 -= gst_audio_mux_element_audio_specific_config (element, &br);
        gst_bit_reader_skip (&br, tmp32);
      }

      gst_bit_reader_get_bits_uint8 (&br, &element->frame_length_type, 3);

      gst_bit_reader_get_bits_uint8 (&br, &element->latm_buffer_fullness, 8);

      // other data present?
      gst_bit_reader_get_bits_uint8 (&br, &tmp8, 1);
      if (tmp8) {
        if (element->audio_mux_version == 1) {
          element->other_data_length =
              gst_audio_mux_element_latm_get_value (&br);
        } else {
          GST_ERROR
              ("Unsupported AudioMuxElement otherDataPresent=1, audioMuxVersion!=1");
        }
      }
      // crc check
      gst_bit_reader_get_bits_uint8 (&br, &tmp8, 1);
      if (tmp8) {
        gst_bit_reader_get_bits_uint8 (&br, &element->crc, 8);
      }
    }
  }

  if (element->all_streams_same_time_framing) {
    if (element->frame_length_type == 0) {
      element->payload_length = 0;
      do {
        gst_bit_reader_get_bits_uint8 (&br, &tmp8, 8);
        element->payload_length += tmp8;
      } while (tmp8 == 255);
    } else {
      GST_ERROR ("Unsupported AudioMuxElement: frameLengthType!=0");
    }
  } else {
    GST_ERROR ("Unsupported AudioMuxElement: allStreamsSameTimeFraming==0");
    return FALSE;
  }

  guint curPos = gst_bit_reader_get_pos (&br);

  // payload starts byte-aligned
  element->payload_pos = curPos / 8;
  if (curPos % 8 != 0) {
    ++curPos;
  }

  GST_DEBUG ("Bits read: %u", curPos);

  return TRUE;
}


static gboolean
gst_rtp_mp4a_depay_setcaps (GstRTPBaseDepayload * depayload, GstCaps * caps)
{
  GstStructure *structure;
  GstRtpMP4ADepay *rtpmp4adepay;
  GstCaps *srccaps;
  const gchar *str;
  const gchar *base_profile;
  gint clock_rate;
  gint object_type;
  gint channels;
  gboolean res;
  gint mpegversion;

  rtpmp4adepay = GST_RTP_MP4A_DEPAY (depayload);

  structure = gst_caps_get_structure (caps, 0);

  if (!gst_structure_get_int (structure, "clock-rate", &clock_rate))
    clock_rate = 90000;         /* default */
  depayload->clock_rate = clock_rate;

  if (!gst_structure_get_int (structure, "object", &object_type))
    object_type = 2;            /* AAC LC default */

  if (!gst_structure_get_int (structure, "mpegversion", &mpegversion))
    mpegversion = 4;            /* default */

  if (!gst_structure_get_int (structure, "channels", &channels))
    channels = 2;               /* default */

  if (!gst_structure_get_boolean (structure, "cpresent",
          &rtpmp4adepay->inband_config))
    rtpmp4adepay->inband_config = TRUE;

  srccaps = gst_caps_new_simple ("audio/mpeg",
      "mpegversion", G_TYPE_INT, mpegversion,
      "framed", G_TYPE_BOOLEAN, TRUE,
      "channels", G_TYPE_INT, channels,
      "stream-format", G_TYPE_STRING, "raw",
      "rate", G_TYPE_INT, clock_rate, NULL);

  if ((base_profile = gst_structure_get_string (structure, "base-profile"))) {
    gst_caps_set_simple (srccaps, "base-profile", G_TYPE_STRING, base_profile,
        NULL);
  }

  if ((str = gst_structure_get_string (structure, "config"))) {
    GValue v = { 0 };

    g_value_init (&v, GST_TYPE_BUFFER);
    if (gst_value_deserialize (&v, str)) {
      GstBuffer *buffer;
      GstMapInfo map;
      guint8 *data;
      gsize size;
      gint i;
      guint32 rate = 0;
      guint8 obj_type = 0, sr_idx = 0, channels = 0;
      GstBitReader br;

      buffer = gst_value_get_buffer (&v);
      gst_buffer_ref (buffer);
      g_value_unset (&v);

      gst_buffer_map (buffer, &map, GST_MAP_READ);
      data = map.data;
      size = map.size;

      if (size < 2) {
        GST_WARNING_OBJECT (depayload, "config too short (%d < 2)",
            (gint) size);
        goto bad_config;
      }

      /* Parse StreamMuxConfig according to ISO/IEC 14496-3:
       *
       * audioMuxVersion           == 0 (1 bit)
       * allStreamsSameTimeFraming == 1 (1 bit)
       * numSubFrames              == rtpmp4adepay->numSubFrames (6 bits)
       * numProgram                == 0 (4 bits)
       * numLayer                  == 0 (3 bits)
       *
       * We only require audioMuxVersion == 0;
       *
       * The remaining bit of the second byte and the rest of the bits are used
       * for audioSpecificConfig which we need to set in codec_info.
       */
      if ((data[0] & 0x80) != 0x00) {
        GST_WARNING_OBJECT (depayload, "unknown audioMuxVersion 1");
        goto bad_config;
      }

      rtpmp4adepay->numSubFrames = (data[0] & 0x3F);

      GST_LOG_OBJECT (rtpmp4adepay, "numSubFrames %d",
          rtpmp4adepay->numSubFrames);

      /* shift rest of string 15 bits down */
      size -= 2;
      for (i = 0; i < size; i++) {
        data[i] = ((data[i + 1] & 1) << 7) | ((data[i + 2] & 0xfe) >> 1);
      }

      gst_bit_reader_init (&br, data, size);

      /* any object type is fine, we need to copy it to the profile-level-id field. */
      if (!gst_bit_reader_get_bits_uint8 (&br, &obj_type, 5))
        goto bad_config;
      if (obj_type == 0) {
        GST_WARNING_OBJECT (depayload, "invalid object type 0");
        goto bad_config;
      }

      if (!gst_bit_reader_get_bits_uint8 (&br, &sr_idx, 4))
        goto bad_config;
      if (sr_idx >= G_N_ELEMENTS (aac_sample_rates) && sr_idx != 15) {
        GST_WARNING_OBJECT (depayload, "invalid sample rate index %d", sr_idx);
        goto bad_config;
      }
      GST_LOG_OBJECT (rtpmp4adepay, "sample rate index %u", sr_idx);

      if (!gst_bit_reader_get_bits_uint8 (&br, &channels, 4))
        goto bad_config;
      if (channels > 7) {
        GST_WARNING_OBJECT (depayload, "invalid channels %u", (guint) channels);
        goto bad_config;
      }

      /* rtp rate depends on sampling rate of the audio */
      if (sr_idx == 15) {
        /* index of 15 means we get the rate in the next 24 bits */
        if (!gst_bit_reader_get_bits_uint32 (&br, &rate, 24))
          goto bad_config;
      } else if (sr_idx >= G_N_ELEMENTS (aac_sample_rates)) {
        goto bad_config;
      } else {
        /* else use the rate from the table */
        rate = aac_sample_rates[sr_idx];
      }

      rtpmp4adepay->frame_len = 1024;

      switch (obj_type) {
        case 1:
        case 2:
        case 3:
        case 4:
        case 6:
        case 7:
        {
          guint8 frameLenFlag = 0;

          if (gst_bit_reader_get_bits_uint8 (&br, &frameLenFlag, 1))
            if (frameLenFlag)
              rtpmp4adepay->frame_len = 960;
          break;
        }
        default:
          break;
      }

      /* ignore remaining bit, we're only interested in full bytes */
      gst_buffer_resize (buffer, 0, size);
      gst_buffer_unmap (buffer, &map);
      data = NULL;

      gst_caps_set_simple (srccaps,
          "channels", G_TYPE_INT, (gint) channels,
          "rate", G_TYPE_INT, (gint) rate,
          "codec_data", GST_TYPE_BUFFER, buffer, NULL);
    bad_config:
      if (data)
        gst_buffer_unmap (buffer, &map);
      gst_buffer_unref (buffer);
    } else {
      g_warning ("cannot convert config to buffer");
    }
  }
  res = gst_pad_set_caps (depayload->srcpad, srccaps);
  gst_caps_unref (srccaps);

  return res;
}

static GstBuffer *
gst_rtp_mp4a_depay_process (GstRTPBaseDepayload * depayload, GstRTPBuffer * rtp)
{
  GstRtpMP4ADepay *rtpmp4adepay;
  GstBuffer *outbuf;
  GstMapInfo map;

  rtpmp4adepay = GST_RTP_MP4A_DEPAY (depayload);

  /* flush remaining data on discont */
  if (GST_BUFFER_IS_DISCONT (rtp->buffer)) {
    gst_adapter_clear (rtpmp4adepay->adapter);
  }

  outbuf = gst_rtp_buffer_get_payload_buffer (rtp);
  outbuf = gst_buffer_make_writable (outbuf);
  GST_BUFFER_PTS (outbuf) = GST_BUFFER_PTS (rtp->buffer);

  if (rtpmp4adepay->inband_config) {
    gst_buffer_map (outbuf, &map, GST_MAP_READ);
    GstAudioMuxElement element = { };
    gst_audio_mux_element_init (&element, map.data, map.size);
    gst_audio_mux_element_print (&element);
    gst_buffer_unmap (outbuf, &map);

    GstBuffer *dataBuf =
        gst_buffer_copy_region (outbuf, GST_BUFFER_COPY_ALL,
        element.payload_pos, element.payload_length);
        
    gst_buffer_unref(outbuf);

    return dataBuf;
  }


  gst_adapter_push (rtpmp4adepay->adapter, outbuf);

  /* RTP marker bit indicates the last packet of the AudioMuxElement => create
   * and push a buffer */
  if (gst_rtp_buffer_get_marker (rtp)) {
    guint avail;
    guint i;
    guint8 *data;
    guint pos;
    GstClockTime timestamp;

    avail = gst_adapter_available (rtpmp4adepay->adapter);
    timestamp = gst_adapter_prev_pts (rtpmp4adepay->adapter, NULL);

    GST_LOG_OBJECT (rtpmp4adepay, "have marker and %u available", avail);

    outbuf = gst_adapter_take_buffer (rtpmp4adepay->adapter, avail);
    gst_buffer_map (outbuf, &map, GST_MAP_READ);
    data = map.data;
    /* position in data we are at */
    pos = 0;

    /* looping through the number of sub-frames in the audio payload */
    for (i = 0; i <= rtpmp4adepay->numSubFrames; i++) {
      /* determine payload length and set buffer data pointer accordingly */
      guint skip;
      guint data_len;
      GstBuffer *tmp = NULL;

      /* each subframe starts with a variable length encoding */
      data_len = 0;
      for (skip = 0; skip < avail; skip++) {
        data_len += data[skip];
        if (data[skip] != 0xff)
          break;
      }
      skip++;

      /* this can not be possible, we have not enough data or the length
       * decoding failed because we ran out of data. */
      if (skip + data_len > avail)
        goto wrong_size;

      GST_LOG_OBJECT (rtpmp4adepay,
          "subframe %u, header len %u, data len %u, left %u", i, skip, data_len,
          avail);

      /* take data out, skip the header */
      pos += skip;
      tmp = gst_buffer_copy_region (outbuf, GST_BUFFER_COPY_ALL, pos, data_len);

      /* skip data too */
      skip += data_len;
      pos += data_len;

      /* update our pointers whith what we consumed */
      data += skip;
      avail -= skip;

      GST_BUFFER_PTS (tmp) = timestamp;
      gst_rtp_drop_non_audio_meta (depayload, tmp);
      gst_rtp_base_depayload_push (depayload, tmp);

      /* shift ts for next buffers */
      if (rtpmp4adepay->frame_len && timestamp != -1
          && depayload->clock_rate != 0) {
        timestamp +=
            gst_util_uint64_scale_int (rtpmp4adepay->frame_len, GST_SECOND,
            depayload->clock_rate);
      }
    }

    /* just a check that lengths match */
    if (avail) {
      GST_ELEMENT_WARNING (depayload, STREAM, DECODE,
          ("Packet invalid"), ("Not all payload consumed: "
              "possible wrongly encoded packet."));
    }
    gst_buffer_unmap (outbuf, &map);
    gst_buffer_unref (outbuf);
  }
  return NULL;

  /* ERRORS */
wrong_size:
  {
    GST_ELEMENT_WARNING (rtpmp4adepay, STREAM, DECODE,
        ("Packet did not validate"), ("wrong packet size"));
    gst_buffer_unmap (outbuf, &map);
    gst_buffer_unref (outbuf);
    return NULL;
  }
}

static GstStateChangeReturn
gst_rtp_mp4a_depay_change_state (GstElement * element,
    GstStateChange transition)
{
  GstRtpMP4ADepay *rtpmp4adepay;
  GstStateChangeReturn ret;

  rtpmp4adepay = GST_RTP_MP4A_DEPAY (element);

  switch (transition) {
    case GST_STATE_CHANGE_READY_TO_PAUSED:
      gst_adapter_clear (rtpmp4adepay->adapter);
      rtpmp4adepay->frame_len = 0;
      rtpmp4adepay->numSubFrames = 0;
      break;
    default:
      break;
  }

  ret = GST_ELEMENT_CLASS (parent_class)->change_state (element, transition);

  switch (transition) {
    default:
      break;
  }
  return ret;
}

gboolean
gst_rtp_mp4a_depay_plugin_init (GstPlugin * plugin)
{
  return gst_element_register (plugin, "rtpmp4adepay",
      GST_RANK_SECONDARY, GST_TYPE_RTP_MP4A_DEPAY);
}