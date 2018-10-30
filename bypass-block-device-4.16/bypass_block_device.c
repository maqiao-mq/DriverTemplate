#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/module.h>
#include<linux/fs.h>

#include "bypass_block_device.h"
#include "device_operations.h"

MODULE_DESCRIPTION("this is a test block device driver");
MODULE_LICENSE("GPL");

#define DEVICE_MAJOR_NUMBER 0
#define DEVICE_MAX_MINOR_NUMBER 1
#define DEVICE_NAME "demo-block"
#define DISK_NAME "demo-disk"
#define DISK_SECTOR_SIZE 512
#define DISK_SIZE_IN_512_BYTES ((1<<30)/512)

struct bypass_block_device gbypass_blkdev;

static int init_block_device(struct bypass_block_device * dev)
{
    int ret = 0;

    memset(dev, 0, sizeof(struct bypass_block_device));

    ret = register_blkdev(DEVICE_MAJOR_NUMBER, DEVICE_NAME);
    if(ret < 0)
    {
        printk(KERN_ERR "unable to register block device\n");
        dev->major_number = DEVICE_MAJOR_NUMBER;
        return -EBUSY;
    }
    dev->major_number = !ret ? DEVICE_MAJOR_NUMBER : ret;

    spin_lock_init(&dev->lock);
    dev->queue = blk_alloc_queue(GFP_KERNEL);
    if(!dev->queue)
    {
        printk(KERN_ERR "cannot allocate reqeust_queue\n");
        return -ENOMEM;
    }
    blk_queue_make_request(dev->queue, bypass_dev_make_request);
    blk_queue_logical_block_size(dev->queue, DISK_SECTOR_SIZE);
    dev->queue->queuedata = dev;

    dev->disk = alloc_disk(DEVICE_MAX_MINOR_NUMBER);
    if(!dev->disk)
    {
        printk(KERN_ERR "cannot allocate a disk structure\n");
        return -ENOMEM;
    }
    dev->disk->major = dev->major_number;
    dev->disk->first_minor = 0;
    dev->disk->fops = &bypass_blkdev_ops;
    dev->disk->queue = dev->queue;
    dev->disk->private_data = dev;
    set_capacity(dev->disk, DISK_SIZE_IN_512_BYTES);
    snprintf(dev->disk->disk_name, 32, DISK_NAME);
    add_disk(dev->disk);

    return 0;
}

static void uninit_block_device(struct bypass_block_device * dev)
{
    unregister_blkdev(dev->major_number, DEVICE_NAME);
    if(dev->queue)
    {
        blk_cleanup_queue(dev->queue);
    }
    if(dev->disk)
    {
        del_gendisk(dev->disk);
    }
}

static int driver_initialize(void)
{
    pr_debug("driver initialize\n");
    return init_block_device(&gbypass_blkdev);
}

static void driver_uninitialize(void)
{
    pr_debug("drive uninitialize\n");
    uninit_block_device(&gbypass_blkdev);
}

module_init(driver_initialize);
module_exit(driver_uninitialize);