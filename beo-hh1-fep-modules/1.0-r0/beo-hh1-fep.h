/*
 * Copyright (C) 2013 StreamUnlimited
 *
 * License Terms: GNU General Public License v2
 * Author: Vladimir Koutny <vladimir.koutny@streamunlimited.com>
 */

#ifndef __BEO_HH1_FEP_H__
#define __BEO_HH1_FEP_H__

#include <linux/workqueue.h>

// #define DEBUG_VERBOSE

// public API for cells:

typedef int (*BEO_HH1_EVENT_CB)(void *priv, u16 event, u16 data);
typedef int (*BEO_HH1_LONGEVENT_CB)(void *priv, u16 event, u32 size, u8 *data);

// forward declaration
struct platform_device;

struct beo_hh1_fep_core {
	// FEP driver private data, do not touch
	void *fep_priv;

	// public API
	int (*send)(struct beo_hh1_fep_core *fepcore, size_t len, u8 *data, size_t outlen, u8* outdata);
	int (*register_events)(const struct platform_device *pdev, u16 events_base, u16 events_size, void *priv, BEO_HH1_EVENT_CB cb);
	int (*register_long_events)(const struct platform_device *pdev, u16 events_base, u16 events_size, void *priv, BEO_HH1_LONGEVENT_CB cb);

	// public API helpers
	u16 (*crc16)(const u8 *data, u16 size);
};


#define BEO_HH1_WPL_FWUPDATE_CHUNK		128
#define BEO_ASE_NOTIF_SOUND_MAX		8
#define DSP_VERSION_LEN		10

#define BEO_HH1_REP_NOP		0x0000

#define BEO_HH1_REP_ACK		0x8001
#define BEO_HH1_REP_NACK	0x8002
#define BEO_HH1_REP_BUSY	0x80ff


#define BEO_HH1_CMD_OUTLEN_FLAG	0x8000

// commands:
#define BEO_HH1_CMD_PING		(0x0001 | BEO_HH1_CMD_OUTLEN_FLAG)
#define BEO_HH1_CMD_IS_APP		(0x0002 | BEO_HH1_CMD_OUTLEN_FLAG)
#define BEO_HH1_CMD_DATA_FETCH	(0x0003 | BEO_HH1_CMD_OUTLEN_FLAG)
#define BEO_HH1_CMD_IRQ_QUERY	(0x0011 | BEO_HH1_CMD_OUTLEN_FLAG)

#define BEO_HH1_CMD_LEDS		(0x0020 | BEO_HH1_CMD_OUTLEN_FLAG)
#define BEO_HH1_CMD_BUTTONS		(0x0030 | BEO_HH1_CMD_OUTLEN_FLAG)
#define BEO_HH1_CMD_LINESENSE		(0x0031 | BEO_HH1_CMD_OUTLEN_FLAG)

#define BEO_HH1_CMD_FWUP_START	(0x0040 | BEO_HH1_CMD_OUTLEN_FLAG)
#define BEO_HH1_CMD_FWUP_DATA	(0x0041 | BEO_HH1_CMD_OUTLEN_FLAG)

#define BEO_HH1_CMD_SETRTC		(0x0050 | BEO_HH1_CMD_OUTLEN_FLAG)
#define BEO_HH1_CMD_GETRTC		(0x0051 | BEO_HH1_CMD_OUTLEN_FLAG)

#define BEO_HH1_CMD_PL_ON		(0x0060 | BEO_HH1_CMD_OUTLEN_FLAG)
#define BEO_HH1_CMD_PL_MUTE		(0x0061 | BEO_HH1_CMD_OUTLEN_FLAG)
#define BEO_HH1_CMD_PL_DATA		(0x0062 | BEO_HH1_CMD_OUTLEN_FLAG)

#define BEO_HH1_CMD_WPL			(0x0065 | BEO_HH1_CMD_OUTLEN_FLAG)
#define BEO_HH1_CMD_WPLFWUPDATE	(0x0066 | BEO_HH1_CMD_OUTLEN_FLAG)

#define BEO_HH1_CMD_POWERSTATE	(0x0070 | BEO_HH1_CMD_OUTLEN_FLAG)
#define BEO_HH1_CMD_POWERSUPPLY	(0x0071 | BEO_HH1_CMD_OUTLEN_FLAG)

#define BEO_ASE_CMD_CRITICALERR	(0x0078 | BEO_HH1_CMD_OUTLEN_FLAG)

#define BEO_HH1_CMD_GETAPPVER	(0x0080 | BEO_HH1_CMD_OUTLEN_FLAG)
#define BEO_HH1_CMD_GETBOOTLVER	(0x0081 | BEO_HH1_CMD_OUTLEN_FLAG)
#define BEO_HH1_CMD_GETHWVER	(0x0082 | BEO_HH1_CMD_OUTLEN_FLAG)
#define BEO_HH1_CMD_GETLOPWRTM	(0x0083 | BEO_HH1_CMD_OUTLEN_FLAG)

#define BEO_HH1_CMD_GETBOOTREASON	(0x0088 | BEO_HH1_CMD_OUTLEN_FLAG)
#define BEO_ASE_CMD_FEP_SHELL_CMD	(0x0090| BEO_HH1_CMD_OUTLEN_FLAG)

#define BEO_ASE_CMD_VOLUME		(0x0101 | BEO_HH1_CMD_OUTLEN_FLAG)
#define BEO_ASE_CMD_SOURCE		(0x0102 | BEO_HH1_CMD_OUTLEN_FLAG)
#define BEO_ASE_CMD_NOTIF_SOUND	(0x0103 | BEO_HH1_CMD_OUTLEN_FLAG)
#define BEO_ASE_CMD_TUNNEL		(0x0104 | BEO_HH1_CMD_OUTLEN_FLAG)
#define BEO_ASE_CMD_MUTE		(0x0105 | BEO_HH1_CMD_OUTLEN_FLAG)
#define BEO_ASE_CMD_SPEAKER_POSITION	(0x0106 | BEO_HH1_CMD_OUTLEN_FLAG)

#define BEO_ASE_CMD_BASS		(0x0108 | BEO_HH1_CMD_OUTLEN_FLAG)
#define BEO_ASE_CMD_TREBLE		(0x0109 | BEO_HH1_CMD_OUTLEN_FLAG)
#define BEO_ASE_CMD_AUTOLOUDNESS	(0x010A | BEO_HH1_CMD_OUTLEN_FLAG)

#define BEO_ASE_CMD_INIT		(0x0110 | BEO_HH1_CMD_OUTLEN_FLAG)
#define BEO_ASE_CMD_DEINIT		(0x0111 | BEO_HH1_CMD_OUTLEN_FLAG)
#define BEO_ASE_CMD_VERSION		(0x0112 | BEO_HH1_CMD_OUTLEN_FLAG)

#define BEO_ASE_CMD_LAYOUT_GET	(0x0120 | BEO_HH1_CMD_OUTLEN_FLAG)
#define BEO_ASE_CMD_LAYOUT_SET	(0x0121 | BEO_HH1_CMD_OUTLEN_FLAG)
#define BEO_ASE_CMD_FILTER_GET	(0x0122 | BEO_HH1_CMD_OUTLEN_FLAG)
#define BEO_ASE_CMD_FILTER_SET	(0x0123 | BEO_HH1_CMD_OUTLEN_FLAG)
#define BEO_ASE_CMD_GAIN_GET	(0x0124 | BEO_HH1_CMD_OUTLEN_FLAG)
#define BEO_ASE_CMD_GAIN_SET	(0x0125 | BEO_HH1_CMD_OUTLEN_FLAG)

#define BEO_ASE_CMD_AUTO_CALIBRATE	(0x0126 | BEO_HH1_CMD_OUTLEN_FLAG)
#define BEO_ASE_CMD_NTC_VALUES	(0x0127 | BEO_HH1_CMD_OUTLEN_FLAG)
#define BEO_ASE_CMD_RESET_COUNTERS	(0x0128 | BEO_HH1_CMD_OUTLEN_FLAG)
#define BEO_ASE_CMD_ERRLOG_GET	(0x0129 | BEO_HH1_CMD_OUTLEN_FLAG)
#define BEO_ASE_CMD_ERRLOG_CLR	(0x012A | BEO_HH1_CMD_OUTLEN_FLAG)

#define BEO_ASE_CMD_ENABLE_TEST_SIGNAL	(0x0130 | BEO_HH1_CMD_OUTLEN_FLAG)
#define BEO_ASE_CMD_DISABLE_TEST_SIGNAL	(0x0131 | BEO_HH1_CMD_OUTLEN_FLAG)
#define BEO_ASE_CMD_SET_AUDIO_SWITCH	(0x0132 | BEO_HH1_CMD_OUTLEN_FLAG)	//internal speaker route
#define BEO_ASE_CMD_CONF_SIN_GEN		(0x0133 | BEO_HH1_CMD_OUTLEN_FLAG)
#define BEO_ASE_CMD_CONF_MT_SIN_GEN		(0x0134 | BEO_HH1_CMD_OUTLEN_FLAG)
#define BEO_ASE_CMD_CONF_SIGNAL			(0x0135 | BEO_HH1_CMD_OUTLEN_FLAG)
#define BEO_ASE_CMD_GET_POSITION		(0x0136 | BEO_HH1_CMD_OUTLEN_FLAG)
#define BEO_ASE_CMD_CONF_AUDIO_SWITCH		(0x0137 | BEO_HH1_CMD_OUTLEN_FLAG)
#define BEO_ASE_CMD_SET_AUDIO_STATE		(0x0138 | BEO_HH1_CMD_OUTLEN_FLAG)

#define BEO_S47_CMD_TOUCH_CALIBRATE_ZERO        (0x0160 | BEO_HH1_CMD_OUTLEN_FLAG)
#define BEO_S47_CMD_TOUCH_CALIBRATE_WITHLOAD    (0x0161 | BEO_HH1_CMD_OUTLEN_FLAG)
#define BEO_S47_CMD_TOUCH_GET_CALIBRATE_DATA    (0x0162 | BEO_HH1_CMD_OUTLEN_FLAG)
#define BEO_S47_CMD_TOUCH_GET_DATA              (0x0163 | BEO_HH1_CMD_OUTLEN_FLAG)

#define BEO_S47_CMD_LIGHT_SENSOR_CALIBRATE          (0x0168 | BEO_HH1_CMD_OUTLEN_FLAG)
#define BEO_S47_CMD_LIGHT_SENSOR_CALIBRATE_GET_DATA (0x0169 | BEO_HH1_CMD_OUTLEN_FLAG)
#define BEO_S47_CMD_LIGHT_SENSOR_GET_DATA           (0x016A | BEO_HH1_CMD_OUTLEN_FLAG)

#define BEO_ASE_CMD_TOUCHRAW	(0x0180 | BEO_HH1_CMD_OUTLEN_FLAG)
#define BEO_ASE_CMD_TOUCHAVER	(0x0181 | BEO_HH1_CMD_OUTLEN_FLAG)
#define BEO_ASE_CMD_TOUCHLEVEL	(0x0182 | BEO_HH1_CMD_OUTLEN_FLAG)
#define BEO_ASE_CMD_TOUCHSYNC	(0x0183 | BEO_HH1_CMD_OUTLEN_FLAG)

#define BEO_ASE_CMD_AMPS_SHUTDOWN	(0x0140 | BEO_HH1_CMD_OUTLEN_FLAG)
#define BEO_ASE_CMD_AMPS_RESTORE	(0x0141 | BEO_HH1_CMD_OUTLEN_FLAG)

#define BEO_ASE_CMD_TOSLINKCTRL         (0x0148 | BEO_HH1_CMD_OUTLEN_FLAG)


#define BEO_ASE_S47_CMD_DISPLAY_DATA	(0x0150 | BEO_HH1_CMD_OUTLEN_FLAG)
#define BEO_ASE_S47_CMD_DISPLAY_DATA_EXTENSION	(0x0151 | BEO_HH1_CMD_OUTLEN_FLAG)
#define BEO_ASE_S47_CMD_DISPLAY_TEST_MODE	(0x0152 | BEO_HH1_CMD_OUTLEN_FLAG)
#define BEO_ASE_S47_CMD_DISPLAY_TEST_PATTERN (0x0153 | BEO_HH1_CMD_OUTLEN_FLAG)
#define BEO_ASE_S47_CMD_DISPLAY_TEST_BRIGHTNESS (0x0154 | BEO_HH1_CMD_OUTLEN_FLAG)
#define BEO_ASE_S47_CMD_DISPLAY_COMMAND         (0x0155 | BEO_HH1_CMD_OUTLEN_FLAG)

// events:
#define BEO_HH1_EV_FWUP_READY	0x1000
#define BEO_HH1_EV_FWUP_ACK		0x1001
#define BEO_HH1_EV_FWUP_NACK	0x1007
#define BEO_HH1_EV_FWUP_CRCNACK	0x1008
#define BEO_HH1_EV_APP_READY	0x100f
#define BEO_HH1_EV_WPL_ACK		0x1010
#define BEO_HH1_EV_WPL_NACK  0x1011
#define BEO_HH1_EV_WPLFW_ACK 0x1014
#define BEO_HH1_EV_WPLFW_NACK 0x1015
#define BEO_HH1_EV_WPLFW_READY  0x1016
#define BEO_ASE_EV_HW_OVERHEAT	0x1020
#define BEO_ASE_EV_PL_ON	0x1021
#define BEO_ASE_EV_PL_OFF	0x1022
#define BEO_ASE_EV_DSP_ERRLEVEL	0x1023
#define BEO_ASE_EV_TOSLINK_UP   0x1048
#define BEO_ASE_EV_TOSLINK_DOWN 0x1049

//watchdog tick event
#define BEO_EV_WDT_TICK		0x10ff

// longid events:
#define BEO_HH1_EV_LONG_MASK	0x8000

#define BEO_HH1_EV_DATA_FETCH	0x80ff
#define BEO_HH1_EV_RTC			0x8000
#define BEO_HH1_EV_CONSOLE		0x8001
#define BEO_HH1_EV_IR			0x8002
#define BEO_HH1_EV_WPL_FWVER 0x8003
#define BEO_HH1_EV_WPL_APIVER 0x8004
#define BEO_HH1_EV_WPL_DFSREV 0x8005
#define BEO_HH1_EV_WPL_STATE	0x8006
#define BEO_HH1_EV_IRRELEASETIME	0x8007

// data fetch id:
#define BEO_HH1_EV_WPL_SPKDESC	0x8010
#define BEO_HH1_EV_WPL_SPKSTATE 0x8011
#define BEO_HH1_EV_WPL_MODINFO 0x8013

#define BEO_HH1_EV_WPL_SPKMAP	0x8014
#define BEO_HH1_EV_WPL_FWDATA	0x8020
#define BEO_HH1_EV_WPL_RAWCMD	0x8021

#define _BEO_ASE_EV_DSP_BEGIN	0x8022
#define BEO_ASE_EV_DSP			0x8022
#define BEO_ASE_EV_DSP_VERSION	0x8025
#define BEO_ASE_EV_DSP_AUTOCALIBRATE 0x8026
#define BEO_ASE_EV_DSP_LAYOUT	0x8030
#define BEO_ASE_EV_DSP_FILTER	0x8031
#define BEO_ASE_EV_DSP_GAIN		0x8032
#define BEO_ASE_EV_DSP_TUNNEL	0x8033
#define BEO_ASE_EV_DSP_NTC_VALUES	0x8034
#define BEO_ASE_EV_DSP_ERRLOG	0x8035
#define BEO_ASE_EV_DSP_POSITION 0x8036
#define _BEO_ASE_EV_DSP_END		0x8036

#define BEO_ASE_EV_DSP_TOUCHSYNC	0x8040

#define BEO_S47_EV_TOUCH_CALIBRATE_STATUS   0x8048
#define BEO_S47_EV_TOUCH_CALIBRATION_DATA   0x8049
#define BEO_S47_EV_TOUCH_SENSOR_VALUE       0x804a

#define BEO_S47_EV_LIGHT_SENSOR_CALIBRATE_STATUS    0x804c
#define BEO_S47_EV_LIGHT_SENSORS_CALIBRATION_DATA   0x804d
#define BEO_S47_EV_LIGHT_SENSORS_DATA               0x804e

/*
API, v1:

- commands from S800
  - command, variable size message
  - reply must immediately follow, ack/nack/busy +-
    - due to LPC SPI controller issues the reply can have several dummy bytes prepended, ie:
      - these extra bytes (currently 16) are NOT included in outlen
      - LPC sends back 0x00 prepended while it is processing the command (max 16 bytes)
      - S800 searches for first non-0x00 byte in the reply, discards these leading 0x00s
      - the format is then:
          cs:   ^^^^_____________________________________________________________________________________^^^^
          clk:       xx xx xx xx xx xx xx xx xx xx xx xx xx xx xx xx xx xx xx xx xx xx xx xx xx xx xx xx
          mosi:      cc cc cc cc cc cc cc cc -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
          miso:      -- -- -- -- -- -- -- -- 00 00 00 rr rr rr rr 00 00 00 00 00 00 00 00 00 00 00 00 00


  - if FEP is busy -> reply with busy response

- events from FEP
  - FEP buffers events as they arrive, wrap-around buffer (size=8/16?)
  - FEP toggles IRQ line to S800
  - S800 sends CMD_IRQ_QUERY command (indicates rx buffer size)
  - FEP clears the IRQ line if all events can be sent
  - FEP responds with CMD_IRQ_DATA command + buffered events


- commands:
  - ping (reply:ack)
  - set multiple LEDs (id, 0/1/pwm, id, 0/1/pwm, ...; reply:ack)
  - read buttons state (reply: id, state, id, state, ...)

  - FEP update process:
    - FEP fw update start (reply: ack/nack)
    - wait for fwup_ready event
    - FEP fw update data (offset, data; reply:ack/nack/...)
    - wait for fwup_ack event
    - repeat last 2 steps until finished
    - wait for app_ready event (app is running)

  - SummitTx API
    - send command
    - ? get event
    - ? fw update

  - ? power management


- events:
  - input event (id, state, id, state, ..)
  - ? summit event (s800 has to poll with summit_query after this event)

  event IDs:
    - nop: 0x0000
    - buttons: 0x0001 - 0x01ff; value: 0/1; volume up/down value: -x..+x
    - IR: 0x0200 - 0x03ff; value: 0/1/? repeat count
    - fw update: 0x1000 - 0x100f
       - 0x1000 = bootloaderready for image
       - 0x1001 = block written (data=seq)
       - ...
    - summit: 0x2000 - 0x200f
    - ...

    - long-events: 0x8000 - 0xffff:
        - rtc: 0x8000


- message formats:
  - command:
    u16 cmd (highest bit = outlen/padding included)
    u16 full length in bytes, incl. this header
    [
    u16 output length in bytes
    u16 padding, set to 00 00
    ]
    ... variable


    (see actual structs below; this is just for reference)

    CMD_PING: {[cmd cmd len len] [outlen outlen 0 0]} -> {[ack ack len len]}
    CMD_IS_APP: {[cmd cmd len len] [outlen outlen 0 0]} -> {[ack ack len len] | [nack nack len len]}
    CMD_LEDS: {[cmd cmd len len] [outlen outlen 0 0] [id id mode val set set set set] [id id mode val set set set set] ...} ((len-8)/8 entries) -> {[ack ack len len]}
        - on/off modes
        - blinking modes by mcu (currently configured by 32bits)
        - pwm-fading modes by mcu (not for HH1 since LEDs are behind i2c expander but for future use..)
               [id id mode val, set set set set]
                  mode: 0=on/off, 1=blink, 2=fade
                  val:
                     mode == 0 ->
                                 val == 0 -> led off
                                 val > 0  -> led on
                     mode == 1 ->
                                 val == 0 -> infinite repeate of blink
                                 val != 0 -> count of repeats
                  set:  32bit defining up to 4 on/off periods (100ms granularity)

    CMD_BUTTONS: {[cmd cmd len len] [outlen outlen 0 0]} -> {[ack ack outlen outlen] [id id val val] [id id val val] ...}
    CMD_IRQ_QUERY: {[cmd cmd len len] [outlen outlen 0 0]} -> {[ack ack outlen outlen] [evid evid val val] [evid evid val val]...}
                                                           -> {[ack ack outlen outlen] [longid longid data data data ...]}
    CMD_FWUP_START: {[cmd cmd len len] [outlen outlen 0 0] [imglen imglen imglen imglen]} -> {[ack ack len len]}
    CMD_FWUP_DATA: {[cmd cmd len len] [outlen outlen 0 0] [imglen(4b)] [ofs(4b)] [seq(2b) crc(2c)][data ...]} -> {[ack ack len len]} | ...
           -> will be followed by fw-event to retry-proceed

    CMD_SETRTC: {[cmd cmd len len] [outlen outlen 0 0] [year year month day hour minute sec]} -> {[ack ack len len]}
    CMD_GETRTC: {[cmd cmd len len] [outlen outlen 0 0] -> {[ack ack len len]}

    CMD_PL_ON:   [cmd cmd len len] [outlen outlen 0/1 0]... 1 = ON      (setgpio 1 15 1)
    CMD_PL_MUTE: [cmd cmd len len] [outlen outlen 0/1 0]... 1 = MUTE    (setgpio 1 25 0)
    CMD_PL_DATA: [cmd cmd len len] [outlen outlen ?? ??] ????           (gpio 1.19, via driver)

    CMD_POWER_STATE: {[cmd cmd len len] [outlen outlen 0 0] [n 0 0 0]} -> {[ack ack len len]}
        n:
          0 = off (fep on, all others off)   -> OFF
          1 = "Network Standby"              -> PwrState1
          2 = "Network Standby" + JB charge  -> PwrState1 + charge
          3 = "Playing wired" + JB charge    -> PwrState5
          4 = "Playing wireless" + JB charge -> PwrState5

    CMD_POWERSUPPLY: [cmd cmd len len] [outlen outlen 0 0] [supp supp state state] -> {[ack ack len len]}
        supp: power supplies mask 16bit, which power supplies change
        state: on/off mask, on/off power supplie specified by mask


  - cmd reply:
    u16 ack/nack/..
    u16 full length
    ... variable

  - events:
    u16 evid
    u16 value

*/

struct word32 {
	u8 hw_hb;
	u8 hw_lb;
	u8 lw_hb;
	u8 lw_lb;
};

struct beo_hh1_event {
	u16 id;
	u16 value;
};

// long event
struct beo_hh1_longevent {
	u16 longid;
};

struct beo_hh1_msg_hdr {
	u16 command;
	u16 len;
};

struct beo_hh1_msg_outlen {
	u16 outlen;
	u16 pad;
};

struct beo_hh1_msg_led {
	u16 id;
	u8 mode;
	u8 val;
	u32 setup;
};

struct beo_hh1_msg_lineinsetting {
	u16 sensitivity;
	u16 timeout;
	u16 sampling_period;
	u16 debounce;
};

struct beo_hh1_data_rtc {
	u16 year;
	u8 month;
	u8 day;
	u8 hour;
	u8 min;
	u8 sec;
	u8 pad; /* size = 8B */
};

struct beo_cmd_msg {
	struct beo_hh1_msg_hdr hdr;
	struct beo_hh1_msg_outlen outlen;
};

//  ping msg
#define beo_hh1_msg_ping beo_cmd_msg
//  is_app msg
#define beo_hh1_msg_is_app beo_cmd_msg
// fw versions
#define beo_hh1_msg_version beo_cmd_msg
// boot_reason msg
#define beo_hh1_msg_boot_reason beo_cmd_msg
// low_power_time msg
#define beo_hh1_msg_low_power_time beo_cmd_msg

struct beo_hh1_rsp_version {
	struct beo_hh1_msg_hdr hdr;

	u32 binaryType;      /* must be set to one of the HOSTMETA_TYPE* */
	u16 major;           /* major version */
	u16 minor;           /* minor version */
	u32 build;           /* build number */
	char buildDate[16];  /* date of the build */
	char buildTime[16];  /* time of the build */
};

// time spent in low power
struct beo_hh1_rsp_lowpowertm {
	struct beo_hh1_msg_hdr hdr;
	u32 minutes;
};

// app/bootl version response
struct beo_hh1_rsp_bootreason {
	struct beo_hh1_msg_hdr hdr;
	u32 bootReason;     /* 0: PowerFail, 1: PowerOn, 2: AfterFWUpdate, 3: FEPCrash, 4: Unknown */
};

// hw versions
struct beo_hh1_rsp_hw_version {
	struct beo_hh1_msg_hdr hdr;
	u16 revision;
	u16 pcbVariant;
	u16 productVariant;
	u16 productId;
};

// data_fetch msg
struct beo_hh1_msg_data_fetch {
	struct beo_hh1_msg_hdr hdr;
	struct beo_hh1_msg_outlen outlen;
	u16 id;
	u16 pad;
};
struct beo_hh1_rsp_data_fetch {
	struct beo_hh1_msg_hdr hdr;
	u16 id;
	u16 pad;
	u32 offset;
	u8 data[];
};
struct beo_hh1_ev_data_fetch {
	u16 id;
	u16 chunksize;
	u32 size;
};

// irq_query msg
#define beo_hh1_msg_irq_query beo_cmd_msg

// leds msg
struct beo_hh1_msg_leds {
	struct beo_hh1_msg_hdr hdr;
	struct beo_hh1_msg_outlen outlen;
	struct beo_hh1_msg_led leds[];
};

// buttons msg
#define beo_hh1_msg_buttons beo_cmd_msg

struct beo_hh1_rsp_buttons {
	struct beo_hh1_msg_hdr hdr;
	struct beo_hh1_event events[16];
};

// line in setting
struct beo_hh1_msg_lineinsense {
	struct beo_hh1_msg_hdr hdr;
	struct beo_hh1_msg_outlen outlen;
	struct beo_hh1_msg_lineinsetting setting;
};

// fwupdate msgs
struct beo_hh1_msg_fwupdate_start {
	struct beo_hh1_msg_hdr hdr;
	struct beo_hh1_msg_outlen outlen;
	u32 imglen;
};
struct beo_hh1_msg_fwupdate_data {
	struct beo_hh1_msg_hdr hdr;
	struct beo_hh1_msg_outlen outlen;
	u32 imglen;
	u32 offset;
	u16 seq;
	u16 crc;
	u8 data[256];
};

// rtc set msg
struct beo_hh1_msg_rtcset {
	struct beo_hh1_msg_hdr hdr;
	struct beo_hh1_msg_outlen outlen;
	struct beo_hh1_data_rtc time;
};

#define beo_hh1_msg_rtcget beo_cmd_msg

// pl msgs
struct beo_hh1_msg_pl_on {
	struct beo_hh1_msg_hdr hdr;
	u16 outlen;
	u8 pl_on;
	u8 pad;
};

struct beo_hh1_msg_pl_mute {
	struct beo_hh1_msg_hdr hdr;
	u16 outlen;
	u8 pl_mute;
	u8 pad;
};

// should be the same as MAX_CMD_LENGTH in ushell_task in fep
#define FEP_SHELL_MAX_CMD_SIZE 100

struct beo_hh1_msg_fep_shell_cmd {
	struct beo_hh1_msg_hdr hdr;
	struct beo_hh1_msg_outlen outlen;
	u8 cmd_str[FEP_SHELL_MAX_CMD_SIZE]; // should be 0-terminated
};

#define LSL_TELEGRAM_SIZE 12

struct beo_hh1_msg_pl_data {
	struct beo_hh1_msg_hdr hdr;
	struct beo_hh1_msg_outlen outlen;
	u16 plcmd;
	u8 data[LSL_TELEGRAM_SIZE];   // sizeof(LSL_TELEGRAM)
};

// power states
struct beo_hh1_msg_powerstate {
	struct beo_hh1_msg_hdr hdr;
	u16 outlen;
	u8 powerState;
	u8 pad;
};

struct beo_hh1_msg_pwrsuppliessetting
{
	u16 pwrSupp;		// bitmask, 1 = apply value in pwrState bitmask, 0 = don't touch
	u16 pwrState;		// bitmask
};

struct beo_hh1_msg_pwrsupplies {
	struct beo_hh1_msg_hdr hdr;
	struct beo_hh1_msg_outlen outlen;
	struct beo_hh1_msg_pwrsuppliessetting pwrSuppliesSetting;
};


// wpl msgs
struct beo_hh1_msg_wpl {
	struct beo_hh1_msg_hdr hdr;
	struct beo_hh1_msg_outlen outlen;
	u16 cmd;
	u16 param;
	u8 data[];
};

struct beo_hh1_msg_wplfwupdate {
	struct beo_hh1_msg_hdr hdr;
	struct beo_hh1_msg_outlen outlen;
	u32 imglen;
	u32 offset;
	u8 spkid;
	u8 moduleid;
	u8 partid;
	u8 pad1;
	u16 crc;
	u16 pad2;
	u8 data[BEO_HH1_WPL_FWUPDATE_CHUNK];
};

// IR release msg
struct beo_hh1_msg_ir_release {
	u32 duration;
};

// dsp msgs
struct beo_ase_dsp_cmd_response {
	u16 originCmd;
	//...
};

struct beo_ase_dsp_response {
	u16 originCmd;
	u16 pad;
	u32 error;
};

struct beo_ase_msg_volume {
	struct beo_hh1_msg_hdr hdr;
	struct beo_hh1_msg_outlen outlen;
	s16 volume;
};

struct beo_ase_msg_mute {
	struct beo_hh1_msg_hdr hdr;
	struct beo_hh1_msg_outlen outlen;
	u16 mute;
};

struct beo_ase_msg_init {
	struct beo_hh1_msg_hdr hdr;
	struct beo_hh1_msg_outlen outlen;
	u16 source;
	s16 volume;
};

#define beo_ase_msg_deinit beo_cmd_msg

struct beo_ase_msg_source {
	struct beo_hh1_msg_hdr hdr;
	struct beo_hh1_msg_outlen outlen;
	u16 source;
};

struct notif_sound_params {
	u8 sound_id;
	u8 time_delay;
};

struct beo_ase_msg_notif_sound {
	struct beo_hh1_msg_hdr hdr;
	struct beo_hh1_msg_outlen outlen;
	u16 forceInit;
	u16 count;
	struct notif_sound_params data[BEO_ASE_NOTIF_SOUND_MAX];
};

#define beo_ase_msg_version beo_cmd_msg

struct beo_ase_dsp_rsp_version {
	u16 originCmd;
	u16 pad;
	u8 values[DSP_VERSION_LEN];
	u8 success;
};

struct beo_ase_msg_tunnel_cmd {
	struct beo_hh1_msg_hdr hdr;
	struct beo_hh1_msg_outlen outlen;
	u16 count;
	u8 data[256];
};

#define beo_ase_msg_touch beo_cmd_msg

struct beo_ase_dsp_response_tunnel {
	u16 originCmd;
	u16 pad;
	u16 count;
	u8 data[256];
	u8 success;
};

#define beo_ase_msg_auto_calibrate_cmd beo_cmd_msg

struct beo_ase_dsp_rsp_autocalibrate {
	u16 originCmd;
	u16 pad;
	u16 result;
	u8 success;
};

struct beo_ase_rsp_touchraw {
    struct beo_hh1_msg_hdr hdr;
    u16 values[12];
};

struct beo_ase_msg_layout_set {
	struct beo_hh1_msg_hdr hdr;
	struct beo_hh1_msg_outlen outlen;
	u8 layout_type_id;
};

#define beo_ase_msg_layout_get beo_cmd_msg

struct beo_ase_msg_filter_set {
	struct beo_hh1_msg_hdr hdr;
	struct beo_hh1_msg_outlen outlen;
	u8 speaker_unit_type;
	struct word32 gain;
};

struct beo_ase_msg_filter_get {
	struct beo_hh1_msg_hdr hdr;
	struct beo_hh1_msg_outlen outlen;
	u8 speaker_unit_type;
};

struct beo_ase_msg_gain_set {
	struct beo_hh1_msg_hdr hdr;
	struct beo_hh1_msg_outlen outlen;
	u8 speaker_unit_type;
	struct word32 gain;
};

struct beo_ase_msg_gain_get {
	struct beo_hh1_msg_hdr hdr;
	struct beo_hh1_msg_outlen outlen;
	u8 speaker_unit_type;
};

#define beo_ase_msg_reset_counters_cmd beo_cmd_msg

struct beo_ase_msg_errlog_get_cmd {
	struct beo_hh1_msg_hdr hdr;
	struct beo_hh1_msg_outlen outlen;
	u16 range_from;
	u16 range_to;
};

struct error_code {
	u8 error_id;
	struct word32 timestamp;
};

struct beo_ase_dsp_rsp_errlog_get {
	u16 originCmd;
	u16 pad;
	u8 success;
	u16 number;
//	struct error_code data[unknown size];
};

#define beo_ase_msg_errlog_clr_cmd beo_cmd_msg

#define beo_ase_msg_ntc_values_cmd beo_cmd_msg

struct beo_ase_dsp_response_ntc_values {
	u16 originCmd;
	u16 pad;
	u8 count;
	u16 data[32];
	u8 success;
};

#define beo_ase_msg_dsp_sw_position_cmd beo_cmd_msg

struct beo_ase_ev_dsp_position {
	u16 originCmd;
	u16 pad;
	u8 position; /* 0x00 - Free, 0x01 - Wall, 0x02 - Corner */
	u8 success;
};

struct beo_ase_msg_enable_test_signal_cmd {
	struct beo_hh1_msg_hdr hdr;
	struct beo_hh1_msg_outlen outlen;
	u8 signal_type;
};

#define beo_ase_msg_disable_test_signal_cmd beo_cmd_msg

#define FREQUENCY_MASK 1
#define GAIN_MASK 2

struct sinus_generator_setting {
	u8 generator_id;
	u8 mask;
	struct word32 frequency;
	struct word32 gain;
};

struct beo_ase_msg_conf_sin_gen_cmd {
	struct beo_hh1_msg_hdr hdr;
	struct beo_hh1_msg_outlen outlen;
	struct sinus_generator_setting setting;
};

struct beo_ase_msg_conf_mt_sin_gen_cmd {
	struct beo_hh1_msg_hdr hdr;
	struct beo_hh1_msg_outlen outlen;
	u8 count;
	struct sinus_generator_setting setting[10];
};

struct beo_ase_msg_conf_signal_cmd {
	struct beo_hh1_msg_hdr hdr;
	struct beo_hh1_msg_outlen outlen;
	u8 outputAllocation;
	u8 signalType;
};

struct beo_ase_msg_set_audio_switch {
	struct beo_hh1_msg_hdr hdr;
	struct beo_hh1_msg_outlen outlen;
	u8 speaker_position;
	u8 speaker_type;
	u8 router_options;
};

struct beo_ase_cmd_conf_audio_switch_new {
	struct beo_hh1_msg_hdr hdr;
	struct beo_hh1_msg_outlen outlen;
	u8 column;
	u8 row;
	u8 connection_data;
	u8 pad;
};

struct beo_ase_cmd_set_audio_state {
	struct beo_hh1_msg_hdr hdr;
	struct beo_hh1_msg_outlen outlen;
	u8 state;	// 0 OFF, 1 ON
	u8 pad;
};

struct beo_ase_toslink_cmd {
	struct beo_hh1_msg_hdr hdr;
	struct beo_hh1_msg_outlen outlen;
	u16 mode; // 0 Disable, 1 Start Capture mode, 2 Start PIN sense mode
	u16 pad;
};

struct beo_ase_msg_bass {
	struct beo_hh1_msg_hdr hdr;
	struct beo_hh1_msg_outlen outlen;
	s32 bass;
};

struct beo_ase_msg_treble {
	struct beo_hh1_msg_hdr hdr;
	struct beo_hh1_msg_outlen outlen;
	s32 treble;
};

struct beo_ase_msg_autoloudness {
	struct beo_hh1_msg_hdr hdr;
	struct beo_hh1_msg_outlen outlen;
	u8 autoLoudness;
	u8 pad;
};

struct beo_ase_rsp_touchaver {
    struct beo_hh1_msg_hdr hdr;
    u16 values[12];
};

struct beo_ase_msg_touchlevel {
    struct beo_hh1_msg_hdr hdr;
    struct beo_hh1_msg_outlen outlen;
    u16 values[4];
};

struct beo_ase_rsp_touchlevel {
    struct beo_hh1_msg_hdr hdr;
    u16 values[4];
};

// S47 display msgs
struct beo_ase_s47_msg_display_data {
	struct beo_hh1_msg_hdr hdr;
	struct beo_hh1_msg_outlen outlen;
	u16 width;
	u16 height;
	u16 imgid;
	u8 format;
	u8 padding;	// not used
	u32 imglen;
	// offset = 0
	u8 frame_data[1280];
};

struct beo_ase_s47_msg_display_data_extension {
	struct beo_hh1_msg_hdr hdr;
	struct beo_hh1_msg_outlen outlen;
	u16 imgid;
	u16 seq;
	u32 offset;
	u8 frame_data[1280];
};

struct beo_ase_s47_msg_display_command {
	struct beo_hh1_msg_hdr hdr;
	struct beo_hh1_msg_outlen outlen;
	u32 command_id;
	u32 data_length;
	u8 command_data[];
};

struct beo_ase_s47_msg_display_test_mode {
	struct beo_hh1_msg_hdr hdr;
	struct beo_hh1_msg_outlen outlen;
	u8 enable;
};

struct beo_ase_s47_msg_display_test_pattern {
	struct beo_hh1_msg_hdr hdr;
	struct beo_hh1_msg_outlen outlen;
	u32 pattern;
};

struct beo_ase_s47_msg_display_test_brightness {
	struct beo_hh1_msg_hdr hdr;
	struct beo_hh1_msg_outlen outlen;
	u32 master_current_control;
	u32 contrast_current;
};

struct beo_ase_msg_speaker_position {
	struct beo_hh1_msg_hdr hdr;
	struct beo_hh1_msg_outlen outlen;
	u8 speaker_position;
};

#define beo_ase_msg_sync_touch_cmd beo_cmd_msg
// ...

#define beo_hh1_rsp_ack beo_hh1_msg_hdr
#define beo_hh1_rsp_nack beo_hh1_msg_hdr
#define beo_ase_dsp_ack beo_hh1_msg_hdr

//S47 touch events/commands
struct beo_ase_msg_s47touch_calibrate_with_load {
	struct beo_hh1_msg_hdr hdr;
	struct beo_hh1_msg_outlen outlen;
	s32 weight;
};

struct beo_ase_ev_s47touch_calibrate_status {
	u16 originCmd;
	u16 pad;
	s32 eS47TouchCalibrateStatus;
};

struct beo_ase_ev_s47touch_calibration_data_type {
	u16 originCmd;
	u8 numOfZeroLoadValues;
	u8 numOfValuesWithLoad;
	s32 calibrationWeight;
	s32 zeroLoadValues[4];
	s32 valuesWithLoad[4];
};

struct beo_ase_ev_s47touch_sensor_value_type {
	u16 originCmd;
	u8  numOfRawValues;
	u8  numOfAverageValues;
	s32 rawValues[4];
	s32 averageValues[4];
};

//S47 light commands
struct beo_ase_msg_s47lightsensor_calibrate {
	struct beo_hh1_msg_hdr hdr;
	struct beo_hh1_msg_outlen outlen;
	u8 sensorId;
	u8 padding[3];
	s32 appliedLux;
};

#endif /* __BEO_HH1_FEP_H__ */
