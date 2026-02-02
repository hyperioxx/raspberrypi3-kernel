#include "driver.h"
#include "../drivers/serial/pl011.h"
#include "../drivers/irq/bcm_local_irq.h"
#include "../drivers/irq/gic400.h"
#include "device.h"

#define MAX_DRIVER 50
static struct driver driver_register[MAX_DRIVER];
static size_t driver_count = 0;


int driver_registry_init(){
    int err = 0;
    err = register_driver("brcm,bcm2835-pl011", pl011_init);
    err = register_driver("arm,pl011", pl011_init);
    err = register_driver("arm,pl011-axi", pl011_init);
    err = register_driver("brcm,bcm2836-l1-intc", bcm_local_irq_init);
    err = register_driver("arm,gic-400", gic400_init);
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

int driver_probe_all(void) {
    int last_err = 0;

    for (size_t i = 0; i < device_count(); i++) {
        const struct device *dev = device_get(i);
        if (!dev || !dev->enabled) continue;

        int err = driver_probe_device(dev);
        if (err != 0) last_err = err; 
    }

    return last_err;
}

int driver_probe_device(const struct device *dev) {
    if (!dev || !dev->enabled)
        return -1;

    for (size_t i = 0; i < driver_count; i++) {
        struct driver *drv = &driver_register[i];

        if (!drv->compat || !drv->init)
            continue;

        if (device_compat_has(dev, drv->compat)) {

            return drv->init(dev);
        }
    }

    return -2;  
}

