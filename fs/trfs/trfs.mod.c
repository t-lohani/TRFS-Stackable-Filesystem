#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

__visible struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0x11ae19b5, __VMLINUX_SYMBOL_STR(module_layout) },
	{ 0xadcd483, __VMLINUX_SYMBOL_STR(vfs_create) },
	{ 0xb6d7ae26, __VMLINUX_SYMBOL_STR(kmem_cache_destroy) },
	{ 0x122e6e41, __VMLINUX_SYMBOL_STR(cpu_tss) },
	{ 0xf29a148e, __VMLINUX_SYMBOL_STR(kmalloc_caches) },
	{ 0xd2b09ce5, __VMLINUX_SYMBOL_STR(__kmalloc) },
	{ 0x476c7b50, __VMLINUX_SYMBOL_STR(fsstack_copy_inode_size) },
	{ 0x32e1b6c6, __VMLINUX_SYMBOL_STR(generic_file_llseek) },
	{ 0x6bf1c17f, __VMLINUX_SYMBOL_STR(pv_lock_ops) },
	{ 0x977a8702, __VMLINUX_SYMBOL_STR(mntget) },
	{ 0xeb59f02d, __VMLINUX_SYMBOL_STR(inode_permission) },
	{ 0x754d539c, __VMLINUX_SYMBOL_STR(strlen) },
	{ 0x23eb4b90, __VMLINUX_SYMBOL_STR(d_set_d_op) },
	{ 0xc6f6d72c, __VMLINUX_SYMBOL_STR(iget5_locked) },
	{ 0xfd40320b, __VMLINUX_SYMBOL_STR(__generic_file_fsync) },
	{ 0x8c27cc23, __VMLINUX_SYMBOL_STR(dget_parent) },
	{ 0xb8b043f2, __VMLINUX_SYMBOL_STR(kfree_link) },
	{ 0xd67fe4bb, __VMLINUX_SYMBOL_STR(vfs_link) },
	{ 0x60954f49, __VMLINUX_SYMBOL_STR(generic_fh_to_parent) },
	{ 0x20c73be6, __VMLINUX_SYMBOL_STR(filemap_write_and_wait) },
	{ 0xfb83dcb6, __VMLINUX_SYMBOL_STR(touch_atime) },
	{ 0x131cce04, __VMLINUX_SYMBOL_STR(generic_delete_inode) },
	{ 0x555f6938, __VMLINUX_SYMBOL_STR(lockref_get) },
	{ 0x5fd2035d, __VMLINUX_SYMBOL_STR(dput) },
	{ 0x388add66, __VMLINUX_SYMBOL_STR(filp_close) },
	{ 0xa65541f, __VMLINUX_SYMBOL_STR(dentry_open) },
	{ 0x335cb745, __VMLINUX_SYMBOL_STR(vfs_path_lookup) },
	{ 0x33f76ebb, __VMLINUX_SYMBOL_STR(vfs_mknod) },
	{ 0x5d8532ee, __VMLINUX_SYMBOL_STR(mutex_unlock) },
	{ 0x9f5cd910, __VMLINUX_SYMBOL_STR(d_add) },
	{ 0xbb449cda, __VMLINUX_SYMBOL_STR(generic_read_dir) },
	{ 0xe977a554, __VMLINUX_SYMBOL_STR(igrab) },
	{ 0x6a07def, __VMLINUX_SYMBOL_STR(vfs_symlink) },
	{ 0x8ed2e3b5, __VMLINUX_SYMBOL_STR(mount_nodev) },
	{ 0xbc09870f, __VMLINUX_SYMBOL_STR(path_get) },
	{ 0x8b071b49, __VMLINUX_SYMBOL_STR(truncate_setsize) },
	{ 0xd0c5ed0, __VMLINUX_SYMBOL_STR(vfs_rmdir) },
	{ 0x2b9c5824, __VMLINUX_SYMBOL_STR(unlock_rename) },
	{ 0xc671e369, __VMLINUX_SYMBOL_STR(_copy_to_user) },
	{ 0x19f82a02, __VMLINUX_SYMBOL_STR(vfs_read) },
	{ 0xaeb229e3, __VMLINUX_SYMBOL_STR(kern_path) },
	{ 0xacecfeb6, __VMLINUX_SYMBOL_STR(current_task) },
	{ 0x27e1a049, __VMLINUX_SYMBOL_STR(printk) },
	{ 0xf7248292, __VMLINUX_SYMBOL_STR(d_rehash) },
	{ 0x55d52a3f, __VMLINUX_SYMBOL_STR(vfs_getxattr) },
	{ 0xa7e3f247, __VMLINUX_SYMBOL_STR(kmem_cache_free) },
	{ 0xf153da73, __VMLINUX_SYMBOL_STR(lock_rename) },
	{ 0x65ad8a27, __VMLINUX_SYMBOL_STR(mutex_lock) },
	{ 0x248cf0e, __VMLINUX_SYMBOL_STR(set_nlink) },
	{ 0xf00ddbfc, __VMLINUX_SYMBOL_STR(dentry_path_raw) },
	{ 0xd6a036c3, __VMLINUX_SYMBOL_STR(fput) },
	{ 0xe10367c3, __VMLINUX_SYMBOL_STR(inode_init_once) },
	{ 0x16b021c5, __VMLINUX_SYMBOL_STR(kmem_cache_alloc) },
	{ 0x8e1d0bba, __VMLINUX_SYMBOL_STR(ilookup) },
	{ 0x2d7c77fb, __VMLINUX_SYMBOL_STR(d_alloc) },
	{ 0xdb7305a1, __VMLINUX_SYMBOL_STR(__stack_chk_fail) },
	{ 0x7bff6b41, __VMLINUX_SYMBOL_STR(unlock_new_inode) },
	{ 0xa3e52ec8, __VMLINUX_SYMBOL_STR(d_drop) },
	{ 0xa869c3a0, __VMLINUX_SYMBOL_STR(inode_newsize_ok) },
	{ 0x8665266f, __VMLINUX_SYMBOL_STR(vfs_statfs) },
	{ 0x26bd8e5a, __VMLINUX_SYMBOL_STR(vfs_mkdir) },
	{ 0x6f20960a, __VMLINUX_SYMBOL_STR(full_name_hash) },
	{ 0x6ec2fbdb, __VMLINUX_SYMBOL_STR(inode_change_ok) },
	{ 0x88abec6b, __VMLINUX_SYMBOL_STR(path_put) },
	{ 0xe259ae9e, __VMLINUX_SYMBOL_STR(_raw_spin_lock) },
	{ 0xb722347b, __VMLINUX_SYMBOL_STR(generic_show_options) },
	{ 0x816b486c, __VMLINUX_SYMBOL_STR(vfs_unlink) },
	{ 0x188c4aaf, __VMLINUX_SYMBOL_STR(kmem_cache_create) },
	{ 0xa3788a93, __VMLINUX_SYMBOL_STR(register_filesystem) },
	{ 0x7c7df945, __VMLINUX_SYMBOL_STR(fsstack_copy_attr_all) },
	{ 0x16aec08c, __VMLINUX_SYMBOL_STR(d_lookup) },
	{ 0x4ff9f67a, __VMLINUX_SYMBOL_STR(iput) },
	{ 0x37a0cba, __VMLINUX_SYMBOL_STR(kfree) },
	{ 0xa45f0c4c, __VMLINUX_SYMBOL_STR(d_splice_alias) },
	{ 0x6cab0898, __VMLINUX_SYMBOL_STR(d_make_root) },
	{ 0xd0454a47, __VMLINUX_SYMBOL_STR(iterate_dir) },
	{ 0xf9bb7591, __VMLINUX_SYMBOL_STR(unregister_filesystem) },
	{ 0x5f4c557c, __VMLINUX_SYMBOL_STR(init_special_inode) },
	{ 0x468fbe95, __VMLINUX_SYMBOL_STR(vfs_rename) },
	{ 0xac9398b9, __VMLINUX_SYMBOL_STR(vfs_getattr) },
	{ 0xc0f33770, __VMLINUX_SYMBOL_STR(generic_fh_to_dentry) },
	{ 0xf856c345, __VMLINUX_SYMBOL_STR(notify_change) },
	{ 0x52130572, __VMLINUX_SYMBOL_STR(vfs_setxattr) },
	{ 0x10fc3465, __VMLINUX_SYMBOL_STR(clear_inode) },
	{ 0xfb3ad0a2, __VMLINUX_SYMBOL_STR(vfs_listxattr) },
	{ 0x10747198, __VMLINUX_SYMBOL_STR(clear_nlink) },
	{ 0x4f3f7561, __VMLINUX_SYMBOL_STR(vfs_removexattr) },
	{ 0x587cffa0, __VMLINUX_SYMBOL_STR(vfs_write) },
	{ 0x90b951d9, __VMLINUX_SYMBOL_STR(vfs_fsync_range) },
	{ 0xb0affbbf, __VMLINUX_SYMBOL_STR(generic_fillattr) },
	{ 0xe914e41e, __VMLINUX_SYMBOL_STR(strcpy) },
	{ 0x365e3533, __VMLINUX_SYMBOL_STR(filp_open) },
	{ 0xa77a9d93, __VMLINUX_SYMBOL_STR(truncate_inode_pages) },
	{ 0xb89f46c9, __VMLINUX_SYMBOL_STR(generic_shutdown_super) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "D0CAC073DBA853AC98EA252");
