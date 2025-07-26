#ifndef _KERN_HEAD
#define _KERN_HEAD

#include<linux/init.h>
#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/cdev.h>
#include<linux/device.h>
#include<linux/fs.h>
#include<linux/slab.h>
#include<linux/uaccess.h>
#include<linux/errno.h>
#include<linux/mutex.h>

#define CBUFMSG 50
#define CBUFSZ 100

//wrapping up the cdev struct

struct cdev_data
{
    struct cdev cdev;
    struct mutex lock;
};

//circ buffer struct

typedef struct circ_buf
{
    char data[CBUFMSG][CBUFSZ];
    int count;
    int head;
    int tail;
}cbuf;

extern void cbufr_init(void);

#endif
