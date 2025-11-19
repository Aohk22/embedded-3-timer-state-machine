#ifndef LIB_TIMERSTATEMACHINE_HPP_INCLUDED
#define LIB_TIMERSTATEMACHINE_HPP_INCLUDED

#include <stdint.h>

typedef enum
{
  EVENT_HANDLED,
  EVENT_IGNORED,
  EVENT_TRANS,
  EVENT_UNCAUGHT
} timer_event_status_t;

typedef enum {
  S_IDLE,
  S_SET,
  S_PAUSED,
  S_COUNT,
  S_STAT
} timer_state_t;

typedef enum {
  TIME_INC,
  TIME_DEC,
  TIME_TIC,
  START,
  ABORT,
  ENTRY,
  EXIT,
} timer_signals_t;

typedef struct {
  uint32_t current;
  uint32_t elapsed;
  uint32_t time;
  timer_state_t state;
} timer_t;

// Event types.
typedef struct
{
  timer_signals_t sig;
} timer_event_t;

typedef struct
{
  timer_event_t super;
} timer_event_user_t;

typedef struct
{
  timer_event_t super;
  uint8_t ss; // The "parameter".
} timer_event_tick_t;

// Function prototypes.
bool timerInit(timer_t *timer);
timer_event_status_t timerStateMachine(timer_t *timer, timer_event_t *e);

#endif