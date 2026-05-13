#include "device_file.h"
#include <linux/init.h>       /* module_init, module_exit */
#include <linux/module.h>     /* version info, MODULE_LICENSE, MODULE_AUTHOR */

MODULE_DESCRIPTION("Bastea Linux driver");
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Apriorit, Inc");

static int bastea_driver_init(void)
{
    int result = 0;
    pr_notice("bastea-driver: Initialization started\n");

    result = register_device();
    if (result)
    {
        pr_notice("bastea-driver: Failed to register character device\n");
    }
    return result;
}

static void bastea_driver_exit(void)
{
    pr_notice("bastea-driver: Exiting\n");
    unregister_device();
}

module_init(bastea_driver_init);
module_exit(bastea_driver_exit);
