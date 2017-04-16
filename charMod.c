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
size_t count, loff_t *f_posi);
loff_t device_llseek(struct file *filep, loff_t diff, int option);
static void onebyte_exit(void);

/* definition of file_operation structure */
struct file_operations onebyte_fops = {
	read: 		onebyte_read,
	write: 		onebyte_write,
	open: 		onebyte_open,
	release: 	onebyte_release,
	llseek:		device_llseek
};

char *device_data = NULL;
size_t size_val = 0;
loff_t llseek_pointer=0;

int onebyte_open(struct inode *inode, struct file *filep)
{
	return 0; // always successful
}

int onebyte_release(struct inode *inode, struct file *filep)
{
	return 0; // always successful
}

loff_t device_llseek(struct file *filep, loff_t diff, int option) {
	return llseek_pointer;
}


ssize_t onebyte_read(struct file *filep, char *buf, size_t count, loff_t *f_pos)
{
	int error_count;
	int bytes_read;
	int bytes_to_copy = strlen(device_data) > count ? count : strlen(device_data);
	printk(KERN_ALERT "charMod: read data , device size is %lu, pointer is at %llu, count is %lu", strlen(device_data), *f_pos, count);

	if (*f_pos >= strlen(device_data)) {
		printk(KERN_ALERT "f_pos >= data length, finished");
		return 0;
	}

	// copy data into user space
	error_count = copy_to_user(buf, device_data + *f_pos, bytes_to_copy);

	bytes_read = count - error_count;
	*f_pos += bytes_read;

	if (error_count != 0) {
		printk(KERN_ALERT "charMod: failed to read the data, bytes_read is %d , device size is %lu, pointer is at %llu, count is %lu", bytes_read, strlen(device_data), *f_pos, count);
	}

	return bytes_read;
}

ssize_t onebyte_write(struct file *filep, const char *buf, size_t count, loff_t *f_pos)
{
	size_t length_of_data_to_copy;
	int result;

	length_of_data_to_copy = count < (DEVICE_SIZE_IN_BYTE - 1) ? count : (DEVICE_SIZE_IN_BYTE - 1);

	result = copy_from_user(device_data, buf, length_of_data_to_copy);
	device_data[length_of_data_to_copy] = '\0';

	if (result) {
		// data to copy is larger than device size
		printk(KERN_ALERT "charMod: data to write size %lu bytes exceeds size limit of 4MB", count);
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
	if (device_data == NULL) {
		onebyte_exit();
		// cannot allocate memory
		// return no memory error, negative signify a failure
		return -ENOMEM;
	}

	// initialize the value to be X
	device_data[0] = 'X';
	device_data[1] = '\0';
	printk(KERN_ALERT "This is a char device module with initial value %s\n", device_data);
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
