/*
 *   the code below is based on the documentation in:
 *         https://www.kernel.org/doc/Documentation/filesystems/sysfs.txt
 *         https://www.kernel.org/doc/Documentation/kobject.txt#:~:text=%2D%20A%20kobject%20is%20an%20object,in%20the%20sysfs%20virtual%20filesystem.
 * https://www.oreilly.com/library/view/linux-device-drivers/0596005903/ch03.html    - info on dev_t 
*/
#include <linux/version.h>
#include <linux/seq_file.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <linux/device.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/cdev.h>

#include "fdriver.h"
#include "fdriver_device.h"
#include "fdriver_debug.h"
#include "fdriver_sysfs.h"

/*   create a kobject for the Driver
 *   create an attri   ( variable/file for the sysfs
 *   create a write function to modify the "Attri varable"
 *          -add kprintf to know it got called.
 *   create a read function to dump the "attri Variable"
 *          - add kprintf to know the function got called
 *
 *   register a kobject  ("driver knobject")  with the kernel       
 *   register (and associate) the "attri varible/file" with "driver kobject"  with the kernel
 *
 *   Test Read and Write 
 *
 *   unregister the attrib?
 *   unregister the kobject?
 *
 */


/*
static ssize_t show_foo(struct device *dev, struct device_attribute *attr,
                         char *buf);
static ssize_t store_foo(struct device *dev, struct device_attribute *attr,
                          const char *buf, size_t count);
*/

#ifdef SCRIPTS
// For example, declaring

static DEVICE_ATTR(foo, S_IWUSR | S_IRUGO, show_foo, store_foo);

// is equivalent to doing:

#else
/*
static struct device_attribute dev_attr_foo = {
	.attr = {
		.name = "foo",
		.mode = S_IWUSR | S_IRUGO,
	},
	.show = show_foo,
	.store = store_foo,
};
*/
#endif

/*
static ssize_t show_foo(struct device *dev, struct device_attribute *attr,
                         char *buf)
{
	return scnprintf(buf, PAGE_SIZE, "%s\n", dev->init_name);
}

static ssize_t store_foo(struct device *dev, struct device_attribute *attr,
                          const char *buf, size_t count)
{
//        snprintf(dev->init_name, FS_DEVICE_INIT_NAME_SZ, "%.*s",
//                 (int)min(count, FS_DEVICE_INIT_NAME_SZ - 1), buf);
	return count;
}
*/


#define WAI()    printk(KERN_INFO "%d:%s-%s\n",__LINE__,__FILE__,__FUNCTION__);    


#undef  FSDRIVER_SYS_DEBUG

#ifdef FSDRIVER_SYS_DEBUG
static int show_sys_debug_level(struct seq_file *s, void *v)
{
    int value = fdriver_get_debug_level();
    WAI();

    seq_printf(s, "%d   [%d - %d]\n", fls(value), fls(MSG_DEV_INIT)-1, fls(MSG_MAX_LEVEL)-1);

    return 0;
}


/* 
 * to write the debug level
 * echo 3 > /dev/FSdriver/debug
 */
static ssize_t write_sys_debug_level(struct file *file, const char __user *buffer,
                     size_t count, loff_t *ppos)
{
     int ret, value = 0;

    WAI();
     ret = kstrtoint_from_user(buffer, count, 10, &value);
     if (ret)
         return ret;

     fdriver_set_debug_level(value);

     return count;
}

static int open_sys_debug_level(struct inode *inode, struct file *file)
{
    WAI();
//     return single_open(file, show_debug_level, NULL);
    return 0;
}

static const struct file_operations sys_debug_level_fops = {
     .owner = THIS_MODULE,
     .open = open_sys_debug_level,
     .read = seq_read,
     .llseek = seq_lseek,
     .write = write_sys_debug_level,
     .release = single_release,
};
#endif

// https://www.kernel.org/doc/Documentation/filesystems/sysfs.txt
// https://lwn.net/Articles/266722/


// Define and create a Kobject for fdriver
//     for me call it "fdriver_kobject"


// Register the Kobject

// register attribute Group
//
//  Read and write values
//
// destroy attribute Group
// Destroy the Kobject


uint32_t  my_attr1 = 50;       
static ssize_t fdriver_do_read_attr1(struct device *dev, struct device_attribute *attr,
                          const char *buf)
{
//        snprintf(dev->name, sizeof(dev->name), "%.*s",
//                 (int)min(count, sizeof(dev->name) - 1), buf);

        WAI();
        return my_attr1;
}


static ssize_t fdriver_do_write_attr1(struct device *dev, struct device_attribute *attr,
                          const char *buf, size_t count)
{
   char * end;
//        snprintf(dev->name, sizeof(dev->name), "%.*s",
//                 (int)min(count, sizeof(dev->name) - 1), buf);
        my_attr1 = simple_strtol(buf,&end,10);
        WAI();
	return my_attr1;
}




// Define an attribute  
//     for me call it "fdriver_attri"

/*
 * #define DEVICE_ATTR(_name, _mode, _show, _store) \
 * struct device_attribute dev_attr_##_name = __ATTR(_name, _mode, _show, _store)
 *
 * Reference: https://www.kernel.org/doc/Documentation/driver-model/device.txt
 */

static DEVICE_ATTR(fdriver_var1, 0600, NULL, fdriver_do_write_attr1);
static DEVICE_ATTR(fdriver_var2, 0200, NULL, fdriver_do_write_attr1);


// next step create a second attr/file/varilable name
// static int sz_fdriver_attr  = 2;

// attrs are the the variables (filenames) in /sysfs ??  
static struct attribute *fdriver_dev_attrs[] = {
        &dev_attr_fdriver_var1.attr,
        &dev_attr_fdriver_var2.attr,
        NULL
};

static struct attribute_group fdriver_dev_attr_group = {
	.attrs = fdriver_dev_attrs,
};

static const struct attribute_group *fdriver_dev_attr_groups[] = {
	&fdriver_dev_attr_group,
	NULL,
};



#define CLASS_NAME "fdriver_sysfs"
// this module is "intrinsic" ??


// we need the following information 
//
int fdriver_init_sysfs( fdriver_data* drv_dat )
{
   int ret;

    WAI();


// EVOL EVOL EVOL
//    g_drv_dat.dev->groups = fdriver_dev_attr_groups;
//    need to do all this before device "dev" is registerd

   // create a kobj     This should create a directory somewhere 
   //                   under /sys with a directory called fdriver_kobj
   //     struct kobject * kobject_create_and_add(const char * name, struct kobject * parent);

    drv_dat->pkobj = kobject_create_and_add("fdriver_kobj",NULL);
    if(drv_dat->pkobj == NULL)
    {
        printk(KERN_INFO  "fdriver: create_kobject failed\n");
        return -11;
    }   
   

// create all the attribute variables
//      if only one, you could use sysfs_create_file()
// https://stackoverflow.com/questions/12970393/whats-the-difference-between-sysfs-create-file-and-sysfs-create-group
//

    ret = sysfs_create_group(drv_dat->pkobj, &fdriver_dev_attr_group);
    if (ret < 0) 
    {
            printk(KERN_INFO  "fdriver:  create_domain sysfs failed\n");
            return ret;
    }
 

#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,10,0)
#else
#endif
    return 0;
}


void fdriver_remove_sysfs(void)
{
    WAI();

    // release the attribute group  associated with sysfs_create_group() 
    sysfs_remove_group(g_drv_dat.pkobj,&fdriver_dev_attr_group);
 



    // release the kobject associated with create&add() 
    kobject_put(g_drv_dat.pkobj);
    return;

//#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,10,0)
//#else
//#endif



}



