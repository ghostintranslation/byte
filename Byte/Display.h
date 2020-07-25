#ifndef Display_h
#define Display_h

#include "Motherboard12.h"

enum DisplayMode { Clock, VoiceDisplay, Bar, Pattern };

/*
   Display
*/
class Display {
  private:
    Motherboard12 *device;
    
    DisplayMode currentDisplay;
    byte cursorIndex = 0;
    bool hideCursor = true;
    byte binary = 0;
    elapsedMillis clock_count;
    elapsedMillis clock_count_blink;
    elapsedMillis clock_count_display;
    const byte interval_time = 50;
    void displayBinary();
    void displayClock();
    void displayVoice();
    void displayBar();
    void displayPattern();

  public:
    Display();
    void update();
    void setCurrentDisplay(DisplayMode displayMode);
    DisplayMode getCurrentDisplayMode();
    void setCursor(byte index);
    void setHideCursor(bool hideCursor);
    void setBinary(unsigned int binary);
    void keepCurrentDisplay();
};


/**
   Constructor
*/
inline Display::Display() {
  this->device = Motherboard12::getInstance();
  this->currentDisplay = Pattern;
}

inline void Display::update() {
  switch (this->currentDisplay) {
    case Clock:
      this->displayClock();
    break;
    
    case VoiceDisplay:
      this->displayVoice();
      
      // Go back to Pattern display after 1000ms
      if (this->clock_count_display >= 1500) {
        this->currentDisplay = Pattern;
      }
    break;
    case Bar:
      this->displayBar();
        // Go back to Pattern display after 1000ms
      if (this->clock_count_display >= 1500) {
        this->currentDisplay = Pattern;
      }
    break;
    default:
    case Pattern:
      this->displayPattern();
    break;
  }


}


inline void Display::setCurrentDisplay(DisplayMode displayMode) {
  //  if(this->currentDisplay != displayMode){
  this->currentDisplay = displayMode;
  this->clock_count_display = 0;
  //  }
}

inline void Display::displayClock() {
  this->device->resetAllLED();
  this->device->setLED(8, this->cursorIndex + 2);
}

inline void Display::displayVoice() {
  for(byte i=0; i<8; i++){
    this->device->setLED(i, 0);
  }
  this->device->setLED(this->cursorIndex, 1);
}

inline void Display::displayBar() {
  for(byte i=0; i<8; i++){
    this->device->setLED(i, 0);
  }
  this->device->setLED(this->cursorIndex, 1);
}

inline void Display::displayPattern(){
  this->device->setAllLED(this->binary, 1);
  if(!this->hideCursor){
    this->device->setLED(this->cursorIndex, 3);
  }
}

inline DisplayMode Display::getCurrentDisplayMode(){
  return currentDisplay;
}

inline void Display::setCursor(byte cursorIndex){
  this->cursorIndex = cursorIndex;
}

inline void Display::setHideCursor(bool hideCursor){
  this->hideCursor = hideCursor;
}

inline void Display::setBinary(unsigned int binary){
  this->binary = binary;

}

inline void Display::keepCurrentDisplay(){
  this->clock_count_display = 0;
}
#endif
