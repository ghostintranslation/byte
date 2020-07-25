#ifndef Byte_h
#define Byte_h

#include <MIDI.h>
MIDI_CREATE_DEFAULT_INSTANCE(); // MIDI library init

#include "Voice.h"
#include "Display.h"
#include "Motherboard12.h"

/*
 * Sequencer
 */
class Byte{
  private:
    static Byte *instance;
    Byte();
    
    // Motherboard
    Motherboard12 *device;
    
    enum ClockMode { Leading = 0, Following = 1 };
    ClockMode clockMode = Leading;
    
    bool isPlaying = true;
    
    byte currentStep = 0;
    byte currentBar = 0;
    unsigned int timeBetweenSteps = 0;
    elapsedMillis stepClock = 0;
    byte tempo = 0;
    elapsedMillis bounceClock = 0;
    Display *display;

    byte selectedVoice = 0;
    byte selectedBar = 0;

    Voice voices[8] = {
      Voice(36),
      Voice(38),
      Voice(45),
      Voice(48),
      Voice(50),
      Voice(39),
      Voice(42),
      Voice(51)
    };

  public:
    static Byte *getInstance();
    void init();
    void update();
    byte getCurrentStep();
    void setTempo(byte tempo);
//    void sendNoteOn(byte note);
//    void sendNoteOff(byte note);
    void sendStop();
    
//    static void onTick();
    static void onSongPosition(unsigned beats);
    static void onStart();
    static void onStop();
    static void onClockShortPress();
    static void onClockLongPress();
    static void onClockChange(bool value);
    static void onVoiceChange(bool value);
    static void onBarChange(bool value);
    static void onPatternChange(bool value);
};

// Singleton pre init
Byte * Byte::instance = nullptr;

/**
 * Constructor
 */
inline Byte::Byte(){
  this->device = Motherboard12::getInstance();
  this->display = new Display();
  this->setTempo(60);
}

/**
 * Singleton instance
 */
inline Byte *Byte::getInstance()    {
  if (!instance)
     instance = new Byte;
  return instance;
}

/**
 * Init
 */
inline void Byte::init(){
  // 0 = empty, 1 = button, 2 = potentiometer, 3 = encoder
  byte controls[12] = {1,1,1,1, 1,1,1,1, 3,3,3,3};
  this->device->init(controls);

  // Midi callbacks
//  MIDI.setHandleClock(onTick);
  MIDI.setHandleSongPosition(onSongPosition);
  MIDI.setHandleStart(onStart);
  MIDI.setHandleStop(onStop);
  MIDI.begin(MIDI_CHANNEL_OMNI);

  // Device callbacks
  this->device->setHandlePress(8, onClockShortPress);
  this->device->setHandleLongPress(8, onClockLongPress);
  this->device->setHandleRotaryChange(8, onClockChange);
  this->device->setHandleRotaryChange(9, onVoiceChange);
  this->device->setHandleRotaryChange(10, onBarChange);
  this->device->setHandleRotaryChange(11, onPatternChange);

}

inline void Byte::update(){
  this->device->update();
  
  MIDI.read();
  usbMIDI.read();

  if(this->tempo > 0 && this->stepClock >= this->timeBetweenSteps){
    this->stepClock = 0;
    
      switch(this->display->getCurrentDisplayMode()){
        case DisplayMode::Pattern:
        
          this->display->setBinary(this->voices[this->selectedVoice].getPattern(this->selectedBar));
          this->display->setCursor(this->currentStep);
          this->display->setHideCursor(!this->isPlaying);
        break;
        default:
        break;
      }
      
    // Moving steps
    if (this->isPlaying) {
//      this->sendNoteOff(this->notes[this->currentStep]);
//      this->activeNotes[this->currentStep] = false;
      
      this->currentStep++;
      
      if(this->currentStep == 8){
        this->currentStep = 0;
      }

//      if(this->currentStep == 0 || this->currentStep == 4){
//        this->device->setDisplay(8, 4);  
//      }
      
//      switch(this->display->getCurrentDisplayMode()){
//        case DisplayMode::Pattern:
//          this->display->setCursor(this->currentStep);
//        break;
//        default:
//        break;
//      }
    }
    else{
      // If not playing then stop any note still active
      for(byte i=0; i<8; i++){
//        if(this->activeNotes[i]){
//          this->sendNoteOff(this->notes[i]);
//          this->activeNotes[i] = false;
//        }
      }
    }
  }

  this->display->update();
}

inline byte Byte::getCurrentStep(){
  return this->currentStep;
}

inline void Byte::setTempo(byte tempo){
  this->tempo = tempo;
  this->timeBetweenSteps = (float)1/tempo*60*1000/4;
}

//inline void Byte::sendNoteOn(byte note){
//  MIDI.sendNoteOn(note, 127, 1);
//  usbMIDI.sendNoteOn(note, 127, 1);
//}
//
//inline void Byte::sendNoteOff(byte note){
//  MIDI.sendNoteOff(note, 127, 1);
//  usbMIDI.sendNoteOff(note, 127, 1);
//}
//
inline void Byte::sendStop(){
  for(byte i=0; i<8; i++){

  }
}

//inline void Byte::onTick(){
//  Serial.println("Tick");
//}

inline void Byte::onSongPosition(unsigned songPosition){
  if(!getInstance()->isPlaying){
    getInstance()->currentStep = songPosition % 8;
    getInstance()->currentBar = songPosition / 8;

//    if(getInstance()->currentStep == 0 || getInstance()->currentStep == 4){
//      getInstance()->device->setDisplay(8, 4);  
//    }
  }
}

inline void Byte::onStart(){
  if(!getInstance()->isPlaying){
    Serial.println("onStart");
  }
}


inline void Byte::onStop(){
  if(!getInstance()->isPlaying){
    Serial.println("onStop");
  }
}

/**
 * On Clock short press 
 */
inline void Byte::onClockShortPress(){
  switch(getInstance()->display->getCurrentDisplayMode()){
    case DisplayMode::Pattern:
      // Play / Stop
      getInstance()->isPlaying = !getInstance()->isPlaying;
      getInstance()->currentStep = 0;
      getInstance()->bounceClock = 0;
      getInstance()->stepClock = 0;
      if(!getInstance()->isPlaying){
        getInstance()->sendStop();
      }
    break;
    
    case DisplayMode::Clock:
      // Exiting the Clock mode
      getInstance()->display->setCurrentDisplay(DisplayMode::Pattern);
    break;
    
    default:
    break;
  }
}

/**
 * On Clock long press 
 */
inline void Byte::onClockLongPress(){
  switch(getInstance()->display->getCurrentDisplayMode()){
    case DisplayMode::Pattern:
      // Entering in Clock mode
      getInstance()->display->setCursor(getInstance()->clockMode);
      getInstance()->display->setCurrentDisplay(DisplayMode::Clock);
    break;

    default:
    break;
  }
}

/**
 * On Clock change 
 */
inline void Byte::onClockChange(bool value){
  int inValue = 1;
  if(!value){
    inValue = -1;
  }
  
  switch(getInstance()->display->getCurrentDisplayMode()){
    case DisplayMode::Pattern:
      getInstance()->tempo += inValue;
      if(getInstance()->tempo > 0){
        getInstance()->setTempo(getInstance()->tempo);
      }else{
        getInstance()->setTempo(0);
      }
    break;
    
    case DisplayMode::Clock:
      getInstance()->clockMode = ClockMode( (getInstance()->clockMode + inValue) % 2 );
      getInstance()->display->setCursor(getInstance()->clockMode);
    break;

    default:
    break;
  }
}

/**
 * On Voice change 
 */
inline void Byte::onVoiceChange(bool value){
  switch(getInstance()->display->getCurrentDisplayMode()){
    case DisplayMode::Pattern:
      getInstance()->display->setCursor(getInstance()->selectedVoice);
      getInstance()->display->setCurrentDisplay(DisplayMode::VoiceDisplay);
    break;
    
    case DisplayMode::VoiceDisplay:
      if(value){
        getInstance()->selectedVoice += 1;
        getInstance()->selectedVoice = constrain(getInstance()->selectedVoice, 0, 7);
      }else{
        if(getInstance()->selectedVoice > 0){
          getInstance()->selectedVoice -= 1;
        }
      }
      getInstance()->display->keepCurrentDisplay();
      getInstance()->display->setCursor(getInstance()->selectedVoice);
    break;
    
    default:
    break;
  }
}


/**
 * On Bar change 
 */
inline void Byte::onBarChange(bool value){
  switch(getInstance()->display->getCurrentDisplayMode()){
    case DisplayMode::Pattern:
      getInstance()->display->setCursor(getInstance()->selectedBar);
      getInstance()->display->setCurrentDisplay(DisplayMode::Bar);
    break;
    
    case DisplayMode::Bar:
      if(value){
        getInstance()->selectedBar += 1;
        getInstance()->selectedBar = constrain(getInstance()->selectedBar, 0, 7);
      }else{
        if(getInstance()->selectedBar > 0){
          getInstance()->selectedBar -= 1;
        }
      }
      getInstance()->display->keepCurrentDisplay();
      getInstance()->display->setCursor(getInstance()->selectedBar);
    break;
    
    default:
    break;
  }
}

/**
 * On Pattern change 
 */
inline void Byte::onPatternChange(bool value){
  int patternInput = 1;
  if(!value){
    patternInput = -1;
  }
  
  switch(getInstance()->display->getCurrentDisplayMode()){
    case DisplayMode::Pattern:
      getInstance()->voices[getInstance()->selectedVoice].setPattern(
        getInstance()->selectedBar, 
        getInstance()->voices[getInstance()->selectedVoice].getPattern(getInstance()->selectedBar) + patternInput
      );
    break;
    
    default:
    break;
  }
}

#endif
