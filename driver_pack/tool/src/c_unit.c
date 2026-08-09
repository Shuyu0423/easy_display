/*
 * @Author       : 824229900@qq.com
 * @Date         : 2025-07-31 21:52
 * @LastEditors  : Please set LastEditors
 * @LastEditTime : 2025-08-07 20:55
 * @Description  : 
 */

 #include "c_unit.h"
 
void* c_memcpy(void* dest, const void* src, size_t num) {
    char* dest_ptr = (char*)dest;       
    const char* src_ptr = (const char*)src; 
    void* ret = dest;                   
    
    while (num--) {
        *dest_ptr++ = *src_ptr++;      
    }
    
    return ret;
}

/**
 * @brief  Fill a memory block with a byte value.
 * @param  dest  Destination memory address.
 * @param  value Value to write, converted to unsigned char.
 * @param  num   Number of bytes to fill.
 * @retval Destination memory address.
 */
void* c_memset(void* dest, int value, size_t num) {
    unsigned char* dest_ptr = (unsigned char*)dest;
    unsigned char fill_value = (unsigned char)value;
    void* ret = dest;

    while (num--) {
        *dest_ptr++ = fill_value;
    }

    return ret;
}


size_t c_strlen(char *arr)
{
	char* p1 = arr, * p2 = arr;
	while (*p2++ != '\0');
	return p2 - p1 - 1;
}


int sunday_search(const char *text, const char *pattern) {
    int text_len = strlen(text);
    int pattern_len = strlen(pattern);
    
    if (pattern_len == 0) return 0;
    if (text_len < pattern_len) return -1;
    
    int shift_table[ALPHABET_SIZE];
    
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        shift_table[i] = pattern_len + 1;
    }
    
    for (int i = 0; i < pattern_len; i++) {
        shift_table[(unsigned char)pattern[i]] = pattern_len - i;
    }
    
    int i = 0; 
    
    while (i <= text_len - pattern_len) {
        int j = 0; 
        
        while (j < pattern_len && text[i + j] == pattern[j]) {
            j++;
        }

        if (j == pattern_len) {
            return i;
        }
        
        if (i + pattern_len >= text_len) {
            break;
        }
        
        unsigned char next_char = text[i + pattern_len];
        
        i += shift_table[next_char];
    }
    
    return -1; 
}

