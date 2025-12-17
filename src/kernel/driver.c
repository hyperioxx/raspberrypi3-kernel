#include "driver.h"
#include "../drivers/serial/pl011.h"

#define MAX_DRIVER 50
static struct driver driver_register[MAX_DRIVER];
static size_t driver_count = 0;


int driver_registry_init(){
    int err = 0;
    err = register_driver("brcm,bcm2835-pl011", pl011_init);
    err = register_driver("arm,pl011", pl011_init);
    err = register_driver("arm,pl011-axi", pl011_init);
    return err;
}


int register_driver(const char *compat, driver_init init){
    //TODO: come up withe better return codes
    if (!compat || !init) return -1;
    if (driver_count >= MAX_DRIVER ) return -1;
    struct driver *driv = &driver_register[driver_count++];
    driv->compat = compat;
    driv->init = init;
    return 0;
}

