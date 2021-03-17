/*
 * (C) Copyright 2000-2008
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

/*
 * Command line user interface to firmware (=U-Boot) environment.
 *
 * Implements:
 *	fw_printenv [[ -n name ] | [ name ... ]]
 *              - prints the value of a single environment variable
 *                "name", the ``name=value'' pairs of one or more
 *                environment variables "name", or the whole
 *                environment if no names are specified.
 *	fw_setenv name [ value ... ]
 *		- If a name without any values is given, the variable
 *		  with this name is deleted from the environment;
 *		  otherwise, all "value" arguments are concatenated,
 *		  separated by single blank characters, and the
 *		  resulting string is assigned to the environment
 *		  variable "name"
 */

#include <fcntl.h>
#include <getopt.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/file.h>
#include <unistd.h>
#include "fw_env.h"

#define	CMD_PRINTENV	"fw_printenv"
#define CMD_SETENV	"fw_setenv"
#define	CMD_PRINTCONST	"fw_printconst"
#define CMD_SETCONST	"fw_setconst"

extern char * config_file;
extern int use_constants_instead_of_env;

static struct option long_options[] = {
	{"script", required_argument, NULL, 's'},
	{"help", no_argument, NULL, 'h'},
	{NULL, 0, NULL, 0}
};


void usage(void)
{

	fprintf(stderr, "fw_printenv/fw_setenv, "
		"a command line interface to U-Boot environment and constants\n\n"
		"usage:\tfw_printenv/fw_printconst [-n] [variable name]\n"
		"\tfw_printenv/fw_printconst -d\n"
		"\tfw_setenv/fw_setconst [variable name] [variable value]\n"
		"\tfw_setenv/fw_setconst -s [ file ]\n"
		"\tfw_setenv/fw_setconst -s - < [ file ]\n\n"
		"The file passed as argument contains only pairs "
		"name / value\n"
		"Example:\n"
		"# Any line starting with # is treated as comment\n"
		"\n"
		"\t      netdev         eth0\n"
		"\t      kernel_addr    400000\n"
		"\t      var1\n"
		"\t      var2          The quick brown fox jumps over the "
		"lazy dog\n"
		"\n"
		"A variable without value will be dropped. It is possible\n"
		"to put any number of spaces between the fields, but any\n"
		"space inside the value is treated as part of the value "
		"itself.\n\n"
	);
}

#include <errno.h>

int main(int argc, char *argv[])
{
	char *p;
	char *cmdname = *argv;
	char *script_file = NULL;
	int print_default_env = 0;
	int c;
	const char *lockname = "/var/lock/" CMD_PRINTENV ".lock";
	int lockfd = -1;
	int retval = EXIT_SUCCESS;
	struct flock lock;
	use_constants_instead_of_env = 0;

	lock.l_type = F_RDLCK;
	lock.l_len = 0;
	lock.l_start = 0;
	lock.l_whence = SEEK_SET;
	lockfd = open(lockname, O_WRONLY | O_CREAT | O_TRUNC, &lock);
	if (-1 == lockfd) {
		fprintf(stderr, "Error opening lock file '%s': %s\n", lockname, strerror(errno));
		return EXIT_FAILURE;
	}

	if (-1 == flock(lockfd, LOCK_EX)) {
		fprintf(stderr, "Error locking file %s\n", lockname);
		close(lockfd);
		return EXIT_FAILURE;
	}

	if ((p = strrchr (cmdname, '/')) != NULL) {
		cmdname = p + 1;
	}

	while ((c = getopt_long (argc, argv, "ns:dh",
		long_options, NULL)) != EOF) {
		switch (c) {
		case 'n':
			/* handled in fw_printenv */
			break;
		case 'd':
			print_default_env = 1;
			/* handled in fw_printenv */
			break;
		case 's':
			script_file = optarg;
			break;
		case 'h':
			usage();
			goto exit;
		default: /* '?' */
			fprintf(stderr, "Try `%s --help' for more information."
				"\n", cmdname);
			retval = EXIT_FAILURE;
			goto exit;
		}
	}

	if (strcmp(cmdname, CMD_PRINTENV) == 0) {
		config_file = CONFIG_ENV_FILE;
		if (print_default_env != 0) {
			fw_print_default_env();
		} else {
			if (fw_printenv(argc, argv) != 0)
				retval = EXIT_FAILURE;
		}
	} else if (strcmp(cmdname, CMD_SETENV) == 0) {
		config_file = CONFIG_ENV_FILE;
		if (!script_file) {
			if (fw_setenv(argc, argv) != 0)
				retval = EXIT_FAILURE;
		} else {
			if (fw_parse_script(script_file) != 0)
				retval = EXIT_FAILURE;
		}
	} else if (strcmp(cmdname, CMD_PRINTCONST) == 0) {
		config_file = CONFIG_CONST_FILE;

		/* !!!! Quick and dirty hack
		 * default environment changed to default constants
		 * Expects that env is empty
		 */
		use_constants_instead_of_env = 1;

		if (print_default_env == 0) {
			if (fw_printenv(argc, argv) != 0)
				retval = EXIT_FAILURE;
		}
	} else if (strcmp(cmdname, CMD_SETCONST) == 0) {

		/* !!!! Quick and dirty hack
		 * default environment changed to default constants
		 * Expects that env is empty
		 */
		use_constants_instead_of_env = 1;

		config_file = CONFIG_CONST_FILE;
		if (!script_file) {
			if (fw_setenv(argc, argv) != 0)
				retval = EXIT_FAILURE;
		} else {
			if (fw_parse_script(script_file) != 0)
				retval = EXIT_FAILURE;
		}
	} else {
		fprintf(stderr,
			"Identity crisis - may be called as `" CMD_PRINTENV
			"' or as `" CMD_PRINTCONST "' or as `" CMD_SETENV
			"' or as `" CMD_SETCONST "' but not as `%s'\n",
			cmdname);
		retval = EXIT_FAILURE;
	}

exit:
	flock(lockfd, LOCK_UN);
	close(lockfd);
	remove(lockname);
	return retval;
}
