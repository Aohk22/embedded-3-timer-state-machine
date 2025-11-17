#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include <Cp437.hpp>
#include <display.hpp>

#define STR_LEN_MAX 10

#define DISP_RST -1
#define DISP_ADDR 0x3C
#define DISP_WIDTH 128
#define DISP_HEIGHT 32

static Adafruit_SSD1306 *ssd1306Display;

static void _intToCp437(uint8_t n, uint8_t arr[TIME_FIELD_LEN]);
static void _timeToFields(uint32_t n, uint8_t arrs[TIME_FIELDS][TIME_FIELD_LEN]);
static void _dWrite(const uint8_t *s, uint8_t l);
static void _dWritec(uint8_t code);
static void _dWriteln(const uint8_t *s, uint8_t l);
static void _dClear();

bool displayInit() {
  static Adafruit_SSD1306 display(DISP_WIDTH, DISP_HEIGHT, &Wire, DISP_RST);
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally.
  if (!display.begin(SSD1306_SWITCHCAPVCC, DISP_ADDR)) 
    return false;
  display.setTextSize(2);              // x2 pixel scale.
  display.setTextColor(SSD1306_WHITE); // Draw white text.
  display.setCursor(0, 0);             // Start at top-left corner.
  display.cp437(true);                 // Use full 256 char 'Code Page 437' font.
  display.clearDisplay();
  ssd1306Display = &display;
  return true;
}

void displayTime(uint32_t n) {
  /* n is in seconds. */
  uint8_t time_fields[TIME_FIELDS][TIME_FIELD_LEN] = {0};

  _timeToFields(n, time_fields);

  _dWritec(CODE_CP437_SPC);
  _dWrite(time_fields[0], TIME_FIELD_LEN);
  _dWritec(CODE_CP437_COLON);
  _dWrite(time_fields[1], TIME_FIELD_LEN);
  _dWritec(CODE_CP437_COLON);
  _dWrite(time_fields[2], TIME_FIELD_LEN);
  _dWritec(CODE_CP437_SPC);

  ssd1306Display->display();
}

void displayMessage(const uint8_t *s, uint8_t l) {
  /* Expects a character array. */
  uint8_t x = ((STR_LEN_MAX/2) - (l/2)) * 12; // 1 character about 12 pixels for font x2.
  ssd1306Display->setCursor(x, 16);
  // ssd1306Display->setCursor(0, 2);

  _dWrite(s, l);

  ssd1306Display->display();
}

void displayClear() {
  _dClear();
}

static void _dWrite(const uint8_t *s, uint8_t l) {
  for (int i = 0; i < l; i++) {
    ssd1306Display->write(*(s + i));
  }
}

static void _dWritec(uint8_t code) {
  const uint8_t arr[1] = {code};
  _dWrite((const uint8_t *)arr, 1);
}

static void _dWriteln(const uint8_t *s, uint8_t l) {
  const uint8_t padLen = STR_LEN_MAX - l;
  const uint8_t pad[padLen] = {CODE_CP437_SPC};
  _dWrite(s, l);
  _dWrite((uint8_t *)pad, padLen);
}

void _dClear() {
  ssd1306Display->clearDisplay();
}

void _intToCp437(uint8_t n, uint8_t arr[TIME_FIELD_LEN]) {
  uint8_t temp[TIME_FIELD_LEN] = {0};

  // Extract digits.
  for (int i = 0; i < TIME_FIELD_LEN; i++) {
    uint8_t r = n % 10;
    temp[i] = r;
    n = n / 10;
  }

  // Digits to printing code.
  for (int i = TIME_FIELD_LEN - 1; i >= 0; i--) {
    arr[(TIME_FIELD_LEN - 1) - i] = CODE_CP437_0 + temp[i];
  }
}

void _timeToFields(uint32_t n, uint8_t arrs[TIME_FIELDS][TIME_FIELD_LEN]) {
  /*
  Each field is an array of printable CP437 code values.
  Example: field[3][2] = [[48, 48], [58, 49], [48, 48]].
  The time format is like: "hh:mm:ss".
  */
  uint8_t fieldIntVal[3] = {0};
  uint8_t remainder;

  // Extract hours, minutes, seconds.
  if (n > 3600) {
    remainder = n % 3600;
    fieldIntVal[0] = n / 3600;
    n = remainder;
  }
  if (n > 60) {
    remainder = n % 60;
    fieldIntVal[1] = n / 60;
    n = remainder;
  }
  fieldIntVal[2] = n;

  // Turn those int values to printable
  for (int i = 0; i < 3; i++) {
    _intToCp437(fieldIntVal[i], arrs[i]);
  }
}
