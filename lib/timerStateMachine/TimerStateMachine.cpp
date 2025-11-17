#include <TimerStateMachine.hpp>
#include <Display.hpp>

// Adafruit_SSD1306 *disp = lcd_init();

const uint8_t MSG_SET[] = "Set time";
const uint8_t MSG_SET_SZ = 8;
const uint8_t MSG_PAUSE[] = "Paused";
const uint8_t MSG_PAUSE_SZ = 6;
const uint8_t MSG_TOTAL[] = "Total time";
const uint8_t MSG_TOTAL_SZ = 10;

void timer_init(timer_t *t);
timer_event_status_t timer_state_machine(timer_t *t, timer_event_t *e);
static timer_event_status_t _handle_state_SET(timer_t *t, timer_event_t *e);
static timer_event_status_t _handle_state_IDLE(timer_t *t, timer_event_t *e);
static timer_event_status_t _handle_state_STAT(timer_t *t, timer_event_t *e);
static timer_event_status_t _handle_state_PAUSE(timer_t *t, timer_event_t *e);
static timer_event_status_t _handle_state_COUNT(timer_t *t, timer_event_t *e);
static void _t_beep(void);

void timer_init(timer_t *t) {
    t->current = 0;
    t->elapsed = 0;
    t->time = 0;
    t->state = S_IDLE;
}

timer_event_status_t timer_state_machine(timer_t *timer, timer_event_t *e) {
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
}

timer_event_status_t _handle_state_IDLE(timer_t *const timer, timer_event_t *const e) {
    switch (e->sig) {
        case ENTRY:
            timer->current = 0;
            timer->elapsed = 0;
            timer->time = 0;
            displayTime(timer->current);
            displayMessage(MSG_SET, MSG_SET_SZ);
            return EVENT_HANDLED;
        case EXIT:
            displayClear();
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
    }
}

timer_event_status_t _handle_state_SET(timer_t *const timer, timer_event_t *const e) {
    switch (e->sig) {
        case ENTRY:
            displayTime(timer->current);
            return EVENT_HANDLED;
        case EXIT:
            displayClear();
            return EVENT_HANDLED;
        case TIME_DEC:
            timer->current -= 60;
            displayTime(timer->current);
            return EVENT_HANDLED;
        case TIME_INC:
            timer->current += 60;
            displayTime(timer->current);
            return EVENT_HANDLED;
        case ABORT:
            timer->state = S_IDLE;
            return EVENT_HANDLED;
        case START:
            timer->state = S_STAT;
            return EVENT_HANDLED;
    }
}

timer_event_status_t _handle_state_COUNT(timer_t *const timer, timer_event_t *const e) {
    switch (e->sig) {
        case ENTRY:
            displayTime(timer->current);
            return EVENT_HANDLED;
        case EXIT:
            timer->time += timer->current;
            timer->elapsed = 0;
            return EVENT_HANDLED;
        case START:
            timer->state = S_PAUSED;
            return EVENT_HANDLED;
        case TIME_TIC:
            if (((timer_event_tick_t *)e)->ss == 10) {
                timer->current -= 1;
                timer->elapsed += 1;
                displayTime(timer->current);
                if (timer->current == 0) {
                    timer->state = S_IDLE;
                    return EVENT_TRANS;
                }
                return EVENT_HANDLED;
            }
            return EVENT_IGNORED;
    }
}

timer_event_status_t _handle_state_PAUSE(timer_t *const timer, timer_event_t *const e) {
    switch (e->sig) {
        case ENTRY:
            displayClear();
            displayTime(timer->current);
            displayMessage(MSG_PAUSE, MSG_PAUSE_SZ);
            return EVENT_HANDLED;
        case EXIT:
            displayClear();
            return EVENT_HANDLED;
        case START:
            timer->state = S_COUNT;
            return EVENT_TRANS;
        case ABORT:
            timer->state = S_IDLE;
            return EVENT_TRANS;
        case TIME_INC:
            timer->state = S_SET;
            return EVENT_TRANS;
        case TIME_DEC:
            timer->state = S_SET;
            return EVENT_TRANS;
    }
}

timer_event_status_t _handle_state_STAT(timer_t *const timer, timer_event_t *const e) {
    switch (e->sig) {
        case ENTRY:
            displayClear();
            displayTime(timer->time);
            displayMessage(MSG_TOTAL, MSG_TOTAL_SZ);
            return EVENT_HANDLED;
        case EXIT:
            displayClear();
            return EVENT_HANDLED;
        case TIME_TIC:
            if (((timer_event_tick_t *)e)->ss == 10) {
                timer->state = S_IDLE;
                return EVENT_HANDLED;
            }
            return EVENT_IGNORED;
    }
}

static void _t_beep(void) {
}