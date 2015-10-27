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
	{ 0x6d289d41, __VMLINUX_SYMBOL_STR(module_layout) },
	{ 0x9ecdae3, __VMLINUX_SYMBOL_STR(STUB_start_elevator) },
	{ 0xe8543091, __VMLINUX_SYMBOL_STR(kmalloc_caches) },
	{ 0x65e75cb6, __VMLINUX_SYMBOL_STR(__list_del_entry) },
	{ 0xf9a482f9, __VMLINUX_SYMBOL_STR(msleep) },
	{ 0x1f3447a5, __VMLINUX_SYMBOL_STR(remove_proc_entry) },
	{ 0x91715312, __VMLINUX_SYMBOL_STR(sprintf) },
	{ 0x343a1a8, __VMLINUX_SYMBOL_STR(__list_add) },
	{ 0x811606d6, __VMLINUX_SYMBOL_STR(STUB_issue_request) },
	{ 0x27e1a049, __VMLINUX_SYMBOL_STR(printk) },
	{ 0x61651be, __VMLINUX_SYMBOL_STR(strcat) },
	{ 0xdf9b386b, __VMLINUX_SYMBOL_STR(STUB_stop_elevator) },
	{ 0xdb7305a1, __VMLINUX_SYMBOL_STR(__stack_chk_fail) },
	{ 0xbdfb6dbb, __VMLINUX_SYMBOL_STR(__fentry__) },
	{ 0x280f1eb0, __VMLINUX_SYMBOL_STR(kmem_cache_alloc_trace) },
	{ 0xbc8af77f, __VMLINUX_SYMBOL_STR(proc_create_data) },
	{ 0x37a0cba, __VMLINUX_SYMBOL_STR(kfree) },
	{ 0x69acdf38, __VMLINUX_SYMBOL_STR(memcpy) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "26B1E47BEF19DDE5E5E780A");
