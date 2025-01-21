#include <linux/init.h>
#include <linux/module.h>

static int __init mod_init(void) {
    printk("hello world!\n");
    return 0;
}

static void __exit mod_exit(void) {
    printk("exit mod\n");
}

module_init(mod_init);
module_exit(mod_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("mod desc");
