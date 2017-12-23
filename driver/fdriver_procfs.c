/*
 *
 *
 */
#include <linux/version.h>
#include <linux/seq_file.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>

#include "fdriver_device.h"
#include "fdriver_debug.h"
#include "fdriver_procfs.h"


static struct proc_dir_entry *base_proc;


#define WAI()    printk(KERN_INFO "%d:%s-%s\n",__LINE__,__FILE__,__FUNCTION__);    


#define FSDRIVER_DEBUG

#ifdef FSDRIVER_DEBUG
static int show_debug_level(struct seq_file *s, void *v)
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
static ssize_t write_debug_level(struct file *file, const char __user *buffer,
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

static int open_debug_level(struct inode *inode, struct file *file)
{
    WAI();
     return single_open(file, show_debug_level, NULL);
}

static const struct file_operations debug_level_fops = {
     .owner = THIS_MODULE,
     .open = open_debug_level,
     .read = seq_read,
     .llseek = seq_lseek,
     .write = write_debug_level,
     .release = single_release,
};
#endif




int fsdriver_init_procfs(void)
{
#ifdef FSDRIVER_DEBUG
    struct proc_dir_entry *entry;
#endif
    WAI();

    // create a directory in /proc with the name BASE_PROC_DIR
    // null says there is no parent directory so place in /proc
    // base_proc is a handle to the directory created.
    base_proc = proc_mkdir(BASE_PROC_DIR, NULL);
    if (!base_proc) {
        pr_err("failed to create proc directory: %s\n", BASE_PROC_DIR);
        return -EFAULT;
    }
#ifdef FSDRIVER_DEBUG
    // create a file in the base directory called "debug"
    //    file properties of "S_IFREG|S_IRUGO|S_IWUGO"
    //    base_proc is the parent directory
    //    functions to call if someone writes or reads to the Debug File.
    //       to read, simply "cat"
    //       to Write, echo  3 > /proc/FSdebug/debug
    entry = proc_create("debug", S_IFREG|S_IRUGO|S_IWUGO, base_proc, &debug_level_fops);
    if (!entry) {
        pr_err("failed to create debug proc entry\n");
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,10,0)
        proc_remove(base_proc);
#else
        remove_proc_subtree(base_proc->name, NULL);
#endif
        return -EFAULT;
    }
#endif
    return 0;
}


void fsdriver_remove_procfs(void)
{
    WAI();

#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,10,0)
    proc_remove(base_proc);
#else
    if (base_proc->subdir)
        remove_proc_subtree(base_proc->subdir->name, base_proc);

    remove_proc_subtree(base_proc->name, NULL);
#endif

}



