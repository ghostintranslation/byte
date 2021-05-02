/*
 __    _  _____   ___      ___ _  _     __    _ ______ _    
/__|_|/ \(_  |     | |\|    | |_)|_||\|(_ |  |_| |  | / \|\|
\_|| |\_/__) |    _|_| |    | | \| || |__)|__| | | _|_\_/| |

BYTE
v1.1.0

Support my work:
https://www.paypal.com/paypalme/ghostintranslation

https://www.ghostintranslation.com/
https://ghostintranslation.bandcamp.com/
https://www.instagram.com/ghostintranslation/
https://www.youtube.com/channel/UCcyUTGTM-hGLIz4194Inxyw
https://github.com/ghostintranslation
*/

#include "Byte.h"

// Instanciation of Byte
Byte * myByte = Byte::getInstance();
    
void setup() {
  Serial.begin(115200);
  
  while (!Serial && millis() < 2500); // wait for serial monitor
  
  myByte->init();

  // Ready!
  Serial.println("Ready!");
}

void loop() {
  // Byte update
  myByte->update();
}
