#include "device_file.h"
#include <linux/fs.h>     /* file stuff */
#include <linux/printk.h> /* pr_* */
#include <linux/errno.h>  /* error codes */
#include <linux/module.h> /* THIS_MODULE */
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h> /* copy_to_user() */
#include <linux/string.h>
#include <linux/mutex.h>
#define BUFFER_SIZE 256

static char device_buffer[BUFFER_SIZE] = "Hello world from Raspberry Pi device driver!\n";
static DEFINE_MUTEX(buffer_lock);

static const char device_name[] = "bastea-driver";
static const char class_name[] = "bastea-driver-class";
dev_t g_devno = 0;
struct cdev g_cdev = {};
static struct class *g_class = NULL;
static struct device *g_device = NULL;

static ssize_t device_file_read(struct file *file_ptr, char __user *user_buffer, size_t count, loff_t *position){
    size_t buffer_size;

    pr_notice("bastea-driver: Read from device file offset = %i, read bytes count = %u\n", (int)*position, (unsigned int)count);

    mutex_lock(&buffer_lock);

    buffer_size = strlen(device_buffer);

    if (*position >= buffer_size) {
        mutex_unlock(&buffer_lock);
        return 0;
    }

    if (*position + count > buffer_size)
        count = buffer_size - *position;

    if (copy_to_user(user_buffer, device_buffer + *position, count) != 0) {
        mutex_unlock(&buffer_lock);
        return -EFAULT;
    }

    *position += count;

    mutex_unlock(&buffer_lock);

    return count;
}

static ssize_t device_file_write(struct file *file_ptr, const char __user *user_buffer, size_t count, loff_t *position){
    size_t bytes_to_copy;

    pr_notice("bastea-driver: Write to device file, bytes count = %u\n", (unsigned int)count);

    bytes_to_copy = min(count, (size_t)(BUFFER_SIZE - 1));

    mutex_lock(&buffer_lock);

    memset(device_buffer, 0, BUFFER_SIZE);

    if (copy_from_user(device_buffer, user_buffer, bytes_to_copy) != 0) {
        mutex_unlock(&buffer_lock);
        return -EFAULT;
    }

    device_buffer[bytes_to_copy] = '\0';

    mutex_unlock(&buffer_lock);

    return bytes_to_copy;
}

static const struct file_operations bastea_driver_fops =
{
    .owner = THIS_MODULE,
    .read = device_file_read,
    .write = device_file_write,
};

int register_device(void)
{
    int result = 0;

    pr_notice("bastea-driver: register_device() is called.\n");

    unsigned baseminor = 0;
    unsigned minor_count_required = 1;
    result = alloc_chrdev_region(&g_devno, baseminor, minor_count_required, device_name);
    if (result)
    {
        pr_err("bastea-driver: alloc_chrdev_region failed: %d\n", result);
        goto err_out;
    }

    cdev_init(&g_cdev, &bastea_driver_fops);
    g_cdev.owner = THIS_MODULE;
    result = cdev_add(&g_cdev, g_devno, minor_count_required);
    if (result)
    {
        pr_err("bastea-driver: cdev_add failed: %d\n", result);
        goto err_unregister_chrdev_region;
    }

    g_class = class_create(class_name);
    if (IS_ERR(g_class))
    {
        result = PTR_ERR(g_class);
        pr_err("bastea-driver: class_create failed: %d\n", result);
        goto err_cdev_del;
    }

    g_device = device_create(g_class, NULL, g_devno, NULL, device_name);
    if (IS_ERR(g_device))
    {
        result = PTR_ERR(g_device);
        pr_err("bastea-driver: device_create failed: %d\n", result);
        goto err_class_destroy;
    }

    pr_notice("bastea-driver: Registered character device with major number = %i, minor number = %i\n", MAJOR(g_devno), MINOR(g_devno));
    return 0;

err_class_destroy:
    if (!IS_ERR_OR_NULL(g_class))
    {
        class_destroy(g_class);
        g_class = NULL;
    }

err_cdev_del:
    cdev_del(&g_cdev);

err_unregister_chrdev_region:
    if (g_devno)
    {
        unsigned minor_count_allocated = 1;
        unregister_chrdev_region(g_devno, minor_count_allocated);
        g_devno = 0;
    }

err_out:
    return result;
}

void unregister_device(void)
{
    pr_notice("bastea-driver: unregister_device() is called\n");

    if (!IS_ERR_OR_NULL(g_device))
    {
        device_destroy(g_class, g_devno);
        g_device = NULL;
    }

    if (!IS_ERR_OR_NULL(g_class))
    {
        class_destroy(g_class);
        g_class = NULL;
    }

    cdev_del(&g_cdev);

    if (g_devno)
    {
        unsigned minor_count_allocated = 1;
        unregister_chrdev_region(g_devno, minor_count_allocated);
        g_devno = 0;
    }
    pr_info("bastea-driver: Unregistered\n");
}
