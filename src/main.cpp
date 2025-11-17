#include <stdint.h>
#include <Arduino.h>
#include <Wire.h>
#include <Display.hpp>
#include <timerStateMachine.hpp>

#define PIN_BUTTON = 

const uint8_t *msg = "Here";
const uint8_t msgLen = 4;
static timer_t timer;

void setup() {
  Serial.begin(9600);
  if (!displayInit()) {
    while(1) {
      Serial.println("Cannot initialize display.");
      delay(2000);
    }
  }
  timer_init(&timer);
}

void loop() {
  /*
  To dos:
    1. Read button status.
    2. Create event.
    3. send to event dispatcher.
  */
  // 1. Read button.

  timer_evt_dispatch();
}

static void timer_evt_dispatch(timer_t *t, timer_event_t *e) {
  timer_event_status_t status;
  timer_state_t state_prev;
  timer_state_t state_curr;
  state_prev = t->state;
  status = timer_state_machine(t, e);
  state_curr = t->state;
  if (status == EVENT_TRANS) {
    timer_event_t event_trigger;
    event_trigger.sig = EXIT;
    t->state = state_prev;
    timer_state_machine(t, &event_trigger);

    event_trigger.sig = ENTRY;
    t->state = state_curr;
    timer_state_machine(t, &event_trigger);
  }
}