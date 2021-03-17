/*
 * (C) Copyright 2008 Semihalf
 *
 * (C) Copyright 2000-2004
 * DENX Software Engineering
 * Wolfgang Denk, wd@denx.de
 *
 * Updated-by: Prafulla Wadaskar <prafulla@marvell.com>
 *		FIT image specific code abstracted from mkimage.c
 *		some functions added to address abstraction
 *
 * All rights reserved.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include "imagetool.h"
#include "mkimage.h"
#include <image.h>
#include <u-boot/crc.h>
#include <stdbool.h>

static image_header_t header;

static int fit_verify_header (unsigned char *ptr, int image_size,
			struct image_tool_params *params)
{
	return fdt_check_header(ptr);
}

static int fit_check_image_types (uint8_t type)
{
	if (type == IH_TYPE_FLATDT)
		return EXIT_SUCCESS;
	else
		return EXIT_FAILURE;
}

/**
 * Map an FDT into memory, optionally increasing its size
 *
 * @params:	Image parameters
 * @fname:	Filename containing FDT
 * @size_inc:	Amount to increase size by (0 = leave it alone)
 * @delete_on_error:	true to delete the file if we get an error
 * @blobp:	Returns pointer to FDT blob
 * @sbuf:	File status information is stored here
 * @return 0 if OK, -1 on error.
 */
static int mmap_fdt(struct image_tool_params *params, const char *fname,
		    size_t size_inc, bool delete_on_error, void **blobp,
		    struct stat *sbuf)
{
	void *ptr;
	int ret;
	int fd;

	/* Load FIT blob into memory (we need to write hashes/signatures) */
	fd = open(fname, O_RDWR | O_BINARY);

	if (fd < 0) {
		fprintf(stderr, "%s: Can't open %s: %s\n",
			params->cmdname, fname, strerror(errno));
		goto err;
	}

	if (fstat(fd, sbuf) < 0) {
		fprintf(stderr, "%s: Can't stat %s: %s\n",
			params->cmdname, fname, strerror(errno));
		goto err;
	}

	if (size_inc) {
		sbuf->st_size += size_inc;
		if (ftruncate(fd, sbuf->st_size)) {
			fprintf(stderr, "%s: Can't expand %s: %s\n",
				params->cmdname, fname, strerror(errno));
		goto err;
		}
	}

	ptr = mmap(0, sbuf->st_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
	if (ptr == MAP_FAILED) {
		fprintf(stderr, "%s: Can't read %s: %s\n",
			params->cmdname, fname, strerror(errno));
		goto err;
	}

	/* check if ptr has a valid blob */
	if (fdt_check_header(ptr)) {
		fprintf(stderr, "%s: Invalid FIT blob\n", params->cmdname);
		goto err;
	}

	/* expand if needed */
	if (size_inc) {
		ret = fdt_open_into(ptr, ptr, sbuf->st_size);
		if (ret) {
			fprintf(stderr, "%s: Cannot expand FDT: %s\n",
				params->cmdname, fdt_strerror(ret));
			goto err;
		}
	}

	*blobp = ptr;
	return fd;

err:
	close(fd);
	if (delete_on_error)
		unlink(fname);
	return -1;
}

static int fit_add_file_data(struct image_tool_params *params, size_t size_inc,
			     const char *tmpfile)
{
	int tfd, destfd = 0;
	void *dest_blob = NULL;
	off_t destfd_size = 0;
	struct stat sbuf;
	void *ptr;
	int ret = 0;

	tfd = mmap_fdt(params, tmpfile, size_inc, true, &ptr, &sbuf);
	if (tfd < 0)
		return -EIO;

	if (params->keydest) {
		struct stat dest_sbuf;

		destfd = mmap_fdt(params, params->keydest, size_inc, false,
				  &dest_blob, &dest_sbuf);
		if (destfd < 0) {
			ret = -EIO;
			goto err_keydest;
		}
		destfd_size = dest_sbuf.st_size;
	}

	/* for first image creation, add a timestamp at offset 0 i.e., root  */
	if (params->datafile)
		ret = fit_set_timestamp(ptr, 0, sbuf.st_mtime);

	if (!ret) {
		ret = fit_add_verification_data(params->keydir, dest_blob, ptr,
						params->comment,
						params->require_keys);
	}

	if (dest_blob) {
		munmap(dest_blob, destfd_size);
		close(destfd);
	}

err_keydest:
	munmap(ptr, sbuf.st_size);
	close(tfd);

	return ret;
}

/**
 * fit_handle_file - main FIT file processing function
 *
 * fit_handle_file() runs dtc to convert .its to .itb, includes
 * binary data, updates timestamp property and calculates hashes.
 *
 * datafile  - .its file
 * imagefile - .itb file
 *
 * returns:
 *     only on success, otherwise calls exit (EXIT_FAILURE);
 */
static int fit_handle_file(struct image_tool_params *params)
{
	char tmpfile[MKIMAGE_MAX_TMPFILE_LEN];
	char cmd[MKIMAGE_MAX_DTC_CMDLINE_LEN];
	size_t size_inc;
	int ret;

	/* Flattened Image Tree (FIT) format  handling */
	debug ("FIT format handling\n");

	/* call dtc to include binary properties into the tmp file */
	if (strlen (params->imagefile) +
		strlen (MKIMAGE_TMPFILE_SUFFIX) + 1 > sizeof (tmpfile)) {
		fprintf (stderr, "%s: Image file name (%s) too long, "
				"can't create tmpfile",
				params->imagefile, params->cmdname);
		return (EXIT_FAILURE);
	}
	sprintf (tmpfile, "%s%s", params->imagefile, MKIMAGE_TMPFILE_SUFFIX);

	/* We either compile the source file, or use the existing FIT image */
	if (params->datafile) {
		/* dtc -I dts -O dtb -p 500 datafile > tmpfile */
		snprintf(cmd, sizeof(cmd), "%s %s %s > %s",
			 MKIMAGE_DTC, params->dtc, params->datafile, tmpfile);
		debug("Trying to execute \"%s\"\n", cmd);
	} else {
		snprintf(cmd, sizeof(cmd), "cp %s %s",
			 params->imagefile, tmpfile);
	}
	if (system (cmd) == -1) {
		fprintf (stderr, "%s: system(%s) failed: %s\n",
				params->cmdname, cmd, strerror(errno));
		goto err_system;
	}

	/*
	 * Set hashes for images in the blob. Unfortunately we may need more
	 * space in either FDT, so keep trying until we succeed.
	 *
	 * Note: this is pretty inefficient for signing, since we must
	 * calculate the signature every time. It would be better to calculate
	 * all the data and then store it in a separate step. However, this
	 * would be considerably more complex to implement. Generally a few
	 * steps of this loop is enough to sign with several keys.
	 */
	for (size_inc = 0; size_inc < 64 * 1024; size_inc += 1024) {
		ret = fit_add_file_data(params, size_inc, tmpfile);
		if (!ret || ret != -ENOSPC)
			break;
	}

	if (ret) {
		fprintf(stderr, "%s Can't add hashes to FIT blob\n",
			params->cmdname);
		goto err_system;
	}

	if (rename (tmpfile, params->imagefile) == -1) {
		fprintf (stderr, "%s: Can't rename %s to %s: %s\n",
				params->cmdname, tmpfile, params->imagefile,
				strerror (errno));
		unlink (tmpfile);
		unlink (params->imagefile);
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;

err_system:
	unlink(tmpfile);
	return -1;
}

static int fit_check_params(struct image_tool_params *params)
{
	return	((params->dflag && (params->fflag || params->lflag)) ||
		(params->fflag && (params->dflag || params->lflag)) ||
		(params->lflag && (params->dflag || params->fflag)));
}

static struct image_type_params fitimage_params = {
	.name = "FIT Image support",
	.header_size = sizeof(image_header_t),
	.hdr = (void*)&header,
	.verify_header = fit_verify_header,
	.print_header = fit_print_contents,
	.check_image_type = fit_check_image_types,
	.fflag_handle = fit_handle_file,
	.set_header = NULL,	/* FIT images use DTB header */
	.check_params = fit_check_params,
};

void init_fit_image_type (void)
{
	register_image_type(&fitimage_params);
}
