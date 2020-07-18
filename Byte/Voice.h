#ifndef Voice_h
#define Voice_h

/*
 * Voice
 */
class Voice{
  private:
    byte midiNote;
    byte patterns[8];
    
  public:
    Voice(byte midiNote);
    byte getPattern(byte bar);
    void setPattern(byte bar, byte pattern);
    byte getMidiNote();
    byte getBarsCount();
    bool isStepActive(byte bar, byte stp);
    void emptyVoice();

};


/**
 * Constructor
 */
inline Voice::Voice(byte midiNote){
  this->midiNote = midiNote;

  for(byte i=0; i<8; i++){
    this->patterns[i] = 0;
  }
}

inline byte Voice::getPattern(byte bar){
  return this->patterns[bar];
}

inline void Voice::setPattern(byte bar, byte pattern){
  if(pattern<1){
    if(this->getBarsCount() -1 > bar){
      this->patterns[bar] = 1;
    }else{
      this->patterns[bar] = pattern;
    }
  }else{
    this->patterns[bar] = pattern;
  }
}

inline byte Voice::getMidiNote(){
  return this->midiNote;
}

inline byte Voice::getBarsCount(){
  // There is always at least 1 bar
  byte count = 1;
  
  for (byte i=1; i<8; i++) {
    if(this->patterns[i] > 0){
      count++; 
    }else{
      break;
    }
  }
  
  return count;
}

inline bool Voice::isStepActive(byte bar, byte stp){
  return (this->patterns[bar] & (1<<stp));
}

inline void Voice::emptyVoice(){
  for (byte i=0; i<8; i++) {
    this->patterns[i] = 0;
  }
}

#endif
