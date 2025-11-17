#define TIME_FIELDS 3
#define TIME_FIELD_LEN 2

// Remove name mangling for these.
#ifdef __cplusplus
extern "C" {
#endif

bool displayInit();
void displayTime(uint32_t n);
void displayMessage(const uint8_t *s, uint8_t l);
void displayClear();

#ifdef __cplusplus
}
#endif