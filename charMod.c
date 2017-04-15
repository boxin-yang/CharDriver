#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>

#define MAJOR_NUMBER 61
#define DEVICE_SIZE_IN_BYTE 4194304 // 4MB = 1024 * 1024 * 4

/* forward declaration */
int onebyte_open(struct inode *inode, struct file *filep);
int onebyte_release(struct inode *inode, struct file *filep);
ssize_t onebyte_read(struct file *filep, char *buf, size_t
count, loff_t *f_pos);
ssize_t onebyte_write(struct file *filep, const char *buf,
size_t count, loff_t *f_pos);
static void onebyte_exit(void);

/* definition of file_operation structure */
struct file_operations onebyte_fops = {
	read: 		onebyte_read,
	write: 		onebyte_write,
	open: 		onebyte_open,
	release: 	onebyte_release
};

char *device_data = NULL;

int onebyte_open(struct inode *inode, struct file *filep)
{
	return 0; // always successful
}

int onebyte_release(struct inode *inode, struct file *filep)
{
	return 0; // always successful
}

ssize_t onebyte_read(struct file *filep, char *buf, size_t count, loff_t *f_pos)
{
	int error_count;
	
	if (*f_pos == 1) {
		return 0; // finish cat
	} else {
		*f_pos = 1; // move the pointer
	}

	// copy data into user space
	error_count = copy_to_user(buf, device_data, strlen(device_data));

	if (error_count == 0) {
		return 1;
	} else {
		printk(KERN_ALERT "charMod: failed to read the data");
		return -EFAULT; // Failed -- return a bad address message (-14)
	}
}

ssize_t onebyte_write(struct file *filep, const char *buf, size_t count, loff_t *f_pos)
{
	size_t length_of_data_to_copy = count < DEVICE_SIZE_IN_BYTE ? count : DEVICE_SIZE_IN_BYTE;
	
	int result;
	result = copy_from_user(device_data, buf, length_of_data_to_copy);
	if (result) {
		// data to copy is larger than device size
		printk(KERN_ALERT "charMod: data to write exceeds size limit of 4MB");
	}
	return length_of_data_to_copy;
}

static int onebyte_init(void)
{
	int result;
	// register the device
	result = register_chrdev(MAJOR_NUMBER, "onebyte", &onebyte_fops);
	if (result < 0) {
		return result;
	}

	// allocate one byte of memory for storage
	// kmalloc is just like malloc, the second parameter is// the type of memory to be allocated.
	// To release the memory allocated by kmalloc, use kfree.
	device_data = kmalloc(DEVICE_SIZE_IN_BYTE, GFP_KERNEL);
	if (!device_data) {
		onebyte_exit();
		// cannot allocate memory
		// return no memory error, negative signify a failure
		return -ENOMEM;
	}

	// initialize the value to be X
	*device_data = 'X';
	printk(KERN_ALERT "This is a onebyte device module\n");
	return 0;
}

static void onebyte_exit(void)
{
	// if the pointer is pointing to something
	if (device_data) {
		// free the memory and assign the pointer to NULL
		kfree(device_data);
		device_data = NULL;
	}
	// unregister the device
	unregister_chrdev(MAJOR_NUMBER, "onebyte");
	printk(KERN_ALERT "Onebyte device module is unloaded\n");
}

MODULE_LICENSE("GPL");
module_init(onebyte_init);
module_exit(onebyte_exit);
