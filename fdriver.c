#if 0
#include "cavium_sysdep.h"
#include "cavium_defs.h"
#include "octeon-opcodes.h"
#include "octeon-common.h"
#include "octeon_user.h"
#include "cavium_release.h"
#include <signal.h>

#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>
#include "oct_req.h"
#endif

//#define MODULE
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>     /* printk() */
#include <linux/slab.h>       /* kmalloc */
#include <linux/fs.h>         /* everything .... */     
#include <linux/errno.h>      /* error codes */     
#include <linux/types.h>      /* size_t */     
//#include <linux/proc_fs.h>           
#include <linux/fcntl.h>      /* O_ACCMODE */     
#include <linux/cdev.h>       /* cdev_init, cdev_add, cdev_delete */ 
#include <asm/system.h>       /* cli(), *_flags */
#include <asm/uaccess.h>      /* has copy_to_user and copy_from_user */
//#include <linux/ip.h>     
#include <linux/device.h>     
#include "fdriver_ioctl.h"


static        dev_t first; // Global variable for the first device number
static struct cdev c_dev;  // Global variable for the character device structure
static struct class *cl;   // Global variable for the device class

#define DRV_BUFFER_SZ 1024
static char g_DrvBuffer[DRV_BUFFER_SZ];
static int  g_DrvBufferSz=0;
static int  g_status; 
static int  g_dignity; 
static int  g_ego; 

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Frank Smith");
MODULE_DESCRIPTION("BasicDriver");

// http://www.linuxforu.com/2011/04/character-device-files-creation-operations/  
// http://www.xml.com/ldd/chapter/book/ch03.html

//
// called at fopen
//
static int my_open(struct inode *i, struct file *f)
{
  printk(KERN_INFO "Driver: open()\n");
  g_status  = 10;
  g_dignity = 20;
  g_ego     = 40;

  return 0;
}
//
//called at fclose
//
  static int my_close(struct inode *i, struct file *f)
{
  printk(KERN_INFO "Driver: close()\n");
  return 0;
}
//
// called at fread
//
  static ssize_t my_read(struct file *f, char __user *buf, size_t len, loff_t *off)
{
  int i;
  printk(KERN_INFO "Driver: read()\n");
  printk(KERN_INFO   "file           %p   \n",f);
  printk(KERN_INFO   "buf            %p   \n",buf);
  printk(KERN_INFO   "len            %d   \n",(int)len);
  printk(KERN_INFO   "loff_t         %p   \n",off);
  printk(KERN_INFO   "*loff_t        %d   \n",(int)(*off));
  printk(KERN_INFO   "g_DrvBufferSz  %d \n",(int)g_DrvBufferSz);
 
  for( i = 0 ; ((i < g_DrvBufferSz) && (i < len)) ;i++)
  {
       *(buf+i) = g_DrvBuffer[i];
       printk(KERN_INFO   "%d  %c  %c  0x%02x\n",i,*(buf+i),g_DrvBuffer[i],g_DrvBuffer[i]);
  }
 
  return i;
}

//
//called at fwrite
//
  static ssize_t my_write(struct file *f, const char __user *buf, size_t len, loff_t *off)
{
  int i;
  printk(KERN_INFO "Driver: write()\n");
  printk(KERN_INFO   "file      %p   \n",f);
  printk(KERN_INFO   "buf       %p   \n",buf);
  printk(KERN_INFO   "len       %d   \n",(int)len);
  printk(KERN_INFO   "loff_t    %p   \n",off);
  printk(KERN_INFO   "*loff_t   %d   \n",(int)(*off));
  
  for( i = 0 ; (i < len) && (i < DRV_BUFFER_SZ);i++)
  {
       g_DrvBuffer[i]=*(buf+i);
  }
  g_DrvBufferSz = i;
  return i;
}



//
//  MY_IOCTL
//
static long my_ioctl(struct file *f, unsigned int cmd, unsigned long arg)
{
    fdriver_arg_t  q;
 
    switch (cmd)
    {
        case FDRIVER_GET_VARIABLES:
            printk(KERN_INFO "FDriver: IOCTL - FDRIVER_GET_VARIABLES\n");
            q.status  = g_status;
            q.dignity = g_dignity;
            q.ego     = g_ego;
            if (copy_to_user((fdriver_arg_t *)arg, &q, sizeof(fdriver_arg_t)))
            {
                return -EACCES;
            }
            break;
        case FDRIVER_CLR_VARIABLES: 
            printk(KERN_INFO "FDriver: IOCTL - FDRIVER_CLR_VARIABLES\n");
            g_status  = 0;
            g_dignity = 0;
            g_ego     = 0;
            break;
        case FDRIVER_SET_VARIABLES:
            printk(KERN_INFO "FDriver: IOCTL - FDRIVER_SET_VARIABLES\n");
            if (copy_from_user(&q, (fdriver_arg_t *)arg, sizeof(fdriver_arg_t)))
            {
                return -EACCES;
            }
            g_status  = q.status;
            g_dignity = q.dignity;
            g_ego     = q.ego;
            break;
        default:
            return -EINVAL;
    }
    return 0;
}



//
//  Driver Mapping Function
//
static struct file_operations fdriver_fops =
{
  .owner   = THIS_MODULE,
  .open    = my_open,
  .release = my_close,
  .read    = my_read,
  .write   = my_write, 
//#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,35))
//    .ioctl = my_ioctl
//#else
    .unlocked_ioctl = my_ioctl
//#endif
};


#define DRIVER_NAME "fdriver"
#define NUMBER_OF_INSTANCES  1
//
//  called at insmod
//
static int __init fdriver_init(void)
{
    printk(KERN_INFO "fdriver_init \n");

// below will allow 3 instances to attach tot the driver.
// at insmod fsdriver.ko  if you "cat /proc/devices" you will see the 247 associated with fdriver. 
// you will need to execute  mknod  /dev/fdriver0 c 247 0
// you will need to execute  mknod  /dev/fdriver1 c 247 1
// you will need to execute  mknod  /dev/fdriver2 c 247 2
// you will need to execute chmod a+w /dev/fdriver*
    if (alloc_chrdev_region(&first, 0, NUMBER_OF_INSTANCES, DRIVER_NAME) < 0)
    {
      return -1;
    }
    printk(KERN_INFO "  <Major, Minor>: <%d, %d>\n", MAJOR(first), MINOR(first));

    if ((cl = class_create(THIS_MODULE, "chardrv")) == NULL)
    {
        unregister_chrdev_region(first, 1);
        return -2;
    }

//Put this in a loop  (NUMBER_OF_INSTANCES)w/
//device_create(cl, NULL, MKNOD(MAJOR(first), MINOR(first) + i), NULL, "mynull%d", i);
  if (device_create(cl, NULL, first, NULL, "mynull") == NULL)
  {
    class_destroy(cl);
    unregister_chrdev_region(first, 1);
    return -3;
  }
  cdev_init(&c_dev, &fdriver_fops);

  if (cdev_add(&c_dev, first, 1) == -1)
  {
    device_destroy(cl, first);
    class_destroy(cl);
    unregister_chrdev_region(first, 1);
    return -4;
  }

// init data structures ??
  memset(g_DrvBuffer,0,DRV_BUFFER_SZ);
  g_DrvBufferSz = 0; 

return 0;
}

//
// called at rmmod
// 
static void __exit fdriver_exit(void)
{
    printk(KERN_INFO "fdriver_exit\n");

    cdev_del(&c_dev);
// need a loop here (NUMBER_OF_INSTANCES)
    device_destroy(cl, first);

    class_destroy(cl);
    unregister_chrdev_region(first, 3);

}

module_init(fdriver_init);
module_exit(fdriver_exit);



