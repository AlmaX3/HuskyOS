#include <Math.h>
#include <Utils.h>
#include <hkStdio.h>
#include <terminal.h>
void hkStdIo::vkprint(const char *fmt, void (*putchar)(const char), va_list args) {
    int state = 0;

    bool is_long = false;
    bool is_long_long = false;

    bool is_short = false;
    bool is_short_short = false;

    bool need_precision = false;

    bool can_reset = true;
    int precision = 6;

    while (*fmt) {
        if (state == 0) {
            if (*fmt == '%') {
                state = 1;
            } else {
                putchar(*fmt);
            }
        } else if (state == 1) {
            switch (*fmt) {
            case 'c': {
                char ch = va_arg(args, int);
                putchar(ch);
                ;
                break;
            }
            case 's': {
                const char *s = va_arg(args, const char *);
                while (*s) {
                    putchar(*s);
                    *s++;
                }
                break;
            }
            case '%': {
                putchar('%');
                break;
            }
            case 'l': {
                if (is_long == true) {
                    is_long_long = true;
                } else {
                    is_long = true;
                }
                can_reset = false;
                break;
            }
            case 'h': {
                if (is_short == true) {
                    is_short_short = true;
                } else {
                    is_short = true;
                }
                can_reset = false;
                break;
            }
            case '.': {
                need_precision = true;
                can_reset = false;
                break;
            }
            case '*': {
                if (need_precision) {
                    int toBePrecision = va_arg(args, int);
                    precision = toBePrecision;
                    can_reset = false;
                }
                break;
            }
            case 'd': {
                if (is_long_long == 1) {
                    int64_t n = va_arg(args, long long);

                    char buff[32];

                    if (need_precision == true) {
                        number_to_string(n, 10, buff, precision);
                    } else {
                        number_to_string(n, 10, buff, 0);
                    }

                    for (int i = 0; buff[i]; i++) {
                        putchar(buff[i]);
                    }
                } else if (is_long == 1) {
                    int32_t n = va_arg(args, long);

                    char buff[32];

                    if (need_precision == true) {
                        number_to_string(n, 10, buff, precision);
                    } else {
                        number_to_string(n, 10, buff, 0);
                    }

                    for (int i = 0; buff[i]; i++) {
                        putchar(buff[i]);
                    }
                } else if (is_short_short == 1) {
                    int8_t n = va_arg(args, int);

                    char buff[32];

                    if (need_precision == true) {
                        number_to_string(n, 10, buff, precision);
                    } else {
                        number_to_string(n, 10, buff, 0);
                    }

                    for (int i = 0; buff[i]; i++) {
                        putchar(buff[i]);
                    }
                } else if (is_short == 1) {
                    int16_t n = va_arg(args, int);

                    char buff[32];

                    if (need_precision == true) {
                        number_to_string(n, 10, buff, precision);
                    } else {
                        number_to_string(n, 10, buff, 0);
                    }

                    for (int i = 0; buff[i]; i++) {
                        putchar(buff[i]);
                    }
                } else {
                    int16_t n = va_arg(args, int);

                    char buff[32];

                    if (need_precision == true) {
                        number_to_string(n, 10, buff, precision);
                    } else {
                        number_to_string(n, 10, buff, 0);
                    }

                    for (int i = 0; buff[i]; i++) {
                        putchar(buff[i]);
                    }
                }

                break;
            }
            case 'f': {
                if (is_long_long == 1) {
                    double n = va_arg(args, double);

                    char buff[32];
                    fnumber_to_string(n, 10, buff, precision);
                    for (int i = 0; buff[i]; i++) {
                        putchar(buff[i]);
                    }
                } else {
                    double n = va_arg(args, double);

                    char buff[32];
                    fnumber_to_string(n, 10, buff, precision);
                    for (int i = 0; buff[i]; i++) {
                        putchar(buff[i]);
                    }
                }

                break;
            }
            case 'u': {
                uint64_t n;
                if (is_long_long == 1) {
                    n = (uint64_t)va_arg(args, unsigned long long);
                } else if (is_long == 1) {
                    n = (uint32_t)va_arg(args, unsigned long);
                } else if (is_short_short == 1) {
                    n = (uint8_t)va_arg(args, unsigned int);
                } else if (is_short == 1) {
                    n = (uint16_t)va_arg(args, unsigned int);
                } else {
                    n = (uint16_t)va_arg(args, unsigned int);
                }

                char buff[32];

                if (need_precision == true) {
                    unsigned_number_to_string(n, 10, buff, precision);
                } else {
                    unsigned_number_to_string(n, 10, buff, 0);
                }

                for (int i = 0; buff[i]; i++) {
                    putchar(buff[i]);
                }

                break;
            }
            case 'x': {

                if (is_long_long == 1) {
                    uint64_t n = va_arg(args, unsigned long long);

                    char buff[32];

                    if (need_precision == true) {
                        unsigned_number_to_string(n, 16, buff, precision);
                    } else {
                        unsigned_number_to_string(n, 16, buff, 0);
                    }

                    for (int i = 0; buff[i]; i++) {
                        putchar(buff[i]);
                    }
                } else if (is_long == 1) {
                    uint32_t n = va_arg(args, unsigned long);

                    char buff[32];

                    if (need_precision == true) {
                        unsigned_number_to_string(n, 16, buff, precision);
                    } else {
                        unsigned_number_to_string(n, 16, buff, 0);
                    }

                    for (int i = 0; buff[i]; i++) {
                        putchar(buff[i]);
                    }
                } else if (is_short_short == 1) {
                    uint8_t n = va_arg(args, unsigned int);

                    char buff[32];

                    if (need_precision == true) {
                        unsigned_number_to_string(n, 16, buff, precision);
                    } else {
                        unsigned_number_to_string(n, 16, buff, 0);
                    }

                    for (int i = 0; buff[i]; i++) {
                        putchar(buff[i]);
                    }
                } else if (is_short == 1) {
                    uint16_t n = va_arg(args, unsigned int);

                    char buff[32];

                    if (need_precision == true) {
                        unsigned_number_to_string(n, 16, buff, precision);
                    } else {
                        unsigned_number_to_string(n, 16, buff, 0);
                    }

                    for (int i = 0; buff[i]; i++) {
                        putchar(buff[i]);
                    }
                } else {
                    uint16_t n = va_arg(args, unsigned int);

                    char buff[32];

                    if (need_precision == true) {
                        unsigned_number_to_string(n, 16, buff, precision);
                    } else {
                        unsigned_number_to_string(n, 16, buff, 0);
                    }

                    for (int i = 0; buff[i]; i++) {
                        putchar(buff[i]);
                    }
                }

                break;
            }
            case 'o': {

                if (is_long_long == 1) {
                    uint64_t n = va_arg(args, unsigned long long);

                    char buff[32];

                    if (need_precision == true) {
                        unsigned_number_to_string(n, 8, buff, precision);
                    } else {
                        unsigned_number_to_string(n, 8, buff, 0);
                    }

                    for (int i = 0; buff[i]; i++) {
                        putchar(buff[i]);
                    }
                } else if (is_long == 1) {
                    uint32_t n = va_arg(args, unsigned long);

                    char buff[32];

                    if (need_precision == true) {
                        unsigned_number_to_string(n, 8, buff, precision);
                    } else {
                        unsigned_number_to_string(n, 8, buff, 0);
                    }

                    for (int i = 0; buff[i]; i++) {
                        putchar(buff[i]);
                    }
                } else if (is_short_short == 1) {
                    uint8_t n = va_arg(args, unsigned int);

                    char buff[32];

                    if (need_precision == true) {
                        unsigned_number_to_string(n, 8, buff, precision);
                    } else {
                        unsigned_number_to_string(n, 8, buff, 0);
                    }

                    for (int i = 0; buff[i]; i++) {
                        putchar(buff[i]);
                    }
                } else if (is_short == 1) {
                    uint16_t n = va_arg(args, unsigned int);

                    char buff[32];

                    if (need_precision == true) {
                        unsigned_number_to_string(n, 8, buff, precision);
                    } else {
                        unsigned_number_to_string(n, 8, buff, 0);
                    }

                    for (int i = 0; buff[i]; i++) {
                        putchar(buff[i]);
                    }
                } else {
                    uint16_t n = va_arg(args, unsigned int);

                    char buff[32];

                    if (need_precision == true) {
                        unsigned_number_to_string(n, 8, buff, precision);
                    } else {
                        unsigned_number_to_string(n, 8, buff, 0);
                    }

                    for (int i = 0; buff[i]; i++) {
                        putchar(buff[i]);
                    }
                }

                break;
            }
            case 'p': {
                putchar('0');
                putchar('x');

                void *n = va_arg(args, void *);
                char buff[32];
                unsigned_number_to_string((uint64_t)n, 16, buff, 0);
                for (int i = 0; buff[i]; i++) {
                    putchar(buff[i]);
                }

                break;
            }
            default:
                break;
            }

            if (can_reset == true) {
                state = 0;
                is_long = false;
                is_long_long = false;
                is_short = false;
                is_short_short = false;
                precision = 6;
                need_precision = false;
            } else {
                can_reset = true;
            }
        }

        fmt++;
    }
}

void putchar(const char Char) {
    if (GFX_MODE == 0) {
        TextMode.putchar(Char);
    } else {
        GfxMode.putchar(Char);
    }
}

void hkStdIo::kprint(const char *fmt, ...) {
    va_list args;

    va_start(args, fmt);
    vkprint(fmt, putchar, args);

    va_end(args);
}

void hkStdIo::statuslog(uint32_t color, const char *status, const char *fmt, ...) {
    va_list args;

    va_start(args, fmt);

    GfxMode.putchar('[');
    GfxMode.putchar(' ');
    GfxMode.changefg(color);
    GfxMode.putstring(status);
    GfxMode.changefg(GfxMode.textColor);
    GfxMode.putchar(' ');
    GfxMode.putchar(']');
    GfxMode.putchar(' ');
    vkprint(fmt, putchar, args);

    va_end(args);
}

void hkStdIo::panic(s_registers *regs, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);

    GfxMode.changefg(0xff0000);
    kprint("KERNEL PANIC: %lld - 0x%llx\n", regs->interrupt_number, regs->interrupt_number);
    kprint("%s\nRAX = %llx RBP = %llx RBX = %llx\nRCX = %llx RDI = %llx RDX = %llx\nRSI = %llx RSP = %llx\n", get_interrupt_name(regs->interrupt_number), regs->rax, regs->rbp, regs->rbx, regs->rcx, regs->rdi, regs->rdx, regs->rsi, regs->rsp);
    kprint("CR0 = %llx CR2 = %llx CR3 = %llx CR4 = %llx", regs->cr0, regs->cr2, regs->cr3, regs->cr4);

    vkprint(fmt, putchar, args);
    va_end(args);

    __asm__ __volatile__("hlt");
}

void hkStdIo::assertpanic(const char *str, ...) {
    va_list args;
    va_start(args, str);
    GfxMode.changefg(0xff0000);
    vkprint(str, putchar, args);
    va_end(args);

    __asm__ __volatile__("hlt");
}
hkStdIo HuskyStandardOutput;