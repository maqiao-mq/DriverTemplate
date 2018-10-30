#ifndef DEVICE_OPERATIONS_H
#define DEVICE_OPERATIONS_H

#include<linux/blkdev.h>

blk_qc_t bypass_dev_make_request(struct request_queue * q, struct bio * bio);

extern struct block_device_operations bypass_blkdev_ops;

#endif