Maker Fair RGBMatrix Shield Test App
====================================

By Paul Kourany, Apr 18/2016

This app uses the RGBMatrixPanel and SDFat libraries to test the new (V4)
RGBMatrix Shield designed for the 2016 San Francisco Maker Fair (peekay123 @2016).

The test app will read 32x32 animated GIF files from "gifs" directory and display
them on the attached 32x32 RGB Matrix panel.  This tests both the microSD socket and
the display panel.

A FAT32 formatted microSD flash is required.  The "gifs" directory must be moved
from the code directory to the root of the microSD and the microSD inserted in the
onboard socket.

The code was locally compiled with gcc 4.9.3 with the latest develop branch.

