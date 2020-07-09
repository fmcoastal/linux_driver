/*
 *  This code is meant to be an example of how to create a file under /sys which can be 
 *  written and read.
 *  
 *  in my example a global variable my_attr1 is modified by 
 *      echo 22 > /sys/fdriver_kobj/fdriver_var1
 *   and read by
 *      cat /sys/fdriver_kobj/fdriver_var1
 *
 *   There is a second "attri" file created, but I did not create "show" and "store" 
 *   functions
 *
 *
 *   To play:
 *      
 *      root@ubuntu:/home/fsmith/linux# pwd
 *      /home/fsmith/linux
 *      root@ubuntu:/home/fsmith/linux# insmod ./drivers/fdriver/fsdriver.ko
 *      [ 3140.896754] fdriver_init
 *      [ 3140.899371]    timeout  200
 *      [ 3140.902620]   <Major, Minor>: <504, 0>
 *      [ 3140.910094] 78:drivers/fdriver/fdriver_procfs.c-fsdriver_init_procfs
 *       root@ubuntu:/home/fsmith/linux# cat /sys/fdriver_kobj/fdriver_var1
 *       50
 *      root@ubuntu:/home/fsmith/linux# echo 22 > /sys/fdriver_kobj/fdriver_var1
 *      [ 3164.092824] fdriver_do_write_attr1  my_attr1: 22
 *      root@ubuntu:/home/fsmith/linux# cat /sys/fdriver_kobj/fdriver_var1
 *      22
 *      root@ubuntu:/home/fsmith/linux# rmmod ./drivers/fdriver/fsdriver.ko
 *      [ 3174.834459] fdriver_exit
 *      [ 3174.836991] 112:drivers/fdriver/fdriver_procfs.c-fsdriver_remove_procfs
 *      root@ubuntu:/home/fsmith/linux#
 *
 *
 *  the code below is based on the documentation in:
 *     https://www.kernel.org/doc/Documentation/filesystems/sysfs.txt
 *     https://www.kernel.org/doc/Documentation/kobject.txt#:~:text=%2D%20A%20kobject%20is%20an%20object,in%20the%20sysfs%20virtual%20filesystem.
 *     https://www.oreilly.com/library/view/linux-device-drivers/0596005903/ch03.html    - info on dev_t 
 *     https://www.cs.swarthmore.edu/~newhall/sysfstutorial.pdf
 *     https://lwn.net/Articles/266722/
 * 
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

#if 1
#define WAI()
#else
#define WAI()    printk(KERN_INFO "%d:%s-%s\n",__LINE__,__FILE__,__FUNCTION__);    
#endif
/*   
 *   create an attri   ( variable/filename for the sysfs )
 *   create a write function to modify the "Attri varable"
 *   create a read function to dump the "attri Variable"
 *   create array of attri stuctures
 *   create "attribute_group" and attach "array" from above as .attrs variable
 *   
 *   create  a kobject for the Driver
 *   register the kobject  ("driver knobject") with the kernel       
 *   register (and associate) the "attribute_group" & "driver kobject" with the kernel
 *
 *   Test Read and Write 
 *
 *   unregister the attribute group
 *   unregister the kobject
 *
 */


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
                          char *buf)
{
//        snprintf(dev->name, sizeof(dev->name), "%.*s",
//                 (int)min(count, sizeof(dev->name) - 1), buf);

        WAI();
        return scnprintf(buf, PAGE_SIZE, "%d\n",my_attr1);
}


static ssize_t fdriver_do_write_attr1(struct device *dev, struct device_attribute *attr,
                          const char *buf, size_t count)
{
   char * end;
//        snprintf(dev->name, sizeof(dev->name), "%.*s",
//                 (int)min(count, sizeof(dev->name) - 1), buf);
        my_attr1 = simple_strtol(buf,&end,10);
        WAI();
        printk(KERN_INFO   "%s  my_attr1: %d \n",__FUNCTION__,my_attr1);
//        fdriver_set_debug_level(my_attr1);
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

static DEVICE_ATTR(fdriver_var1, 0600, fdriver_do_read_attr1, fdriver_do_write_attr1);
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

/* - array of groups.  not needed for my simple driver
static const struct attribute_group *fdriver_dev_attr_groups[] = {
	&fdriver_dev_attr_group,
	NULL,
};
*/


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


/***************************************************************
 ***************************************************************
 *
 * Outline reference
 *
 **************************************************************
 **************************************************************
 *
static ssize_t show_foo(struct device *dev, struct device_attribute *attr,
                         char *buf);
static ssize_t store_foo(struct device *dev, struct device_attribute *attr,
                          const char *buf, size_t count);

// For example, declaring

static DEVICE_ATTR(foo, S_IWUSR | S_IRUGO, show_foo, store_foo);

// is equivalent to doing:

static struct device_attribute dev_attr_foo = {
	.attr = {
		.name = "foo",
		.mode = S_IWUSR | S_IRUGO,
	},
	.show = show_foo,
	.store = store_foo,
};

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





