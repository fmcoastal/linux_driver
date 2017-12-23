#ifndef FDRIVER_IOCTL_H
#define FDRIVER_IOCTL_H
#include <linux/ioctl.h>

#define FDRIVER_READ_ADDRESS  0
#define FDRIVER_WRITE_ADDRESS 1


typedef struct
{
    uint64_t  function;
    uint64_t  address;
    uint64_t  value; 
} fdrive_arg_t ;




#define FDRIVER_GET_VARIABLES _IOR('q', 1, fdrive_arg_t *)
#define FDRIVER_CLR_VARIABLES _IO ('q', 2)
#define FDRIVER_SET_VARIABLES _IOW('q', 3, fdrive_arg_t *)
#define FDRIVER_CSR_64_RD     _IOR('q', 4, fdrive_arg_t *)
#define FDRIVER_CSR_64_WR     _IOW('q', 5, fdrive_arg_t *)
#define FDRIVER_TEST_FUNCTION _IO ('q', 6 )



#endif
