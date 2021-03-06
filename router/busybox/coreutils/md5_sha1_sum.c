/* vi: set sw=4 ts=4: */
/*
 *  Copyright (C) 2003 Glenn L. McGrath
 *  Copyright (C) 2003-2004 Erik Andersen
 *
 * Licensed under the GPL v2 or later, see the file LICENSE in this tarball.
 */

#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "busybox.h"

typedef enum { HASH_SHA1, HASH_MD5 } hash_algo_t;

#define FLAG_SILENT	1
#define FLAG_CHECK	2
#define FLAG_WARN	4

/* This might be useful elsewhere */
static unsigned char *hash_bin_to_hex(unsigned char *hash_value,
									  unsigned char hash_length)
{
	int x, len, max;
	unsigned char *hex_value;

	max = (hash_length * 2) + 2;
	hex_value = xmalloc(max);
	for (x = len = 0; x < hash_length; x++) {
		len += snprintf((char*)(hex_value + len), max - len, "%02x", hash_value[x]);
	}
	return (hex_value);
}

static uint8_t *hash_file(const char *filename, hash_algo_t hash_algo)
{
	int src_fd, hash_len, count;
	union _ctx_ {
		sha1_ctx_t sha1;
		md5_ctx_t md5;
	} context;
	uint8_t *hash_value = NULL;
	RESERVE_CONFIG_UBUFFER(in_buf, 4096);
	void (*update)(const void*, size_t, void*);
	void (*final)(void*, void*);

	if (strcmp(filename, "-") == 0) {
		src_fd = STDIN_FILENO;
	} else if(0 > (src_fd = open(filename, O_RDONLY))) {
		bb_perror_msg("%s", filename);
		return NULL;
	}

	/* figure specific hash algorithims */
	if (ENABLE_MD5SUM && hash_algo==HASH_MD5) {
		md5_begin(&context.md5);
		update = (void (*)(const void*, size_t, void*))md5_hash;
		final = (void (*)(void*, void*))md5_end;
		hash_len = 16;
	} else if (ENABLE_SHA1SUM && hash_algo==HASH_SHA1) {
		sha1_begin(&context.sha1);
		update = (void (*)(const void*, size_t, void*))sha1_hash;
		final = (void (*)(void*, void*))sha1_end;
		hash_len = 20;
	} else {
		bb_error_msg_and_die("algorithm not supported");
	}

	while (0 < (count = read(src_fd, in_buf, 4096))) {
		update(in_buf, count, &context);
	}

	if (count == 0) {
		final(in_buf, &context);
		hash_value = hash_bin_to_hex(in_buf, hash_len);
	}

	RELEASE_CONFIG_BUFFER(in_buf);

	if (src_fd != STDIN_FILENO) {
		close(src_fd);
	}

	return hash_value;
}

/* This could become a common function for md5 as well, by using md5_stream */
static int hash_files(int argc, char **argv, hash_algo_t hash_algo)
{
	int return_value = EXIT_SUCCESS;
	uint8_t *hash_value;
	unsigned int flags;

	if (ENABLE_FEATURE_MD5_SHA1_SUM_CHECK)
		flags = bb_getopt_ulflags(argc, argv, "scw");
	else optind = 1;

	if (ENABLE_FEATURE_MD5_SHA1_SUM_CHECK && !(flags & FLAG_CHECK)) {
		if (flags & FLAG_SILENT) {
			bb_error_msg_and_die
				("the -s option is meaningful only when verifying checksums");
		} else if (flags & FLAG_WARN) {
			bb_error_msg_and_die
				("the -w option is meaningful only when verifying checksums");
		}
	}

	if (argc == optind) {
		argv[argc++] = "-";
	}

	if (ENABLE_FEATURE_MD5_SHA1_SUM_CHECK && flags & FLAG_CHECK) {
		FILE *pre_computed_stream;
		int count_total = 0;
		int count_failed = 0;
		char *file_ptr = argv[optind];
		char *line;

		if (optind + 1 != argc) {
			bb_error_msg_and_die
				("only one argument may be specified when using -c");
		}

		if (strcmp(file_ptr, "-") == 0) {
			pre_computed_stream = stdin;
		} else {
			pre_computed_stream = bb_xfopen(file_ptr, "r");
		}

		while ((line = bb_get_chomped_line_from_file(pre_computed_stream)) != NULL) {
			char *filename_ptr;

			count_total++;
			filename_ptr = strstr(line, "  ");
			if (filename_ptr == NULL) {
				if (flags & FLAG_WARN) {
					bb_error_msg("Invalid format");
				}
				count_failed++;
				return_value = EXIT_FAILURE;
				free(line);
				continue;
			}
			*filename_ptr = '\0';
			filename_ptr += 2;

			hash_value = hash_file(filename_ptr, hash_algo);

			if (hash_value && (strcmp((char*)hash_value, line) == 0)) {
				if (!(flags & FLAG_SILENT))
					printf("%s: OK\n", filename_ptr);
			} else {
				if (!(flags & FLAG_SILENT))
					printf("%s: FAILED\n", filename_ptr);
				count_failed++;
				return_value = EXIT_FAILURE;
			}
			/* possible free(NULL) */
			free(hash_value);
			free(line);
		}
		if (count_failed && !(flags & FLAG_SILENT)) {
			bb_error_msg("WARNING: %d of %d computed checksums did NOT match",
						 count_failed, count_total);
		}
		if (bb_fclose_nonstdin(pre_computed_stream) == EOF) {
			bb_perror_msg_and_die("Couldnt close file %s", file_ptr);
		}
	} else {
		while (optind < argc) {
			char *file_ptr = argv[optind++];

			hash_value = hash_file(file_ptr, hash_algo);
			if (hash_value == NULL) {
				return_value = EXIT_FAILURE;
			} else {
				printf("%s  %s\n", hash_value, file_ptr);
				free(hash_value);
			}
		}
	}
	return (return_value);
}

#ifdef CONFIG_MD5SUM
int md5sum_main(int argc, char **argv)
{
	return (hash_files(argc, argv, HASH_MD5));
}
#endif

#ifdef CONFIG_SHA1SUM
int sha1sum_main(int argc, char **argv)
{
	return (hash_files(argc, argv, HASH_SHA1));
}
#endif
