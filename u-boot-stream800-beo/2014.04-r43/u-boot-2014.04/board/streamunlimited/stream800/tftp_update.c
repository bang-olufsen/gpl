/*
 * Copyright 2013 StreamUnlimited GmbH
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * Version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>

static int do_stftpup(cmd_tbl_t * cmdtp, int flag, int argc, char * const argv[])
{
	char *cmd = NULL;

	if (argc < 2)
		goto usage;

	cmd = argv[1];

	debug("CMD:%s\n", cmd);

	if (strcmp(cmd, "data") == 0) {
		/* run dhcp request */
		return run_command("dhcp", 0);
	} else if (strcmp(cmd, "use_local_ip") == 0) {
		char tmp[20];
		int i;

		if (NetOurIP) {
			ip_to_string(NetOurIP, tmp);

			debug("old serverip: %s\n", tmp);

			/* replace last digit with 10 and use it as serverip */
			for (i = sizeof(tmp); i >= 0; i--) {
				if (tmp[i] == '.' && (sizeof(tmp) - i) >= 3) {
					tmp[i+1] = '1';
					tmp[i+2] = '0';
					tmp[i+3] = '\0';
					break;
				}
			}
			setenv("serverip", tmp);

			debug("new serverip: %s\n", tmp);
		} else {
			printf("serverip is empty\n");
			return 1;
		}
	} else if (strcmp(cmd, "use_static_data") == 0) {
		/* set static configuration */
		setenv("ipaddr", "169.254.0.100");
		setenv("netmask", "255.255.255.0");
		setenv("bootfile", "169_254_0_100");
		setenv("serverip", "169.254.0.10");
	} else if (strcmp(cmd, "check_modify_bootfile") == 0) {
		char *bootfile = getenv("bootfile");
		char tmp[20];
		int i;
		printf("Actual bootfile: %s\n", bootfile);

		// invalid bootfile -> use local ipaddr from dhcp
		if ((bootfile[0] == 0) || (strcmp(bootfile, "uImage") == 0)) {
			ip_to_string(NetOurIP, tmp);

			// replace '.' by '_' and use as bootfile
			for (i = 0; i < strlen(tmp); i++) {
				if (tmp[i] == '.')
					tmp[i] = '_';
			}

			setenv("bootfile", tmp);
		}
	}

	return 0;
usage:
	return cmd_usage(cmdtp);
}

U_BOOT_CMD(
		stftpup, CONFIG_SYS_MAXARGS, 1, do_stftpup,
		"Streamunlimited tftp firmware update",
		"data - run dhcp to get bootfile, rootpath and serverip\n"
		"stftpup use_local_ip - serverip unknown use local ip just replace last digit with 10\n"
		"stftpup - use_static_data - set serverip, netmask, bootfile to static values\n"
		"check_modify_bootfile - create correct bootfile if empty received from dhcp lease\n"
		);

