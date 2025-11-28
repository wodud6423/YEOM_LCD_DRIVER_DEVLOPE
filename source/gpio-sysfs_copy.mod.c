#include <linux/build-salt.h>
#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

BUILD_SALT;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(.gnu.linkonce.this_module) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif

static const struct modversion_info ____versions[]
__used __section(__versions) = {
	{ 0x441199c1, "module_layout" },
	{ 0xebc96d1c, "class_destroy" },
	{ 0x4c5731ee, "platform_driver_unregister" },
	{ 0x1e6d42b5, "__platform_driver_register" },
	{ 0xc5850110, "printk" },
	{ 0xdcfb973, "__class_create" },
	{ 0x1decaf25, "_dev_warn" },
	{ 0x645bc945, "device_create_with_groups" },
	{ 0x85389cfd, "devm_fwnode_get_index_gpiod_from_child" },
	{ 0xc358aaf8, "snprintf" },
	{ 0xab27726d, "of_property_read_string" },
	{ 0x376a8759, "of_get_next_available_child" },
	{ 0x4333ebfe, "devm_kmalloc" },
	{ 0xcc9632ed, "of_get_next_child" },
	{ 0xa84ff2fa, "device_unregister" },
	{ 0xdecd0b29, "__stack_chk_fail" },
	{ 0xeb0c0578, "_dev_info" },
	{ 0x17c39e9a, "gpiod_set_value" },
	{ 0x8c8569cb, "kstrtoint" },
	{ 0x966efd2b, "gpiod_get_value" },
	{ 0xcbea4cc4, "gpiod_direction_input" },
	{ 0x75d40653, "gpiod_direction_output" },
	{ 0x222e7ce2, "sysfs_streq" },
	{ 0x59efa71e, "_dev_err" },
	{ 0x3c3ff9fd, "sprintf" },
	{ 0x3ecf66e8, "gpiod_get_direction" },
	{ 0xb1ad28e0, "__gnu_mcount_nc" },
};

MODULE_INFO(depends, "");

