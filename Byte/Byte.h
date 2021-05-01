#ifndef Byte_h
#define Byte_h

#include "Voice.h"
#include "Display.h"
#include "Motherboard.h"

/*
 * Sequencer
 */
class Byte{
  private:
    static Byte *instance;
    Byte();
    
    // Motherboard
    Motherboard *device;
    
    enum ClockMode { Leading = 0, Following = 1 };
    ClockMode clockMode = Leading;
    
    bool isPlaying = true;
    
    byte currentStep = 0;
    byte currentBar = 0;
    unsigned int timeBetweenSteps = 0;
    unsigned int timeBetweenTicks = 0;
    elapsedMillis stepClock = 0;
    elapsedMillis ticksClock;
    byte ticksReadings;
    elapsedMillis lastTick;
    unsigned int *ticks;
    byte tempo = 0;
    elapsedMillis bounceClock = 0;
    Display *display;

    byte selectedVoice = 0;
    byte selectedBar = 0;

    Voice voices[8] = {
      Voice(36),
      Voice(40),
      Voice(39),
      Voice(45),
      Voice(37),
      Voice(42),
      Voice(46),
      Voice(51)
    };

  public:
    static Byte *getInstance();
    void init();
    void update();
    void setTempo(byte tempo);
    void sendNoteOn(byte note);
    void sendNoteOff(byte note);
    void sendStop();
    void onStepIncrement();
    
    // Midi callbacks
    static void onMidiClockChange(byte channel, byte control, byte value);
    static void onMidiSongPosition(unsigned beats);
    static void onMidiClock();
    static void onMidiStart();
    static void onMidiStop();

    // Controls callbacks
    // Clock
    static void onClockShortPress(byte inputIndex);
    static void onClockLongPress(byte inputIndex);
    static void onClockChange(byte inputIndex, bool value);
    // Voice
    static void onVoicePress(byte inputIndex);
    // Bar
    static void onBarPress(byte inputIndex);
    // Mode
    static void onModePress(byte inputIndex);
    // Steps buttons
    static void onStepPressDown(byte inputIndex);
    static void onStepPressUp(byte inputIndex);
    
};

// Singleton pre init
Byte * Byte::instance = nullptr;

/**
 * Constructor
 */
inline Byte::Byte(){
  this->display = new Display();
  this->setTempo(60);

  this->ticksReadings = 0;
  this->ticks = new unsigned int[20];
  for (byte i = 0; i < 20; i++) {
    this->ticks[i] = 0;
  }
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
  // Motherboard init
  this->device = Motherboard::init(
    "Byte",
    {
      Button,        Button, Button, Button,
      Button,        Button, Button, Button,
      RotaryEncoder, Button, Button, Button,
    }
  );

  // Device callbacks
  this->device->setHandlePressDown(0, onStepPressDown);
  this->device->setHandlePressUp(0, onStepPressUp);
  this->device->setHandlePressDown(1, onStepPressDown);
  this->device->setHandlePressUp(1, onStepPressUp);
  this->device->setHandlePressDown(2, onStepPressDown);
  this->device->setHandlePressUp(2, onStepPressUp);
  this->device->setHandlePressDown(3, onStepPressDown);
  this->device->setHandlePressUp(3, onStepPressUp);
  this->device->setHandlePressDown(4, onStepPressDown);
  this->device->setHandlePressUp(4, onStepPressUp);
  this->device->setHandlePressDown(5, onStepPressDown);
  this->device->setHandlePressUp(5, onStepPressUp);
  this->device->setHandlePressDown(6, onStepPressDown);
  this->device->setHandlePressUp(6, onStepPressUp);
  this->device->setHandlePressDown(7, onStepPressDown);
  this->device->setHandlePressUp(7, onStepPressUp);
  this->device->setHandlePressUp(8, onClockShortPress);
  this->device->setHandleLongPressUp(8, onClockLongPress);
  this->device->setHandleRotaryChange(8, onClockChange);
  this->device->setHandlePressUp(9, onVoicePress);
  this->device->setHandlePressUp(10, onBarPress);
  this->device->setHandlePressUp(11, onModePress);
  
  // Midi callbacks
  device->setHandleMidiControlChange(0, 0, "Clock", onMidiClockChange);
  MIDI.setHandleSongPosition(onMidiSongPosition);
  MIDI.setHandleClock(onMidiClock);
  usbMIDI.setHandleClock(onMidiClock);
  MIDI.setHandleStart(onMidiStart);
  usbMIDI.setHandleStart(onMidiStart);
  MIDI.setHandleStop(onMidiStop);
  usbMIDI.setHandleStop(onMidiStop);
}

/**
 * Update
 */
inline void Byte::update(){
  this->device->update();

  if(this->tempo > 0 && this->stepClock >= this->timeBetweenSteps && this->isPlaying){
    this->stepClock = 0;

    // Step increment
    this->onStepIncrement();
    
    this->currentStep++;
    
    byte totalBars = 0;
    for(byte i=0; i<8; i++){
      if(this->voices[i].getBarsCount() > totalBars){
        totalBars = this->voices[i].getBarsCount();
      }
    }
    
    if(this->currentStep == 8){
      this->currentStep = 0;

      if(totalBars-1 > this->currentBar){
        this->currentBar++;
      }else{
        this->currentBar = 0;
      }
  
      if(this->currentBar == 8){
        this->currentBar = 0;
      }
    }
  }

  // Clock
  switch(this->clockMode){
    case ClockMode::Leading:
      if(this->tempo > 0 && this->ticksClock >= this->timeBetweenTicks){
        MIDI.sendClock();
        this->ticksClock = 0;
      }
    break;
    
    case ClockMode::Following:
    break;
  }
  
  this->display->update();
}


/**
 * Set the tempo
 */
inline void Byte::setTempo(byte tempo){
  this->tempo = tempo;
  this->timeBetweenSteps = (float)1/tempo*60*1000/4;
  this->timeBetweenTicks = (float)1/tempo*60*1000/24;
}

/**
 * Send Midi note on
 */
inline void Byte::sendNoteOn(byte note){
  MIDI.sendNoteOn(note, 127, this->device->getMidiChannel());
  usbMIDI.sendNoteOn(note, 127, this->device->getMidiChannel());
}

/**
 * Send Midi note off
 */
inline void Byte::sendNoteOff(byte note){
  MIDI.sendNoteOff(note, 127, this->device->getMidiChannel());
  usbMIDI.sendNoteOff(note, 127, this->device->getMidiChannel());
}

/**
 * Send Midi stop
 */
inline void Byte::sendStop(){
  for(byte i=0; i<8; i++){

  }
}



/**
 * Midi clock callback
 */
inline void Byte::onMidiClock(){
  switch(getInstance()->clockMode){
    case ClockMode::Following:

      if(getInstance()->ticksReadings == 0){
        getInstance()->lastTick = 0;
      }else{
        getInstance()->ticks[getInstance()->ticksReadings] = getInstance()->lastTick;
        getInstance()->lastTick = 0;
      }
      
      if (getInstance()->ticksReadings == 20) {
        unsigned int averageTime = 0;
        for (byte i = 0; i < 20; i++) {
          averageTime += getInstance()->ticks[i];
        }
        averageTime = averageTime / 19; // index 0 is always 0
        getInstance()->setTempo((byte)((float)1/averageTime*60*1000/24));

        // Reinit the ticks
        getInstance()->ticksReadings = 0;
        for (byte i = 0; i < 20; i++) {
          getInstance()->ticks[i] = 0;
        }
      }else{
        getInstance()->ticksReadings++;
      }
    break;
    
    default:
    break;
  }
}

/**
 * Midi Song position callback
 */
inline void Byte::onMidiSongPosition(unsigned songPosition){
  Serial.println("onSongPosition");
  switch(getInstance()->clockMode){
    case ClockMode::Following:
//      getInstance()->currentStep = songPosition % 8;
//      getInstance()->currentBar = songPosition / 8;
      if(songPosition == 0){
        getInstance()->currentStep = 0;
        
        // Step increment
        getInstance()->onStepIncrement();
      }
    break;
    
    default:
    break;
  }
}

/**
 * Midi Start callback
 */
inline void Byte::onMidiStart(){
  switch(getInstance()->clockMode){
    case ClockMode::Following:
      Serial.println("onStart");
      getInstance()->currentStep = 0;
    break;
    
    default:
    break;
  }
}

/**
 * Midi Stop callback
 */
inline void Byte::onMidiStop(){
  switch(getInstance()->clockMode){
    case ClockMode::Following:
      Serial.println("onStop");
      getInstance()->currentStep = 0;
    break;
    
    default:
    break;
  }
}

/**
 * On Clock short press 
 */
inline void Byte::onClockShortPress(byte inputIndex){
  switch(getInstance()->display->getCurrentDisplayMode()){
    case DisplayMode::Sequencer:
    case DisplayMode::Mixer:
    case DisplayMode::Trigger:
      // Play / Stop
      getInstance()->isPlaying = !getInstance()->isPlaying;
      getInstance()->currentStep = 0;
      getInstance()->currentBar = 0;
      getInstance()->bounceClock = 0;
      getInstance()->stepClock = 0;
      if(!getInstance()->isPlaying){
        getInstance()->sendStop();
      }
    break;
    
    case DisplayMode::Clock:
      // Exiting the Clock mode
      getInstance()->display->setCurrentDisplay(DisplayMode::Sequencer);
    break;
    
    default:
    break;
  }
}

/**
 * On Clock long press 
 */
inline void Byte::onClockLongPress(byte inputIndex){
  switch(getInstance()->display->getCurrentDisplayMode()){
    case DisplayMode::Sequencer:
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
inline void Byte::onClockChange(byte inputIndex, bool value){
  int inValue = 1;
  if(!value){
    inValue = -1;
  }
  
  switch(getInstance()->display->getCurrentDisplayMode()){
    case DisplayMode::Sequencer:
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
 * On MIDI Clock Change
 */
void Byte::onMidiClockChange(byte channel, byte control, byte value){
  byte mapValue = map(value, 0, 127, 0, 255);
  getInstance()->setTempo(mapValue);
}

/**
 * On Voice press 
 */
inline void Byte::onVoicePress(byte inputIndex){
  switch(getInstance()->display->getCurrentDisplayMode()){
    case DisplayMode::Sequencer:
      getInstance()->display->setCursor(getInstance()->selectedVoice);
      getInstance()->display->setCurrentDisplay(DisplayMode::VoiceDisplay);
    break;    
    default:
    break;
  }
}


/**
 * On Bar press 
 */
inline void Byte::onBarPress(byte inputIndex){
  switch(getInstance()->display->getCurrentDisplayMode()){
    case DisplayMode::Sequencer:
      getInstance()->display->setCursor(getInstance()->selectedBar);
      getInstance()->display->setCurrentDisplay(DisplayMode::Bar);
    break;
    default:
    break;
  }
}

/**
 * On Mode press 
 */
inline void Byte::onModePress(byte inputIndex){
  switch(getInstance()->display->getCurrentDisplayMode()){
    case DisplayMode::Sequencer:
      getInstance()->display->setCurrentDisplay(DisplayMode::Mixer);
    break;
    case DisplayMode::Mixer:
      getInstance()->display->setCurrentDisplay(DisplayMode::Trigger);
    break;
    case DisplayMode::Trigger:
      getInstance()->display->setCurrentDisplay(DisplayMode::Sequencer);
    break;
    default:
    break;
  }
}

/**
 * On Step press down
 */
inline void Byte::onStepPressDown(byte inputIndex){
  switch(getInstance()->display->getCurrentDisplayMode()){
    case DisplayMode::Sequencer:
    {
      byte newPattern = getInstance()->voices[getInstance()->selectedVoice].getPattern(getInstance()->selectedBar);
      newPattern ^= 1 << inputIndex;
      
      getInstance()->voices[getInstance()->selectedVoice].setPattern(
        getInstance()->selectedBar, 
        newPattern
      );
    }
    break;
    
    case DisplayMode::VoiceDisplay:
      getInstance()->selectedVoice = inputIndex;
      constrain(getInstance()->selectedVoice, 0, 7);
      
      getInstance()->display->keepCurrentDisplayShort();
      getInstance()->display->setCursor(getInstance()->selectedVoice);
    break;
    
    case DisplayMode::Bar:
      getInstance()->selectedBar = inputIndex;
      constrain(getInstance()->selectedBar, 0, 7);
      
      getInstance()->display->keepCurrentDisplayShort();
      getInstance()->display->setCursor(getInstance()->selectedBar);
    break;
    
    case DisplayMode::Mixer:
      getInstance()->voices[inputIndex].toggleMuted();
    break;
    
    case DisplayMode::Trigger:
      getInstance()->sendNoteOn(getInstance()->voices[inputIndex].getMidiNote());
      getInstance()->display->setCursor(inputIndex);
      getInstance()->display->setHideCursor(false);
    break;
    
    default:
    break;
  }
}


/**
 * On Step press up
 */
inline void Byte::onStepPressUp(byte inputIndex){
  switch(getInstance()->display->getCurrentDisplayMode()){
    case DisplayMode::Trigger:
      getInstance()->sendNoteOff(getInstance()->voices[inputIndex].getMidiNote());
      getInstance()->display->setHideCursor(true);
    break;
    
    default:
    break;
  }
}

/**
 * On step increment
 */
inline void Byte::onStepIncrement(){
  switch(this->display->getCurrentDisplayMode()){
    case DisplayMode::Sequencer:
      this->display->setBinary(this->voices[this->selectedVoice].getPattern(this->selectedBar));
      this->display->setCursor(this->currentStep);
      this->display->setHideCursor(!this->isPlaying || this->selectedBar != this->currentBar);  
    break;
    
    case DisplayMode::Mixer:
    {
      byte mutedVoices = 0;
      byte activeVoices = 0;
      for(byte i=0; i<8; i++){

        // Mute
        if(this->voices[i].isMuted()){
          mutedVoices |= 1 << i;
        }

        // Active
        if(this->voices[i].isStepActive(this->currentBar, this->currentStep)){
          activeVoices |= 1 << i;
        }
      }
      
      byte data[3] = {mutedVoices, activeVoices, 0};
      this->display->setData(data);
    }
    break;
    
    case DisplayMode::Trigger:
    {
      byte activeVoices = 0;
      for(byte i=0; i<8; i++){
        // Active
        if(this->voices[i].isStepActive(this->currentBar, this->currentStep)){
          activeVoices |= 1 << i;
        }
      }
      
      byte data[3] = {activeVoices, 0, 0};
      this->display->setData(data);
    }
    break;
    
    default:
    break;
  }

  for(byte i=0; i<8; i++){
    if(this->voices[i].isStepActive(this->currentBar, this->currentStep)){
      this->sendNoteOn(this->voices[i].getMidiNote());
    }
  }
  
  // Song position
  switch(this->clockMode){
    case ClockMode::Leading:
      MIDI.sendSongPosition(this->currentBar * 8 +  this->currentStep);
    break;
    
    default:
    break;
  }
}

#endif
