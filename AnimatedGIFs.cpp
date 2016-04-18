/*
 * Animated GIFs Display Code for SmartMatrix and 32x32 RGB LED Panels
 *
 * Uses SmartMatrix Library for Teensy 3.1 written by Louis Beaudoin at pixelmatix.com
 *
 * Written by: Craig A. Lindley
 *
 * Copyright (c) 2014 Craig A. Lindley
 * Refactoring by Louis Beaudoin (Pixelmatix)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

 /*
  * This example displays 32x32 GIF animations loaded from a SD Card connected to the Photon
  * The GIFs can be up to 32 pixels in width and height.
  * This code has been tested with 32x32 pixel and 16x16 pixel GIFs, but is optimized for 32x32 pixel GIFs.
  *
  * Wiring is on the default Photon SPI pins, and chip select uses the WKP pin,

  *
  * This code first looks for .gif files in the /gifs/ directory
  * (customize below with the GIF_DIRECTORY definition) then plays random GIFs in the directory,
  * looping each GIF for DISPLAY_TIME_SECONDS
  *
  * This example is meant to give you an idea of how to add GIF playback to your own sketch.
  */


#include "RGBmatrixPanel.h" // Hardware-specific library
#include "GIFDecoder.h"
#include "math.h"
#include "SdFat.h"

SYSTEM_THREAD(ENABLED);
SYSTEM_MODE(MANUAL);

/** Define RGB matrix panel GPIO pins **/
#define CLK D6
#define OE  D7
#define LAT TX
#define A   A0
#define B   A1
#define C   A2
#define D	RX
/****************************************/


/** Define SD configuration and CS pins**/
#define SPI_CONFIGURATION 0

SdFat sd;
const uint8_t chipSelect = WKP;
/****************************************/

/********** RGB565 Color definitions **********/
#define Black           0x0000
#define Navy            0x000F
#define DarkGreen       0x03E0
#define DarkCyan        0x03EF
#define Maroon          0x7800
#define Purple          0x780F
#define Olive           0x7BE0
#define LightGrey       0xC618
#define DarkGrey        0x7BEF
#define Blue            0x001F
#define Green           0x07E0
#define Cyan            0x07FF
#define Red             0xF800
#define Magenta         0xF81F
#define Yellow          0xFFE0
#define White           0xFFFF
#define Orange          0xFD20
#define GreenYellow     0xAFE5
#define Pink			0xF81F
/**********************************************/


/***** Create RGBmatrix Panel instance *****
 Next to last parameter = 'true' enables double-buffering, for flicker-free,
 buttery smooth animation.  Note that NOTHING WILL SHOW ON THE DISPLAY
 until the first call to swapBuffers().  This is normal.  Last parameter
 defines a 32 high panel (by 32 wide) */
RGBmatrixPanel matrix(A, B, C, D,CLK, LAT, OE, true, 32);	// 32x32
/*******************************************/

#define DISPLAY_TIME_SECONDS 10

#define GIF_DIRECTORY "/gifs/"

int num_files;

void screenClearCallback(void) {
  matrix.fillScreen(Black);
}

void updateScreenCallback(void) {
  matrix.swapBuffers(false);
}

void drawPixelCallback(int16_t x, int16_t y, uint8_t red, uint8_t green, uint8_t blue) {
  matrix.drawPixel(x, y, matrix.Color888(red, green, blue, true));
}

void scrollText(const char *m){
	matrix.setTextSize(1);
	int l = (strlen(m)*-6) - 32;
	for(int i = 32; i > l; i--){
		matrix.fillScreen(Black);
		matrix.setCursor(i,1);
		matrix.setTextColor(matrix.Color444(1,1,1));
		matrix.print(m);
		matrix.swapBuffers(false);
		delay(50);
	}
}

// Setup method runs once, when the sketch starts
void setup() {

	Serial.begin(9600);

    setScreenClearCallback(screenClearCallback);
    setUpdateScreenCallback(updateScreenCallback);
    setDrawPixelCallback(drawPixelCallback);

    // Initialize matrix
    matrix.begin();

    // Clear screen
    matrix.fillScreen(Blue);
    matrix.swapBuffers(false);
	
	matrix.setTextWrap(false); // Allow text to run off right edge
	scrollText("Ready to go!");

    // initialize the SD card at full speed
    if (!sd.begin(chipSelect, SPI_FULL_SPEED)) {
        //Serial.println("No SD card");
        scrollText("No SD card");
        while(1) ;//scrollText("No SD card");
    }

    // Determine how many animated GIF files exist
    num_files = enumerateGIFFiles(GIF_DIRECTORY, true);

    if(num_files < 0) {
        //Serial.println("No gifs directory");
        scrollText("No gifs directory");
        while(1);
    }

    if(!num_files) {
        //Serial.println("Empty gifs directory");
        scrollText("Empty gifs directory");
        while(1);
    }
}


void loop() {

    unsigned long futureTime;
    char pathname[30];

    int index = random(num_files);

    // Do forever
    while (true) {
        // Can clear screen for new animation here, but this might cause flicker with short animations
        // matrix.fillScreen(Black);
        // matrix.swapBuffers(false);

        getGIFFilenameByIndex(GIF_DIRECTORY, index++, pathname);
        if (index >= num_files) {
            index = 0;
        }

        // Calculate time in the future to terminate animation
        futureTime = millis() + (DISPLAY_TIME_SECONDS * 1000);

        while (futureTime > millis()) {
            processGIFFile(pathname);
        }
    }
}
