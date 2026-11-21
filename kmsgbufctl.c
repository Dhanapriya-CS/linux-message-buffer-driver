// SPDX-License-Identifier: GPL-2.0-only
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define DEFAULT_DEVICE "/dev/kmsgbuf"
#define MAX_READ_SIZE 4096

static void usage(const char *program)
{
	fprintf(stderr,
		"Usage: %s [device] read|write TEXT|clear\n"
		"Examples:\n"
		"  %s read\n"
		"  %s write 'hello'\n"
		"  %s clear\n",
		program, program, program, program);
}

int main(int argc, char **argv)
{
	const char *device = DEFAULT_DEVICE;
	const char *operation;
	int fd;

	if (argc >= 3 && argv[1][0] == '/') {
		device = argv[1];
		operation = argv[2];
	} else if (argc >= 2) {
		operation = argv[1];
	} else {
		usage(argv[0]);
		return EXIT_FAILURE;
	}

	fd = open(device, (strcmp(operation, "read") == 0) ? O_RDONLY : O_WRONLY);
	if (fd < 0) {
		perror(device);
		return EXIT_FAILURE;
	}

	if (strcmp(operation, "read") == 0) {
		char buffer[MAX_READ_SIZE];
		ssize_t bytes = read(fd, buffer, sizeof(buffer));
		if (bytes < 0) {
			perror("read");
			close(fd);
			return EXIT_FAILURE;
		}
		if (fwrite(buffer, 1, (size_t)bytes, stdout) != (size_t)bytes) {
			perror("stdout");
			close(fd);
			return EXIT_FAILURE;
		}
	} else if (strcmp(operation, "write") == 0 && argc >= 3) {
		const char *message = argv[argc - 1];
		size_t length = strlen(message);
		ssize_t bytes = write(fd, message, length);
		if (bytes < 0) {
			perror("write");
			close(fd);
			return EXIT_FAILURE;
		}
		if ((size_t)bytes != length) {
			fprintf(stderr, "short write: %zd of %zu bytes\n", bytes,
				length);
			close(fd);
			return EXIT_FAILURE;
		}
	} else if (strcmp(operation, "clear") == 0) {
		static const char clear_command[] = "CLEAR\n";
		if (write(fd, clear_command, sizeof(clear_command) - 1) < 0) {
			perror("clear");
			close(fd);
			return EXIT_FAILURE;
		}
	} else {
		usage(argv[0]);
		close(fd);
		return EXIT_FAILURE;
	}

	if (close(fd) < 0) {
		perror("close");
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
