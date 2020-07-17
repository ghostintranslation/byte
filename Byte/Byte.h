#ifndef Byte_h
#define Byte_h

#include <MIDI.h>
MIDI_CREATE_DEFAULT_INSTANCE(); // MIDI library init

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
    
    bool play = true;
    byte currentStep = 0;
    unsigned int timeBetweenSteps = 0;
    elapsedMillis stepClock = 0;
    byte beatCount = 0;
    byte tempo = 0;
    elapsedMillis bounceClock = 0;
    Display *display;

    byte currentVoice = 0;
    byte currentBar = 0;

  public:
    static Byte *getInstance();
    void init();
    void update();
    byte getCurrentStep();
    void setTempo(byte tempo);
//    void sendNoteOn(byte note);
//    void sendNoteOff(byte note);
    void sendStop();
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
inline static Byte *Byte::getInstance()    {
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

  MIDI.begin(MIDI_CHANNEL_OMNI);
}

inline void Byte::update(){
  this->device->update();
  
  MIDI.read();
  usbMIDI.read();

  switch(this->display->getCurrentDisplayMode()){
    default:
    case DisplayMode::Steps:
    { // This bracket is to prevent issues with variables declared inside the case
      this->display->setCursorIndex(this->currentStep);

      // Voice input
      int voice = this->device->getInput(9);
      if(voice != 0){
        Serial.println(voice);
        this->display->setCursorIndex(this->currentVoice);
        this->display->setCurrentDisplay(DisplayMode::Voice);
      }
  
      // Bar input
      int bar = this->device->getInput(10);
      if(bar != 0){
        this->display->setCursorIndex(this->currentBar);
        this->display->setCurrentDisplay(DisplayMode::Bar);
      }

    }
    break;
    case DisplayMode::Voice:
    {
      // Voice input
      int voice = this->device->getInput(9);
      if(voice != 0){
        if(voice>0){
          this->currentVoice += 1;
          this->currentVoice = constrain(this->currentVoice, 0, 7);
        }else{
          if(this->currentVoice > 0){
            this->currentVoice -= 1;
          }
        }
        this->display->keepCurrentDisplay();
        this->display->setCursorIndex(this->currentVoice);
      }
    }
    break;
    case DisplayMode::Bar:
    {
      // Bar input
      int bar = this->device->getInput(10);
      if(bar != 0){
        if(bar>0){
          this->currentBar += 1;
          this->currentBar = constrain(this->currentBar, 0, 7);
        }else{
          if(this->currentBar > 0){
            this->currentBar -= 1;
          }
        }
        this->display->keepCurrentDisplay();
        this->display->setCursorIndex(this->currentBar);
      }
    }
    break;
  }

  // Tempo
  int tempoInput = device->getInput(8);
  if(tempoInput != 0){
    this->tempo += tempoInput;
    if(this->tempo > 0){
      this->setTempo(this->tempo);
    }else{
      this->setTempo(0);
    }
  }
  if(this->device->getEncoderSwitch(8)){
    // Play / Stop
    if(this->bounceClock > 500){
      this->play = !this->play;
      this->currentStep = 0;
      this->bounceClock = 0;
      this->stepClock = 0;
      this->beatCount = 0;
      if(!this->play){
        this->sendStop();
      }
    }
  }

  if(this->tempo > 0 && this->stepClock >= this->timeBetweenSteps){
    this->stepClock = 0;

    this->beatCount++;
    this->beatCount = this->beatCount % 4;

    // Display the tempo
    if(this->beatCount == 0){
      this->device->setDisplay(8, 4);  
    }

    // Moving steps
    if (this->play) {
//      this->sendNoteOff(this->notes[this->currentStep]);
//      this->activeNotes[this->currentStep] = false;
  
      this->currentStep++;
      
      if(this->currentStep == 8){
        this->currentStep = 0;
      }
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
#endif
