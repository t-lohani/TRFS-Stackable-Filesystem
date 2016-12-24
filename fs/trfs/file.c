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
#include <linux/string.h>
#include <linux/syscalls.h>
#include <linux/kernel.h>
#include <linux/dcache.h>

static ssize_t trfs_read(struct file *file, char __user *buf,
			   size_t count, loff_t *ppos)
{
	int err;
	struct file *lower_file;
	struct dentry *dentry = file->f_path.dentry;
	struct trfs_sb_info *sb_info;
	struct file *file_op;
	unsigned long mask;

	char *filename;
	mm_segment_t old_fs;
	loff_t offset = 0;
	int errval = 0;

	struct trfs_trace_ds *trace_str = NULL;
	int rec_id;
	unsigned char rec_type = READ_CALL;
	unsigned int open_flags;
	umode_t perm_mode;
	short len_path1;
	char *path_name1 = NULL;
	char *buffer = NULL;
	char *writer = NULL;

/*	printk("file.c : Intercepting trfs_read------>>>>\n"); */

	lower_file = trfs_lower_file(file);
	err = vfs_read(lower_file, buf, count, ppos);
	/* update our inode atime upon a successful lower read */
	if (err >= 0)
		fsstack_copy_attr_atime(d_inode(dentry),
					file_inode(lower_file));

	sb_info = (struct trfs_sb_info *) file->f_path.dentry->d_sb->s_fs_info;
	mask = sb_info->mask;
/*	printk("Mask value : %lu\n", mask); */
	if (((mask >> rec_type) & 1) == 1) {
/*		printk("Tracer read.\n"); */
		if (dentry != NULL) {
/*			printk("&&&&&&&&&&&&&& sb_info_tfile_path %s\n", sb_info->tfile_path); */
/*			printk("&&&&&&&&&&&&&& sb_info_record_id %d\n", sb_info->record_id); */

			filename = sb_info->mount_path->tfile_path;
			rec_id = sb_info->record_id;
			open_flags = dentry->d_inode->i_opflags;
			perm_mode = dentry->d_inode->i_mode;

			buffer = kmalloc(256, GFP_KERNEL);
			path_name1 = dentry_path_raw(file->f_path.dentry, buffer, 256);
			len_path1 = strlen(path_name1);

			file_op = filp_open(filename, O_WRONLY|O_CREAT|O_APPEND, 0);
/*			printk("File opened ^^^^^^^^^^^~~~~~~~~~$$$$$$$$$%s\n", path_name); */

			trace_str = (struct trfs_trace_ds *)
					kmalloc(sizeof(struct trfs_trace_ds), GFP_KERNEL);
/*			printk("Size of struct : %d\n", sizeof(struct trfs_trace_ds)); */
			old_fs = get_fs();
			set_fs(KERNEL_DS);

			trace_str->path_len1 = len_path1;
			trace_str->path_len2 = 0;
			trace_str->write_len = 0;
			trace_str->record_id = rec_id;
			trace_str->record_type = rec_type;
			trace_str->open_flags = open_flags;
			trace_str->permission_mode = perm_mode;
			trace_str->err = err;

/*
			printk("Record ID : %d\n", trace_str->record_id);
			printk("Record Size : %d\n", trace_str->path_len);
			printk("Record Type : %d\n", trace_str->record_type);
			printk("Record Open flags : %d\n", trace_str->open_flags);
			printk("Record Permission mode : %d\n", trace_str->permission_mode);
			printk("Record Err : %d\n", trace_str->err);
*/
			writer = (char *)kmalloc(sizeof(struct trfs_trace_ds), GFP_KERNEL);
			memcpy(writer, trace_str, sizeof(struct trfs_trace_ds));

			errval = vfs_write(file_op, writer,
						sizeof(struct trfs_trace_ds), &offset);
/*			printk("Byte written trfs_read: %d\n", errval); */
			errval = vfs_write(file_op, path_name1, len_path1, &offset);
/*			printk("Byte written trfs_read pathname: %d\n", errval); */
			set_fs(old_fs);

			filp_close(file_op, NULL);

			sb_info->record_id = ++rec_id;
		}
	} else {
/*		printk("Don't trace read.\n"); */
	}

	if (trace_str != NULL) {
		kfree(trace_str);
	}
	if (buffer != NULL) {
		kfree(buffer);
	}
	if (writer != NULL) {
		kfree(writer);
	}
	return err;
}

static ssize_t trfs_write(struct file *file, const char __user *buf,
			    size_t count, loff_t *ppos)
{
	int err;

	struct file *lower_file;
	struct dentry *dentry = file->f_path.dentry;

	struct trfs_sb_info *sb_info;
	struct file *file_op;
	unsigned long mask;
	char *filename;
	mm_segment_t old_fs;
	loff_t offset = 0;
	int errval = 0;

	struct trfs_trace_ds *trace_str = NULL;
	int rec_id;
	unsigned char rec_type = WRITE_CALL;
	unsigned int open_flags;
	umode_t perm_mode;
	short len_path1;
	char *path_name1 = NULL;
	char *buffer = NULL;
	char *writer = NULL;

/*	printk("file.c : Intercepting trfs_write------>>>>\n"); */

	lower_file = trfs_lower_file(file);
	err = vfs_write(lower_file, buf, count, ppos);
	/* update our inode times+sizes upon a successful lower write */

	sb_info = (struct trfs_sb_info *) file->f_path.dentry->d_sb->s_fs_info;
	mask = sb_info->mask;
/*	printk("Mask value : %lu\n", mask); */

	if (((mask >> rec_type) & 1) == 1) {
		printk("Tracer write.\n");
		if (dentry != NULL) {
/*			printk("&&&&&&&&&&&&&& sb_info_tfile_path %s\n", sb_info->tfile_path); */
/*			printk("&&&&&&&&&&&&&& sb_info_record_id %d\n", sb_info->record_id); */

			filename = sb_info->mount_path->tfile_path;
			rec_id = sb_info->record_id;
			open_flags = dentry->d_inode->i_opflags;
			perm_mode = dentry->d_inode->i_mode;

			buffer = kmalloc(256, GFP_KERNEL);
			path_name1 = dentry_path_raw(file->f_path.dentry, buffer, 256);
			len_path1 = strlen(path_name1);

			file_op = filp_open(filename, O_WRONLY|O_CREAT|O_APPEND, 0);
/*			printk("File opened ^^^^^^^^^^^~~~~~~~~~$$$$$$$$$%s\n", path_name); */

			trace_str = (struct trfs_trace_ds *)
						kmalloc(sizeof(struct trfs_trace_ds), GFP_KERNEL);
/*			printk("Size of struct : %d\n", sizeof(struct trfs_trace_ds)); */
			old_fs = get_fs();
			set_fs(KERNEL_DS);

			trace_str->path_len1 = len_path1;
			trace_str->path_len2 = 0;
			trace_str->write_len = err;
			trace_str->record_id = rec_id;
			trace_str->record_type = rec_type;
			trace_str->open_flags = open_flags;
			trace_str->permission_mode = perm_mode;
			trace_str->err = err;

			printk("Record ID : %d\n", trace_str->record_id);
			printk("Record Size : %d\n", trace_str->path_len1);
			printk("Write content length : %d\n", trace_str->write_len);
			printk("Record Type : %d\n", trace_str->record_type);
			printk("Record Open flags : %d\n", trace_str->open_flags);
			printk("Record Permission mode : %d\n", trace_str->permission_mode);
			printk("Record Err : %d\n", trace_str->err);

			writer = (char *)kmalloc(sizeof(struct trfs_trace_ds), GFP_KERNEL);
			memcpy(writer, trace_str, sizeof(struct trfs_trace_ds));

			errval = vfs_write(file_op, writer,
						sizeof(struct trfs_trace_ds), &offset);
			printk("Byte written trfs_write: %d\n", errval);
			errval = vfs_write(file_op, path_name1, len_path1, &offset);
			printk("Byte written trfs_write pathname: %d\n", errval);
			errval = vfs_write(file_op, buf, err, &offset);
			printk("Byte written trfs_write content : %d\n", errval);
			set_fs(old_fs);

			filp_close(file_op, NULL);

			sb_info->record_id = ++rec_id;
		}
	} else {
		printk("Don't trace write.\n");
	}

	if (err >= 0) {
		fsstack_copy_inode_size(d_inode(dentry),
					file_inode(lower_file));
		fsstack_copy_attr_times(d_inode(dentry),
					file_inode(lower_file));
	}

	if (trace_str != NULL) {
		kfree(trace_str);
	}
	if (buffer != NULL) {
		kfree(buffer);
	}
	if (writer != NULL) {
		kfree(writer);
	}
	return err;
}

static int trfs_readdir(struct file *file, struct dir_context *ctx)
{
	int err;
	struct file *lower_file = NULL;
	struct dentry *dentry = file->f_path.dentry;

/*	printk("file.c : Intercepting trfs_readdir------>>>>\n"); */

	lower_file = trfs_lower_file(file);
	err = iterate_dir(lower_file, ctx);
	file->f_pos = lower_file->f_pos;
	if (err >= 0)		/* copy the atime */
		fsstack_copy_attr_atime(d_inode(dentry),
					file_inode(lower_file));
	return err;
}

static long trfs_unlocked_ioctl(struct file *file, unsigned int cmd,
				  unsigned long arg)
{
	long err = -ENOTTY;
	struct file *lower_file;

	struct trfs_sb_info *sb_info;

	unsigned long *src;
	int res;

	sb_info = (struct trfs_sb_info *) file->f_path.dentry->d_sb->s_fs_info;
	src = &sb_info->mask;

	switch (cmd) {
	case ALL:
/*		printk("Ioctl trigerred ---->>>>>> cmd = ALL. Value passed : %lu\n", arg);
		printk("Previous mask value : %lu\n", sb_info->mask); */
		sb_info->mask = arg;
/*		printk("Current mask value : %lu\n", sb_info->mask); */
		break;
	case NONE:
/*		printk("Ioctl trigerred ---->>>>>> cmd = NONE. Value passed : %lu\n", arg);
		printk("Previous mask value : %lu\n", sb_info->mask); */
		sb_info->mask = arg;
/*		printk("Current mask value : %lu\n", sb_info->mask); */
		break;
	case HEX:
/*		printk("Ioctl trigerred ---->>>>>> cmd = HEX. Value passed : %lu\n", arg);
		printk("Previous mask value : %lu\n", sb_info->mask); */
		sb_info->mask = arg;
/*		printk("Current mask value : %lu\n", sb_info->mask); */
		break;
	case RET:
/*		printk("Ioctl triggered ---->>>>>> cmd = RET.\n"); */
		res = copy_to_user((unsigned long *)arg, src, sizeof(unsigned long));
		break;
	}

	lower_file = trfs_lower_file(file);

	/* XXX: use vfs_ioctl if/when VFS exports it */
	if (!lower_file || !lower_file->f_op)
		goto out;
	if (lower_file->f_op->unlocked_ioctl)
		err = lower_file->f_op->unlocked_ioctl(lower_file, cmd, arg);

	/* some ioctls can change inode attributes (EXT2_IOC_SETFLAGS) */
	if (!err)
		fsstack_copy_attr_all(file_inode(file),
				      file_inode(lower_file));
out:
	return err;
}

#ifdef CONFIG_COMPAT
static long trfs_compat_ioctl(struct file *file, unsigned int cmd,
				unsigned long arg)
{
	long err = -ENOTTY;
	struct file *lower_file;

/*	printk("trfs_compat_ioctl function called ############"); */

	lower_file = trfs_lower_file(file);

	/* XXX: use vfs_ioctl if/when VFS exports it */
	if (!lower_file || !lower_file->f_op)
		goto out;
	if (lower_file->f_op->compat_ioctl)
		err = lower_file->f_op->compat_ioctl(lower_file, cmd, arg);

out:
	return err;
}
#endif

static int trfs_mmap(struct file *file, struct vm_area_struct *vma)
{
	int err = 0;
	bool willwrite;
	struct file *lower_file;
	const struct vm_operations_struct *saved_vm_ops = NULL;

/*	printk("file.c : Intercepting trfs_mmap--->>>>\n"); */

	/* this might be deferred to mmap's writepage */
	willwrite = ((vma->vm_flags | VM_SHARED | VM_WRITE) == vma->vm_flags);

	/*
	 * File systems which do not implement ->writepage may use
	 * generic_file_readonly_mmap as their ->mmap op.  If you call
	 * generic_file_readonly_mmap with VM_WRITE, you'd get an -EINVAL.
	 * But we cannot call the lower ->mmap op, so we can't tell that
	 * writeable mappings won't work.  Therefore, our only choice is to
	 * check if the lower file system supports the ->writepage, and if
	 * not, return EINVAL (the same error that
	 * generic_file_readonly_mmap returns in that case).
	 */
	lower_file = trfs_lower_file(file);
	if (willwrite && !lower_file->f_mapping->a_ops->writepage) {
		err = -EINVAL;
		printk(KERN_ERR "trfs: lower file system does not "
		       "support writeable mmap\n");
		goto out;
	}

	/*
	 * find and save lower vm_ops.
	 *
	 * XXX: the VFS should have a cleaner way of finding the lower vm_ops
	 */
	if (!TRFS_F(file)->lower_vm_ops) {
		err = lower_file->f_op->mmap(lower_file, vma);
		if (err) {
			printk(KERN_ERR "trfs: lower mmap failed %d\n", err);
			goto out;
		}
		saved_vm_ops = vma->vm_ops; /* save: came from lower ->mmap */
	}

	/*
	 * Next 3 lines are all I need from generic_file_mmap.  I definitely
	 * don't want its test for ->readpage which returns -ENOEXEC.
	 */
	file_accessed(file);
	vma->vm_ops = &trfs_vm_ops;

	file->f_mapping->a_ops = &trfs_aops; /* set our aops */
	if (!TRFS_F(file)->lower_vm_ops) /* save for our ->fault */
		TRFS_F(file)->lower_vm_ops = saved_vm_ops;

out:
	return err;
}

static int trfs_open(struct inode *inode, struct file *file)
{
	int err = 0;
	struct file *lower_file = NULL;
	struct path lower_path;

	struct trfs_sb_info *sb_info;
	char *filename;
	mm_segment_t old_fs;
	loff_t offset = 0;
	int errval = 0;
	struct file *file_op;
	unsigned long mask;

	struct trfs_trace_ds *trace_str = NULL;
	int rec_id;
	unsigned char rec_type = OPEN_CALL;
	unsigned int open_flags;
	umode_t perm_mode;
	short len_path1 = 0;
	char *path_name1 = NULL;
	char *buffer = NULL;
	char *writer = NULL;

/*	printk("file.c : Intercepting trfs_open----->>>>\n"); */

	/* don't open unhashed/deleted files */
	if (d_unhashed(file->f_path.dentry)) {
		err = -ENOENT;
		goto out_err;
	}

	file->private_data =
		kzalloc(sizeof(struct trfs_file_info), GFP_KERNEL);
	if (!TRFS_F(file)) {
		err = -ENOMEM;
		goto out_err;
	}

	/* open lower object and link trfs's file struct to lower's */
	trfs_get_lower_path(file->f_path.dentry, &lower_path);
	lower_file = dentry_open(&lower_path, file->f_flags, current_cred());
	path_put(&lower_path);
	if (IS_ERR(lower_file)) {
		err = PTR_ERR(lower_file);
		lower_file = trfs_lower_file(file);
		if (lower_file) {
			trfs_set_lower_file(file, NULL);
			fput(lower_file); /* fput calls dput for lower_dentry */
		}
	} else {
		trfs_set_lower_file(file, lower_file);
	}

	sb_info = (struct trfs_sb_info *) file->f_path.dentry->d_sb->s_fs_info;
	mask = sb_info->mask;
/*	printk("Mask value : %lu\n", mask); */
	if (((mask >> rec_type) & 1) == 1) {

		printk("Trace open.\n");
		filename = sb_info->mount_path->tfile_path;

		rec_id = sb_info->record_id;
		open_flags = file->f_flags;
		perm_mode = file->f_path.dentry->d_inode->i_mode;

		buffer = kmalloc(256, GFP_KERNEL);
		path_name1 = dentry_path_raw(file->f_path.dentry, buffer, 256);
		len_path1 = strlen(path_name1);

		file_op = filp_open(filename, O_WRONLY|O_CREAT|O_APPEND, 0);

		trace_str = (struct trfs_trace_ds *) kmalloc(sizeof(struct trfs_trace_ds), GFP_KERNEL);

		old_fs = get_fs();
		set_fs(KERNEL_DS);

		trace_str->path_len1 = len_path1;
		trace_str->path_len2 = 0;
		trace_str->write_len = 0;
		trace_str->record_id = rec_id;
		trace_str->record_type = rec_type;
		trace_str->open_flags = open_flags;
		trace_str->permission_mode = perm_mode;
		trace_str->err = err;

/*
		printk("Record ID : %d\n", trace_str->record_id);
		printk("Record Size : %d\n", trace_str->path_len);
		printk("Record Type : %d\n", trace_str->record_type);
		printk("Record Open flags : %d\n", trace_str->open_flags);
		printk("Record Permission mode : %d\n", trace_str->permission_mode);
		printk("Record Err : %d\n", trace_str->err);
*/
		writer = (char *)kmalloc(sizeof(struct trfs_trace_ds), GFP_KERNEL);
/*		memcpy(writer, trace_str, (sizeof(struct trfs_trace_ds) + strlen(trace_str->pathname))); */
		memcpy(writer, trace_str, sizeof(struct trfs_trace_ds));

		errval = vfs_write(file_op, writer, sizeof(struct trfs_trace_ds), &offset);
/*		printk("Byte written trfs_open: %d\n", errval); */
		errval = vfs_write(file_op, path_name1, len_path1, &offset);

		set_fs(old_fs);

		filp_close(file_op, NULL);

		sb_info->record_id = ++rec_id;
	} else {
		printk("Don't trace open.\n");
	}

	if (err)
		kfree(TRFS_F(file));
	else
		fsstack_copy_attr_all(inode, trfs_lower_inode(inode));
out_err:

	if (trace_str != NULL) {
		kfree(trace_str);
	}
	if (buffer != NULL) {
		kfree(buffer);
	}
	if (writer != NULL) {
		kfree(writer);
	}

	return err;
}

static int trfs_flush(struct file *file, fl_owner_t id)
{
	int err = 0;
	struct file *lower_file = NULL;

/*	printk("file.c : Intercepting trfs_flush--------->>>>\n"); */

	lower_file = trfs_lower_file(file);
	if (lower_file && lower_file->f_op && lower_file->f_op->flush) {
		filemap_write_and_wait(file->f_mapping);
		err = lower_file->f_op->flush(lower_file, id);
	}

	return err;
}

/* release all lower object references & free the file info structure */
static int trfs_file_release(struct inode *inode, struct file *file)
{
	struct file *lower_file;

	struct trfs_sb_info *sb_info;
	char *filename;
	mm_segment_t old_fs;
	loff_t offset = 0;
	int errval = 0;
	struct file *file_op;
	unsigned long mask;

	struct trfs_trace_ds *trace_str = NULL;
	int rec_id;
	unsigned char rec_type = RELEASE_CALL;
	unsigned int open_flags;
	umode_t perm_mode;
	short len_path1 = 0;
	char *path_name1 = NULL;
	char *buffer = NULL;
	char *writer = NULL;

/*	printk("file.c : Intercepting trfs_file_release---->>>>\n"); */

	lower_file = trfs_lower_file(file);
	if (lower_file) {
		trfs_set_lower_file(file, NULL);
		fput(lower_file);
	}

	sb_info = (struct trfs_sb_info *) file->f_path.dentry->d_sb->s_fs_info;
	mask = sb_info->mask;

	if (((mask >> rec_type) & 1) == 1) {

		printk("Trace release.\n");
		filename = sb_info->mount_path->tfile_path;

		rec_id = sb_info->record_id;
		open_flags = 0;
		perm_mode = 0;

		buffer = kmalloc(256, GFP_KERNEL);
		path_name1 = dentry_path_raw(file->f_path.dentry, buffer, 256);
		len_path1 = strlen(path_name1);

		file_op = filp_open(filename, O_WRONLY|O_CREAT|O_APPEND, 0);

		trace_str = (struct trfs_trace_ds *) kmalloc(sizeof(struct trfs_trace_ds), GFP_KERNEL);

		old_fs = get_fs();
		set_fs(KERNEL_DS);

		trace_str->path_len1 = len_path1;
		trace_str->path_len2 = 0;
		trace_str->write_len = 0;
		trace_str->record_id = rec_id;
		trace_str->record_type = rec_type;
		trace_str->open_flags = open_flags;
		trace_str->permission_mode = perm_mode;
		trace_str->err = 0;

/*
		printk("Record ID : %d\n", trace_str->record_id);
		printk("Record Size : %d\n", trace_str->path_len);
		printk("Record Type : %d\n", trace_str->record_type);
		printk("Record Open flags : %d\n", trace_str->open_flags);
		printk("Record Permission mode : %d\n", trace_str->permission_mode);
		printk("Record Err : %d\n", trace_str->err);
*/
		writer = (char *)kmalloc(sizeof(struct trfs_trace_ds), GFP_KERNEL);
		memcpy(writer, trace_str, sizeof(struct trfs_trace_ds));

		errval = vfs_write(file_op, writer, sizeof(struct trfs_trace_ds), &offset);
/*		printk("Byte written trfs_open: %d\n", errval); */
		errval = vfs_write(file_op, path_name1, len_path1, &offset);

		set_fs(old_fs);

		filp_close(file_op, NULL);

		sb_info->record_id = ++rec_id;
	} else {
		printk("Don't trace release.\n");
	}

	kfree(TRFS_F(file));

	if (trace_str != NULL) {
		kfree(trace_str);
	}
	if (buffer != NULL) {
		kfree(buffer);
	}
	if (writer != NULL) {
		kfree(writer);
	}
	return 0;
}

static int trfs_fsync(struct file *file, loff_t start, loff_t end,
			int datasync)
{
	int err;
	struct file *lower_file;
	struct path lower_path;
	struct dentry *dentry = file->f_path.dentry;

/*	printk("file.c : Intercepting trfs_fsync----->>>>\n"); */

	err = __generic_file_fsync(file, start, end, datasync);
	if (err)
		goto out;
	lower_file = trfs_lower_file(file);
	trfs_get_lower_path(dentry, &lower_path);
	err = vfs_fsync_range(lower_file, start, end, datasync);
	trfs_put_lower_path(dentry, &lower_path);
out:
	return err;
}

static int trfs_fasync(int fd, struct file *file, int flag)
{
	int err = 0;
	struct file *lower_file = NULL;

/*	printk("file.c : Intercepting trfs_fasync----->>>>\n");	*/

	lower_file = trfs_lower_file(file);
	if (lower_file->f_op && lower_file->f_op->fasync)
		err = lower_file->f_op->fasync(fd, lower_file, flag);

	return err;
}

/*
 * Trfs cannot use generic_file_llseek as ->llseek, because it would
 * only set the offset of the upper file.  So we have to implement our
 * own method to set both the upper and lower file offsets
 * consistently.
 */
static loff_t trfs_file_llseek(struct file *file, loff_t offset, int whence)
{
	int err;
	struct file *lower_file;

/*	printk("file.c : Intercepting trfs_file_llseek---->>>>\n"); */

	err = generic_file_llseek(file, offset, whence);
	if (err < 0)
		goto out;

	lower_file = trfs_lower_file(file);
	err = generic_file_llseek(lower_file, offset, whence);

out:
	return err;
}

/*
 * Trfs read_iter, redirect modified iocb to lower read_iter
 */
ssize_t
trfs_read_iter(struct kiocb *iocb, struct iov_iter *iter)
{
	int err;
	struct file *file = iocb->ki_filp, *lower_file;

/*	printk("file.c : Intercept trfs_read_iter---->>>>.\n");	*/

	lower_file = trfs_lower_file(file);
	if (!lower_file->f_op->read_iter) {
		err = -EINVAL;
		goto out;
	}

	get_file(lower_file); /* prevent lower_file from being released */
	iocb->ki_filp = lower_file;
	err = lower_file->f_op->read_iter(iocb, iter);
	iocb->ki_filp = file;
	fput(lower_file);
	/* update upper inode atime as needed */
	if (err >= 0 || err == -EIOCBQUEUED)
		fsstack_copy_attr_atime(d_inode(file->f_path.dentry),
					file_inode(lower_file));
out:
	return err;
}

/*
 * Trfs write_iter, redirect modified iocb to lower write_iter
 */
ssize_t
trfs_write_iter(struct kiocb *iocb, struct iov_iter *iter)
{
	int err;
	struct file *file = iocb->ki_filp, *lower_file;

/*	printk("file.c : Intercepting trfs_write_iter--->>>>\n"); */

	lower_file = trfs_lower_file(file);
	if (!lower_file->f_op->write_iter) {
		err = -EINVAL;
		goto out;
	}

	get_file(lower_file); /* prevent lower_file from being released */
	iocb->ki_filp = lower_file;
	err = lower_file->f_op->write_iter(iocb, iter);
	iocb->ki_filp = file;
	fput(lower_file);
	/* update upper inode times/sizes as needed */
	if (err >= 0 || err == -EIOCBQUEUED) {
		fsstack_copy_inode_size(d_inode(file->f_path.dentry),
					file_inode(lower_file));
		fsstack_copy_attr_times(d_inode(file->f_path.dentry),
					file_inode(lower_file));
	}
out:
	return err;
}

const struct file_operations trfs_main_fops = {
	.llseek		= generic_file_llseek,
	.read		= trfs_read,
	.write		= trfs_write,
	.unlocked_ioctl	= trfs_unlocked_ioctl,
#ifdef CONFIG_COMPAT
	.compat_ioctl	= trfs_compat_ioctl,
#endif
	.mmap		= trfs_mmap,
	.open		= trfs_open,
	.flush		= trfs_flush,
	.release	= trfs_file_release,
	.fsync		= trfs_fsync,
	.fasync		= trfs_fasync,
	.read_iter	= trfs_read_iter,
	.write_iter	= trfs_write_iter,
};

/* trimmed directory options */
const struct file_operations trfs_dir_fops = {
	.llseek		= trfs_file_llseek,
	.read		= generic_read_dir,
	.iterate	= trfs_readdir,
	.unlocked_ioctl	= trfs_unlocked_ioctl,
#ifdef CONFIG_COMPAT
	.compat_ioctl	= trfs_compat_ioctl,
#endif
	.open		= trfs_open,
	.release	= trfs_file_release,
	.flush		= trfs_flush,
	.fsync		= trfs_fsync,
	.fasync		= trfs_fasync,
};
