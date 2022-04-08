#pragma once
#include <stdint.h>

void unsigned_number_to_string(uint32_t number, int base, char *buffer, int precision);
void number_to_string(int32_t number, int base, char *buffer, int precision);
int fnumber_to_string(float floatNum, int base, char *buffer, int precision);