#include<linux/fs.h>
#include<linux/genhd.h>
#include"device_operations.h"

#define sector_no_in_512_bytes_SHIFT 9

static int transfer_data(struct bio_vec * bvec, int direction, sector_t sector_no_in_512_bytes)
{
    char * start_linear_addr;
    struct page * page;
    // unsigned long flags;

    page = bvec->bv_page;
    if(direction == READ)
    {
        // start_linear_addr = bvec_kmap_irq(bvec, &flags);
        start_linear_addr = (char *)kmap_atomic(page) + bvec->bv_offset;

        memset(start_linear_addr, 0, bvec->bv_len);
        pr_debug("read operation\n");
        flush_dcache_page(page);

        // bvec_kunmap_irq(start_linear_addr, &flags);
        kunmap_atomic(start_linear_addr);
    }
    else
    {
        start_linear_addr = (char *)kmap_atomic(page) + bvec->bv_offset;

        flush_dcache_page(page);
        //copy or transfer data

        kunmap_atomic(start_linear_addr);
    }

    return 0;
}

blk_qc_t bypass_dev_make_request(struct request_queue * q, struct bio * bio)
{
    struct bio_vec bvec;
    struct bvec_iter iter;
    int direction;
    sector_t sector_no_in_512_bytes;
    int ret = 0;

    direction = bio_data_dir(bio);
    sector_no_in_512_bytes = bio->bi_iter.bi_sector;
    bio_for_each_segment(bvec, bio, iter) {
        ret = transfer_data(&bvec, direction, sector_no_in_512_bytes);
        if(ret < 0)
        {
            break;
        }
        sector_no_in_512_bytes += (bvec.bv_len >> sector_no_in_512_bytes_SHIFT);
    }

    if(ret < 0)
    {
        bio_io_error(bio);
    }
    else
    {
        bio_endio(bio);
    }

    return BLK_QC_T_NONE;
}

static int bypass_blkdev_open(struct block_device * device, fmode_t mode)
{
    pr_debug("disk open\n");
    return 0;
}

static void bypass_blkdev_release(struct gendisk * disk, fmode_t mode)
{
    pr_debug("disk close\n");
}

struct block_device_operations bypass_blkdev_ops = 
{
    .owner = THIS_MODULE,
    .open = bypass_blkdev_open,
    .release = bypass_blkdev_release
};