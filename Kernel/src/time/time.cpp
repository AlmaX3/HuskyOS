#include "../../include/time.h"
#include "../../include/string.h"
#include <pit.h>
#include <stdint.h>

#define IO_RTC_INDEX 0x70
#define IO_RTC_TARGET 0x71

#define REG_SEC 0x00
#define REG_SEC_ALARM 0x01
#define REG_MIN 0x02
#define REG_MIN_ALARM 0x03
#define REG_HOUR 0x04
#define REG_HOUR_ALARM 0x05
#define REG_WEEK_DAY 0x06
#define REG_DAY 0x07
#define REG_MONTH 0x08
#define REG_YEAR 0x09
#define REG_A 0x0a
#define REG_B 0x0b
#define REG_C 0x0c
#define REG_D 0x0d

#define REGA_UIP (1 << 7) // Update In Progress

#define REGB_HOURFORM (1 << 1) // Hour Format (0 = 12hr, 1 = 24hr)
#define REGB_DM (1 << 2)       // Data Mode (0 = BCD, 1 = Binary)

static uint8_t RtcRead(uint8_t addr) {
    outportb(IO_RTC_INDEX, addr);
    return inportb(IO_RTC_TARGET);
}

void RtcWrite(uint8_t addr, uint8_t val) {
    outportb(IO_RTC_INDEX, addr);
    outportb(IO_RTC_TARGET, val);
}

uint8_t BcdToBin(uint8_t val) {
    return (val & 0xf) + (val >> 4) * 10;
}

uint8_t BinToBcd(uint8_t val) {
    return ((val / 10) << 4) + (val % 10);
}

void GetTime(DateTime *dt) {
    if (RtcRead(REG_A) & REGA_UIP) {
        Sleep_pit(3);
    }

    uint8_t sec = RtcRead(REG_SEC);
    uint8_t min = RtcRead(REG_MIN);
    uint8_t hour = RtcRead(REG_HOUR);
    uint8_t weekDay = RtcRead(REG_WEEK_DAY);
    uint8_t day = RtcRead(REG_DAY);
    uint8_t month = RtcRead(REG_MONTH);
    uint16_t year = RtcRead(REG_YEAR);

    uint8_t regb = RtcRead(REG_B);

    if (~regb & REGB_DM) {
        sec = BcdToBin(sec);
        min = BcdToBin(min);
        hour = BcdToBin(hour);
        day = BcdToBin(day);
        month = BcdToBin(month);
        year = BcdToBin(year);
    }

    year += 2000;
    weekDay--;

    // Write results
    dt->second = sec;
    dt->minute = min;
    dt->hour = hour;
    dt->day = day;
    dt->month = month;
    dt->year = year;
    dt->DayOfWeek = weekDay;
    dt->TimeZoneOffset = 1;
}

void FormatTime(DateTime *dt) {
    char* Months[12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
    char* WeekDays[7] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thrusday", "Friday", "Saturday"};
    char day[2];
    char hour[2];
    char minute[2];
    char second[2];
    HuskyStandardOutput.kprint("%lld. %s. %lld. (%s), %lld:%lld:%lld", dt->year, Months[(int)dt->month], dt->day, WeekDays[(int)dt->DayOfWeek], dt->hour, dt->minute, dt->second);
}