#include <TimerStateMachine.hpp>
#include <Ssd1306Adapter.hpp>

extern Ssd1306Adapter display;
const uint8_t MSG_SET[] = "Set time";
const uint8_t MSG_SET_SZ = 8;
const uint8_t MSG_PAUSE[] = "Paused";
const uint8_t MSG_PAUSE_SZ = 6;
const uint8_t MSG_TOTAL[] = "Total time";
const uint8_t MSG_TOTAL_SZ = 10;

static timer_event_status_t _handle_state_SET(timer_t *t, timer_event_t *e);
static timer_event_status_t _handle_state_IDLE(timer_t *t, timer_event_t *e);
static timer_event_status_t _handle_state_STAT(timer_t *t, timer_event_t *e);
static timer_event_status_t _handle_state_PAUSE(timer_t *t, timer_event_t *e);
static timer_event_status_t _handle_state_COUNT(timer_t *t, timer_event_t *e);
static void _t_beep(void);

bool timerInit(timer_t *t) {
    timer_event_t entryEvent = {ENTRY};
    t->current = 0;
    t->elapsed = 0;
    t->time = 0;
    t->state = S_IDLE;
    timerStateMachine(t, &entryEvent);
    return true;
}

timer_event_status_t timerStateMachine(timer_t *timer, timer_event_t *e) {
    switch (timer->state) {
        case S_IDLE:
            return _handle_state_IDLE(timer, e);
        case S_SET:
            return _handle_state_SET(timer, e);
        case S_PAUSED:
            return _handle_state_PAUSE(timer, e);
        case S_COUNT:
            return _handle_state_COUNT(timer, e);
        case S_STAT:
            return _handle_state_STAT(timer, e);
    }
    return EVENT_UNCAUGHT;
}

timer_event_status_t _handle_state_IDLE(timer_t *const timer, timer_event_t *const e) {
    switch (e->sig) {
        case ENTRY:
            display.displayTime(timer->current);
            display.displayMessage(MSG_SET, MSG_SET_SZ);
            return EVENT_HANDLED;
        case EXIT:
            display.clear();
            return EVENT_HANDLED;
        case TIME_TIC:
            if (((timer_event_tick_t *)e)->ss >= 5) {
                _t_beep(); // Unimplemented.
                return EVENT_HANDLED;
            }
            return EVENT_IGNORED;
        case TIME_INC:
            timer->state = S_SET;
            return EVENT_TRANS;
        case TIME_DEC:
            timer->state = S_SET;
            return EVENT_TRANS;
        case START:
            timer->state = S_STAT;
            return EVENT_TRANS;
        case ABORT:
            return EVENT_IGNORED;
    }
    return EVENT_UNCAUGHT;
}

timer_event_status_t _handle_state_SET(timer_t *const timer, timer_event_t *const e) {
    switch (e->sig) {
        case ENTRY:
            display.displayTime(timer->current);
            return EVENT_HANDLED;
        case EXIT:
            display.clear();
            return EVENT_HANDLED;
        case TIME_DEC:
            if (timer->current < 60) {
                return EVENT_IGNORED;
            }
            else {
                timer->current -= 60;
                display.displayTime(timer->current);
                return EVENT_HANDLED;
            }
        case TIME_INC:
            timer->current += 60;
            display.displayTime(timer->current);
            return EVENT_HANDLED;
        case TIME_TIC:
            return EVENT_IGNORED;
        case ABORT:
            timer->state = S_IDLE;
            return EVENT_TRANS;
        case START:
            timer->state = S_COUNT;
            return EVENT_TRANS;
    }
    return EVENT_UNCAUGHT;
}

timer_event_status_t _handle_state_COUNT(timer_t *const timer, timer_event_t *const e) {
    switch (e->sig) {
        case ENTRY:
            display.displayTime(timer->current);
            return EVENT_HANDLED;
        case EXIT:
            timer->time += timer->elapsed;
            timer->elapsed = 0;
            display.clear();
            return EVENT_HANDLED;
        case START:
            timer->state = S_PAUSED;
            return EVENT_TRANS;
        case TIME_INC:
            return EVENT_IGNORED;
        case TIME_DEC:
            return EVENT_IGNORED;
        case TIME_TIC:
            if (((timer_event_tick_t *)e)->ss == 10) {
                timer->current -= 1;
                timer->elapsed += 1;
                display.displayTime(timer->current);
                if (timer->current == 0) {
                    timer->state = S_IDLE;
                    return EVENT_TRANS;
                }
                return EVENT_HANDLED;
            }
            return EVENT_IGNORED;
        case ABORT:
            return EVENT_IGNORED;
    }
    return EVENT_UNCAUGHT;
}

timer_event_status_t _handle_state_PAUSE(timer_t *const timer, timer_event_t *const e) {
    switch (e->sig) {
        case ENTRY:
            // display.clear();
            display.displayTime(timer->current);
            display.displayMessage(MSG_PAUSE, MSG_PAUSE_SZ);
            return EVENT_HANDLED;
        case EXIT:
            display.clear();
            return EVENT_HANDLED;
        case TIME_INC:
            timer->state = S_SET;
            return EVENT_TRANS;
        case TIME_DEC:
            timer->state = S_SET;
            return EVENT_TRANS;
        case TIME_TIC:
            return EVENT_IGNORED;
        case START:
            timer->state = S_COUNT;
            return EVENT_TRANS;
        case ABORT:
            timer->state = S_IDLE;
            return EVENT_TRANS;
    }
    return EVENT_UNCAUGHT;
}

timer_event_status_t _handle_state_STAT(timer_t *const timer, timer_event_t *const e) {
    switch (e->sig) {
        case ENTRY:
            // display.clear();
            display.displayTime(timer->time);
            display.displayMessage(MSG_TOTAL, MSG_TOTAL_SZ);
            return EVENT_HANDLED;
        case EXIT:
            display.clear();
            return EVENT_HANDLED;
        case TIME_INC:
            return EVENT_IGNORED;
        case TIME_DEC:
            return EVENT_IGNORED;
        case TIME_TIC:
            if (((timer_event_tick_t *)e)->ss == 10) {
                timer->state = S_IDLE;
                return EVENT_TRANS;
            }
            return EVENT_IGNORED;
        case START:
            return EVENT_IGNORED;
        case ABORT:
            return EVENT_IGNORED;
    }
    return EVENT_UNCAUGHT;
}

static void _t_beep(void) {
}