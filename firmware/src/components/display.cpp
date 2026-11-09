#include <Arduino.h>
#include <Adafruit_ST7735.h>

#include "display.h"
#include "config.h"
#include "pins.h"
#include "self_test.h"

constexpr uint8_t DISPLAY_TEXT_SIZE = 2;

class DisplayTft : public Adafruit_ST7735 {
public:
    using Adafruit_ST7735::Adafruit_ST7735;
    using Adafruit_ST77xx::setColRowStart;
};

static DisplayTft s_tft(DISPLAY_CS_PIN, DISPLAY_DC_PIN, DISPLAY_RES_PIN);

bool display_setup() {
    s_tft.initR(DISPLAY_INIT_VARIANT);
    s_tft.setSPISpeed(DISPLAY_SPI_FREQUENCY_HZ);
    s_tft.setColRowStart(DISPLAY_COL_OFFSET, DISPLAY_ROW_OFFSET);
    s_tft.setRotation(DISPLAY_ROTATION);
    return true;
}

void display_show_measures(const Bme680Data &data, uint16_t mq2_raw) {
    s_tft.fillScreen(ST77XX_BLACK);
    s_tft.setCursor(0, 0);
    s_tft.setTextSize(DISPLAY_TEXT_SIZE);
    s_tft.setTextColor(ST77XX_WHITE);
    s_tft.print(data.temperature_c, 1);
    s_tft.println(" C");
    s_tft.print(data.humidity_pct, 1);
    s_tft.println(" %");
    s_tft.print(data.pressure_hpa, 1);
    s_tft.println(" hPa");
    s_tft.print(data.gas_kohm, 1);
    s_tft.println(" kOhm");
    s_tft.print("MQ2 ");
    s_tft.println(mq2_raw);
}

bool display_test() {
    unsigned long start_ms = millis();
    s_tft.fillScreen(ST77XX_BLACK);
    s_tft.setCursor(0, 0);
    s_tft.setTextSize(DISPLAY_TEXT_SIZE);
    s_tft.setTextColor(ST77XX_WHITE);
    s_tft.println("Cypher Home");
    s_tft.println("Ecran OK");
    s_tft.setTextColor(ST77XX_RED);
    s_tft.println("Rouge");
    s_tft.setTextColor(ST77XX_GREEN);
    s_tft.println("Vert");
    s_tft.setTextColor(ST77XX_BLUE);
    s_tft.println("Bleu");
    unsigned long draw_ms = millis() - start_ms;

    char resolution[16];
    snprintf(resolution, sizeof(resolution), "%dx%d", s_tft.width(), s_tft.height());
    self_test_print("display", "resolution", resolution);
    self_test_print("display", "affichage", draw_ms, "ms");
    return true;
}
