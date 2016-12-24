#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "treplay.h"

int fd = -1;

bool replay_record(struct trfs_trace_ds *tracer, char *path1, char *path2, char *content)
{
	int rec_type = tracer->record_type;
	int errval = 0;
	char *replay_path = "/usr/src/replay";
	char *fullpath1 = NULL;
	char *fullpath2 = NULL;
	char *writer_content;
	char buf[4096];
	int bytes_read = 0;

	fullpath1 = malloc(strlen(replay_path) + strlen(path1));
	strcpy(fullpath1, replay_path);
	strcat(fullpath1, path1);

	if (path2 != NULL) {
		fullpath2 = malloc(strlen(replay_path) + strlen(path2));
		strcpy(fullpath2, replay_path);
		strcat(fullpath2, path2);
	}

	if (content != NULL) {
		writer_content = malloc(strlen(content)+1);
		strcpy(writer_content, content);
	}
/*	printf("------>>>>>>>>>>>>>>>> Mount Path : %s\n", replay_path);
	printf("------>>>>>>>>>>>>>>>> Full Path : %s\n", fullpath1);
	printf("----->>>>>>>>>>> Inside replay record\n");
	printf("----->>>>>>>>>>> Record Type : %d\n", rec_type); */

	printf("Replay result ->\n\n");
	printf("Original return value : %d\n", tracer->err);

	switch (rec_type) {

	case OPEN_CALL:
/*		printf("---->>>> Replay Open %s\n", fullpath1); */
		errval = open(fullpath1, tracer->open_flags);
		if (errval < 0) {
			errval = -1;
			printf("Replay return value : %d\n", errval);
		} else {
			fd = errval;
			errval = 0;
			printf("Replay return value : %d\n", errval);
		}

		if (tracer->err == 0 && errval > 0) {
/*			printf("Replay Success!\n"); */
			return true;
		} else {
/*			printf("Replay Failed!\n"); */
			return false;
		}

	case READ_CALL:
/*		printf("---->>>> Replay Read %s\n", fullpath1); */
			errval = read(fd, buf, bytes_read);

		if (errval < 0) {
			printf("Replay return value : %d\n", errval);
		} else {
			printf("Replay return value : %d\n", errval);
		}

		if (tracer->err == errval) {
/*			printf("Replay Success!\n"); */
			return true;
		} else {
/*			printf("Replay Failed!\n"); */
			return false;
		}

	case WRITE_CALL:
/*		printf("---->>>> Replay write. %s\n", fullpath1);
		printf("---->>>> Content : %s\n", content);
		printf("FD for writing : %d\n", fd); */

		errval = write(fd, content, strlen(content));
/*		perror("Value : "); */
		if (errval < 0) {
			printf("Replay return value : %d\n", errval);
		} else {
			printf("Replay return value : %d\n", errval);
		}

		if (tracer->err == errval) {
/*			printf("Replay Success!\n"); */
			return true;
		} else {
/*			printf("Replay Failed!\n"); */
			return false;
		}

	case RELEASE_CALL:
/*		printf("---->>>> Replay release. %s\n", fullpath1);
		printf("FD for closing : %d\n", fd); */
		errval = close(fd);

		if (errval < 0) {
			printf("Replay return value : %d\n", errval);
		} else {
			printf("Replay return value : %d\n", errval);
		}

		if (tracer->err == errval) {
/*			printf("Replay Success!\n"); */
			return true;
		} else {
/*			printf("Replay Failed!\n"); */
			return false;
		}

	case CREATE_CALL:
/*		printf("---->>>> Replay Create %s\n", fullpath1); */
		errval = open(fullpath1, tracer->open_flags, tracer->permission_mode);

		if (errval < 0) {
			printf("Replay return value : %d\n", errval);
		} else {
			printf("Replay return value : %d\n", errval);
		}

		if (tracer->err == errval) {
/*			printf("Replay Success!\n"); */
			return true;
		} else {
/*			printf("Replay Failed!\n"); */
			return false;
		}

	case LINK_CALL:
/*		printf("---->>>> Replay Link.%s\n", fullpath1);
		printf("---->>>> Replay Link.%s\n", fullpath2); */
		errval = link(fullpath1, fullpath2);

		if (errval < 0) {
			printf("Replay return value : %d\n", errval);
		} else {
			printf("Replay return value : %d\n", errval);
		}

		if (tracer->err == errval) {
/*			printf("Replay Success!\n"); */
			return true;
		} else {
/*			printf("Replay Failed!\n"); */
			return false;
		}

	case UNLINK_CALL:
/*		printf("---->>>> Replay Unlink.%s\n", fullpath1); */
		errval = unlink(fullpath1);

		if (errval < 0) {
			printf("Replay return value : %d\n", errval);
		} else {
			printf("Replay return value : %d\n", errval);
		}

		if (tracer->err == errval) {
/*			printf("Replay Success!\n"); */
			return true;
		} else {
/*			printf("Replay Failed!\n"); */
			return false;
		}

	case SYMLINK_CALL:
/*		printf("---->>>> Replay Symlink.%s\n", fullpath1);
		printf("---->>>> Replay Symlink.%s\n", path2); */
		errval = symlink(fullpath1, path2);

		if (errval < 0) {
			printf("Replay return value : %d\n", errval);
		} else {
			printf("Replay return value : %d\n", errval);
		}

		if (tracer->err == errval) {
/*			printf("Replay Success!\n"); */
			return true;
		} else {
/*			printf("Replay Failed!\n"); */
			return false;
		}

	case MKDIR_CALL:
/*		printf("---->>>> Replay Makedir %s\n", fullpath1); */
		errval = mkdir(fullpath1, tracer->permission_mode);

		if (errval < 0) {
			printf("Replay return value : %d\n", errval);
		} else {
			printf("Replay return value : %d\n", errval);
		}

		if (tracer->err == errval) {
/*			printf("Replay Success!\n"); */
			return true;
		} else {
/*			printf("Replay Failed!\n"); */
			return false;
		}

	case RMDIR_CALL:
/*		printf("---->>>> Replay Remove dir %s\n", fullpath1); */
		errval = rmdir(fullpath1);

		if (errval < 0) {
			printf("Replay return value : %d\n", errval);
		} else {
			printf("Replay return value : %d\n", errval);
		}

		if (tracer->err == errval) {
/*			printf("Replay Success!\n"); */
			return true;
		} else {
/*			printf("Replay Failed!\n"); */
			return false;
		}

	case MKNOD_CALL:
/*		printf("---->>>> Replay Mknod %s\n", fullpath1); */
		errval = mknod(fullpath1, tracer->permission_mode, tracer->open_flags);

		if (errval < 0) {
			printf("Replay return value : %d\n", errval);
		} else {
			printf("Replay return value : %d\n", errval);
		}

		if (tracer->err == errval) {
/*			printf("Replay Success!\n"); */
			return true;
		} else {
/*			printf("Replay Failed!\n"); */
			return false;
		}

	case RENAME_CALL:
/*		printf("---->>>> Replay rename %s\n", fullpath1);
		printf("---->>>> Replay rename %s\n", fullpath2); */
		errval = rename(fullpath1, fullpath2);

		if (errval < 0) {
			printf("Replay return value : %d\n", errval);
		} else {
			printf("Replay return value : %d\n", errval);
		}

		if (tracer->err == errval) {
/*			printf("Replay Success!\n"); */
			return true;
		} else {
/*			printf("Replay Failed!\n"); */
			return false;
		}

	}

	if (fullpath1 != NULL) {
		free(fullpath1);
	}
	if (fullpath2 != NULL) {
		free(fullpath2);
	}
	return true;
}

void print_record(struct trfs_trace_ds *tracer, char *path1, char *path2, char *content)
{
	char *replay_path = "/usr/src/replay";
	char *fullpath1 = NULL;
	char *fullpath2 = NULL;

	fullpath1 = malloc(strlen(replay_path) + strlen(path1));
	strcpy(fullpath1, replay_path);
	strcat(fullpath1, path1);

	if (path2 != NULL) {
		fullpath2 = malloc(strlen(replay_path) + strlen(path2));
		strcpy(fullpath2, replay_path);
		strcat(fullpath2, path2);
	}

	printf("Record details ->\n\n");
	printf("Record ID : %d\n", tracer->record_id);

	switch (tracer->record_type) {
	case OPEN_CALL:
		printf("System call : open()\n");
		printf("Filepath to open : %s\n", fullpath1);
		printf("Open flags : %d\n", tracer->open_flags);
		printf("Permission mode : %d\n", tracer->permission_mode);
		break;
	case READ_CALL:
		printf("System call : read()\n");
		printf("Filepath to read : %s\n", fullpath1);
		break;
	case WRITE_CALL:
		printf("System call : write()\n");
		printf("Filepath where to write: %s\n", fullpath1);
		printf("Write content : %s\n", content);
		break;
	case RELEASE_CALL:
		printf("System call : close()\n");
		printf("Filepath to be closed : %s\n", fullpath1);
		break;
	case CREATE_CALL:
		printf("System call : create()\n");
		printf("Filepath to be created : %s\n", fullpath1);
		break;
	case LINK_CALL:
		printf("System call : link()\n");
		printf("Source filepath : %s\n", fullpath1);
		printf("Target filepath : %s\n", fullpath2);
		break;
	case UNLINK_CALL:
		printf("System call : unlink()\n");
		printf("Filepath to be unlinked : %s\n", fullpath1);
		break;
	case SYMLINK_CALL:
		printf("System call : symlink()\n");
		printf("Source filepathd : %s\n", fullpath1);
		printf("Target filepath : %s\n", path2);
		break;
	case MKDIR_CALL:
		printf("System call : mkdir()\n");
		printf("Dir path to be created : %s\n", fullpath1);
		break;
	case RMDIR_CALL:
		printf("System call : rmdir()\n");
		printf("Dir path to be removed : %s\n", fullpath1);
		break;
	case MKNOD_CALL:
		printf("System call : mknod()\n");
		printf("Filepath to be created : %s\n", fullpath1);
		break;
	case RENAME_CALL:
		printf("System call : rename()\n");
		printf("Source filepath : %s\n", fullpath1);
		printf("Destination filepath : %s\n", fullpath2);
		break;

	}
	printf("Return value : %d\n\n", tracer->err);
	return;
}

int main(int argc, char *argv[])
{
	int rc = 0;
	extern int optind;
	int c;
	bool errVal = false;
	bool nVal, sVal;
	int param_len = 0;
	int fd = -1;
	int fileread;
	void *buffer = NULL;
	char *file_path = NULL;
	struct trfs_trace_ds *tracer;
	char *pathbuf1 = NULL;
	char *pathbuf2 = NULL;
	char *content = NULL;
	struct stat *stat_buf;
	int prev_rec_size = 0;
	bool replay = false;
	bool abort = false;
	bool replay_res = false;

/*	printf("ARGC : %d\n", argc); */

	if (argc < 2) {
		printf("Input parameters missing.");
		goto exit;
	}

	if (argc > 3) {
		printf("Too many input parameters.");
	}

	while ((c = getopt(argc, argv, "ns")) != -1) {

		switch (c) {
		case 'n':
			param_len++;
			nVal = true;
			break;

		case 's':
			param_len++;
			sVal = true;
			break;

		case '?':
			errVal = true;
			break;
		}

	}

	if (errVal) {
		printf("Input options are incorrect. Terminating treplay call.\n");
		goto exit;
	}

	if (param_len > 1) {
		printf("Cannot take more than 1 parameters. Terminating treplay call.\n");
		goto exit;
	}

	if (argc == 2 && param_len > 0) {
		printf("Wrong input parameters. Terminating treplay call.\n");
		goto exit;
	}

	if (argc == 3) {
		if (!nVal && sVal) {
/*			printf("Replay and abort replaying if error occurs\n"); */
			replay = true;
			abort = true;
		} else if (nVal && !sVal) {
/*			printf("Only print all the records. Don't replay\n"); */
			replay = false;
		}
	} else {
/*		printf("Replay and continue if any error occurs\n"); */
		replay = true;
		abort = false;
	}

	file_path = argv[argc-1];
/*	printf("Path of treplay file : %s\n", file_path); */

	fd = open(file_path, O_RDONLY);

	stat_buf = (struct stat *) malloc(sizeof(struct stat));
	stat(file_path, stat_buf);
/*	printf("File size :  %d\n", stat_buf->st_size); */

	if (stat_buf->st_size == 0) {
		goto exit_i;
	}

	buffer = malloc(stat_buf->st_size);
	fileread = read(fd, buffer, stat_buf->st_size);
	printf("Fileread : %d\n", fileread);

	while (prev_rec_size < (stat_buf->st_size)-1) {
		tracer = malloc(sizeof(struct trfs_trace_ds));
		memcpy(tracer, buffer + prev_rec_size, sizeof(struct trfs_trace_ds));
/*
		printf("Record size : %hd\n", tracer->path_len);
		printf("Record ID : %d\n", tracer->record_id);
		printf("Record type : %d\n", tracer->record_type);
		printf("Record Open flags : %d\n", tracer->open_flags);
		printf("Record permission mode : %d\n", tracer->permission_mode);
		printf("Err value : %d\n", tracer->err);
*/
		pathbuf1 = malloc(tracer->path_len1);
		memcpy(pathbuf1, buffer + prev_rec_size + sizeof(struct trfs_trace_ds), tracer->path_len1);

		if (tracer->path_len2 != 0) {
			pathbuf2 = malloc(tracer->path_len2);
			memcpy(pathbuf2, buffer + prev_rec_size + sizeof(struct trfs_trace_ds)
					+ tracer->path_len1, tracer->path_len2);
		}

		if (tracer->write_len != 0) {
			content = malloc(tracer->write_len);
			memcpy(content, buffer + prev_rec_size + sizeof(struct trfs_trace_ds)
					+ tracer->path_len1 + tracer->path_len2, tracer->write_len);
/*			printf("--->>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Content : %s\n", content); */
		}

/*		printf("File Path : %s\n", pathbuf1);
		printf("New file path : %s\n", pathbuf1); */

		if (replay) {
/*			printf("Replaying records\n"); */
			print_record(tracer, pathbuf1, pathbuf2, content);
			replay_res = replay_record(tracer, pathbuf1, pathbuf2, content);
			printf("-----------------------------------------------------------------\n\n");

			if (abort && !replay_res) {
				printf("Deviation occured. Terminating replay!!!\n");
				goto exit;
			}
		} else {
/*			printf("Printing the records\n"); */
			print_record(tracer, pathbuf1, pathbuf2, content);
			printf("-----------------------------------------------------------------\n\n");
		}

		prev_rec_size = prev_rec_size + sizeof(struct trfs_trace_ds) +
						tracer->path_len1 + tracer->path_len2 + tracer->write_len;

/*		printf("Previous record length : %d\n", prev_rec_size); */
	}

exit:

	if (buffer != NULL) {
		free(buffer);
	}
	if (tracer != NULL) {
		free(tracer);
	}
	if (pathbuf1 != NULL) {
		free(pathbuf1);
	}
	if (pathbuf2 != NULL) {
		free(pathbuf2);
	}
	if (content != NULL) {
		free(content);
	}

exit_i:
	close(fd);
	exit(rc);
}
