#ifndef DriverFileOperations_H
#define DriverFileOperations_H

int DriverOpen(struct block_device * device, fmode_t mode);

void DriverRelease(struct gendisk * disk, fmode_t mode);

int DriverIOControl(struct block_device * device, fmode_t mode, unsigned uiCmd, unsigned long uiArg);

void DriverFullRequestQueueHandle(struct request_queue * queue);

void DriverMakeRequestFunc(struct request_queue * q, struct bio * bio);

#endif
