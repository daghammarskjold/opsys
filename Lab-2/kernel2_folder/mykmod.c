#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched/signal.h>

/* This function is called when the module is loaded. */
int kmod_init(void)
{      printk(KERN_INFO "Listing processes...\n");

       struct task_struct *tinfo;
       int process_count = 0;

       for_each_process(tinfo){
       printk(KERN_INFO "id: %d | name: %s\n", tinfo->pid, tinfo->comm);
       process_count = process_count + 1;
       }

       printk(KERN_INFO "Started! Active processes: %d\n", process_count);

       return 0;
}

/* This function is called when the module is removed. */
void kmod_exit(void) {
	printk(KERN_INFO "Exiting, done.\n");
}

/* Macros for registering module entry and exit points. */
module_init( kmod_init );
module_exit( kmod_exit );

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("My Kernel Module");
MODULE_AUTHOR("krille");

