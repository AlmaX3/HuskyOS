#include <pit.h>
#include <terminal.h>
#include <serial.h>

#define PIT_A 0x40
#define PIT_B 0x41
#define PIT_C 0x42
#define PIT_CONTROL 0x43

#define PIT_MASK 0xFF
#define PIT_SCALE 1193180
#define PIT_SET 0x36

#define TIMER_IRQ 0

#define SUBTICKS_PER_TICK 100

void timer_phase(int hz) {
	int divisor = PIT_SCALE / hz;
	outportb(PIT_CONTROL, PIT_SET);
	outportb(PIT_A, divisor & PIT_MASK);
	outportb(PIT_A, (divisor >> 8) & PIT_MASK);
}

extern unsigned long timer_ticks = 0;
unsigned char timer_subticks = 0;

void timer_handler(s_registers *regs) {
	if (++timer_subticks == SUBTICKS_PER_TICK) {
		timer_ticks++;
		timer_subticks = 0;
	}
}


void Sleep_pit(uint32_t s) {
	uint32_t startTime = timer_ticks;
	while (timer_ticks < startTime + s) {
		//kprint("%d %s %d\n", timer_ticks, timer_ticks < startTime + s ? "!=" : "==", startTime + s);
        asm("hlt");
	}
}

uint32_t getUptime() {
	return timer_ticks;
}

uint32_t getUpSubtime() {
	return timer_subticks;
}

void timer_install() {
    pic_clear_mask(0);
    register_interrupt_handler(0, timer_handler);
	timer_phase(100);
    HuskyStandardOutput.statuslog(GREEN,"Timer", "Inititialized\n");
	debug("Timer initialized.\n");
}
