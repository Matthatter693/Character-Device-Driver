#include"kernhead.h"

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


//driver variables

static int cdev_major = 0;
static struct class *mycdev_class =NULL;
static struct cdev_data mycdev_data;
static cbuf *cbufr=NULL;

//buffer init function

void cbufr_init(void)
{
    cbufr=kzalloc(sizeof(cbuf),GFP_KERNEL);
    cbufr->count=0;
    cbufr->head=0;
    cbufr->tail=0;
}

//file operation definitions


//open
static int cdev_open(struct inode *inode,struct file *file)
{
    printk(KERN_DEBUG"Entering: %s\n",__func__);
    return 0;
}


//close
static int cdev_release(struct inode *inode,struct file *file)
{
    printk(KERN_DEBUG"Entering: %s\n",__func__);
    return 0;
}

//ioctl-unused currently
static long cdev_ioctl(struct file *file,unsigned int cmd,unsigned long arg)
{
    printk(KERN_DEBUG"Entering: %s\n",__func__);
    return 0;
}

//read call for buffer
static ssize_t cdev_read(struct file *file, char __user *buf,size_t count,loff_t *offset)
{
    size_t udatalen;
   

    //critical section entering mutex lock
    if(mutex_lock_interruptible(&mycdev_data.lock))
        return -ERESTARTSYS;
    
    printk(KERN_DEBUG"Entering: %s\n",__func__);
    
    //buffer-check
    if((cbufr->count)==0)
    {
        printk(KERN_ERR"Buffer is empty\n");
        return -EFAULT;
    }
    
    //determining read length 
    udatalen=strlen(cbufr->data[cbufr->tail]);
    printk(KERN_DEBUG"User data len: %zu\n",udatalen);

    if(count>udatalen)
        udatalen=count;
    
    //copying to userspace from kernel space
    if(copy_to_user(buf,cbufr->data[cbufr->tail],count)!=0)
    { 
        printk(KERN_ERR"Copy to the user failed\n");
        return -EFAULT;
    }
    
    //updating buffer stats
    cbufr->tail=(cbufr->tail+1)%CBUFMSG;
    cbufr->count--;

    //mutex unlock
    mutex_unlock(&mycdev_data.lock);
    return count;
}


//write operation
static ssize_t cdev_write(struct file *file,const char __user *buf,size_t count,loff_t *offset)
{
    size_t udatalen=CBUFSZ;
    size_t nbr_chars = 0;

    if(mutex_lock_interruptible(&mycdev_data.lock))
        return -ERESTARTSYS;

    printk(KERN_DEBUG"Entering %s\n",__func__);

    if((cbufr->count)==CBUFMSG)
    {
        printk(KERN_ERR"Buffer is full\n");
        return -EFAULT;
    }

    if(count<udatalen)
        udatalen=count;

    nbr_chars = copy_from_user(cbufr->data[cbufr->head],buf,udatalen);

    if(nbr_chars == 0)
    {
        printk(KERN_DEBUG"copied %zu bytes from the user\n",udatalen);
        printk(KERN_DEBUG"Receive data from user:%s",cbufr->data[cbufr->head]);
        cbufr->head=(cbufr->head+1)%CBUFMSG;
        cbufr->count++;
    }
    else
    {
        printk(KERN_ERR"Copy data from user failed\n");
        return -EFAULT;
    }

    mutex_unlock(&mycdev_data.lock);
    return count;
}


// creating driver file operation class
static const struct file_operations cdev_fops =
{
    .owner= THIS_MODULE,
    .open = cdev_open,
    .release = cdev_release,
    .unlocked_ioctl = cdev_ioctl,
    .read = cdev_read,
    .write = cdev_write
};


int init_module(void)
{
    int err;
    struct device *dev_ret;
    dev_t dev;

    //mutex-init
    mutex_init(&mycdev_data.lock);

    printk(KERN_DEBUG "Entering: %s\n", __func__);

    //allocating major and minor numbers
    err = alloc_chrdev_region(&dev,0,1,"cbuf");
    if(err<0)
    {
        printk(KERN_ERR"Allocate a range of char device numbers failed.\n");
        return err;
    }
    cdev_major = MAJOR(dev);
    printk(KERN_DEBUG"device major number is:%d\n",cdev_major);

    //creating device class
    if(IS_ERR(mycdev_class = class_create("cbuf")))
    {
        unregister_chrdev_region(MKDEV(cdev_major,0),1);
        return PTR_ERR(mycdev_class);
    }

    //creating device object
    if(IS_ERR(dev_ret = device_create(mycdev_class, NULL,MKDEV(cdev_major,0),NULL,"cbuf-0")))
    {
        class_destroy(mycdev_class);
        unregister_chrdev_region(MKDEV(cdev_major,0),1);
        return PTR_ERR(dev_ret);
    }

    cdev_init(&mycdev_data.cdev,&cdev_fops);
    mycdev_data.cdev.owner=THIS_MODULE;

    err=cdev_add(&mycdev_data.cdev,MKDEV(cdev_major,0),1);

    if(err<0)
    {
        printk(KERN_ERR"Unable to add a char device\n");
        device_destroy(mycdev_class,MKDEV(cdev_major,0));
        class_unregister(mycdev_class);
        class_destroy(mycdev_class);
        unregister_chrdev_region(MKDEV(cdev_major,0),1);
        return err;
    }

    //allocating memory for the circular buffer

    cbufr_init();

    if(cbufr==NULL)
    {
        printk(KERN_ERR"Allocation data buffer memory failed\n");
        device_destroy(mycdev_class,MKDEV(cdev_major,0));
        class_unregister(mycdev_class);
        class_destroy(mycdev_class);
        unregister_chrdev_region(MKDEV(cdev_major,0),1);
        return -ENOMEM;
    }

    return 0;

}

//cleanup function
void cleanup_module(void)
{
    printk(KERN_DEBUG"Entering:%s\n",__func__);
    device_destroy(mycdev_class,MKDEV(cdev_major,0));
    class_unregister(mycdev_class);
    class_destroy(mycdev_class);
    unregister_chrdev_region(MKDEV(cdev_major,0),1);
    if(cbufr != NULL)
        kfree(cbufr);
}

MODULE_DESCRIPTION("CIRCULAR BUFFER DEVICE");
MODULE_VERSION("1.0");
MODULE_AUTHOR("Praveen Shanmugam <pra060903@gmail.com>");
MODULE_LICENSE("GPL");
