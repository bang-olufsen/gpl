/*
 *
 *  BlueZ - Bluetooth protocol stack for Linux
 *
 *  Copyright (C) 2020 Bang & Olufsen a/s
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdbool.h>

#include <glib.h>

#include "lib/bluetooth.h"
#include "lib/sdp.h"
#include "lib/uuid.h"

#include "src/plugin.h"
#include "src/adapter.h"
#include "src/device.h"
#include "src/profile.h"
#include "src/service.h"
#include "src/log.h"

static int baldwin_probe(struct btd_service *service)
{
	return 0;
}

static void baldwin_remove(struct btd_service *service)
{
}

static int baldwin_accept(struct btd_service *service)
{
	struct btd_device *device = btd_service_get_device(service);
	char addr[18];

	ba2str(device_get_address(device), addr);
	DBG("baldwin profile accept (%s)", addr);

	btd_service_connecting_complete(service, 0);

	return 0;
}

static int baldwin_disconnect(struct btd_service *service)
{
	btd_service_disconnecting_complete(service, 0);

	return 0;
}

static struct btd_profile baldwin_profile = {
	.name		= "baldwin",
	.remote_uuid	= BALDWIN_UUID,
	.external	= true,
	.device_probe	= baldwin_probe,
	.device_remove	= baldwin_remove,
	.accept		= baldwin_accept,
	.disconnect	= baldwin_disconnect,
	.auto_connect	= true,
};

static int baldwin_init(void)
{
	return btd_profile_register(&baldwin_profile);
}

static void baldwin_exit(void)
{
	btd_profile_unregister(&baldwin_profile);
}

BLUETOOTH_PLUGIN_DEFINE(baldwin, VERSION, BLUETOOTH_PLUGIN_PRIORITY_DEFAULT,
					baldwin_init, baldwin_exit)
