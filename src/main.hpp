#ifndef SRC_MAIN_HPP_INCLUDED
#define SRC_MAIN_HPP_INCLUDED

#include <stdint.h>
#include <TimerStateMachine.hpp>

#define P_BUTT_INC 4
#define P_BUTT_DEC 5
#define P_BUTT_STRT 6
#define P_BUTT_ABRT 7
#define P_LED 8

#define B_INC (1 << 0)
#define B_DEC (1 << 1)
#define B_STRT (1 << 2)
#define B_ABRT (1 << 3)

uint8_t debounceBtnArr(uint8_t bVal);
void timerEvtDispatch(timer_t *t, timer_event_t *e);

#endif