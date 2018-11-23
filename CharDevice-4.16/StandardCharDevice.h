#ifndef UNIT_TEST_MODULE_H
#define UNIT_TEST_MODULE_H

#include <linux/fs.h>
#include <linux/cdev.h>

struct StandardCharDevice
{
    struct cdev cdev;
    dev_t dev_no;
    struct class * class;
    struct device *device;
};

extern struct StandardCharDevice gstandard_cdev;

#endif