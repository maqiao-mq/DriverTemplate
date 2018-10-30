#ifndef BYPASS_BLOCK_DEVICE_H
#define BYPASS_BLOCK_DEVICE_H

#include <linux/genhd.h>
#include <linux/fs.h>
#include <linux/blkdev.h>

struct bypass_block_device
{
    int major_number;
    struct gendisk * disk;
    struct request_queue * queue;
    spinlock_t lock;
};

extern struct bypass_block_device gblkdev;

#endif