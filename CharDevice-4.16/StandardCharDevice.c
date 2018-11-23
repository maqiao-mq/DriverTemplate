#include "StandardCharDevice.h"
#include "CharDeviceOperations.h"
#include <linux/module.h>

MODULE_DESCRIPTION("this is a test char device driver");
MODULE_LICENSE("GPL");
#define STANDARD_DRIVER_NAME "test_chardev"

struct StandardCharDevice gstandard_cdev;

static int driver_initialize(void)
{
    int err;
    dev_t dev_no;
    struct class * class;
    struct device *device;

    printk(KERN_INFO "test module initialize\n");

    err = alloc_chrdev_region(&dev_no, 0, 1, STANDARD_DRIVER_NAME);
    if(err)
    {
        printk(KERN_ERR "allcate device number error\n");
        return err;
    }

    gstandard_cdev.dev_no = dev_no;
    cdev_init(&gstandard_cdev.cdev, &StandardCharDeviceOps);
    gstandard_cdev.cdev.owner = THIS_MODULE;
    cdev_add(&gstandard_cdev.cdev, dev_no, 1);

    class =  class_create(THIS_MODULE, STANDARD_DRIVER_NAME);
    if(IS_ERR(class))
    {
        printk(KERN_ERR "create class failed\n");
        return PTR_ERR(class);
    }
    gstandard_cdev.class = class;

    device = device_create(class, NULL, dev_no, NULL, STANDARD_DRIVER_NAME);
    if(IS_ERR(device))
    {
        printk(KERN_ERR "create device failed\n");
        return PTR_ERR(device);
    }
    gstandard_cdev.device = device;

    return 0;
}

static void driver_uninitialize(void)
{
    printk(KERN_INFO "test module uninitialize\n");
    if(gstandard_cdev.device)
    {
        device_destroy(gstandard_cdev.class, gstandard_cdev.dev_no);
    }
    if(gstandard_cdev.class)
    {
        class_destroy(gstandard_cdev.class);
    }
    if(gstandard_cdev.dev_no)
    {
        cdev_del(&gstandard_cdev.cdev);
        unregister_chrdev_region(gstandard_cdev.dev_no, 1);
    }
}

module_init(driver_initialize);
module_exit(driver_uninitialize);

