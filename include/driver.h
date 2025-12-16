#ifndef DRIVER_H
#define DRIVER_H
#include "device.h"


typedef int (*driver_init)(const struct device*);

struct driver {
   const char * compat;
   driver_init init; 
};


#endif
