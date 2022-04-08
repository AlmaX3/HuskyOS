#include <terminal.h>
#include "../../include/string.h"


void textmode::initializeTextMode(stivale2_struct_tag_terminal* term) {
    terminal = term;
}

void textmode::println(const char* string) {
    int length = strlen(string+'\0');
    void *term_write_ptr = (void *)terminal->term_write;
    void (*term_write)(const char *string, size_t length) = (void (*)(const char *string, size_t length))term_write_ptr;
    term_write(string+'\0', length);
} 



void textmode::putchar(const char Char) {
    char buff[1];
    buff[0] = Char;

    println(buff);

}

textmode TextMode;