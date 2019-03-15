/*
this code operates the controller that reads the 8 bits from the output bus on my Ben Eater clone CPU or other 8 bit output source
with a clock or strobe that indicates when a byte has updated
decoded data is essentially converted to serial (RS232 and synchronus (SPI?/shift register)) to drive either a serial terminal or
a shift register that drives additional decoders (EEPROM + analogue switches for my ASCII to typewriter)  
*/

byte buttonEncode = 0; //throwback from when I used similar code on a different project that encoded bits into a byte
  bool ASCIImode = 0;
  bool modeTrig = 0;
  bool bootStrapDet = 0;
  bool hexMode = 0;
  byte bufferBytes[1024];
  int charLine = 0;
  int bufferIndex = 0;
  unsigned int loopCount = 0;
  //byte oldVal = 0;
void setup() {
  bufferBytes[1] = 255;
  Serial.begin(9600);
  // put your setup code here, to run once:
#define DataPin 9 //shift register data pin
#define DataClock 12 //shift register/data clock pin
#define Strobe 11 // can be called R clock or similar
#define OutputEn 10 // attach to the OE of the register or in my case one of the decode ROMs that isnt controlling shift
#define OutBusB0 13 //output bus bit 0 (LSB) input
#define OutBusB1 2  //output bus bit 1 input
#define OutBusB2 3  //output bus bit 2 input
#define OutBusB3 4  //output bus bit 3 input
#define OutBusB4 5  //output bus bit 4 input
#define OutBusB5 6  //output bus bit 5 input
#define OutBusB6 7  //output bus bit 6 input
#define OutBusB7 8  //output bus bit 7 (MSB) input
#define OutWrite A0 //output latch clock control line 
#define lineLength 59 //length of a line on the typewriter/device only; serial term unaffected

pinMode(DataPin, OUTPUT);
pinMode(DataClock, OUTPUT);
pinMode(Strobe, OUTPUT);
pinMode(OutputEn, OUTPUT);
pinMode(OutBusB0, INPUT);
pinMode(OutBusB1, INPUT);
pinMode(OutBusB2, INPUT);
pinMode(OutBusB3, INPUT);
pinMode(OutBusB4, INPUT);
pinMode(OutBusB5, INPUT);
pinMode(OutBusB6, INPUT);
pinMode(OutBusB7, INPUT);
pinMode(OutWrite, INPUT);
digitalWrite(OutputEn,HIGH);
sendCarriageRet(); //send a CR to the typewriter if the carriage position is not known
 shiftOut(DataPin,DataClock, MSBFIRST,0); //clear the shift reg, 
  digitalWrite(Strobe,HIGH);
   digitalWrite(Strobe,LOW);

}

void loop() {
  digitalWrite(DataPin, LOW);
  loopCount ++;
if(digitalRead(OutWrite)){ // if it picks up the control line pulse
    loopCount =0; // reset the timer
 if(digitalRead(OutBusB0) == 1){
    bitSet (buttonEncode,0);}else{bitClear (buttonEncode,0);}
  if(digitalRead(OutBusB1) == 1){
    bitSet (buttonEncode,1);}else{bitClear (buttonEncode,1);}
      if(digitalRead(OutBusB2) == 1){
    bitSet (buttonEncode,2);}else{bitClear (buttonEncode,2);}
      if(digitalRead(OutBusB3) == 1){
    bitSet (buttonEncode,3);}else{bitClear (buttonEncode,3);}
      if(digitalRead(OutBusB4) == 1){
    bitSet (buttonEncode,4);}else{bitClear (buttonEncode,4);}
      if(digitalRead(OutBusB5) == 1){
    bitSet (buttonEncode,5);}else{bitClear (buttonEncode,5);}
      if(digitalRead(OutBusB6) == 1){
    bitSet (buttonEncode,6);}else{bitClear (buttonEncode,6);}
      if(digitalRead(OutBusB7) == 1){
    bitSet (buttonEncode,7);}else{bitClear (buttonEncode,7);} 
    
   bufferBytes[bufferIndex] = buttonEncode; // store the output bus byte into the buffer
   if(!digitalRead(OutWrite)){ //only when the control line falls
   bufferIndex ++;} //increment the buffer index

}

if(bufferIndex >=1023){ //if buffer is full


bootStrapDet = bootStrapDetect(); //check if a bootstrap is detected
    for(int J = 0; J < bufferIndex; J++){ //flush the buffer
       if(bootStrapDet){J = 248; bootStrapDet = 0;} //skip to 248 if a bootstrap was dumped into the buffer
       if(bufferBytes[J] == 1 || bufferBytes[J]== 2){//check which mode the routine needs to be in
      ASCIImode = checkAsciiMode(J);}
      if(bufferBytes[J] == 3){hexMode = checkHexMode(J);}
      if(modeTrig){J += 2; modeTrig = 0;}// skip sending the control code bytes
      if(ASCIImode && bufferBytes[J] == 8){charLine --;} // backspace
      if(ASCIImode && bufferBytes[J] == 13){charLine = 0;} //return
      typeOut(bufferBytes[J],ASCIImode,hexMode);} //output routine
      bufferIndex = 0;
    }

  


  if(loopCount > 32767){ //if buffer routine times out (~300+ms)
    loopCount = 0;// reset loop counter
    if(bufferIndex > 0){ //if index is not zero (something in the buffer)
  
  bootStrapDet = bootStrapDetect();
     for(int J = 0; J < bufferIndex; J++){ //flush out the buffer
      if(bootStrapDet){J = 248; bootStrapDet = 0;} //duplicate of above
      if(bufferBytes[J] == 1 || bufferBytes[J]== 2){
      ASCIImode = checkAsciiMode(J);}
       if(bufferBytes[J] == 3){hexMode = checkHexMode(J);}
      if(modeTrig){ J += 2; modeTrig = 0;}
      if(ASCIImode && bufferBytes[J] == 8){charLine --;} // backspace, take charLine back one space
      if(ASCIImode && bufferBytes[J] == 13){charLine = 0;} //return, reset charLine
      typeOut(bufferBytes[J],ASCIImode,hexMode);}
      bufferIndex = 0;
      }
      }


}

void typeOut(byte chrCode, bool AsciiMode, bool HexMode){
 //Serial.println(charLine);
  digitalWrite(OutputEn,HIGH);
 
if(AsciiMode){
 
  
  shiftOut(DataPin,DataClock, MSBFIRST,chrCode); //in ASCII mode, send out the raw byte, no processing required
  digitalWrite(Strobe,HIGH);
   digitalWrite(Strobe,LOW);
    delay(60);// adjust to suit typewriter, this is the maximum speed my one can handle before dropping characters
    digitalWrite(OutputEn,LOW);
   delay(60);// adjust to suit typewriter, this is the maximum speed my one can handle before dropping characters
    digitalWrite(OutputEn,HIGH);
    Serial.write(chrCode);
    if (charLine >= lineLength){sendCarriageRet();}else{charLine ++;}// if line has exceeded length, send a carriage return
    //Serial.println();
    }
    else{//in numbers mode we need to do some more stuff to it to decode into up to 3 digits 0 - 255
      //converts 8 bit value to up to 3 digits up to 255
     //increment charLine depending on how many characters it needed to send
         // Serial.println(charLine);
        if(!HexMode){ 
      byte hundreds = chrCode / 100;
      byte tens = (chrCode % 100)/10;
      byte units = chrCode % 10;
     byte convertNumb[4];
      convertNumb[0] = hundreds + 48;
      convertNumb[1] = tens + 48;
      convertNumb[2] = units + 48;
      convertNumb[3] = 32; //seperator. 13 for new line(CRLF), 32 space, 44 comma.
      byte i = 0;
      for(i = 0; i < 4; i++){ 
        if(convertNumb[i] == 48 && i==0){i++;} //skip any (leading) zeros
        if(convertNumb[i] == 48 && i==1){
          if(convertNumb[0]<49 && convertNumb[1]==48){i++;} //except if it is part of 100 or 200
          
        }
         
         
        Serial.write(convertNumb[i]);
        shiftOut(DataPin,DataClock, MSBFIRST,convertNumb[i]);
       
        digitalWrite(Strobe,HIGH);
        digitalWrite(Strobe,LOW);
        delay(60);
          digitalWrite(OutputEn,LOW);
       delay(60);
    digitalWrite(OutputEn,HIGH);
   if (charLine >= lineLength){sendCarriageRet();}else{charLine ++;}
        }
        }else{
          //hex convertor code
          byte hiNyble = chrCode / 16;
        
      byte loNyble = chrCode % 16;
      
     byte convertNumb[3];
      convertNumb[0] = hiNyble;
      convertNumb[1] = loNyble;
      convertNumb[2] = 32;  //seperator. 13 for new line(CRLF), 32 space, 44 comma.
      if(convertNumb[0] > 9){convertNumb[0] += 55; }else{convertNumb[0] += 48;}
       if(convertNumb[1] > 9){convertNumb[1] += 55; }else{convertNumb[1] += 48;}//convertNumb[3] = 32; //seperator. 13 for new line(CRLF), 32 space, 44 comma.
      byte i = 0;
      for(i = 0; i < 3; i++){ 
        //if(convertNumb[i] == 48 && i==0){i++;} //skip any (leading) zeros
       // if(convertNumb[i] == 48 && i==1){
        //  if(convertNumb[0]<49 && convertNumb[1]==48){i++;} //except if it is part of 100 or 200
          
        
         
         
        Serial.write(convertNumb[i]);
        shiftOut(DataPin,DataClock, MSBFIRST,convertNumb[i]);
       
        digitalWrite(Strobe,HIGH);
        digitalWrite(Strobe,LOW);
        delay(60);
          digitalWrite(OutputEn,LOW);
       delay(60);
    digitalWrite(OutputEn,HIGH);
    if (charLine >= lineLength){sendCarriageRet();}else{charLine ++;}
    }
        
        //Serial.println(charLine);
        
      
        }
      
      
      }
    
    if(AsciiMode && chrCode == 13){
    Serial.println();// if you're only sending a 13 to the typewriter for CRLF the terminal wont work properly, this compensates for that 
    delay(1500);}   
    if(AsciiMode && chrCode == 8){
    delay(500);}    
  }

 bool checkAsciiMode(int location){
    if(bufferBytes[location] == 1 && bufferBytes[location + 1] == 0){

       modeTrig = 1;
       Serial.println();
       sendCarriageRet();
    Serial.println("ASCII mode"); //comment out if using on serial and dont want these messages
    return 1;


  }
  
  if(bufferBytes[location] == 2 && bufferBytes[location + 1 ] == 0){
      //bufferBytes[0] = 255;
      hexMode = 0;
       Serial.println();
       sendCarriageRet();
    Serial.println("Num mode"); //comment out if using on serial and dont want these messages
     modeTrig = 1;
     return 0;
   



  }else{modeTrig = 0; return 0; }//default action if no condition is met

  
  }
bool checkHexMode(int location){
    if(bufferBytes[location] == 3 && bufferBytes[location + 1] == 0){
       //bufferBytes[0] = 255;
       //bufferIndex = 0;
       ASCIImode = 0;
       
       modeTrig = 1;
        Serial.println();
        sendCarriageRet();
    Serial.println("Hex Mode "); //comment out if using on serial and dont want these messages
    return 1;


  }
  
 else{modeTrig = 0; return 0; }//default action if no condition is met

  
  }
  bool bootStrapDetect(){
    if(bufferBytes[0] == 0 && bufferBytes[247] == 247){
       Serial.println();
       sendCarriageRet();
  Serial.println("bootstrapping detected"); //comment out if using on serial and dont want these messages
    //bufferBytes[0] = 255;
  return 1;
    
  }else{return 0;}
  
  }

  void sendCarriageRet(){
    digitalWrite(OutputEn,HIGH);
    Serial.println(" CRLF"); //comment out if using on serial and dont want these messages
    charLine = 0; //reset line counter
    shiftOut(DataPin,DataClock, MSBFIRST,13); // send a character 13 to the typewriter decode to reset carriage
  digitalWrite(Strobe,HIGH);
   digitalWrite(Strobe,LOW);
delay(60);
      digitalWrite(OutputEn,LOW);
    delay(60);//    shiftOut(DataPin,DataClock, MSBFIRST,0);
//  digitalWrite(Strobe,HIGH);
 //  digitalWrite(Strobe,LOW);
//    Serial.write(chrCode);
    //Serial.println();
    digitalWrite(OutputEn,HIGH);delay(1250);}
