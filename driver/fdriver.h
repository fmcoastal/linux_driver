#ifndef _fdriver_h
#define _fdriver_h


typedef struct fdriver_data_struct {
     dev_t        first;    // Global variable for the first device number
                            //        this contains Major and Minor Numbers
     struct cdev  c_dev;    // Global variable for the character device structure
     struct class *cl;      // Global variable for the device class
     struct device *pdevice ;  // Global variable for the device.
     struct kobject * pkobj ;   // kobj created for sysfs variables
}fdriver_data;

extern fdriver_data  g_drv_dat; // garbage can of variable data.



#endif


