#include "driver.h"
#include "console.h"

device_write write_func;

void console_register_write(device_write fn) {
    if (!fn) return ;
    write_func = fn ; 
}


void console_write(const char *string) {
    write_func(string);
}
