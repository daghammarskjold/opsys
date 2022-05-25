#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

/* This function is called when the module is loaded. */
int kmod_init(void)
{
       printk(KERN_INFO "Hej :)\n");

       return 0;
}

/* This function is called when the module is removed. */
void kmod_exit(void) {
	printk(KERN_INFO "Hejdå :(\n");
}

/* Macros for registering module entry and exit points. */
module_init( kmod_init );
module_exit( kmod_exit );

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("My Kernel Module");
MODULE_AUTHOR("krille");

