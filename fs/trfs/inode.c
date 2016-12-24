/*
 * Copyright (c) 1998-2015 Erez Zadok
 * Copyright (c) 2009	   Shrikar Archak
 * Copyright (c) 2003-2015 Stony Brook University
 * Copyright (c) 2003-2015 The Research Foundation of SUNY
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include "trfs.h"

static int trfs_create(struct inode *dir, struct dentry *dentry,
			 umode_t mode, bool want_excl)
{
	int err;
	struct dentry *lower_dentry;
	struct dentry *lower_parent_dentry = NULL;
	struct path lower_path;

	struct trfs_sb_info *sb_info;
	struct file *file_op;
    unsigned long mask;

	char *filename;
	mm_segment_t old_fs;
	loff_t offset = 0;
	int errval = 0;

	struct trfs_trace_ds *trace_str = NULL;
	int rec_id;
	unsigned char rec_type = CREATE_CALL;
	unsigned int open_flags;
	umode_t perm_mode;
	short len_path;
	char *path_name = NULL;
	char *buffer = NULL;
	char *writer = NULL;

	printk("inode.c : Intercepting trfs_create--->>>>\n");

	trfs_get_lower_path(dentry, &lower_path);
	lower_dentry = lower_path.dentry;
	lower_parent_dentry = lock_parent(lower_dentry);

	err = vfs_create(d_inode(lower_parent_dentry), lower_dentry, mode,
			 want_excl);

	sb_info = (struct trfs_sb_info *) dir->i_sb->s_fs_info;
	mask = sb_info->mask;

	if (((mask >> rec_type) & 1) == 1) {
/*		printk("Tracer create.\n"); */
		if (dentry != NULL) {

			filename = sb_info->mount_path->tfile_path;
			rec_id = sb_info->record_id;
			open_flags = dir->i_opflags;
			perm_mode = mode;

			buffer = kmalloc(256, GFP_KERNEL);
			path_name = dentry_path_raw(dentry, buffer, 256);
			len_path = strlen(path_name);
			printk("Pathname : %s\n", path_name);
			file_op = filp_open(filename, O_WRONLY|O_CREAT|O_APPEND, 0);

			trace_str = (struct trfs_trace_ds *) kmalloc(sizeof(struct trfs_trace_ds), GFP_KERNEL);

			old_fs = get_fs();
			set_fs(KERNEL_DS);
/*
			printk("Record ID : %d\n", rec_id);
			printk("Record Size : %d\n", len_path);
			printk("Record Type : %d\n", rec_type);
			printk("Record Open flags : %d\n", open_flags);
			printk("Record Permission mode : %d\n", perm_mode);
			printk("Record Err : %d\n", err);
*/
			trace_str->path_len1 = len_path;
			trace_str->path_len2 = 0;
			trace_str->write_len = 0;
			trace_str->record_id = rec_id;
			trace_str->record_type = rec_type;
			trace_str->open_flags = open_flags;
			trace_str->permission_mode = perm_mode;
			trace_str->err = err;

			writer = (char *)kmalloc(sizeof(struct trfs_trace_ds), GFP_KERNEL);
			memcpy(writer, trace_str, sizeof(struct trfs_trace_ds));

			errval = vfs_write(file_op, writer, sizeof(struct trfs_trace_ds), &offset);
			errval = vfs_write(file_op, path_name, len_path, &offset);

			set_fs(old_fs);

			filp_close(file_op, NULL);

			sb_info->record_id = ++rec_id;

		}
    } else {
/*		printk("Don't trace create.\n"); */
    }

	if (err)
		goto out;
	err = trfs_interpose(dentry, dir->i_sb, &lower_path);
	if (err)
		goto out;
	fsstack_copy_attr_times(dir, trfs_lower_inode(dir));
	fsstack_copy_inode_size(dir, d_inode(lower_parent_dentry));

out:
	unlock_dir(lower_parent_dentry);
	trfs_put_lower_path(dentry, &lower_path);
	if (trace_str != NULL) {
		kfree(trace_str);
	}
	if (writer != NULL) {
		kfree(writer);
	}
	if (buffer != NULL) {
		kfree(buffer);
	}
	return err;
}

static int trfs_link(struct dentry *old_dentry, struct inode *dir,
		       struct dentry *new_dentry)
{
	struct dentry *lower_old_dentry;
	struct dentry *lower_new_dentry;
	struct dentry *lower_dir_dentry;
	u64 file_size_save;
	int err;
	struct path lower_old_path, lower_new_path;

	struct trfs_sb_info *sb_info;
	struct file *file_op;
    unsigned long mask;

	char *filename;
	mm_segment_t old_fs;
	loff_t offset = 0;
	int errval = 0;

	struct trfs_trace_ds *trace_str = NULL;
	int rec_id;
	unsigned char rec_type = LINK_CALL;
	unsigned int open_flags;
	umode_t perm_mode;
	short len_path1;
	short len_path2;
	char *path_name1 = NULL;
	char *path_name2 = NULL;
	char *buffer = NULL;
	char *writer = NULL;

/*	printk("inode.c : Intercepting trfs_link--->>>>\n"); */

	file_size_save = i_size_read(d_inode(old_dentry));
	trfs_get_lower_path(old_dentry, &lower_old_path);
	trfs_get_lower_path(new_dentry, &lower_new_path);
	lower_old_dentry = lower_old_path.dentry;
	lower_new_dentry = lower_new_path.dentry;
	lower_dir_dentry = lock_parent(lower_new_dentry);

	err = vfs_link(lower_old_dentry, d_inode(lower_dir_dentry),
		       lower_new_dentry, NULL);
	if (err || !d_inode(lower_new_dentry))
		goto out;

	err = trfs_interpose(new_dentry, dir->i_sb, &lower_new_path);
	if (err)
		goto out;

	sb_info = (struct trfs_sb_info *) old_dentry->d_sb->s_fs_info;;
	mask = sb_info->mask;

	if (((mask >> rec_type) & 1) == 1) {
		printk("Tracer link.\n");
		if (old_dentry != NULL) {

			filename = sb_info->mount_path->tfile_path;
			rec_id = sb_info->record_id;
			open_flags = 0;
			perm_mode = 0;

			buffer = kmalloc(256, GFP_KERNEL);
			path_name1 = dentry_path_raw(old_dentry, buffer, 256);
			len_path1 = strlen(path_name1);
			path_name2 = dentry_path_raw(new_dentry, buffer, 256);
			len_path2 = strlen(path_name2);
			printk("Pathname1 : %s\n", path_name1);
			printk("Pathname2 : %s\n", path_name2);

			file_op = filp_open(filename, O_WRONLY|O_CREAT|O_APPEND, 0);

			trace_str = (struct trfs_trace_ds *) kmalloc(sizeof(struct trfs_trace_ds), GFP_KERNEL);

			old_fs = get_fs();
			set_fs(KERNEL_DS);
/*
			printk("Record ID : %d\n", rec_id);
			printk("Record Size : %d\n", len_path);
			printk("Record Type : %d\n", rec_type);
			printk("Record Open flags : %d\n", open_flags);
			printk("Record Permission mode : %d\n", perm_mode);
			printk("Record Err : %d\n", err);
*/
			trace_str->path_len1 = len_path1;
			trace_str->path_len2 = len_path2;
			trace_str->write_len = 0;
			trace_str->record_id = rec_id;
			trace_str->record_type = rec_type;
			trace_str->open_flags = open_flags;
			trace_str->permission_mode = perm_mode;
			trace_str->err = err;

			writer = (char *)kmalloc(sizeof(struct trfs_trace_ds), GFP_KERNEL);
			memcpy(writer, trace_str, sizeof(struct trfs_trace_ds));

			errval = vfs_write(file_op, writer, sizeof(struct trfs_trace_ds), &offset);
			errval = vfs_write(file_op, path_name1, len_path1, &offset);
			errval = vfs_write(file_op, path_name2, len_path2, &offset);

			set_fs(old_fs);

			filp_close(file_op, NULL);

			sb_info->record_id = ++rec_id;

		}
    } else {
/*		printk("Don't trace link.\n"); */
    }

	fsstack_copy_attr_times(dir, d_inode(lower_new_dentry));
	fsstack_copy_inode_size(dir, d_inode(lower_new_dentry));
	set_nlink(d_inode(old_dentry),
		  trfs_lower_inode(d_inode(old_dentry))->i_nlink);
	i_size_write(d_inode(new_dentry), file_size_save);
out:
	unlock_dir(lower_dir_dentry);
	trfs_put_lower_path(old_dentry, &lower_old_path);
	trfs_put_lower_path(new_dentry, &lower_new_path);

	if (trace_str != NULL) {
		kfree(trace_str);
	}
	if (writer != NULL) {
		kfree(writer);
	}
	if (buffer != NULL) {
		kfree(buffer);
	}
	return err;
}

static int trfs_unlink(struct inode *dir, struct dentry *dentry)
{
	int err;
	struct dentry *lower_dentry;
	struct inode *lower_dir_inode = trfs_lower_inode(dir);
	struct dentry *lower_dir_dentry;
	struct path lower_path;

	struct trfs_sb_info *sb_info;
	struct file *file_op;
    unsigned long mask;

	char *filename;
	mm_segment_t old_fs;
	loff_t offset = 0;
	int errval = 0;

	struct trfs_trace_ds *trace_str = NULL;
	int rec_id;
	unsigned char rec_type = UNLINK_CALL;
	unsigned int open_flags;
	umode_t perm_mode;
	short len_path1;
	char *path_name1 = NULL;
	char *buffer = NULL;
	char *writer = NULL;

/*	printk("inode.c : Intercepting trfs_unlink--->>>>\n"); */

	trfs_get_lower_path(dentry, &lower_path);
	lower_dentry = lower_path.dentry;
	dget(lower_dentry);
	lower_dir_dentry = lock_parent(lower_dentry);

	err = vfs_unlink(lower_dir_inode, lower_dentry, NULL);

	/*
	 * Note: unlinking on top of NFS can cause silly-renamed files.
	 * Trying to delete such files results in EBUSY from NFS
	 * below.  Silly-renamed files will get deleted by NFS later on, so
	 * we just need to detect them here and treat such EBUSY errors as
	 * if the upper file was successfully deleted.
	 */
	if (err == -EBUSY && lower_dentry->d_flags & DCACHE_NFSFS_RENAMED)
		err = 0;
	if (err)
		goto out;

	sb_info = (struct trfs_sb_info *) dentry->d_sb->s_fs_info;;
	mask = sb_info->mask;

	if (((mask >> rec_type) & 1) == 1) {
		printk("Tracer unlink.\n");
		if (dentry != NULL) {

			filename = sb_info->mount_path->tfile_path;
			rec_id = sb_info->record_id;
			open_flags = 0;
			perm_mode = 0;

			buffer = kmalloc(256, GFP_KERNEL);
			path_name1 = dentry_path_raw(dentry, buffer, 256);
			len_path1 = strlen(path_name1);
			printk("Pathname1 : %s\n", path_name1);

			file_op = filp_open(filename, O_WRONLY|O_CREAT|O_APPEND, 0);

			trace_str = (struct trfs_trace_ds *) kmalloc(sizeof(struct trfs_trace_ds), GFP_KERNEL);

			old_fs = get_fs();
			set_fs(KERNEL_DS);
/*
			printk("Record ID : %d\n", rec_id);
			printk("Record Size : %d\n", len_path);
			printk("Record Type : %d\n", rec_type);
			printk("Record Open flags : %d\n", open_flags);
			printk("Record Permission mode : %d\n", perm_mode);
			printk("Record Err : %d\n", err);
*/
			trace_str->path_len1 = len_path1;
			trace_str->path_len2 = 0;
			trace_str->write_len = 0;
			trace_str->record_id = rec_id;
			trace_str->record_type = rec_type;
			trace_str->open_flags = open_flags;
			trace_str->permission_mode = perm_mode;
			trace_str->err = err;

			writer = (char *)kmalloc(sizeof(struct trfs_trace_ds), GFP_KERNEL);
			memcpy(writer, trace_str, sizeof(struct trfs_trace_ds));

			errval = vfs_write(file_op, writer, sizeof(struct trfs_trace_ds), &offset);
			errval = vfs_write(file_op, path_name1, len_path1, &offset);

			set_fs(old_fs);

			filp_close(file_op, NULL);

			sb_info->record_id = ++rec_id;

		}
    } else {
/*		printk("Don't trace unlink.\n"); */
    }

	fsstack_copy_attr_times(dir, lower_dir_inode);
	fsstack_copy_inode_size(dir, lower_dir_inode);
	set_nlink(d_inode(dentry),
		  trfs_lower_inode(d_inode(dentry))->i_nlink);
	d_inode(dentry)->i_ctime = dir->i_ctime;
	d_drop(dentry); /* this is needed, else LTP fails (VFS won't do it) */
out:
	unlock_dir(lower_dir_dentry);
	dput(lower_dentry);
	trfs_put_lower_path(dentry, &lower_path);

	if (trace_str != NULL) {
		kfree(trace_str);
	}
	if (writer != NULL) {
		kfree(writer);
	}
	if (buffer != NULL) {
		kfree(buffer);
	}
	return err;
}

static int trfs_symlink(struct inode *dir, struct dentry *dentry,
			  const char *symname)
{
	int err;
	struct dentry *lower_dentry;
	struct dentry *lower_parent_dentry = NULL;
	struct path lower_path;

	struct trfs_sb_info *sb_info;
	struct file *file_op;
    unsigned long mask;

	char *filename;
	mm_segment_t old_fs;
	loff_t offset = 0;
	int errval = 0;

	struct trfs_trace_ds *trace_str = NULL;
	int rec_id;
	unsigned char rec_type = SYMLINK_CALL;
	unsigned int open_flags;
	umode_t perm_mode;
	short len_path1;
	short len_path2;
	char *path_name1 = NULL;
	char *buffer = NULL;
	char *writer = NULL;

/*	printk("inode.c : Intercepting trfs_symlink--->>>>\n"); */

	trfs_get_lower_path(dentry, &lower_path);
	lower_dentry = lower_path.dentry;
	lower_parent_dentry = lock_parent(lower_dentry);

	err = vfs_symlink(d_inode(lower_parent_dentry), lower_dentry, symname);
	if (err)
		goto out;
	err = trfs_interpose(dentry, dir->i_sb, &lower_path);
	if (err)
		goto out;

	sb_info = (struct trfs_sb_info *) dentry->d_sb->s_fs_info;;
	mask = sb_info->mask;

	if (((mask >> rec_type) & 1) == 1) {
		printk("Tracer symlink.\n");
		if (dentry != NULL) {

			filename = sb_info->mount_path->tfile_path;
			rec_id = sb_info->record_id;
			open_flags = 0;
			perm_mode = 0;

			buffer = kmalloc(256, GFP_KERNEL);
			path_name1 = dentry_path_raw(dentry, buffer, 256);
			len_path1 = strlen(path_name1);
			len_path2 = strlen(symname);
			printk("Pathname1 : %s\n", path_name1);
			printk("Pathname2 : %s\n", symname);

			file_op = filp_open(filename, O_WRONLY|O_CREAT|O_APPEND, 0);

			trace_str = (struct trfs_trace_ds *) kmalloc(sizeof(struct trfs_trace_ds), GFP_KERNEL);

			old_fs = get_fs();
			set_fs(KERNEL_DS);
/*
			printk("Record ID : %d\n", rec_id);
			printk("Record Size : %d\n", len_path);
			printk("Record Type : %d\n", rec_type);
			printk("Record Open flags : %d\n", open_flags);
			printk("Record Permission mode : %d\n", perm_mode);
			printk("Record Err : %d\n", err);
*/
			trace_str->path_len1 = len_path1;
			trace_str->path_len2 = len_path2;
			trace_str->write_len = 0;
			trace_str->record_id = rec_id;
			trace_str->record_type = rec_type;
			trace_str->open_flags = open_flags;
			trace_str->permission_mode = perm_mode;
			trace_str->err = err;

			writer = (char *)kmalloc(sizeof(struct trfs_trace_ds), GFP_KERNEL);
			memcpy(writer, trace_str, sizeof(struct trfs_trace_ds));

			errval = vfs_write(file_op, writer, sizeof(struct trfs_trace_ds), &offset);
			errval = vfs_write(file_op, path_name1, len_path1, &offset);
			errval = vfs_write(file_op, symname, len_path2, &offset);

			set_fs(old_fs);

			filp_close(file_op, NULL);

			sb_info->record_id = ++rec_id;

		}
    } else {
/*		printk("Don't trace symlink.\n"); */
    }

	fsstack_copy_attr_times(dir, trfs_lower_inode(dir));
	fsstack_copy_inode_size(dir, d_inode(lower_parent_dentry));

out:
	unlock_dir(lower_parent_dentry);
	trfs_put_lower_path(dentry, &lower_path);

	if (trace_str != NULL) {
		kfree(trace_str);
	}
	if (writer != NULL) {
		kfree(writer);
	}
	if (buffer != NULL) {
		kfree(buffer);
	}
	return err;
}

static int trfs_mkdir(struct inode *dir, struct dentry *dentry, umode_t mode)
{
	int err;
	struct dentry *lower_dentry;
	struct dentry *lower_parent_dentry = NULL;
	struct path lower_path;

	struct trfs_sb_info *sb_info;
	struct file *file_op;
	unsigned long mask;

	char *filename;
	mm_segment_t old_fs;
	loff_t offset = 0;
	int errval = 0;

	struct trfs_trace_ds *trace_str = NULL;
	int rec_id;
	unsigned char rec_type = MKDIR_CALL;
	unsigned int open_flags;
	umode_t perm_mode;
	short len_path1;
	char *path_name1 = NULL;
	char *buffer = NULL;
	char *writer = NULL;

/*	printk("inode.c : Intercepting trfs_mkdir --->>>>\n"); */

	trfs_get_lower_path(dentry, &lower_path);
	lower_dentry = lower_path.dentry;
	lower_parent_dentry = lock_parent(lower_dentry);

	err = vfs_mkdir(d_inode(lower_parent_dentry), lower_dentry, mode);
	if (err)
		goto out;

	sb_info = (struct trfs_sb_info *) dir->i_sb->s_fs_info;
	mask = sb_info->mask;

	if (((mask >> rec_type) & 1) == 1) {
		printk("Tracer mkdir.\n");
		if (dentry != NULL) {

			filename = sb_info->mount_path->tfile_path;
			rec_id = sb_info->record_id;
			open_flags = dir->i_opflags;
			perm_mode = mode;

			buffer = kmalloc(256, GFP_KERNEL);
			path_name1 = dentry_path_raw(dentry, buffer, 256);
			len_path1 = strlen(path_name1);

			file_op = filp_open(filename, O_WRONLY|O_CREAT|O_APPEND, 0);

			trace_str = (struct trfs_trace_ds *) kmalloc(sizeof(struct trfs_trace_ds), GFP_KERNEL);

			old_fs = get_fs();
			set_fs(KERNEL_DS);
/*
			printk("Record ID : %d\n", rec_id);
			printk("Record Size : %d\n", len_path);
			printk("Record Type : %d\n", rec_type);
			printk("Record Open flags : %d\n", open_flags);
			printk("Record Permission mode : %d\n", perm_mode);
			printk("Record Err : %d\n", err);
*/
			trace_str->path_len1 = len_path1;
			trace_str->path_len2 = 0;
			trace_str->write_len = 0;
			trace_str->record_id = rec_id;
			trace_str->record_type = rec_type;
			trace_str->open_flags = open_flags;
			trace_str->permission_mode = perm_mode;
			trace_str->err = err;

			writer = (char *)kmalloc(sizeof(struct trfs_trace_ds), GFP_KERNEL);
			memcpy(writer, trace_str, sizeof(struct trfs_trace_ds));

			errval = vfs_write(file_op, writer, sizeof(struct trfs_trace_ds), &offset);
			errval = vfs_write(file_op, path_name1, len_path1, &offset);

			set_fs(old_fs);

			filp_close(file_op, NULL);

			sb_info->record_id = ++rec_id;

		}
	} else {
/*		printk("Don't trace mkdir.\n"); */
	}

	err = trfs_interpose(dentry, dir->i_sb, &lower_path);
	if (err)
		goto out;

	fsstack_copy_attr_times(dir, trfs_lower_inode(dir));
	fsstack_copy_inode_size(dir, d_inode(lower_parent_dentry));
	/* update number of links on parent directory */
	set_nlink(dir, trfs_lower_inode(dir)->i_nlink);

out:
	unlock_dir(lower_parent_dentry);
	trfs_put_lower_path(dentry, &lower_path);

	if (trace_str != NULL) {
		kfree(trace_str);
	}
	if (writer != NULL) {
		kfree(writer);
	}
	if (buffer != NULL) {
		kfree(buffer);
	}

	return err;
}

static int trfs_rmdir(struct inode *dir, struct dentry *dentry)
{
	struct dentry *lower_dentry;
	struct dentry *lower_dir_dentry;
	int err;
	struct path lower_path;

	struct trfs_sb_info *sb_info;
	struct file *file_op;
	unsigned long mask;

	char *filename;
	mm_segment_t old_fs;
	loff_t offset = 0;
	int errval = 0;

	struct trfs_trace_ds *trace_str = NULL;
	int rec_id;
	unsigned char rec_type = RMDIR_CALL;
	unsigned int open_flags;
	umode_t perm_mode;
	short len_path1;
	char *path_name1 = NULL;
	char *buffer = NULL;
	char *writer = NULL;

/*	printk("inode.c : Intercepting trfs_rmdir --->>>>\n"); */

	trfs_get_lower_path(dentry, &lower_path);
	lower_dentry = lower_path.dentry;
	lower_dir_dentry = lock_parent(lower_dentry);

	err = vfs_rmdir(d_inode(lower_dir_dentry), lower_dentry);
	if (err)
		goto out;

	sb_info = (struct trfs_sb_info *) dir->i_sb->s_fs_info;
	mask = sb_info->mask;

	if (((mask >> rec_type) & 1) == 1) {
		printk("Tracer rmdir.\n");
		if (dentry != NULL) {
			filename = sb_info->mount_path->tfile_path;
			rec_id = sb_info->record_id;
			open_flags = dir->i_opflags;
			perm_mode = dir->i_mode;

			buffer = kmalloc(256, GFP_KERNEL);
			path_name1 = dentry_path_raw(dentry, buffer, 256);
			len_path1 = strlen(path_name1);

			file_op = filp_open(filename, O_WRONLY|O_CREAT|O_APPEND, 0);

			trace_str = (struct trfs_trace_ds *) kmalloc(sizeof(struct trfs_trace_ds), GFP_KERNEL);

			old_fs = get_fs();
			set_fs(KERNEL_DS);
/*
			printk("Record ID : %d\n", rec_id);
			printk("Record Size : %d\n", len_path);
			printk("Record Type : %d\n", rec_type);
			printk("Record Open flags : %d\n", open_flags);
			printk("Record Permission mode : %d\n", perm_mode);
			printk("Record Err : %d\n", err);
*/
			trace_str->path_len1 = len_path1;
			trace_str->path_len2 = 0;
			trace_str->write_len = 0;
			trace_str->record_id = rec_id;
			trace_str->record_type = rec_type;
			trace_str->open_flags = open_flags;
			trace_str->permission_mode = perm_mode;
			trace_str->err = err;

			writer = (char *)kmalloc(sizeof(struct trfs_trace_ds), GFP_KERNEL);
			memcpy(writer, trace_str, sizeof(struct trfs_trace_ds));

			errval = vfs_write(file_op, writer, sizeof(struct trfs_trace_ds), &offset);
			errval = vfs_write(file_op, path_name1, len_path1, &offset);

			set_fs(old_fs);

			filp_close(file_op, NULL);

			sb_info->record_id = ++rec_id;
		}
	} else {
/*		printk("Don't trace rmdir.\n"); */
	}

	d_drop(dentry);	/* drop our dentry on success (why not VFS's job?) */
	if (d_inode(dentry))
		clear_nlink(d_inode(dentry));
	fsstack_copy_attr_times(dir, d_inode(lower_dir_dentry));
	fsstack_copy_inode_size(dir, d_inode(lower_dir_dentry));
	set_nlink(dir, d_inode(lower_dir_dentry)->i_nlink);

out:
	unlock_dir(lower_dir_dentry);
	trfs_put_lower_path(dentry, &lower_path);

	if (trace_str != NULL) {
		kfree(trace_str);
	}
	if (writer != NULL) {
		kfree(writer);
	}
	if (buffer != NULL) {
		kfree(buffer);
	}

	return err;
}

static int trfs_mknod(struct inode *dir, struct dentry *dentry, umode_t mode,
			dev_t dev)
{
	int err;
	struct dentry *lower_dentry;
	struct dentry *lower_parent_dentry = NULL;
	struct path lower_path;

	struct trfs_sb_info *sb_info;
	struct file *file_op;
	unsigned long mask;

	char *filename;
	mm_segment_t old_fs;
	loff_t offset = 0;
	int errval = 0;

	struct trfs_trace_ds *trace_str = NULL;
	int rec_id;
	unsigned char rec_type = MKNOD_CALL;
	unsigned int open_flags;
	umode_t perm_mode;
	short len_path1;
	char *path_name1 = NULL;
	char *buffer = NULL;
	char *writer = NULL;

/*	printk("inode.c : Intercepting trfs_mknod--->>>>\n"); */

	trfs_get_lower_path(dentry, &lower_path);
	lower_dentry = lower_path.dentry;
	lower_parent_dentry = lock_parent(lower_dentry);

	err = vfs_mknod(d_inode(lower_parent_dentry), lower_dentry, mode, dev);
	if (err)
		goto out;

	err = trfs_interpose(dentry, dir->i_sb, &lower_path);
	if (err)
		goto out;

	sb_info = (struct trfs_sb_info *) dir->i_sb->s_fs_info;
	mask = sb_info->mask;

	if (((mask >> rec_type) & 1) == 1) {
		printk("Tracer mknod.\n");
		if (dentry != NULL) {
			filename = sb_info->mount_path->tfile_path;
			rec_id = sb_info->record_id;
			open_flags = dev;
			perm_mode = mode;

			buffer = kmalloc(256, GFP_KERNEL);
			path_name1 = dentry_path_raw(dentry, buffer, 256);
			len_path1 = strlen(path_name1);

			file_op = filp_open(filename, O_WRONLY|O_CREAT|O_APPEND, 0);

			trace_str = (struct trfs_trace_ds *) kmalloc(sizeof(struct trfs_trace_ds), GFP_KERNEL);

			old_fs = get_fs();
			set_fs(KERNEL_DS);
/*
			printk("Record ID : %d\n", rec_id);
			printk("Record Size : %d\n", len_path);
			printk("Record Type : %d\n", rec_type);
			printk("Record Open flags : %d\n", open_flags);
			printk("Record Permission mode : %d\n", perm_mode);
			printk("Record Err : %d\n", err);
*/
			trace_str->path_len1 = len_path1;
			trace_str->path_len2 = 0;
			trace_str->write_len = 0;
			trace_str->record_id = rec_id;
			trace_str->record_type = rec_type;
			trace_str->open_flags = open_flags;
			trace_str->permission_mode = perm_mode;
			trace_str->err = err;

			writer = (char *)kmalloc(sizeof(struct trfs_trace_ds), GFP_KERNEL);
			memcpy(writer, trace_str, sizeof(struct trfs_trace_ds));

			errval = vfs_write(file_op, writer, sizeof(struct trfs_trace_ds), &offset);
			errval = vfs_write(file_op, path_name1, len_path1, &offset);

			set_fs(old_fs);

			filp_close(file_op, NULL);

			sb_info->record_id = ++rec_id;
		}
	} else {
/*		printk("Don't trace mknod.\n"); */
	}

	fsstack_copy_attr_times(dir, trfs_lower_inode(dir));
	fsstack_copy_inode_size(dir, d_inode(lower_parent_dentry));

out:
	unlock_dir(lower_parent_dentry);
	trfs_put_lower_path(dentry, &lower_path);

	if (trace_str != NULL) {
		kfree(trace_str);
	}
	if (writer != NULL) {
		kfree(writer);
	}
	if (buffer != NULL) {
		kfree(buffer);
	}
	return err;
}

/*
 * The locking rules in trfs_rename are complex.  We could use a simpler
 * superblock-level name-space lock for renames and copy-ups.
 */
static int trfs_rename(struct inode *old_dir, struct dentry *old_dentry,
			 struct inode *new_dir, struct dentry *new_dentry)
{
	int err = 0;
	struct dentry *lower_old_dentry = NULL;
	struct dentry *lower_new_dentry = NULL;
	struct dentry *lower_old_dir_dentry = NULL;
	struct dentry *lower_new_dir_dentry = NULL;
	struct dentry *trap = NULL;
	struct path lower_old_path, lower_new_path;

	struct trfs_sb_info *sb_info;
	struct file *file_op;
	unsigned long mask;

	char *filename;
	mm_segment_t old_fs;
	loff_t offset = 0;
	int errval = 0;

	struct trfs_trace_ds *trace_str = NULL;
	int rec_id;
	unsigned char rec_type = RENAME_CALL;
	unsigned int open_flags;
	umode_t perm_mode;
	short len_path1;
	short len_path2;
	char *path_name1 = NULL;
	char *path_name2 = NULL;
	char *buffer = NULL;
	char *writer = NULL;

/*	printk("inode.c : Intercepting trfs_rename --->>>>\n"); */

	trfs_get_lower_path(old_dentry, &lower_old_path);
	trfs_get_lower_path(new_dentry, &lower_new_path);
	lower_old_dentry = lower_old_path.dentry;
	lower_new_dentry = lower_new_path.dentry;
	lower_old_dir_dentry = dget_parent(lower_old_dentry);
	lower_new_dir_dentry = dget_parent(lower_new_dentry);

	trap = lock_rename(lower_old_dir_dentry, lower_new_dir_dentry);
	/* source should not be ancestor of target */
	if (trap == lower_old_dentry) {
		err = -EINVAL;
		goto out;
	}
	/* target should not be ancestor of source */
	if (trap == lower_new_dentry) {
		err = -ENOTEMPTY;
		goto out;
	}

	err = vfs_rename(d_inode(lower_old_dir_dentry), lower_old_dentry,
			 d_inode(lower_new_dir_dentry), lower_new_dentry,
			 NULL, 0);
	if (err)
		goto out;

	sb_info = (struct trfs_sb_info *) old_dir->i_sb->s_fs_info;
	mask = sb_info->mask;

	if (((mask >> rec_type) & 1) == 1) {
		printk("Tracer mknod.\n");
		if (old_dentry != NULL) {
			filename = sb_info->mount_path->tfile_path;
			rec_id = sb_info->record_id;
			open_flags = 0;
			perm_mode = 0;

			buffer = kmalloc(256, GFP_KERNEL);
			path_name1 = dentry_path_raw(old_dentry, buffer, 256);
			len_path1 = strlen(path_name1);
			path_name2 = dentry_path_raw(new_dentry, buffer, 256);
			len_path2 = strlen(path_name2);

			file_op = filp_open(filename, O_WRONLY|O_CREAT|O_APPEND, 0);

			trace_str = (struct trfs_trace_ds *) kmalloc(sizeof(struct trfs_trace_ds), GFP_KERNEL);

			old_fs = get_fs();
			set_fs(KERNEL_DS);
/*
			printk("Record ID : %d\n", rec_id);
			printk("Record Size : %d\n", len_path);
			printk("Record Type : %d\n", rec_type);
			printk("Record Open flags : %d\n", open_flags);
			printk("Record Permission mode : %d\n", perm_mode);
			printk("Record Err : %d\n", err);
*/
			trace_str->path_len1 = len_path1;
			trace_str->path_len2 = len_path2;
			trace_str->write_len = 0;
			trace_str->record_id = rec_id;
			trace_str->record_type = rec_type;
			trace_str->open_flags = open_flags;
			trace_str->permission_mode = perm_mode;
			trace_str->err = err;

			writer = (char *)kmalloc(sizeof(struct trfs_trace_ds), GFP_KERNEL);
			memcpy(writer, trace_str, sizeof(struct trfs_trace_ds));

			errval = vfs_write(file_op, writer, sizeof(struct trfs_trace_ds), &offset);
			errval = vfs_write(file_op, path_name1, len_path1, &offset);
			errval = vfs_write(file_op, path_name2, len_path2, &offset);

			set_fs(old_fs);

			filp_close(file_op, NULL);

			sb_info->record_id = ++rec_id;
		}
	} else {
/*		printk("Don't trace mknod.\n"); */
	}

	fsstack_copy_attr_all(new_dir, d_inode(lower_new_dir_dentry));
	fsstack_copy_inode_size(new_dir, d_inode(lower_new_dir_dentry));
	if (new_dir != old_dir) {
		fsstack_copy_attr_all(old_dir,
				      d_inode(lower_old_dir_dentry));
		fsstack_copy_inode_size(old_dir,
					d_inode(lower_old_dir_dentry));
	}

out:
	unlock_rename(lower_old_dir_dentry, lower_new_dir_dentry);
	dput(lower_old_dir_dentry);
	dput(lower_new_dir_dentry);
	trfs_put_lower_path(old_dentry, &lower_old_path);
	trfs_put_lower_path(new_dentry, &lower_new_path);

	if (trace_str != NULL) {
		kfree(trace_str);
	}
	if (writer != NULL) {
		kfree(writer);
	}
	if (buffer != NULL) {
		kfree(buffer);
	}
	return err;
}

static int trfs_readlink(struct dentry *dentry, char __user *buf, int bufsiz)
{
	int err;
	struct dentry *lower_dentry;
	struct path lower_path;

/*	printk("inode.c : Intercepting trfs_readlink --->>>>\n"); */

	trfs_get_lower_path(dentry, &lower_path);
	lower_dentry = lower_path.dentry;
	if (!d_inode(lower_dentry)->i_op ||
	    !d_inode(lower_dentry)->i_op->readlink) {
		err = -EINVAL;
		goto out;
	}

	err = d_inode(lower_dentry)->i_op->readlink(lower_dentry,
						    buf, bufsiz);
	if (err < 0)
		goto out;
	fsstack_copy_attr_atime(d_inode(dentry), d_inode(lower_dentry));

out:
	trfs_put_lower_path(dentry, &lower_path);
	return err;
}

static const char *trfs_get_link(struct dentry *dentry, struct inode *inode,
				   struct delayed_call *done)
{
	char *buf;
	int len = PAGE_SIZE, err;
	mm_segment_t old_fs;

/*	printk("inode.c : Intercepting trfs_get_link--->>>>\n"); */

	if (!dentry)
		return ERR_PTR(-ECHILD);

	/* This is freed by the put_link method assuming a successful call. */
	buf = kmalloc(len, GFP_KERNEL);
	if (!buf) {
		buf = ERR_PTR(-ENOMEM);
		return buf;
	}

	/* read the symlink, and then we will follow it */
	old_fs = get_fs();
	set_fs(KERNEL_DS);
	err = trfs_readlink(dentry, buf, len);
	set_fs(old_fs);
	if (err < 0) {
		kfree(buf);
		buf = ERR_PTR(err);
	} else {
		buf[err] = '\0';
	}
	set_delayed_call(done, kfree_link, buf);
	return buf;
}

static int trfs_permission(struct inode *inode, int mask)
{
	struct inode *lower_inode;
	int err;

/*	printk("inode.c : Intercepting trfs_permission --->>>>\n"); */

	lower_inode = trfs_lower_inode(inode);
	err = inode_permission(lower_inode, mask);
	return err;
}

static int trfs_setattr(struct dentry *dentry, struct iattr *ia)
{
	int err;
	struct dentry *lower_dentry;
	struct inode *inode;
	struct inode *lower_inode;
	struct path lower_path;
	struct iattr lower_ia;

/*	printk("inode.c : Intercepting trfs_setattr --->>>>\n"); */

	inode = d_inode(dentry);

	/*
	 * Check if user has permission to change inode.  We don't check if
	 * this user can change the lower inode: that should happen when
	 * calling notify_change on the lower inode.
	 */
	err = inode_change_ok(inode, ia);
	if (err)
		goto out_err;

	trfs_get_lower_path(dentry, &lower_path);
	lower_dentry = lower_path.dentry;
	lower_inode = trfs_lower_inode(inode);

	/* prepare our own lower struct iattr (with the lower file) */
	memcpy(&lower_ia, ia, sizeof(lower_ia));
	if (ia->ia_valid & ATTR_FILE)
		lower_ia.ia_file = trfs_lower_file(ia->ia_file);

	/*
	 * If shrinking, first truncate upper level to cancel writing dirty
	 * pages beyond the new eof; and also if its' maxbytes is more
	 * limiting (fail with -EFBIG before making any change to the lower
	 * level).  There is no need to vmtruncate the upper level
	 * afterwards in the other cases: we fsstack_copy_inode_size from
	 * the lower level.
	 */
	if (ia->ia_valid & ATTR_SIZE) {
		err = inode_newsize_ok(inode, ia->ia_size);
		if (err)
			goto out;
		truncate_setsize(inode, ia->ia_size);
	}

	/*
	 * mode change is for clearing setuid/setgid bits. Allow lower fs
	 * to interpret this in its own way.
	 */
	if (lower_ia.ia_valid & (ATTR_KILL_SUID | ATTR_KILL_SGID))
		lower_ia.ia_valid &= ~ATTR_MODE;

	/* notify the (possibly copied-up) lower inode */
	/*
	 * Note: we use d_inode(lower_dentry), because lower_inode may be
	 * unlinked (no inode->i_sb and i_ino==0.  This happens if someone
	 * tries to open(), unlink(), then ftruncate() a file.
	 */
	inode_lock(d_inode(lower_dentry));
	err = notify_change(lower_dentry, &lower_ia, /* note: lower_ia */
			    NULL);
	inode_unlock(d_inode(lower_dentry));
	if (err)
		goto out;

	/* get attributes from the lower inode */
	fsstack_copy_attr_all(inode, lower_inode);
	/*
	 * Not running fsstack_copy_inode_size(inode, lower_inode), because
	 * VFS should update our inode size, and notify_change on
	 * lower_inode should update its size.
	 */

out:
	trfs_put_lower_path(dentry, &lower_path);
out_err:
	return err;
}

static int trfs_getattr(struct vfsmount *mnt, struct dentry *dentry,
			  struct kstat *stat)
{
	int err;
	struct kstat lower_stat;
	struct path lower_path;

/*	printk("inode.c : Intercepting trfs_getattr --->>>>\n"); */

	trfs_get_lower_path(dentry, &lower_path);
	err = vfs_getattr(&lower_path, &lower_stat);
	if (err)
		goto out;
	fsstack_copy_attr_all(d_inode(dentry),
			      d_inode(lower_path.dentry));
	generic_fillattr(d_inode(dentry), stat);
	stat->blocks = lower_stat.blocks;
out:
	trfs_put_lower_path(dentry, &lower_path);
	return err;
}

static int
trfs_setxattr(struct dentry *dentry, const char *name, const void *value,
		size_t size, int flags)
{
	int err; struct dentry *lower_dentry;
	struct path lower_path;

/*	printk("inode.c : Intercepting trfs_setxattr --->>>>\n"); */

	trfs_get_lower_path(dentry, &lower_path);
	lower_dentry = lower_path.dentry;
	if (!d_inode(lower_dentry)->i_op->setxattr) {
		err = -EOPNOTSUPP;
		goto out;
	}
	err = vfs_setxattr(lower_dentry, name, value, size, flags);
	if (err)
		goto out;
	fsstack_copy_attr_all(d_inode(dentry),
			      d_inode(lower_path.dentry));
out:
	trfs_put_lower_path(dentry, &lower_path);
	return err;
}

static ssize_t
trfs_getxattr(struct dentry *dentry, const char *name, void *buffer,
		size_t size)
{
	int err;
	struct dentry *lower_dentry;
	struct path lower_path;

/*	printk("inode.c : Intercepting trfs_getxattr --->>>>\n"); */

	trfs_get_lower_path(dentry, &lower_path);
	lower_dentry = lower_path.dentry;
	if (!d_inode(lower_dentry)->i_op->getxattr) {
		err = -EOPNOTSUPP;
		goto out;
	}
	err = vfs_getxattr(lower_dentry, name, buffer, size);
	if (err)
		goto out;
	fsstack_copy_attr_atime(d_inode(dentry),
				d_inode(lower_path.dentry));
out:
	trfs_put_lower_path(dentry, &lower_path);
	return err;
}

static ssize_t
trfs_listxattr(struct dentry *dentry, char *buffer, size_t buffer_size)
{
	int err;
	struct dentry *lower_dentry;
	struct path lower_path;

/*	printk("inode.c : Intercepting trfs_listxattr --->>>>\n"); */

	trfs_get_lower_path(dentry, &lower_path);
	lower_dentry = lower_path.dentry;
	if (!d_inode(lower_dentry)->i_op->listxattr) {
		err = -EOPNOTSUPP;
		goto out;
	}
	err = vfs_listxattr(lower_dentry, buffer, buffer_size);
	if (err)
		goto out;
	fsstack_copy_attr_atime(d_inode(dentry),
				d_inode(lower_path.dentry));
out:
	trfs_put_lower_path(dentry, &lower_path);
	return err;
}

static int
trfs_removexattr(struct dentry *dentry, const char *name)
{
	int err;
	struct dentry *lower_dentry;
	struct path lower_path;

/*	printk("inode.c : Intercepting trfs_removexattr --->>>>\n"); */

	trfs_get_lower_path(dentry, &lower_path);
	lower_dentry = lower_path.dentry;
	if (!d_inode(lower_dentry)->i_op ||
	    !d_inode(lower_dentry)->i_op->removexattr) {
		err = -EINVAL;
		goto out;
	}
	err = vfs_removexattr(lower_dentry, name);
	if (err)
		goto out;
	fsstack_copy_attr_all(d_inode(dentry),
			      d_inode(lower_path.dentry));
out:
	trfs_put_lower_path(dentry, &lower_path);
	return err;
}
const struct inode_operations trfs_symlink_iops = {
	.readlink	= trfs_readlink,
	.permission	= trfs_permission,
	.setattr	= trfs_setattr,
	.getattr	= trfs_getattr,
	.get_link	= trfs_get_link,
	.setxattr	= trfs_setxattr,
	.getxattr	= trfs_getxattr,
	.listxattr	= trfs_listxattr,
	.removexattr	= trfs_removexattr,
};

const struct inode_operations trfs_dir_iops = {
	.create		= trfs_create,
	.lookup		= trfs_lookup,
	.link		= trfs_link,
	.unlink		= trfs_unlink,
	.symlink	= trfs_symlink,
	.mkdir		= trfs_mkdir,
	.rmdir		= trfs_rmdir,
	.mknod		= trfs_mknod,
	.rename		= trfs_rename,
	.permission	= trfs_permission,
	.setattr	= trfs_setattr,
	.getattr	= trfs_getattr,
	.setxattr	= trfs_setxattr,
	.getxattr	= trfs_getxattr,
	.listxattr	= trfs_listxattr,
	.removexattr	= trfs_removexattr,
};

const struct inode_operations trfs_main_iops = {
	.permission	= trfs_permission,
	.setattr	= trfs_setattr,
	.getattr	= trfs_getattr,
	.setxattr	= trfs_setxattr,
	.getxattr	= trfs_getxattr,
	.listxattr	= trfs_listxattr,
	.removexattr	= trfs_removexattr,
};
