#ifndef FDRIVER_IOCTL_H
#define FDRIVER_IOCTL_H
#include <linux/ioctl.h>
 
typedef struct
{
    int status; 
    int dignity; 
    int ego;
} fdriver_arg_t;

#define MAJIC_VALUE 'q'
//#define MAJIC_VALUE 0xC1
 
#define FDRIVER_GET_VARIABLES _IOR( MAJIC_VALUE , 1 , fdriver_arg_t *)
#define FDRIVER_CLR_VARIABLES _IO ( MAJIC_VALUE , 2 )
#define FDRIVER_SET_VARIABLES _IOW( MAJIC_VALUE , 3 , fdriver_arg_t *)
 
#endif
