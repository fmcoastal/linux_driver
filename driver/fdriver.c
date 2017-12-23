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
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/moduleparam.h>
#ifdef FS_DMA
#include <linux/pci.h>          // for devices mapped by ecams.
#endif

#include <asm/uaccess.h>        // for copy_from_user()
#include <asm/io.h>             // for ioremap().....

#include "fdriver_device.h"    // device Structure
#include "fdriver_procfs.h"
#include "fdriver_ioctl.h"     // for IOCTL functions
//#include "../common/fdriver_ioctl.h"     // for IOCTL functions




static dev_t first;       // Global variable for the first device number
static struct cdev c_dev; // Global variable for the character device structure
static struct class *cl;  // Global variable for the device class

#define DRV_BUFFER_SZ 1024
static char g_DrvBuffer[DRV_BUFFER_SZ];
static int  g_DrvBufferSz=0;

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Frank Smith");
MODULE_DESCRIPTION("BasicDriver");

static uint64_t function = 0;
static uint64_t address = 0;
static uint64_t value = 0;
static unsigned long timeout = 200;

// http://www.linuxforu.com/2011/04/character-device-files-creation-operations/  
// http://www.xml.com/ldd/chapter/book/ch03.html
// http://www.tldp.org/LDP/lkmpg/2.6/html/x323.html

// timeout variable will show up in /sys/module/fsdriver/parameters/timeout
module_param(timeout, ulong , S_IRUGO|S_IWUSR );
MODULE_PARM_DESC(timeout, "A character string");


//
// called at fopen
//
static int my_open(struct inode *i, struct file *f)
{
  printk(KERN_INFO "Driver: open()\n");
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
  printk(KERN_INFO   "*loff_t        %d   ??\n",(int)*off);
  printk(KERN_INFO   "g_DrvBufferSz  %d \n",(int)g_DrvBufferSz);
 
  for( i = 0 ; ((i < g_DrvBufferSz) && (i < len)) ;i++)
  {
       *(buf+i) = g_DrvBuffer[i];
       printk(KERN_INFO   "%d  %c   %c\n",i,*(buf+i),g_DrvBuffer[i]);
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
  printk(KERN_INFO   "*loff_t   %d  ?? \n",(int)*off);
  
  for( i = 0 ; (i < len) && (i < DRV_BUFFER_SZ);i++)
  {
       g_DrvBuffer[i]=*(buf+i);
  }
  g_DrvBufferSz = i;
  return i;
}


//
// called at ioctl
//
static long my_ioctl(struct file *f, unsigned int cmd, unsigned long arg)
{
    fdrive_arg_t q;     // fdrive_arg_t defined in fdriver_ioctl.h
    uint64_t tmp64; 

    switch (cmd)
    {
        case FDRIVER_GET_VARIABLES:
            printk(KERN_INFO "Driver: ioctl()- FDRIVER_GET_VARIABLES\n");
            q.function = function;
            q.address  = address;
            q.value    = value;
            printk(KERN_INFO "     q.function = 0x%llx\n",q.function);
            printk(KERN_INFO "     q.address  = 0x%llx\n",q.address);
            printk(KERN_INFO "     q.value    = 0x%llx\n",q.value);
 
            if (copy_to_user((fdrive_arg_t *)arg, &q, sizeof(fdrive_arg_t)))
            {
                return -EACCES;
            }
            break;
        case FDRIVER_CLR_VARIABLES:
            printk(KERN_INFO "Driver: ioctl()- FDRIVER_CLR_VARIABLES\n");
            function = 0;
            address = 0;
            value = 0;
            break;
        case FDRIVER_SET_VARIABLES:
            printk(KERN_INFO "Driver: ioctl()- FDRIVER_SET_VARIABLES\n");
            if (copy_from_user(&q, (fdrive_arg_t *)arg, sizeof(fdrive_arg_t)))
            {
                return -EACCES;
            }
            printk(KERN_INFO "     q.function = 0x%llx\n",q.function);
            printk(KERN_INFO "     q.address  = 0x%llx\n",q.address);
            printk(KERN_INFO "     q.value    = 0x%llx\n",q.value);
            function = q.function;
            address = q.address;
            value = q.value;
            break;
        case FDRIVER_CSR_64_WR:
            printk(KERN_INFO "Driver: ioctl()- FDRIVER_CSR_64_WR \n");
            if (copy_from_user(&q, (fdrive_arg_t *)arg, sizeof(fdrive_arg_t)))
            {
                return -EACCES;
            }
      
            printk(KERN_INFO "     q.function = 0x%llx\n",q.function);
            printk(KERN_INFO "     q.address  = 0x%llx\n",q.address);
            printk(KERN_INFO "     q.value    = 0x%llx\n",q.value);

//           if(request_mem_region(q.address, 8, "FDRIVER_CSR_64_WR") != NULL)
            {
                void * map;
                map = ioremap(q.address, 8);

                if(map != NULL)
                {
                    tmp64  = readq(map);
                    writeq(q.value, map);
                    q.value = readq(map);
                    printk(KERN_INFO "reg value 0x%llx  old 0x%llx new 0x%llx\n", 
                                q.address,
                                tmp64,
                                q.value);
                     iounmap(map);
                }
                else
                {
                    printk("ioremap FAIL \n");
                }
//                release_mem_region(q.address, 8);
            }
  /*          else
            {
                printk(KERN_INFO "%d:%s request_mem_region returned null \n",__LINE__,__FILE__); 
            }
    */        if (copy_to_user((fdrive_arg_t *)arg, &q, sizeof(fdrive_arg_t)))
            {
                return -EACCES;
            }
            break;
        case FDRIVER_CSR_64_RD:
            printk(KERN_INFO "Driver: ioctl()- FDRIVER_CSR_64_RD\n");
            if (copy_from_user(&q, (fdrive_arg_t *)arg, sizeof(fdrive_arg_t)))
            {
                return -EACCES;
            }

            printk(KERN_INFO "     q.function = 0x%llx\n",q.function);
            printk(KERN_INFO "     q.address  = 0x%llx\n",q.address);
            printk(KERN_INFO "     q.value    = 0x%llx\n",q.value);

//            if(request_mem_region(q.address, 8, "FDRIVER_CSR_64_RD") != NULL)
            {
                void * map;
                map = ioremap(q.address, 8);
                printk(KERN_INFO "     map   = 0x%p\n",map);
                if(map != NULL)
                {
//                    q.value = ioread64(map); // old value
                    q.value = readq(map); // old value
                    printk(KERN_INFO "reg value 0x%llx :  0x%llx \n",q.address,q.value);
                    iounmap(map);
                }
                else
                {
                    printk(KERN_INFO "ioremap FAIL \n");
                }
//                release_mem_region(q.address, 8);
            }
/*            else
            {
                printk(KERN_INFO "%d:%s request_mem_region returned null \n",__LINE__,__FILE__); 
            }
  */          if (copy_to_user((fdrive_arg_t *)arg, &q, sizeof(fdrive_arg_t)))
            {
                return -EACCES;
            }
            break;
 

        case FDRIVER_TEST_FUNCTION:
            printk(KERN_INFO "Driver: ioctl()- FDRIVER_TEST_FUNCTION\n");

//            if(request_mem_region(0x803000000400, 8, "GPIO_BIT_CONFIG") != NULL)
            {
                void * map;
                map = ioremap(0x803000000400, 8);

                if(map != NULL)
                {
                   tmp64 = readq(map);
                    printk(KERN_INFO  "reg value  0x%llx \n", tmp64);

                   // iowrite32(0x0000FFFF, map);

                   // printk("reg value  %X \n", ioread32(map));
                }
                else
                {
                    printk(KERN_INFO "ioremap FAIL \n");
                }
            }
//            release_mem_region(0x803000000400, 8);

            break;

        default:
            return -EINVAL;
    }
    return 0;
}

/*
 * file_operations defined in fs.h
 */
static struct file_operations fdriver_fops =
{
  .owner   = THIS_MODULE,
  .open    = my_open,
  .release = my_close,
  .read    = my_read,
  .write   = my_write , 
  .unlocked_ioctl   = my_ioctl 
};


#define NUMBER_OF_INSTANCES  1
// called at insmod
static int __init fdriver_init(void)
{
    int ret;
    printk(KERN_INFO "fdriver_init \n");
    printk(KERN_INFO "   timeout  %ld \n",timeout);

// below will allow 3 instances to attach tot the driver.
// at insmod fsdriver.ko  if you "cat /proc/devices" you will see the 247 associated with fdriver. 
// you will need to execute  mknod  /dev/fdevice0 c 247 0
// you will need to execute  mknod  /dev/fdevice1 c 247 1
// you will need to execute  mknod  /dev/fdevice2 c 247 0
// you will need to execute chmod a+w /dev/fdevice*
    if (alloc_chrdev_region(&first, 0, NUMBER_OF_INSTANCES, "fsdriver") < 0)
    {
      return -1;
    }
    printk(KERN_INFO "  <Major, Minor>: <%d, %d>\n", MAJOR(first), MINOR(first));

    if ((cl = class_create(THIS_MODULE, "fschardrv")) == NULL)
    {
        unregister_chrdev_region(first, 1);
        printk(KERN_INFO " fsdriver: failed class_create\n");
        return -2;
    }

//Put this in a loop  (NUMBER_OF_INSTANCES)w/
//device_create(cl, NULL, MKNOD(MAJOR(first), MINOR(first) + i), NULL, "mynull%d", i);
  if (device_create(cl, NULL, first, NULL, "myfsdriver") == NULL)
  {
    class_destroy(cl);
    unregister_chrdev_region(first, 1);
    printk(KERN_INFO " fsdriver: failed device_create\n");
    return -3;
  }

  cdev_init(&c_dev, &fdriver_fops);

  if (cdev_add(&c_dev, first, 1) == -1)
  {
    device_destroy(cl, first);
    class_destroy(cl);
    unregister_chrdev_region(first, 1); 
    printk(KERN_INFO " fsdriver: failed cdev_add\n");
    return -4;
  }

// init /proc files
  ret = fsdriver_init_procfs(); 
  if (ret)
  {  
     goto procfs_fail;
  }

// init data structures
  memset(g_DrvBuffer,0,DRV_BUFFER_SZ);
  g_DrvBufferSz = 0; 

return 0;

procfs_fail:
   // fail need to clean up the rest of the items?
   fsdriver_remove_procfs();

return ret;



}

// called at rmmod
static void __exit fdriver_exit(void)
{


    printk(KERN_INFO "fdriver_exit\n");
    // remove the entries in /proc
    fsdriver_remove_procfs();

    cdev_del(&c_dev);
// need a loop here (NUMBER_OF_INSTANCES)
    device_destroy(cl, first);

    class_destroy(cl);
    unregister_chrdev_region(first, 3);



}





module_init(fdriver_init);

module_exit(fdriver_exit);


// ---------------------------------------
//
//  ISR Function
//

#ifdef FS_DMA

static int  fdriver_write_reg(uint64_t addr, uint64_t value)
{
void * map;
uint64_t tmp64;

    map = ioremap(address, 8);
    printk(KERN_INFO "%s     map   = 0x%p\n",__FUNCTION__,map);
    if(map != NULL)
    {
         tmp64  = readq(map);
         writeq( value, map);
         value = readq(map);
         printk(KERN_INFO "reg value 0x%llx  old 0x%llx new 0x%llx\n", 
                                address,
                                tmp64,
                                value);
          iounmap(map);
     }
     else
     {
          printk(KERN_INFO "ioremap FAIL \n");
          return -1;
     }
     return 0;
}

static uint64_t  fdriver_read_reg(uint64_t addr)
{
void * map;
uint64_t tmp64;

    map = ioremap(address, 8);
    printk(KERN_INFO "%s     map   = 0x%p\n",__FUNCTION__,map);
    if(map != NULL)
    {
         tmp64  = readq(map);
         printk(KERN_INFO "reg value 0x%llx   0x%llx \n", 
                                address,
                                tmp64);
          iounmap(map);
     }
     else
     {
          printk(KERN_INFO "ioremap FAIL \n");
          return 0;
     }
     return tmp64;
}


static int gpio_probe(struct pci_dev *pdev, const struct pci_device_id *ent)
{
#if 0
     int err;
     struct device *dev = &pdev->dev;
     struct bgx *bgx = NULL;
     u8 lmac;
     u16 sdevid;

     bgx = devm_kzalloc(dev, sizeof(*bgx), GFP_KERNEL);
     if (!bgx)
         return -ENOMEM;
     bgx->pdev = pdev;

     pci_set_drvdata(pdev, bgx);

     err = pci_enable_device(pdev);
     if (err) {
         dev_err(dev, "Failed to enable PCI device\n");
         pci_set_drvdata(pdev, NULL);
         return err;
     }

     err = pci_request_regions(pdev, DRV_NAME);
     if (err) {
         dev_err(dev, "PCI request regions failed 0x%x\n", err);
         goto err_disable_device;
     }

     /* MAP configuration registers */
     bgx->reg_base = pcim_iomap(pdev, PCI_CFG_REG_BAR_NUM, 0);
     if (!bgx->reg_base) {
         dev_err(dev, "BGX: Cannot map CSR memory space, aborting\n");
         err = -ENOMEM;
         goto err_release_regions;
     }

     pci_read_config_word(pdev, PCI_DEVICE_ID, &sdevid);
     if (sdevid != PCI_DEVICE_ID_THUNDER_RGX) {
         bgx->bgx_id = (pci_resource_start(pdev,
             PCI_CFG_REG_BAR_NUM) >> 24) & BGX_ID_MASK;
         bgx->bgx_id += nic_get_node_id(pdev) * MAX_BGX_PER_NODE;
         bgx->max_lmac = MAX_LMAC_PER_BGX;
         bgx_vnic[bgx->bgx_id] = bgx;
     } else {
         bgx->is_rgx = true;
         bgx->max_lmac = 1;
         bgx->bgx_id = MAX_BGX_PER_CN81XX - 1;
         bgx_vnic[bgx->bgx_id] = bgx;
         xcv_init_hw();
     }

     /* On 81xx all are DLMs and on 83xx there are 3 BGX QLMs and one
      * BGX i.e BGX2 can be split across 2 DLMs.
      */
     pci_read_config_word(pdev, PCI_SUBSYSTEM_ID, &sdevid);
     if ((sdevid == PCI_SUBSYS_DEVID_81XX_BGX) ||
         ((sdevid == PCI_SUBSYS_DEVID_83XX_BGX) && (bgx->bgx_id == 2)))
         bgx->is_dlm = true;

     bgx_get_qlm_mode(bgx);

     err = bgx_init_phy(bgx);
     if (err)
         goto err_enable;

     bgx_init_hw(bgx);

     /* Enable all LMACs */
     for (lmac = 0; lmac < bgx->lmac_count; lmac++) {
         err = bgx_lmac_enable(bgx, lmac);
         if (err) {
             dev_err(dev, "BGX%d failed to enable lmac%d\n",
                 bgx->bgx_id, lmac);
             while (lmac)
                 bgx_lmac_disable(bgx, --lmac);
             goto err_enable;
         }
     }
#endif
     return 0;
#if 0
 err_enable:
     bgx_vnic[bgx->bgx_id] = NULL;
 err_release_regions:
     pci_release_regions(pdev);
 err_disable_device:
     pci_disable_device(pdev);
     pci_set_drvdata(pdev, NULL);
     return err;
#endif
}

static void gpio_remove(struct pci_dev *pdev)
{
#if 0
   1368     struct bgx *bgx = pci_get_drvdata(pdev);
   1369     u8 lmac;
   1370
   1371     /* Disable all LMACs */
   1372     for (lmac = 0; lmac < bgx->lmac_count; lmac++)
   1373         bgx_lmac_disable(bgx, lmac);
   1374
   1375     bgx_vnic[bgx->bgx_id] = NULL;
   1376     pci_release_regions(pdev);
   1377     pci_disable_device(pdev);
   1378     pci_set_drvdata(pdev, NULL);
#endif
}




// Items needed to find the GPIO hardware in the OCTEONTX device

/* PCI Vendor ID */
#define PCI_VENDOR_ID_CAVIUM         0x177d
/* PCI device IDs */
#define PCI_DEVICE_ID_GPIO_PF         0xa00a
/* Subsystem device IDs */
#define PCI_SUBSYS_DEVID_GPIO_PF        0xa30a

#define DRV_NAME "gpio-fsdriver"


/* Supported devices */
static const struct pci_device_id gpio_id_table[] = {
     { PCI_DEVICE(PCI_VENDOR_ID_CAVIUM, PCI_DEVICE_ID_GPIO_PF) },
     { 0, }  /* end of table */
 };

MODULE_DEVICE_TABLE(pci, gpio_id_table);


static struct pci_driver gpio_driver = {
     .name = DRV_NAME,
     .id_table = gpio_id_table,
     .probe = gpio_probe,
     .remove = gpio_remove,
 };



///**
// * enum irqreturn
// * @IRQ_NONE            interrupt was not from this device
// * @IRQ_HANDLED         interrupt was handled by this device
// * @IRQ_WAKE_THREAD     handler requests to wake the handler thread
// */
// enum irqreturn {
//      IRQ_NONE                = (0 << 0),
//      IRQ_HANDLED             = (1 << 0),
//      IRQ_WAKE_THREAD         = (1 << 1),
//   };
//
// typedef enum irqreturn irqreturn_t;
// #define IRQ_RETVAL(x)   ((x) != IRQ_NONE)

#if 0
static irqreturn_t fdriver_intr_handler(int irq, void *nic_irq)
{
    struct nicpf *nic = (struct nicpf *)nic_irq;
    int mbx;
    u64 intr;
    u8  vf, vf_per_mbx_reg = 64;

    if (irq == nic->msix_entries[NIC_PF_INTR_ID_MBOX0].vector)
        mbx = 0;
    else
        mbx = 1;

    intr = nic_reg_read(nic, NIC_PF_MAILBOX_INT + (mbx << 3));
    dev_dbg(&nic->pdev->dev, "PF interrupt Mbox%d 0x%llx\n", mbx, intr);
    for (vf = 0; vf < vf_per_mbx_reg; vf++) {
        if (intr & (1ULL << vf)) {
            dev_dbg(&nic->pdev->dev, "Intr from VF %d\n",
                vf + (mbx * vf_per_mbx_reg));

            nic_handle_mbx_intr(nic, vf + (mbx * vf_per_mbx_reg));
            nic_clear_mbx_intr(nic, vf, mbx);
        }
    }
    return IRQ_HANDLED;
}
#endif


// PCI device ID is 0x000A for GPIO  
//           HRM Table: PCC Device ID Low Enumeration PCC_DEV_IDL_E
//   on CN83xx  PCI:  0000:00:06.0
//

//#define GPIO_MSIX_VECx_CTL(x)  0x803000F00000 + x<<
#define GPIO_MSIX_VECx_CTL(x)
#define GPIO_MSIX_PBA(x)    

#if 0
static int nic_register_interrupts(struct nicpf *nic)
{
    int i, ret;

    /* Enable MSI-X */
    ret = nic_enable_msix(nic);
    if (ret)
        return ret;

    /* Register mailbox interrupt handler */
    for (i = NIC_PF_INTR_ID_MBOX0; i < nic->num_vec; i++) {
        sprintf(nic->irq_name[i],
            "NICPF Mbox%d", (i - NIC_PF_INTR_ID_MBOX0));

        ret = request_irq(nic->msix_entries[i].vector,
                  nic_mbx_intr_handler, 0,
                  nic->irq_name[i], nic);
        if (ret)
            goto fail;

        nic->irq_allocated[i] = true;
    }

    /* Enable mailbox interrupt */
    nic_enable_mbx_intr(nic);
    return 0;

fail:
    dev_err(&nic->pdev->dev, "Request irq failed\n");
    nic_free_all_interrupts(nic);
    nic_disable_msix(nic);
    return ret;
}

#endif







#define GPIO_TX_SET  0x803000000008; 
#define GPIO_TX_CLR  0x8030000000010;
#define GPIO_BIT_CFG(x) (0x803000000400 + x*0x8)
#define GPIO_INTR(x)    (0x803000000800 + x*0x8)


#define INT_TYPE_LEVEL_SENSITIVE 0
#define INT_TYPE_LEVEL_SENSITIVE (0x0  << 3)
#define INT_TYPE_EDGE_SENSITIVE  (0x01 << 3)

static int fdriver_register_interrupts(int pin, int lvl)
{

uint64_t address = GPIO_BIT_CFG(pin);
uint64_t val;


// Register the IRQ





// Set up the Configuration 
  val = 0x04;   // 25..16  PIN_SEL  0
             //     12  TXOD     0
             //  11..8  FILT     0 no filt for now
             //   7..4  FIL_CNT  0 no filt Count
             //      2  INT_EN   1 not now.   
             //      1  XOR      0 do not invert.
             //      0  TX_OE    0 no Output



 // set up glitch Filters





}
#endif
