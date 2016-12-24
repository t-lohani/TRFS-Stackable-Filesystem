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
#include <linux/module.h>
#include <linux/string.h>
/*
 * There is no need to lock the trfs_super_info's rwsem as there is no
 * way anyone can have a reference to the superblock at this point in time.
 */

/* Initializing global variable mount path*/

static int trfs_read_super(struct super_block *sb, void *raw_data, int silent)
{
	int err = 0;
	struct super_block *lower_sb;
	struct path lower_path;
/*	char *dev_name = (char *) raw_data; */
	char *dev_name;
	struct inode *inode;
	struct trfs_mount_path *mount_path;
	struct file *file_op;

/*	printk("Inside trfs_read_super #############\n"); */

	mount_path = (struct trfs_mount_path *) raw_data;

	if (!mount_path) {
		printk(KERN_ERR
		       "trfs: read_super: missing dev_name argument\n");
		err = -EINVAL;
		goto out;
	}

	dev_name = mount_path->device_path;

	/* parse lower path */
	err = kern_path(dev_name, LOOKUP_FOLLOW | LOOKUP_DIRECTORY,
			&lower_path);
	if (err) {
		printk(KERN_ERR	"trfs: error accessing "
		       "lower directory '%s'\n", dev_name);
		goto out;
	}

	/* allocate superblock private data */
	sb->s_fs_info = kzalloc(sizeof(struct trfs_sb_info), GFP_KERNEL);
	if (!TRFS_SB(sb)) {
		printk(KERN_CRIT "trfs: read_super: out of memory\n");
		err = -ENOMEM;
		goto out_free;
	}

/*	printk("tfile_path : %s\n", mount_path->tfile_path); */

	file_op = filp_open(mount_path->tfile_path, O_WRONLY|O_CREAT|O_TRUNC, 0);
	filp_close(file_op, NULL);
	/* set the lower superblock field of upper superblock */
	lower_sb = lower_path.dentry->d_sb;
	atomic_inc(&lower_sb->s_active);

	trfs_set_lower_super(sb, lower_sb, mount_path, 0, BITMAP_MAX);

	/* inherit maxbytes from lower file system */
	sb->s_maxbytes = lower_sb->s_maxbytes;

	/*
	 * Our c/m/atime granularity is 1 ns because we may stack on file
	 * systems whose granularity is as good.
	 */
	sb->s_time_gran = 1;

	sb->s_op = &trfs_sops;

	sb->s_export_op = &trfs_export_ops; /* adding NFS support */

	/* get a new inode and allocate our root dentry */
	inode = trfs_iget(sb, d_inode(lower_path.dentry));
	if (IS_ERR(inode)) {
		err = PTR_ERR(inode);
		goto out_sput;
	}
	sb->s_root = d_make_root(inode);
	if (!sb->s_root) {
		err = -ENOMEM;
		goto out_iput;
	}
	d_set_d_op(sb->s_root, &trfs_dops);

	/* link the upper and lower dentries */
	sb->s_root->d_fsdata = NULL;
	err = new_dentry_private_data(sb->s_root);
	if (err)
		goto out_freeroot;

	/* if get here: cannot have error */

	/* set the lower dentries for s_root */
	trfs_set_lower_path(sb->s_root, &lower_path);

	/*
	 * No need to call interpose because we already have a positive
	 * dentry, which was instantiated by d_make_root.  Just need to
	 * d_rehash it.
	 */
	d_rehash(sb->s_root);
	if (!silent)
		printk(KERN_INFO
		       "trfs: mounted on top of %s type %s\n",
		       dev_name, lower_sb->s_type->name);
	goto out; /* all is well */

	/* no longer needed: free_dentry_private_data(sb->s_root); */
out_freeroot:
	dput(sb->s_root);
out_iput:
	iput(inode);
out_sput:
	/* drop refs we took earlier */
	atomic_dec(&lower_sb->s_active);
	kfree(TRFS_SB(sb));
	sb->s_fs_info = NULL;
out_free:
	path_put(&lower_path);

out:
	return err;
}

struct dentry *trfs_mount(struct file_system_type *fs_type, int flags,
			    const char *dev_name, void *raw_data)
{
	void *lower_path_name = (void *) dev_name;
	char *tracker;
	struct trfs_mount_path *mount_path = NULL;
	int err = 0;

/*	printk("Inside trfs_mount #######\n");
	printk("raw_data : %s\n", raw_data);
	printk("dev_name : %s\n", dev_name); */

	if (raw_data == NULL) {
		err = -EINVAL;
		goto errorl;
	}

	if (dev_name == NULL) {
		err = -EINVAL;
		goto errorl;
	}

	tracker = raw_data;

	while (*tracker != '=' && *tracker != '\0') {
		tracker++;
	}

	if (tracker == '\0') {
		err = -EINVAL;
		goto errorl;
	}

	tracker++;

	mount_path = (struct trfs_mount_path *) kmalloc(sizeof(struct trfs_mount_path), GFP_KERNEL);

	if (mount_path == NULL) {
		err = -ENOMEM;
		goto errorl;
	}

	/*Storing the path of tfile and device in mount_path structure.*/
	mount_path->tfile_path = kmalloc(strlen(tracker)+1, GFP_KERNEL);
	strcpy(mount_path->tfile_path, tracker);
	mount_path->device_path = kmalloc(strlen((char *) lower_path_name)+1, GFP_KERNEL);
	strcpy(mount_path->device_path, (char *) lower_path_name);

/*	printk("File Path : %s\n",mount_path->tfile_path);
	printk("Device Path : %s\n", mount_path->device_path); */

errorl:
	return mount_nodev(fs_type, flags, (void *) mount_path,
			trfs_read_super);

}


void trfs_shutdown_super(struct super_block *sb)
{

	struct trfs_sb_info *sb_info;

	sb_info = (struct trfs_sb_info *) sb->s_fs_info;

	if (sb_info->mount_path->tfile_path != NULL) {
		kfree(sb_info->mount_path->tfile_path);
	}

	if (sb_info->mount_path->device_path != NULL) {
		kfree(sb_info->mount_path->device_path);
	}

	if (sb_info->mount_path != NULL) {
		kfree(sb_info->mount_path);
	}

	generic_shutdown_super(sb);
}


static struct file_system_type trfs_fs_type = {
	.owner		= THIS_MODULE,
	.name		= TRFS_NAME,
	.mount		= trfs_mount,
	.kill_sb	= trfs_shutdown_super,
/*	.kill_sb	= generic_shutdown_super, */
	.fs_flags	= 0,
};
MODULE_ALIAS_FS(TRFS_NAME);

static int __init init_trfs_fs(void)
{
	int err;

	pr_info("Registering trfs " TRFS_VERSION "\n");

	err = trfs_init_inode_cache();
	if (err)
		goto out;
	err = trfs_init_dentry_cache();
	if (err)
		goto out;
	err = register_filesystem(&trfs_fs_type);
out:
	if (err) {
		trfs_destroy_inode_cache();
		trfs_destroy_dentry_cache();
	}
	return err;
}

static void __exit exit_trfs_fs(void)
{
	trfs_destroy_inode_cache();
	trfs_destroy_dentry_cache();
	unregister_filesystem(&trfs_fs_type);
	pr_info("Completed trfs module unload\n");
}

MODULE_AUTHOR("Tarun Lohani, Stony Brook University");
MODULE_DESCRIPTION("Trfs " TRFS_VERSION);
MODULE_LICENSE("GPL");

module_init(init_trfs_fs);
module_exit(exit_trfs_fs);
