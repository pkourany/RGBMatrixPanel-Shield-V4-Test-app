/*
 * Animated GIFs Display Code for SmartMatrix and 32x32 RGB LED Panels
 *
 * This file contains code to enumerate and select animated GIF files by name
 *
 * Written by: Craig A. Lindley
 */

#include "SdFat.h"

extern SdFat sd;
extern File file;

int numberOfFiles;

bool isAnimationFile(const char filename[]) {
	String fname = String(filename);
	fname.toUpperCase();
	
    if (filename[0] == '_')
        return false;

    if (filename[0] == '~')
        return false;

    if (filename[0] == '.')
        return false;

    //String filenameString = String(filename).toUpperCase();
    //String filenameString = fname;
    //if (filenameString.endsWith(".GIF") != 1)
    if (fname.endsWith(".GIF") != 1)
        return false;

    return true;
}

// Enumerate and possibly display the animated GIF filenames in GIFS directory
int enumerateGIFFiles(const char *directoryName, boolean displayFilenames) {

	char fname[30];
	numberOfFiles = 0;

    File directory = sd.open(directoryName);
    if (!directory) {
        return -1;
    }

    File file = directory.openNextFile();
    while (file) {
		file.getName(fname, sizeof(fname));
  		Serial.println(fname);
        if (isAnimationFile(fname)) {
            numberOfFiles++;
            if (displayFilenames) {
                Serial.println(fname);
            }
        }
        file.close();
        file = directory.openNextFile();
    }

    file.close();
    directory.close();

    return numberOfFiles;
}

// Get the full path/filename of the GIF file with specified index
void getGIFFilenameByIndex(const char *directoryName, int index, char *pnBuffer) {

	char fname[30];

    // Make sure index is in range
    if ((index < 0) || (index >= numberOfFiles))
        return;

    File directory = sd.open(directoryName);
    if (!directory)
        return;

    File file = directory.openNextFile();
    while (file && (index >= 0)) {
        file.getName(fname, sizeof(fname));

        if (isAnimationFile(fname)) {
            index--;

            // Copy the directory name into the pathname buffer
            strcpy(pnBuffer, directoryName);

            // Append the filename to the pathname
            strcat(pnBuffer, fname);
        }

        file.close();
        file = directory.openNextFile();
    }

    file.close();
    directory.close();
}

// Return a random animated gif path/filename from the specified directory
void chooseRandomGIFFilename(const char *directoryName, char *pnBuffer) {

    int index = random(numberOfFiles);
    getGIFFilenameByIndex(directoryName, index, pnBuffer);
}
