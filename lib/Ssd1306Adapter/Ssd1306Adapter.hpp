#ifndef LIB_SSD1306ADAPTER_HPP_INCLUDED
#define LIB_SSD1306ADAPTER_HPP_INCLUDED

#include <stdint.h>
#include <Adafruit_SSD1306.h>

#define TIME_FIELDS 3
#define TIME_FIELD_LEN 2

#define STR_LEN_MAX 10

#define SSD1306Adapter_RST -1
#define SSD1306Adapter_ADDR 0x3C
#define SSD1306Adapter_WIDTH 128
#define SSD1306Adapter_HEIGHT 32

class Ssd1306Adapter 
{
    Adafruit_SSD1306 _display;

public:
    Ssd1306Adapter() : _display(SSD1306Adapter_WIDTH, SSD1306Adapter_HEIGHT, &Wire, SSD1306Adapter_RST) {};    

    bool displayInit();
    void displayTime(uint32_t n);
    void displayMessage(const uint8_t *s, uint8_t l);
    void clear();
    void clearBottomRow();
    void clearTopRow();

private:
    void _write(const uint8_t *s, uint8_t l);
    void _writec(uint8_t c);
    void _intToCp437Arr(uint8_t n, uint8_t arr[TIME_FIELD_LEN]);
    void _timeToFields(uint32_t n, uint8_t arrs[TIME_FIELDS][TIME_FIELD_LEN]);
};

#endif