#ifndef _FDRIVER_DEBUG_H
#define _FDRIVER_DEBUG_H





extern int g_fdriver_debug;


enum {
     MSG_DEV_INIT    = 0x001,    /**< Device resource initialization */
     MSG_FN_TRACE    = 0x002,    /**< Function trace */
     MSG_REQ_DUMP    = 0x004,    /**< Dump NITROX commands */
     MSG_DUMP_ALL    = 0x008,    /**< Dump all information */
     MSG_MAX_LEVEL   = 0x010,
 };


#define msg_init_trace(debug)   (debug & MSG_DEV_INIT)
#define msg_fn_trace(debug) (debug & MSG_FN_TRACE)
#define msg_req_dump(debug) (debug & MSG_REQ_DUMP)
#define msg_dump_all(debug) (debug & MSG_DUMP_ALL)


static inline void fdriver_set_debug_level(int level)
{
    if (level < 0 || level > 4)
        g_fdriver_debug = 0;
    else
        g_fdriver_debug = (1 << level) - 1;
}

static inline int fdriver_get_debug_level(void)
{
     return (g_fdriver_debug);
}






#endif





