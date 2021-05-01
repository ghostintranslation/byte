#ifndef Display_h
#define Display_h

#include "Motherboard.h"

enum DisplayMode { Clock, VoiceDisplay, Bar, Sequencer, Mixer, Trigger };

/*
   Display
*/
class Display {
  private:
    Motherboard *device;
    
    DisplayMode currentDisplay;
    byte cursorIndex = 0;
    bool hideCursor = true;
    byte binary = 0;
    byte data[3];
    elapsedMillis clock_count;
    elapsedMillis clock_count_blink;
    elapsedMillis clock_count_display;
    const byte interval_time = 50;
    void displayBinary();
    void displayClock();
    void displayVoice();
    void displayBar();
    void displaySequener();
    void displayMixer();
    void displayTrigger();

  public:
    Display();
    void update();
    void setCurrentDisplay(DisplayMode displayMode);
    DisplayMode getCurrentDisplayMode();
    void setCursor(byte index);
    void setHideCursor(bool hideCursor);
    void setBinary(unsigned int binary);
    void setData(byte data[3]);
    void keepCurrentDisplay();
    void keepCurrentDisplayShort();
};


/*
 * Constructor
 */
inline Display::Display() {
  this->device = Motherboard::getInstance();
  this->currentDisplay = Sequencer;
}

/*
 * Update
 */
inline void Display::update() {
  switch (this->currentDisplay) {
    // Clock display
    case Clock:
      this->displayClock();
    break;
    
    // Voice display
    case VoiceDisplay:
      this->displayVoice();
      
      // Go back to Pattern display after 1000ms
      if (this->clock_count_display >= 2000) {
        this->currentDisplay = Sequencer;
      }
    break;
    
    // Bar display
    case Bar:
      this->displayBar();
        // Go back to Pattern display after 1000ms
      if (this->clock_count_display >= 2000) {
        this->currentDisplay = Sequencer;
      }
    break;
    
    // Sequencer display
    default:
    case Sequencer:
      this->displaySequener();
    break;

    // Mixer display
    case Mixer:
      this->displayMixer();
    break;
    
    // Trigger display
    case Trigger:
      this->displayTrigger();
    break;
  }


}

/**
 * Set the current display
 */
inline void Display::setCurrentDisplay(DisplayMode displayMode) {
  //  if(this->currentDisplay != displayMode){
  this->currentDisplay = displayMode;
  this->clock_count_display = 0;
  //  }
}

/**
 * Clock display
 */
inline void Display::displayClock() {
  this->device->resetAllLED();
  this->device->setLED(8, this->cursorIndex + 2);
}

/**
 * Voice display
 */
inline void Display::displayVoice() {
  for(byte i=0; i<8; i++){
    this->device->setLED(i, 0);
  }
  this->device->setLED(this->cursorIndex, 3);
}

/**
 * Bar diplay
 */
inline void Display::displayBar() {
  for(byte i=0; i<8; i++){
    this->device->setLED(i, 0);
  }
  this->device->setLED(this->cursorIndex, 3);
}

/**
 * Sequencer display
 */
inline void Display::displaySequener(){
  this->device->setAllLED(this->binary, 1);
  if(!this->hideCursor){
    this->device->setLED(this->cursorIndex, 3);
  }
}

/**
 * Mixer display
 */
inline void Display::displayMixer() {
  for(byte i=0; i<8; i++){

    // Set the LED solid
    this->device->setLED(i, 1);
    
    // Set the LED solid or off according to mute
    byte isMute = bitRead(this->data[0], i);
    this->device->setLED(i, !isMute);
    
    // Set the LED to blink according to active
    byte isActive = bitRead(this->data[1], i);
    if(isActive == 1){
      this->device->setLED(i, 3);
    }
    
  }
}

/**
 * Trigger display
 */
inline void Display::displayTrigger() {
  for(byte i=0; i<8; i++){

    // Set the LED off
    this->device->setLED(i, 0);

    // Set the LED to blink according to active
    byte isActive = bitRead(this->data[0], i);
    if(isActive == 1){
      this->device->setLED(i, 1);
    }
  }
  if(!this->hideCursor){
    this->device->setLED(this->cursorIndex, 1);
  }
}

/**
 * Get the current display
 */
inline DisplayMode Display::getCurrentDisplayMode(){
  return currentDisplay;
}

/**
 * Set the display's cursor
 */
inline void Display::setCursor(byte cursorIndex){
  this->cursorIndex = cursorIndex;
}

/**
 * Hide / Show the cursor
 */
inline void Display::setHideCursor(bool hideCursor){
  this->hideCursor = hideCursor;
}

/**
 * 
 */
inline void Display::setBinary(unsigned int binary){
  this->binary = binary;
}

/**
 * Set the display's datas
 */
inline void Display::setData(byte data[3]){
  for(byte i=0; i<3; i++){
    this->data[i] = data[i];
  }
}

/**
 * Keep current display active
 */
inline void Display::keepCurrentDisplay(){
  this->clock_count_display = 0;
}


/**
 * Keep current display active for 200ms
 */
inline void Display::keepCurrentDisplayShort(){
  this->clock_count_display = 1800;
}
#endif
