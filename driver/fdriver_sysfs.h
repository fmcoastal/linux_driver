#ifndef _FSDRIVER_SYSFS_H
#define _FSDRIVER_SYSFS_H

#define BASE_SYS_DIR "FScavium"


extern int device_init_sysfs(struct fsdriver_device *ndev);
extern void device_remove_sysfs(struct fsdriver_device *ndev);

// called from Init
extern int fdriver_init_sysfs( fdriver_data* drv_data);
extern void fdriver_remove_sysfs(void);

#endif /* _FSDRIVER_SYSFS_H */
