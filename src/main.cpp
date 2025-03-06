#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

#define PIN D4
#define NUMPIXELS 8
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);




void setup() {
    pixels.begin();
}

void loop() {
    pixels.clear();

    for (int i = 0; i < NUMPIXELS; i++)
    { // For each pixel...

        // pixels.Color() takes RGB values, from 0,0,0 up to 255,255,255
        // Here we're using a moderately bright green color:
        pixels.setPixelColor(i, pixels.Color(0, 150, 0));

        pixels.show(); // Send the updated pixel colors to the hardware.    
    }
    delay(1000); // Pause before next pass through loop
}

