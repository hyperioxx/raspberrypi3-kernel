

#ifndef CONSOLE_H
#define CONSOLE_H

typedef void (*device_write)(const char *s);

void console_register_write(device_write fn);
void console_write(const char *string); 
#endif
