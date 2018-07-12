#include "DriverMain.h"
#include "DriverFileOperations.h"
#include "ToolFunctions.h"
#include <linux/fs.h>
#include <linux/genhd.h>
#include <linux/blkdev.h>

MODULE_LICENSE("Dual BSD/GPL");

struct SLDriverParameters gslDriverParameters = {0};

struct block_device_operations gslDriverFileOperations = 
{
	.owner = THIS_MODULE,
	.open  = DriverOpen,
	.release = DriverRelease,
	.ioctl = DriverIOControl
};

static int InitBlockDevice(void)
{
	int major = 0;
	struct gendisk * disk = NULL;
	struct request_queue * queue = NULL;
	int ret = 0;

	gslDriverParameters.uiLogicalSectSize = 512;
	gslDriverParameters.uiNumberOfSectors = 0x20000000;
	gslDriverParameters.uiNumberOfMinors = 16;
	spin_lock_init(&gslDriverParameters.lock);

	major = register_blkdev(major, DEVICE_NAME);
	if(major <= 0)
	{
		DEBUG_PRINT("cannot alloc dev num\n");
		ret = -EBUSY;
		goto failed;
	}

#ifdef USE_QUEUE
	queue = blk_init_queue(DriverFullRequestQueueHandle, &gslDriverParameters.lock);
	if(queue == NULL)
	{
		DEBUG_PRINT(DEVICE_NAME "cannot alloc queue\n");
		ret = -EBUSY;
		goto failed;
	}
#else
	queue = blk_alloc_queue(GFP_KERNEL);
	if(queue == NULL)
	{
		DEBUG_PRINT(DEVICE_NAME "cannot alloc queue\n");
		ret = -EBUSY;
		goto failed;
	}
	blk_queue_make_request(queue, DriverMakeRequestFunc);
#endif

	blk_queue_logical_block_size(queue, gslDriverParameters.uiLogicalSectSize);
	queue->queuedata = &gslDriverParameters;

	disk = alloc_disk(gslDriverParameters.uiNumberOfMinors);
	if(!disk)
	{
		DEBUG_PRINT(DEVICE_NAME "cannot alloc disk\n");
		ret = -ENOMEM;
		goto failed;
	}
	disk->major = major;
	disk->first_minor = 0;
	snprintf(disk->disk_name, 32, DEVICE_NAME);
	disk->fops = &gslDriverFileOperations;
	disk->queue = queue;
	set_capacity(disk, 0);
	disk->private_data = &gslDriverParameters;
	add_disk(disk);
	set_capacity(disk, gslDriverParameters.uiNumberOfSectors * (gslDriverParameters.uiLogicalSectSize/ KERNEL_SECTOR_SIZE));

	gslDriverParameters.uiDeviceNumber = major;
	gslDriverParameters.disk = disk;
	gslDriverParameters.queue = queue;
	return 0;

failed:
	gslDriverParameters.uiDeviceNumber = 0;
	gslDriverParameters.disk = disk;
	gslDriverParameters.queue = queue;
	return ret;
}

static void UninitBlockDevice(void)
{
	if(gslDriverParameters.disk)
	{
		del_gendisk(gslDriverParameters.disk);
	}
	if(gslDriverParameters.queue)
	{
		blk_cleanup_queue(gslDriverParameters.queue);
	}
	if(gslDriverParameters.uiDeviceNumber)
	{
		unregister_blkdev(gslDriverParameters.uiDeviceNumber, DEVICE_NAME);
	}
}

static int DriverInitialize(void)
{
	int ret = 0;
	DEBUG_PRINT(DEVICE_NAME " Initialize\n");
	ret = InitBlockDevice();
	if(ret < 0)
	{
		DEBUG_PRINT(DEVICE_NAME " Initialize failed\n");
	}

	return ret;
}

static void DriverUninitialize(void)
{
	DEBUG_PRINT(DEVICE_NAME " Uninitialize\n");
	UninitBlockDevice();
}

module_init(DriverInitialize);
module_exit(DriverUninitialize);
