#define OPEN_CALL 0
#define READ_CALL 1
#define WRITE_CALL 2
#define RELEASE_CALL 3
#define CREATE_CALL 4
#define LINK_CALL 5
#define UNLINK_CALL 6
#define SYMLINK_CALL 7
#define MKDIR_CALL 8
#define RMDIR_CALL 9
#define MKNOD_CALL 10
#define RENAME_CALL 11

unsigned long BITMAP_MAX = 0xfff;

struct trfs_trace_ds {
    short path_len1;
    short path_len2;
	int write_len;
    int record_id;
    unsigned char record_type;
    unsigned int open_flags;
    unsigned short permission_mode;
    int err;
};

