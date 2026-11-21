// SPDX-License-Identifier: GPL-2.0-only
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/utsname.h>
#include <linux/cpu.h>

#define PROC_ENTRY_NAME "kernel_status"

static struct proc_dir_entry *status_entry;

static int kernel_status_show(struct seq_file *seq, void *unused)
{
	seq_printf(seq, "kernel_release: %s\n", init_uts_ns.name.release);
	seq_printf(seq, "timer_hz: %d\n", HZ);
	seq_printf(seq, "online_cpus: %u\n", num_online_cpus());
	return 0;
}

static int kernel_status_open(struct inode *inode, struct file *file)
{
	return single_open(file, kernel_status_show, NULL);
}

static const struct proc_ops kernel_status_ops = {
	.proc_open = kernel_status_open,
	.proc_read = seq_read,
	.proc_lseek = seq_lseek,
	.proc_release = single_release,
};

static int __init kernel_status_init(void)
{
	status_entry = proc_create(PROC_ENTRY_NAME, 0444, NULL,
				   &kernel_status_ops);
	if (!status_entry) {
		pr_err("kernel_status: unable to create /proc/%s\n",
		       PROC_ENTRY_NAME);
		return -ENOMEM;
	}

	pr_info("kernel_status: created /proc/%s\n", PROC_ENTRY_NAME);
	return 0;
}

static void __exit kernel_status_exit(void)
{
	proc_remove(status_entry);
	pr_info("kernel_status: removed /proc/%s\n", PROC_ENTRY_NAME);
}

module_init(kernel_status_init);
module_exit(kernel_status_exit);

MODULE_DESCRIPTION("Read-only kernel status information via procfs");
MODULE_AUTHOR("Portfolio project");
MODULE_LICENSE("GPL");
