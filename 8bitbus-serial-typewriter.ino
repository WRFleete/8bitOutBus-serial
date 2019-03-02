    byte buttonEncode = 0;
  bool ASCIImode = 0;
  //bool bootStrapDet = 0;
  byte bufferBytes[1024];
  int bufferIndex = 0;
  unsigned int loopCount = 0;
  //byte oldVal = 0;
void setup() {
  bufferBytes[1] = 255;
  Serial.begin(9600);
  // put your setup code here, to run once:
#define DataPin 9
#define DataClock 12
#define Strobe 11
#define OutputEn 10
#define OutBusB0 13
#define OutBusB1 2
#define OutBusB2 3
#define OutBusB3 4
#define OutBusB4 5
#define OutBusB5 6
#define OutBusB6 7
#define OutBusB7 8
#define OutWrite A0

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


}

void loop() {
  loopCount ++;
if(digitalRead(OutWrite)){
    loopCount =0;
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
    
   bufferBytes[bufferIndex] = buttonEncode;
   bufferIndex ++;

}

if(bufferIndex >=1023){
    for(int J = 0; J < bufferIndex; J++){
      //if(bootStrapDet){J = 248; bootStrapDet = 0;}
      typeOut(bufferBytes[J],ASCIImode);}
      bufferIndex = 0;
    }
 /**if(bufferBytes[0] == 0 && bufferBytes[247] == 247){
  bootStrapDet = 1;
    Serial.println("bootstrapping");
    bufferBytes[0] = 255;
  }//else{bootStrapDet = 0;}**/
  
 if(bufferBytes[0] == 1 && bufferBytes[1] == 0){
  ASCIImode = 1;
  bufferBytes[0] = 0;
  bufferIndex = 0;
  Serial.println("ASCII mode");
  }
  if(bufferBytes[0] == 2 && bufferBytes[1] == 0){
  ASCIImode = 0;
  bufferBytes[0] = 0;
  bufferIndex = 0;
  Serial.println("Num mode");
  }

  if(loopCount > 32767){
    loopCount = 0;
     for(int J = 0; J < bufferIndex; J++){
     // if(bootStrapDet){J = 248; bootStrapDet = 0;}
      typeOut(bufferBytes[J],ASCIImode);}
      bufferIndex = 0;
      }


}

void typeOut(byte chrCode, bool AsciiMode){
  digitalWrite(OutputEn,HIGH);
 
if(AsciiMode){
  shiftOut(DataPin,DataClock, MSBFIRST,chrCode);
  digitalWrite(Strobe,HIGH);
   digitalWrite(Strobe,LOW);
    digitalWrite(OutputEn,LOW);
    Serial.write(chrCode);
    //Serial.println();
    delay(20);}
    else{
      //converts 8 bit value to up to 3 digits up to 255
      byte hundreds = chrCode / 100;
      byte tens = (chrCode % 100)/10;
      byte units = chrCode % 10;
     byte convertNumb[4];
      convertNumb[0] = hundreds + 48;
      convertNumb[1] = tens + 48;
      convertNumb[2] = units + 48;
      convertNumb[3] = 13;
      byte i = 0;
      for(i = 0; i < 3; i++){ 
        if(convertNumb[i] == 48 && i==0){i++;} //skip (leading) zeros
        if(convertNumb[i] == 48 && i==1){
          if(convertNumb[0]<49 && convertNumb[1]==48){i++;} //except if it is part of 100 or 200
          
        
        }
        Serial.write(convertNumb[i]);
        shiftOut(DataPin,DataClock, MSBFIRST,convertNumb[i]);
       
        digitalWrite(Strobe,HIGH);
        digitalWrite(Strobe,LOW);
          digitalWrite(OutputEn,LOW);
        delay(20);
        }
      Serial.println();
      
      
      }
    delay(10);
    digitalWrite(OutputEn,HIGH);
      //delay(250);
  }
