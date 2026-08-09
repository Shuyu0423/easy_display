#ifndef C_UNIT_H__
#define C_UNIT_H__

#include <stdio.h>
#include <string.h>

#define ALPHABET_SIZE 256  



void* c_memcpy(void* dest, const void* src, size_t num);
void* c_memset(void* dest, int value, size_t num);
size_t c_strlen(char *arr);

/**string search */
int sunday_search(const char *text, const char *pattern);

#endif
