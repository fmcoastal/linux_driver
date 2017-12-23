#ifndef _FSDRIVER_PROCFS_H
#define _FSDRIVER_PROCFS_H

#define BASE_PROC_DIR "FScavium"


extern int device_init_procfs(struct fsdriver_device *ndev);
extern void device_remove_procfs(struct fsdriver_device *ndev);

// called from Init
extern int fsdriver_init_procfs(void);
extern void fsdriver_remove_procfs(void);

#endif /* _FSDRIVER_PROCFS_H */
