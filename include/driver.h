#ifndef DRIVER_H
#define DRIVER_H
#include "device.h"


typedef int (*driver_init)(const struct device*);

struct driver {
   const char * compat;
   driver_init init; 
};
int driver_registry_init(void); 
int register_driver(const char *compat, driver_init init);
int driver_probe_all(void);
int driver_probe_device(const struct device *dev);
#endif
