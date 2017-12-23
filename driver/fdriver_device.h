#ifndef _FSDRIVER_DEVICE_H
#define _FSDRIVER_DEVICE_H


#define Uint64  unsigned long
#define Uint16  short
#define Uint8   unsigned char

//#include "osi_sysdep.h"
//#include "command_queue.h"
//#include "pending_queue.h"
//#include "mbox_type.h"
//#include "vf_defs.h"



#define BASE_PROC_DIR "FScavium"

#define PHYS_ADDR_T void *

struct fsdriver_hw {
    char part_name[64];
    };

struct nitrox_mmio {
    PHYS_ADDR_T start;      /**< Start of MMIO address */
    PHYS_ADDR_T end;        /**< End of MMIO address */
    unsigned int len;       /**< MMIO length */
    unsigned char *hw_addr;
    int done;
};

struct nitrox_q_vector {
    void *ndev;
    int qno;
    int valid;
    char name[30];
};

struct msix_info {
    struct nitrox_q_vector *q_vectors;
    int vec_count;      /**< Total vectors allocated */
    void *entries;      /**< MSI-X vectore table */
};





/**
 * nitrox device structure
 */
struct fsdriver_device {
    Uint16 model;           /**< Nitrox Model (device id + revision id) */
    Uint16 core_freq;       /**< Nitrox Device Frequency */
    Uint16 flags;           /**< Flags to hold device status bits */
    Uint8 idx;          /**< Device Index (0...(MAX_NITROX_DEVICES-1)) */

    struct fsdriver_hw hw;
//    struct nitrox_mmio mmio;        /**< Memory mapped Information */
   struct msix_info msix;          /**< MSI-X information */
//    struct nitrox_vf_info vf_info;

//    struct os_layer os;

    Uint8 next_mc_block;        /**< next microcode block number */
    Uint8 next_mc_idx;      /**< next microcode index */
    Uint8 next_ae_group;
    Uint8 next_se_group;

    Uint8 total_se_cores;
    Uint8 total_ae_cores;
    Uint8 total_zip_engines;

    Uint8 avail_se_cores;
    Uint8 avail_ae_cores;

    int int_count_thold;
    int int_time_thold;

    Uint64 se_core_mask;
    Uint64 low_ae_core_mask;
    Uint64 hi_ae_core_mask;
    Uint64 avail_se_coremask;
    Uint64 avail_ae_coremask_lo;
    Uint64 avail_ae_coremask_hi;

//    struct nitrox_mbox_info mbox;
  /* OS specific */
    void *pdev;             /**< pci device handle */
    void *wqe_info;             /**< BH worker threads */
    void *sysdev;               /**< sysfs device */
    void *proc;             /**< proc dir */
    void *context ;             /**< Context Specific Infomation*/
    void *nqueue_info ;         /**< Queue Specific Information*/

    /* command queue information */
    int qlen;
    int nr_queues;

    /* AE command and pending queues */
//    struct command_qinfo ae_cqinfo;     /**< AE command queue information */
//    struct pending_qinfo ae_pqinfo;     /**< AE pending queue information */

    /* SE command and pending queues */
//    struct command_qinfo se_cqinfo;     /**< SE command queue information */
//    struct pending_qinfo se_pqinfo;     /**< SE pending queue information */

    /* structure to store data stats */
//    struct nitrox_stats stats;

#ifdef ENABLE_UNSOLICIT_PORT
    struct command_qinfo uns_cqinfo;    /**< SE unsolict queue information */
#endif

};






#endif /* _FSDRIVER_DRIVER_H */
