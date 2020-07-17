#ifndef Display_h
#define Display_h

#include "Motherboard12.h"

enum DisplayMode { Steps, Voice, Bar, Pattern };

/*
   Display
*/
class Display {
  private:
    Motherboard12 *device;
    
    DisplayMode currentDisplay;
    byte cursorIndex = 0;
    elapsedMillis clock_count;
    elapsedMillis clock_count_blink;
    elapsedMillis clock_count_display;
    const byte interval_time = 50;
    void displaySteps();
    void displayVoice();
    void displayBar();
    void displayPattern();

  public:
    Display();
    void update();
    void setCurrentDisplay(DisplayMode displayMode);
    DisplayMode getCurrentDisplayMode();
    void setCursorIndex(byte index);
    void keepCurrentDisplay();
};


/**
   Constructor
*/
inline Display::Display() {
  this->device = Motherboard12::getInstance();
  this->currentDisplay = Steps;
}

inline void Display::update() {
  switch (this->currentDisplay) {
    case Voice:
      this->displayVoice();
      
      // Go back to Pattern display after 1000ms
      if (this->clock_count_display >= 1500) {
        this->currentDisplay = Steps;
      }
    break;
    case Bar:
      this->displayBar();
        // Go back to Pattern display after 1000ms
      if (this->clock_count_display >= 1500) {
        this->currentDisplay = Steps;
      }
    break;
    case Pattern:
      this->displayPattern();
        // Go back to Pattern display after 1000ms
      if (this->clock_count_display >= 1500) {
        this->currentDisplay = Steps;
      }
    break;
    case Steps:
    default:
      this->displaySteps();
    break;
  }


}


inline void Display::setCurrentDisplay(DisplayMode displayMode) {
  //  if(this->currentDisplay != displayMode){
  this->currentDisplay = displayMode;
  this->clock_count_display = 0;
  //  }
}

inline void Display::displayVoice() {
  this->device->resetDisplay();

  for(byte i=0; i<8; i++){
    this->device->setDisplay(i, 0);
  }
  this->device->setDisplay(this->cursorIndex, 3);
}

inline void Display::displayBar() {
  this->device->resetDisplay();

  for(byte i=0; i<8; i++){
    this->device->setDisplay(i, 0);
  }
  this->device->setDisplay(this->cursorIndex, 3);
}

inline void Display::displayPattern() {
  this->device->resetDisplay();

}

inline void Display::displaySteps(){
  for(byte i=0; i<8; i++){
    this->device->setDisplay(i, 0);
  }
  this->device->setDisplay(cursorIndex, 5);
}

inline DisplayMode Display::getCurrentDisplayMode(){
  return currentDisplay;
}

inline void Display::setCursorIndex(byte cursorIndex){
  this->cursorIndex = cursorIndex;
}

inline void Display::keepCurrentDisplay(){
  this->clock_count_display = 0;
}
#endif
