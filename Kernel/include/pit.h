#pragma once
#include <stddef.h>
#include <stdint.h>
#include "string.h"
#include <isr.h>
#include <hkStdio.h>
#include <pic.h>


void timer_phase(int hz);
void Sleep_pit(uint32_t s);
void timer_handler(s_registers *regs); 
uint32_t getUptime();
uint32_t getUpSubtime();
void timer_install();