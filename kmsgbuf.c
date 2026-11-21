// SPDX-License-Identifier: GPL-2.0-only
#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/string.h>
#include <linux/uaccess.h>

#define DEVICE_NAME "kmsgbuf"
#define BUFFER_CAPACITY 4096
#define CLEAR_COMMAND "CLEAR\n"

static char message_buffer[BUFFER_CAPACITY];
static size_t message_length;
static DEFINE_MUTEX(buffer_lock);
static int device_major;

static int kmsgbuf_open(struct inode *inode, struct file *file)
{
	return 0;
}

static int kmsgbuf_release(struct inode *inode, struct file *file)
{
	return 0;
}

static ssize_t kmsgbuf_read(struct file *file, char __user *user_buffer,
			    size_t count, loff_t *position)
{
	size_t bytes_to_copy;

	if (count == 0)
		return 0;

	if (mutex_lock_interruptible(&buffer_lock))
		return -ERESTARTSYS;

	if (message_length == 0) {
		mutex_unlock(&buffer_lock);
		return 0;
	}

	bytes_to_copy = min(count, message_length);
	if (copy_to_user(user_buffer, message_buffer, bytes_to_copy)) {
		mutex_unlock(&buffer_lock);
		return -EFAULT;
	}

	memmove(message_buffer, message_buffer + bytes_to_copy,
		message_length - bytes_to_copy);
	message_length -= bytes_to_copy;
	mutex_unlock(&buffer_lock);
	return bytes_to_copy;
}

static ssize_t kmsgbuf_write(struct file *file, const char __user *user_buffer,
			     size_t count, loff_t *position)
{
	char command[sizeof(CLEAR_COMMAND)];
	size_t available;
	size_t bytes_to_copy;

	if (count == 0)
		return 0;

	if (count == sizeof(CLEAR_COMMAND) - 1) {
		if (copy_from_user(command, user_buffer, count))
			return -EFAULT;
		command[count] = '\0';
		if (strcmp(command, CLEAR_COMMAND) == 0) {
			if (mutex_lock_interruptible(&buffer_lock))
				return -ERESTARTSYS;
			message_length = 0;
			mutex_unlock(&buffer_lock);
			return count;
		}
	}

	if (mutex_lock_interruptible(&buffer_lock))
		return -ERESTARTSYS;
	available = BUFFER_CAPACITY - message_length;
	if (count > available) {
		mutex_unlock(&buffer_lock);
		return -ENOSPC;
	}

	bytes_to_copy = count;
	if (copy_from_user(message_buffer + message_length, user_buffer,
			   bytes_to_copy)) {
		mutex_unlock(&buffer_lock);
		return -EFAULT;
	}
	message_length += bytes_to_copy;
	mutex_unlock(&buffer_lock);
	return bytes_to_copy;
}

static const struct file_operations kmsgbuf_fops = {
	.owner = THIS_MODULE,
	.open = kmsgbuf_open,
	.release = kmsgbuf_release,
	.read = kmsgbuf_read,
	.write = kmsgbuf_write,
};

static int __init kmsgbuf_init(void)
{
	device_major = register_chrdev(0, DEVICE_NAME, &kmsgbuf_fops);
	if (device_major < 0) {
		pr_err(DEVICE_NAME ": failed to register character device: %d\n",
		       device_major);
		return device_major;
	}

	pr_info(DEVICE_NAME ": registered with major %d; create /dev/%s if needed\n",
		device_major, DEVICE_NAME);
	return 0;
}

static void __exit kmsgbuf_exit(void)
{
	unregister_chrdev(device_major, DEVICE_NAME);
	pr_info(DEVICE_NAME ": unregistered\n");
}

module_init(kmsgbuf_init);
module_exit(kmsgbuf_exit);

MODULE_DESCRIPTION("Bounded educational message buffer character device");
MODULE_AUTHOR("Portfolio project");
MODULE_LICENSE("GPL");
