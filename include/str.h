#ifdef STRINGS_H
#define STRINGS_H
static  inline int streq(const char *a, const char *b) {
    while (*a && *b && *a == *b) { a++; b++; }
    return (*a == 0 && *b == 0);
}
#endif
