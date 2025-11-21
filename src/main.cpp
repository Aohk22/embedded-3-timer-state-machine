#include <main.hpp>
#include <Arduino.h>
#include <TimerStateMachine.hpp>
#include <Ssd1306Adapter.hpp>

const uint16_t debounce = 500;
Ssd1306Adapter display;
timer_t timer;
timer_event_tick_t tickEvent = {{(timer_signals_t)0}, 0};
timer_event_user_t userEvent = {(timer_signals_t)0};
uint32_t timePrev = millis();
uint32_t btnTime = millis();
uint8_t btnState = 0;

void setup() {
  Serial.begin(9600);
  Serial.println("Serial init done.");

  if (!display.displayInit()) {
    Serial.println("Cannot init display");
    while (true) {}
  }
  Serial.println("Display init done.");

  if (!timerInit(&timer)) {
    Serial.println("Cannot init timer.");
    while (true) {}
  }
  Serial.println("Timer init done.");

  pinMode(P_BUTT_INC, INPUT_PULLUP);
  pinMode(P_BUTT_DEC, INPUT_PULLUP);
  pinMode(P_BUTT_STRT, INPUT_PULLUP);
  pinMode(P_BUTT_ABRT, INPUT_PULLUP);
  pinMode(P_LED, OUTPUT);
  Serial.println("Pin config done.");
}

void loop() {
  /* 
  bArrValue bit mapping:
  | (unused) | (unused) | (unused) | (unused) | ABRT | STRT | DEC | INC |
  */
  uint8_t bArrValue;
  uint8_t bInc, bDec, bStrt, bAbrt;
  // 1. Read button.
  bInc = ~digitalRead(P_BUTT_INC) & 1;
  bDec = ~digitalRead(P_BUTT_DEC) & 1;
  bStrt = ~digitalRead(P_BUTT_STRT) & 1;
  bAbrt = ~digitalRead(P_BUTT_ABRT) & 1;
  bArrValue = (bInc) | (bDec << 1) | (bStrt << 2) | (bAbrt << 3);
  bArrValue = debounceBtnArr(bArrValue);
  if (bArrValue) {
  // 2. Create event.
    if (bArrValue == B_INC) {
      userEvent.super.sig = TIME_INC;
    }
    else if (bArrValue == B_DEC) {
      userEvent.super.sig = TIME_DEC;
    }
    else if (bArrValue == B_STRT) {
      userEvent.super.sig = START;
    }
    else if (bArrValue == B_ABRT) {
      userEvent.super.sig = ABORT;
    }
  // 3. Send event to dispatcher.
    timerEvtDispatch(&timer, &userEvent.super);
  }
  // 4. Dispatch time tick event every x ms.
  // If 100 ms -> 10 tick = 1s.
  if (millis() - timePrev > 100) {
    timePrev = millis();
    tickEvent.super.sig = TIME_TIC;
    tickEvent.ss += 1;
    timerEvtDispatch(&timer, &tickEvent.super);
    if (tickEvent.ss > 10) tickEvent.ss = 0;
    // if (tickEvent.ss == 10) {
    //   Serial.println("Timer elapsed: " + String(timer.elapsed));
    //   Serial.println("Timer current: " + String(timer.current));
    //   Serial.println("Timer total: " + String(timer.time));
    // }
  }
}

uint8_t debounceBtnArr(uint8_t bVal) {
  if (bVal && btnState == 1) {
    if (millis() - btnTime > debounce) {
      btnTime = millis();
      return bVal;
    }
  }
  else if (bVal && btnState == 0) {
    btnState = 1;
    btnTime = millis();
  }
  else {
    btnState = 0;
  }
  return 0;
}

void timerEvtDispatch(timer_t *t, timer_event_t *e) {
  timer_event_status_t status;
  timer_state_t state_prev;
  timer_state_t state_curr;
  state_prev = t->state;
  status = timerStateMachine(t, e);
  // Serial.println("------ State machine Cycle ------");
  // Serial.println("Evt Signal: " + String(e->sig));
  // Serial.println("Timer State previous: " + String(state_prev));
  // Serial.println("Timer State now: " + String(t->state));
  // Serial.println("Event status: " + String(status));
  // Serial.println("timer->current = " + String(t->current));
  state_curr = t->state;
  if (status == EVENT_TRANS) {
    timer_event_t event_trigger;

    tickEvent.ss = 0; // Reset the tick when transitioning to new state else BUGS.

    event_trigger.sig = EXIT;
    t->state = state_prev;
    timerStateMachine(t, &event_trigger);
    // Serial.println("Triggered EXIT event of state " + String(t->state));

    event_trigger.sig = ENTRY;
    t->state = state_curr;
    timerStateMachine(t, &event_trigger);
    // Serial.println("Triggered ENTRY event of state " + String(t->state));
  }
}
