
//#include <stdint.h>
#include "fdriver_debug.h"


int g_fdriver_debug = 0;



// ! 12/17/2016

//http://www.tldp.org/LDP/lkmpg/2.6/html/x323.html
// TO pass a value in at load - ./insmod fdriver.ko g_fsdriver_debug=5

// module_param(g_fdriver_debug, int, 0);

// MODULE_PARM_DESC(g_fdriver_debug, "Debug level (0=default, 1=device init, 2=function trace, 3=command dump, 4=all above and payload dumps)");
// ! 12/17/2016




