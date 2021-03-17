/*
 * ase_fep.c
 *
 * Copyright (C) 2014, StreamUnlimited Engineering GmbH, http://www.streamunlimited.com/
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR /PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include "common.h"
#include "ase_fep.h"
#include "mux.h"
#include <malloc.h>

#define MSG_HEADER_SIZE 4
#define UART_SPEED 1000000
#define UART_PORT	5
#define UPDATE_BUTTON 0x13

struct beo_ase_button {
	u16 button_id;
	u16 button_value;
};

struct beo_ase_rsp_buttons {
	u32 hdr;
	struct beo_ase_button buttons[16];
};

static void dump_data(u8* data, int size)
{
	u8 dump[3*32 + 1] = {0};
	int i;
	int max_size = size <= 32 ? size : 32;

	for (i = 0; i < max_size; i++)
		sprintf(&dump[3*i], "%02x ", data[i]);
	printf("received %d bytes: %s\n", max_size, dump);
}

static void add_character(u8* data, int max_size, int position, int character)
{
	if(max_size > position) {
		data[position] = character;
	}
}

static void send_ase_fep(struct ase_fep *fep, const u8* data, int size)
{
	int i;
	if(!fep || !fep->fep_uart)
		return;

	for (i = 0; i < size; i++) {
		_serial_putc(data[i], fep->fep_uart);
	}
}

static int receive_ase_fep(struct ase_fep *fep, u8* data, int max_size)
{
	int i, data_size, character;
	if (!fep || !fep->fep_uart) {
		return -1;
	}

	do {
	  character = _serial_getc(fep->fep_uart);
	} while (character == 0);
	add_character(data, max_size, 0, character);

	for (i = 1; i < MSG_HEADER_SIZE; i++) {
		character = _serial_getc(fep->fep_uart);
		add_character(data, max_size, i, character);
		if (i == 2) {
			data_size = character;
		}
	}

	for (i = 0; i < data_size - MSG_HEADER_SIZE; i++) {
		character = _serial_getc(fep->fep_uart);
		add_character(data, max_size, i + MSG_HEADER_SIZE, character);
	}
	return data_size > max_size ? max_size : data_size;
}

struct ase_fep *init_ase_fep(void)
{
	struct ase_fep *fep;

	enable_uart4_pin_mux();
	_serial_init(UART_PORT, UART_SPEED);

	fep = (struct ase_fep *) malloc(sizeof(struct ase_fep));
	fep->fep_uart = UART_PORT;
	return fep;
}

struct ase_fep_version version_ase_fep(struct ase_fep *fep)
{
	struct ase_fep_version version;
	const u8 getHWVersionCmd[] = {0x82, 0x80, 0x08, 0x00, 0x0c, 0x00, 0x00, 0x00};
	u8 reply[12];

	send_ase_fep(fep, getHWVersionCmd, 8);
	int len = receive_ase_fep(fep, reply, 12);
	if (len) {
		memcpy(&version, reply, len);
	}

	printf("board version: %c - %d - %d - %d\n", version.revision, version.pcbVariant, version.productVariant, version.productId);
	return version;
}

bool is_pressed_update_button(struct ase_fep *fep)
{
	int i;
	const u8 buttonsStateCmd[] = { 0x30, 0x80, 0x08, 0x00, 0x44, 0x00, 0x00, 0x00};
	u8 buttonsStateReplay[80];

	send_ase_fep(fep, buttonsStateCmd, 8);
	int len = receive_ase_fep(fep, buttonsStateReplay, 80);

	for (i = MSG_HEADER_SIZE; i < len; i += sizeof(struct beo_ase_button) ) {
		struct beo_ase_button *button = buttonsStateReplay + i;

		button->button_id = le16_to_cpu(button->button_id);
		button->button_value = le16_to_cpu(button->button_value);

		if (button->button_id == UPDATE_BUTTON) {
			return button->button_value;
		}
	}

	return false;
}

static void disable_net_leds(struct ase_fep *fep)
{
	/* disable all network colors leds */
	int led; // network led start at index 0 and end at 3
	u8 stateLedCmd[] = { 0x20,0x80,0x10,0x00,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x0,0x00,0x0,0x0,0x0 };
	u8 stateReply[20];

	for (led = 0; led < 4; led ++) {
		stateLedCmd[8] = led;
		send_ase_fep(fep, stateLedCmd, 16);
		receive_ase_fep(fep, stateReply, 20);
	}
}

static void disable_prod_leds(struct ase_fep *fep)
{
	/* disable all production colors leds */
	int led; // production led start at index 4 and end at 7
	u8 stateLedCmd[] = { 0x20,0x80,0x10,0x00,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x0,0x00,0x0,0x0,0x0 };
	u8 stateReply[20];

	for (led = 0; led < 4; led ++) {
		stateLedCmd[8] = led + 4;
		send_ase_fep(fep, stateLedCmd, 16);
		receive_ase_fep(fep, stateReply, 20);
	}
}

void set_boot_state_ase_fep(struct ase_fep *fep, const struct Stream800Board *board)
{
	// Prod_white and Net_white, slow blink (600/200ms)
	u8 bootingStateLedCmd[] = { 0x20,0x80,0x18,0x00,0x04,0x00,0x00,0x00,0x04,0x00,0x01,0x0,0x26,0x0,0x0,0x0,0x00,0x00,0x01,0x0,0x26,0x0,0x0,0x0 };
	u8 bootStateReplay[28];

	/* disable all leds */
	disable_net_leds(fep);
	disable_prod_leds(fep);

	// for ez2mk2 we blink with Prod_Green instead of Prod_white
	if (board->carrierBoardType == CBT_BeoASE_EZ2MK2) {
		bootingStateLedCmd[8] = 0x07;
	}

	send_ase_fep(fep, bootingStateLedCmd, 24);
	receive_ase_fep(fep, bootStateReplay, 28);
}

void set_error_state_ase_fep(struct ase_fep *fep)
{
	// Prod_red, fast blink (200/100ms)
	const u8 failureStateLedCmd[] = { 0x20,0x80,0x10,0x00,0x04,0x00,0x00,0x00,0x05,0x00,0x01,0x0,0x12,0x0,0x0,0x0 };
	u8 bootStateReplay[20];

	/* disable all leds */
	disable_net_leds(fep);
	disable_prod_leds(fep);

	send_ase_fep(fep, failureStateLedCmd, 16);
	receive_ase_fep(fep, bootStateReplay, 20);
}

void set_flashing_on_state_ase_fep(struct ase_fep *fep)
{
	// Prod_red + Net_red, fast blink (100/100ms)
	int received;
	const u8 flashingStateLedCmd[] = { 0x20,0x80,0x18,0x00,0x04,0x00,0x00,0x00,0x02,0x00,0x01,0x0,0x11,0x0,0x0, 0x0,0x05,0x00,0x01,0x0,0x11,0x0,0x0,0x0 };
	u8 bootStateReplay[28];

	/* disable all leds */
	disable_net_leds(fep);
	disable_prod_leds(fep);

	send_ase_fep(fep, flashingStateLedCmd, 24);
	int s = receive_ase_fep(fep, bootStateReplay, 28);
	dump_data(bootStateReplay, s);
}

void set_flashing_off_state_ase_fep(struct ase_fep *fep)
{
	// ON_red, off
	const u8 flashingOffStateLedCmd[] = { 0x20,0x80,0x10,0x00,0x04,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x0,0x0,0x0 };
	u8 bootStateReplay[20];

	/* disable all leds */
	disable_net_leds(fep);
	disable_prod_leds(fep);

	send_ase_fep(fep, flashingOffStateLedCmd, 16);
	receive_ase_fep(fep, bootStateReplay, 20);
}
