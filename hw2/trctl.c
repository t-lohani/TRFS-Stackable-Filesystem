#include <sys/ioctl.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include "trctl.h"
#include "treplay.h"

int main(int argc, char *argv[])
{
	char *command;
	unsigned long bitmapval;
	unsigned long retmapval;
	int fd = -1;
	int rc = 0;
	bool extra_cr = false;
/*	printf("Argument count --->>>>>>>  %d \n", argc);
	printf("Argument value ---->>>>>>> %s \n", *argv); */

	if (argc < 2) {
		printf("Too few arguments. Terminating call.\n");
		goto errorl;
	}

	fd = open("/mnt/trfs", O_RDONLY);

	if (argc == 2) {
/*		printf("Retrieve the current value of the bitmap and display it in hex.\n"); */
		rc = ioctl(fd, RET, &retmapval);
		printf("Current bitmap value is : 0x%lx\n", retmapval);
	} else if (argc == 3) {
/*		printf("Arguments passed %s, %s\n", argv[1], argv[2]); */
		command = argv[1];
/*		printf("Command : %s\n", command); */
		if (!strcmp("all", command)) {
/*			printf("Trace everything\n"); */
			bitmapval = BITMAP_MAX;
/*			printf("Bitmapval : %lx\n", bitmapval); */
			rc = ioctl(fd, ALL, bitmapval);
		} else if (!strcmp("none", command)) {
/*			printf("Trace nothing\n"); */
			bitmapval = 0;
/*			printf("Bitmapval : %lx\n", bitmapval); */
			rc = ioctl(fd, NONE, bitmapval);
		} else {
/*			printf("Trace the specified values only\n"); */

			if (command[0] == '+' || command[0] == '-') {
/*				printf("Making extra credit true.\n"); */
				extra_cr = true;
			} else {
				if (command[0] != '0') {
					printf("Please enter a hex value in 0xNN format\n");
					goto errorl;
				}
				if (!(command[1] == 'x' || command[1] == 'X')) {
					printf("Please enter a hex value in 0xNN format\n");
					goto errorl;
				}

				bitmapval = (unsigned long)strtol(command+2, NULL, 16);
				if (bitmapval > BITMAP_MAX) {
					printf("Please enter a lesser value than 0x%lx.\n", BITMAP_MAX);
					goto errorl;
				}
/*				printf("Bitmapval : %lx\n", bitmapval); */
				rc = ioctl(fd, HEX, bitmapval);
			}
		}
	}
	if (argc > 3) {
		extra_cr = true;
	}

	if (extra_cr) {
		rc = ioctl(fd, RET, &retmapval);
/*		printf("Ex_cr current bitmap value is : 0x%lx\n", retmapval); */
		int index = 1;
		while (index < argc-1) {
/*			printf("Index : %d\n", index); */
			if (!strcmp("+open", argv[index])) {
				if (((retmapval >> OPEN_CALL) & 1) == 0) {
					retmapval = retmapval + (1 << OPEN_CALL);
					rc = ioctl(fd, HEX, retmapval);
				}
			} else if (!strcmp("-open", argv[index])) {
				if (((retmapval >> OPEN_CALL) & 1) == 1) {
					retmapval = retmapval - (1 << OPEN_CALL);
					rc = ioctl(fd, HEX, retmapval);
				}
			}
			if (!strcmp("+read", argv[index])) {
				if (((retmapval >> READ_CALL) & 1) == 0) {
					retmapval = retmapval + (1 << READ_CALL);
					rc = ioctl(fd, HEX, retmapval);
				}
			} else if (!strcmp("-read", argv[index])) {
/*				printf("read - "); */
				if (((retmapval >> READ_CALL) & 1) == 1) {
/*					printf("read i - %d\n", (1 << READ_CALL)); */
					retmapval = retmapval - (1 << READ_CALL);
/*					printf("BIt val %lx\n", retmapval); */
					rc = ioctl(fd, HEX, retmapval);
				}
			}
			if (!strcmp("+write", argv[index])) {
				if (((retmapval >> WRITE_CALL) & 1) == 0) {
					retmapval = retmapval + (1 << WRITE_CALL);
					rc = ioctl(fd, HEX, retmapval);
				}
			} else if (!strcmp("-write", argv[index])) {
				if (((retmapval >> WRITE_CALL) & 1) == 1) {
					retmapval = retmapval - (1 << WRITE_CALL);
					rc = ioctl(fd, HEX, retmapval);
				}
			}
			if (!strcmp("+close", argv[index])) {
				if (((retmapval >> RELEASE_CALL) & 1) == 0) {
					retmapval = retmapval + (1 << RELEASE_CALL);
					rc = ioctl(fd, HEX, retmapval);
				}
			} else if (!strcmp("-close", argv[index])) {
				if (((retmapval >> RELEASE_CALL) & 1) == 1) {
					retmapval = retmapval - (1 << RELEASE_CALL);
					rc = ioctl(fd, HEX, retmapval);
				}
			}
			if (!strcmp("+create", argv[index])) {
				if (((retmapval >> CREATE_CALL) & 1) == 0) {
					retmapval = retmapval + (1 << CREATE_CALL);
					rc = ioctl(fd, HEX, retmapval);
				}
			} else if (!strcmp("-create", argv[index])) {
				if (((retmapval >> CREATE_CALL) & 1) == 1) {
					retmapval = retmapval - (1 << CREATE_CALL);
					rc = ioctl(fd, HEX, retmapval);
				}
			}
			if (!strcmp("+link", argv[index])) {
				if (((retmapval >> LINK_CALL) & 1) == 0) {
					retmapval = retmapval + (1 << LINK_CALL);
					rc = ioctl(fd, HEX, retmapval);
				}
			} else if (!strcmp("-link", argv[index])) {
				if (((retmapval >> LINK_CALL) & 1) == 1) {
					retmapval = retmapval - (1 << LINK_CALL);
					rc = ioctl(fd, HEX, retmapval);
				}
			}
			if (!strcmp("+unlink", argv[index])) {
				if (((retmapval >> UNLINK_CALL) & 1) == 0) {
					retmapval = retmapval + (1 << UNLINK_CALL);
					rc = ioctl(fd, HEX, retmapval);
				}
			} else if (!strcmp("-unlink", argv[index])) {
				if (((retmapval >> UNLINK_CALL) & 1) == 1) {
					retmapval = retmapval - (1 << UNLINK_CALL);
					rc = ioctl(fd, HEX, retmapval);
				}
			}
			if (!strcmp("+symlink", argv[index])) {
				if (((retmapval >> SYMLINK_CALL) & 1) == 0) {
					retmapval = retmapval + (1 << SYMLINK_CALL);
					rc = ioctl(fd, HEX, retmapval);
				}
			} else if (!strcmp("-symlink", argv[index])) {
				if (((retmapval >> SYMLINK_CALL) & 1) == 1) {
					retmapval = retmapval - (1 << SYMLINK_CALL);
					rc = ioctl(fd, HEX, retmapval);
				}
			}
			if (!strcmp("+mkdir", argv[index])) {
				if (((retmapval >> MKDIR_CALL) & 1) == 0) {
					retmapval = retmapval + (1 << MKDIR_CALL);
					rc = ioctl(fd, HEX, retmapval);
				}
			} else if (!strcmp("-mkdir", argv[index])) {
				if (((retmapval >> MKDIR_CALL) & 1) == 1) {
					retmapval = retmapval - (1 << MKDIR_CALL);
					rc = ioctl(fd, HEX, retmapval);
				}
			}
			if (!strcmp("+rmdir", argv[index])) {
				if (((retmapval >> RMDIR_CALL) & 1) == 0) {
					retmapval = retmapval + (1 << RMDIR_CALL);
					rc = ioctl(fd, HEX, retmapval);
				}
			} else if (!strcmp("-rmdir", argv[index])) {
				if (((retmapval >> RMDIR_CALL) & 1) == 1) {
					retmapval = retmapval - (1 << RMDIR_CALL);
					rc = ioctl(fd, HEX, retmapval);
				}
			}
			if (!strcmp("+mknod", argv[index])) {
				if (((retmapval >> MKNOD_CALL) & 1) == 0) {
					retmapval = retmapval + (1 << MKNOD_CALL);
					rc = ioctl(fd, HEX, retmapval);
				}
			} else if (!strcmp("-mknod", argv[index])) {
				if (((retmapval >> MKNOD_CALL) & 1) == 1) {
					retmapval = retmapval - (1 << MKNOD_CALL);
					rc = ioctl(fd, HEX, retmapval);
				}
			}
			if (!strcmp("+rename", argv[index])) {
				if (((retmapval >> RENAME_CALL) & 1) == 0) {
					retmapval = retmapval + (1 << RENAME_CALL);
					rc = ioctl(fd, HEX, retmapval);
				}
			} else if (!strcmp("-rename", argv[index])) {
				if (((retmapval >> RENAME_CALL) & 1) == 1) {
					retmapval = retmapval - (1 << RENAME_CALL);
					rc = ioctl(fd, HEX, retmapval);
				}
			}

			index++;
		}
	}

errorl:

	close(fd);
	return rc;
}
