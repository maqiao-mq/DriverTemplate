#ifndef DriverMain_H
#define DriverMain_H

#include <linux/init.h>
#include <linux/module.h>
#include <asm/mtrr.h>
#include <linux/device.h>
#include <linux/mm.h>
#include <linux/spinlock.h>

#define DEVICE_NAME "msg_printer"
#define KERNEL_SECTOR_SIZE (512)

struct SLDriverParameters
{
	struct gendisk * disk;
	struct request_queue * queue;
	int uiDeviceNumber;
	int uiLogicalSectSize;
	int uiNumberOfSectors;
	int uiNumberOfMinors;
	spinlock_t lock;
};

extern struct SLDriverParameters gslDriverParameters;

#endif
