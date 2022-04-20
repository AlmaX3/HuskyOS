#pragma once
#include <stdint.h>

typedef struct DateTime {
    int32_t second;
    int32_t minute;
    int32_t hour;
    int32_t day;
    int32_t month;
    int32_t year;
    int32_t DayOfWeek;
    int32_t DayOfYear;
    int32_t TimeZoneOffset;
} DateTime;

void GetTime(DateTime *dt);
void FormatTime(DateTime *dt);