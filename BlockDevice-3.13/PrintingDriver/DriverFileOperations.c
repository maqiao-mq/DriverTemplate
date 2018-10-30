#include <linux/mm.h>
#include <linux/highmem.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <asm/atomic.h>
#include <linux/rwsem.h>
#include <asm/uaccess.h>
#include <linux/fs.h>
#include <linux/bio.h>
#include <linux/blkdev.h>
#include "DriverFileOperations.h"
#include "DriverMain.h"
#include "ToolFunctions.h"

static void transfer_data(unsigned long diskAddr, char * buffer, unsigned long nbytes, int write)
{
	if(!write)
	{
		memset(buffer, 0, nbytes);
	}
}

static int xfer_bio(struct bio * bio)
{
	int i;
	struct bio_vec * bvec;
	unsigned long diskAddr = bio->bi_sector * KERNEL_SECTOR_SIZE;
	int size;

	size = 0;
	bio_for_each_segment(bvec, bio, i)
	{
		char * buffer;

		buffer = __bio_kmap_atomic(bio, i);
		transfer_data(diskAddr, buffer, bvec->bv_len, bio_data_dir(bio)==WRITE);
		diskAddr += bvec->bv_len;
		size += bvec->bv_len;
		__bio_kunmap_atomic(buffer);
	}

	return size;
}

static int xfer_request(struct request * req)
{
	struct bio * bio;
	int nbytes = 0;
	
	__rq_for_each_bio(bio, req)
	{
		xfer_bio(bio);
		nbytes += bio->bi_size;
	}

	return nbytes;
}

int DriverOpen(struct block_device * device, fmode_t mode)
{
	DEBUG_PRINT(DEVICE_NAME ": open invoked");

	return 0;
}

void DriverRelease(struct gendisk * disk, fmode_t mode)
{
	DEBUG_PRINT(DEVICE_NAME ": close invoked, do nothing\n");
}

int DriverIOControl(struct block_device * device, fmode_t mode, unsigned uiCmd, unsigned long uiArg)
{
	DEBUG_PRINT(DEVICE_NAME ": ioctl invoked, do nothing\n");
	return 0;
}

void DriverFullRequestQueueHandle(struct request_queue * queue)
{
	struct request * req;
	int bytes_xferred;

	DEBUG_PRINT(DEVICE_NAME ": request handle\n");

	while((req = blk_fetch_request(queue)) != NULL)
	{
		// if (blk_rq_is_passthrough(req)) {
        //     printk (KERN_NOTICE "Skip non-fs request\n");
        //     __blk_end_request_all(req, -EIO);
        //    continue;
        // }

		bytes_xferred = xfer_request(req);
		__blk_end_request_all(req,0);
	}
}


void DriverMakeRequestFunc(struct request_queue * q, struct bio * bio)
{
	DEBUG_PRINT(DEVICE_NAME ": make request func invoked.\n");
	xfer_bio(bio);
	bio_endio(bio, 0);
}
