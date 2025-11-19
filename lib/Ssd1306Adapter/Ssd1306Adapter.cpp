#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include <Cp437.hpp>
#include <Ssd1306Adapter.hpp>

bool Ssd1306Adapter::displayInit() {
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally.
  if (!_display.begin(SSD1306_SWITCHCAPVCC, SSD1306Adapter_ADDR)) 
    return false;
  _display.setTextSize(2);              // x2 pixel scale.
  _display.setTextColor(SSD1306_WHITE); // Draw white text.
  _display.setCursor(0, 0);             // Start at top-left corner.
  _display.cp437(true);                 // Use full 256 char 'Code Page 437' font.
  _display.clearDisplay();
  return true;
}

void Ssd1306Adapter::displayTime(uint32_t n) {
  /* n is in seconds. */
  // Converts seconds into hours, minutes, seconds.
  // Then convert those into printable CP437 code.
  uint8_t time_fields[TIME_FIELDS][TIME_FIELD_LEN] = {0};

  _timeToFields(n, time_fields);

  // Clears top row.
  clearTopRow();

  // Now write.
  _display.setCursor(0, 0);
  _writec(CODE_CP437_SPC);
  _write(time_fields[0], TIME_FIELD_LEN);
  _writec(CODE_CP437_COLON);
  _write(time_fields[1], TIME_FIELD_LEN);
  _writec(CODE_CP437_COLON);
  _write(time_fields[2], TIME_FIELD_LEN);
  _writec(CODE_CP437_SPC);

  _display.setCursor(0, 0);

  _display.display();
}

void Ssd1306Adapter::displayMessage(const uint8_t *s, uint8_t l) {
  /* Expects a character array. */
  uint8_t x = ((STR_LEN_MAX/2) - (l/2)) * 12; // 1 character about 12 pixels for font x2.

  // Clear bottom row.
  clearBottomRow();

  _display.setCursor(x, 16);
  _write(s, l);

  _display.setCursor(0, 0);

  _display.display();
}

void Ssd1306Adapter::clear() {
  _display.clearDisplay();
  _display.display();
}

void Ssd1306Adapter::clearTopRow() {
  // Number of bytes to write = (128 (width) / 8) (n bytes to write per row) x 16 (height/ 2) = 256
  memset(_display.getBuffer(), 0, 256); 
}

void Ssd1306Adapter::clearBottomRow() {
  uint8_t *pBuffer;
  pBuffer = _display.getBuffer();
  pBuffer += 256;
  memset(pBuffer, 0, 256);
}

void Ssd1306Adapter::_write(const uint8_t *s, uint8_t l) {
  // Expecting char array.
  for (int i = 0; i < l; i++) {
    _display.write(*(s + i));
  }
}

void Ssd1306Adapter::_writec(uint8_t c) {
  _display.write(c);
}

void Ssd1306Adapter::_intToCp437Arr(uint8_t n, uint8_t arr[TIME_FIELD_LEN]) {
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

void Ssd1306Adapter::_timeToFields(uint32_t n, uint8_t arrs[TIME_FIELDS][TIME_FIELD_LEN]) {
  /*
  Each field is an array of printable CP437 code values.
  Example: field[3][2] = [[48, 48], [58, 49], [48, 48]].
  The time format is like: "hh:mm:ss".
  */
  uint8_t fieldIntVal[3] = {0};
  uint8_t remainder;

  // Extract hours, minutes, seconds.
  if (n >= 3600) {
    remainder = n % 3600;
    fieldIntVal[0] = n / 3600;
    n = remainder;
  }
  if (n >= 60) {
    remainder = n % 60;
    fieldIntVal[1] = n / 60;
    n = remainder;
  }
  fieldIntVal[2] = n;

  // Turn those int values to printable
  for (int i = 0; i < 3; i++) {
    _intToCp437Arr(fieldIntVal[i], arrs[i]);
  }
}