#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
MODULE_LICENSE("Dual BSD/GPL");
static int demo_init(void)
{
	printk("<1>I am the initial function!\n");
	return 0;
}
static void demo_exit(void)
{
	printk("<1>I am the exit function!\n");
}
module_init(demo_init);
module_exit(demo_exit);