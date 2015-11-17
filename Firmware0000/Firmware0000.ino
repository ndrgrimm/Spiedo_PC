/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.
 
  This example code is in the public domain.
 */
 
// Pin 13 has an LED connected on most Arduino boards.
// give it a name:
const int led_write = 13;
const int led_read =  12;
const int led_auto =  11;
const int led_menu =  9;




const int Sample_length=1000;
void ReadData(double &_Mean, double &_Sigma);
const int PWM_pin=10;
void setPwmFrequency(int pin, int divisor);

void establishContact(const char *SketchCode);
void SetPWM(int dutyCicle, int time=1000);
// the setup routine runs once when you press reset:
void setup() {                
  // initialize the digital pin as an output.
  //setPwmFrequency(PWM_pin,2);
  analogReference(EXTERNAL);

  pinMode(led_write, OUTPUT);   
  pinMode(led_read, OUTPUT);
  pinMode(led_auto, OUTPUT);
  pinMode(led_menu, OUTPUT);
  pinMode(PWM_pin, OUTPUT);
  
  digitalWrite(led_write,HIGH);
  digitalWrite(led_read,HIGH);
  digitalWrite(led_auto,HIGH);
  digitalWrite(led_menu,HIGH);
  delay(500);
  digitalWrite(led_write,LOW);
  digitalWrite(led_read,LOW);
  digitalWrite(led_auto,LOW);
  digitalWrite(led_menu,LOW);
  
  //Opening SerialPort  
  Serial.begin(115200);
  while( !( Serial ) ){
  digitalWrite(led_write,HIGH);
  delay(300);
  digitalWrite(led_write,LOW);
  digitalWrite(led_read,HIGH);
  delay(300);
  digitalWrite(led_read,LOW);
  digitalWrite(led_auto,HIGH);
  delay(300);
  digitalWrite(led_auto,LOW);
  digitalWrite(led_menu,HIGH);
  delay(300);
  digitalWrite(led_menu,LOW);

  }
  
}

// the loop routine runs over and over again forever:
void loop() {
  char msg;
  String arg;
  int duty=0;                            ////////////////////////////////////////////
  int tmp=0;
  int arg_length=0;
  
  double Sample;
  double Mean;
  double Sigma;
  
  int int_mean=0;
  unsigned long time=0;
  unsigned long time2=0;
  boolean exit=false;
  establishContact("TestPiezo");
  digitalWrite(led_menu,HIGH);
  while(true){
    
    if( Serial.available() ){
      
      msg=Serial.read();
      Serial.readString();
      
      switch (msg){
        case 'w':
          
          digitalWrite(led_write,HIGH);
          while(  Serial.available() <= 0  ){}
          delay(300);
          
          duty=0;
          arg=Serial.readString();
          arg_length=arg.length();
          while( arg[arg_length-1] == '\r' ||  arg[arg_length-1] == '\n' ) --arg_length; 
          
          for( int i = 1; i< arg_length+1 ; ++i ){
            
              if( arg[arg_length-i] < 48 || arg[arg_length-i]>57){
                 goto mydefault;
               }
              tmp=1;
              for( int j=0; j<i-1; ++j){
                tmp*=10;
              }
              duty+=(arg[arg_length-i]-48)*tmp;

          }
          SetPWM(duty);
          digitalWrite(led_write,LOW);

          break;
          
         case 'r':
           digitalWrite(led_read,HIGH);
           ReadData(Mean, Sigma);           
           Serial.print(duty);
           Serial.print(":");
           Serial.print(Mean);
           Serial.print(":");
           Serial.print(Sigma);
           Serial.print("#");
           delay(500);
           digitalWrite(led_read,LOW);
           break;
         case 'a':
           digitalWrite(led_auto,HIGH);
           for(int duty =0; duty<256; duty+=2){
             
             SetPWM(duty);
             ReadData(Mean, Sigma);           
             digitalWrite(led_read,HIGH);
             Serial.print(duty);
             Serial.print(":");
             Serial.print(Mean);
             Serial.print(":");
             Serial.print(Sigma);
             Serial.println("#");
             delay(200);
             digitalWrite(led_read,LOW);
             
           }
           Serial.print("$");
           digitalWrite(led_auto,LOW);
	   
           break;
          case 'q':
             exit=true;
             break; 
          case 't':
             while(  Serial.available() <= 0  ){}
              delay(300);
              
              duty=0;
              arg=Serial.readString();
              arg_length=arg.length();
              while( arg[arg_length-1] == '\r' ||  arg[arg_length-1] == '\n' ) --arg_length; 
              
              for( int i = 1; i< arg_length+1 ; ++i ){
                
                  if( arg[arg_length-i] < 48 || arg[arg_length-i]>57){
                    
                     goto mydefault;
                  }
                  
                  tmp=1;
                  for( int j=0; j<i-1; ++j){
                    
                    tmp*=10;
                  }
                  duty+=(arg[arg_length-i]-48)*tmp;
              }
              
              for( int i=0; i<duty; ++i){
                time=micros();
                int_mean=analogRead(A0);

                Serial.print( time );
                Serial.print( ":" );
                Serial.print( int_mean );
                Serial.print( "#" );
              }
              Serial.print("$");
              duty=0;
              break;
          default:
              mydefault:
              for( int i=0; i < 5; ++i){
                  digitalWrite(led_write,HIGH);
                  delay(200);
                  digitalWrite(led_read,HIGH);
                  delay(200);
                  digitalWrite(led_auto,HIGH);
                  delay(200);
                  digitalWrite(led_menu,HIGH);
                  delay(200);
                  delay(200);
                  digitalWrite(led_menu,LOW);
                  digitalWrite(led_auto,LOW);
                  digitalWrite(led_read,LOW);
                  digitalWrite(led_write,LOW);
                  delay(200);
              }
              
             ;
       
     }
   delay(1000);
    
   }
   if(exit) break;
 }
 digitalWrite(led_menu,LOW);
}
   



void establishContact(const char *SketchCode) {
  while (Serial.available() <= 0) {
    delay(200);
    Serial.println(SketchCode);      // send sketchCode
    digitalWrite(led_write,HIGH);
    delay(200);
    digitalWrite(led_write,LOW);
    digitalWrite(led_read,HIGH);
    delay(200);
    digitalWrite(led_read,LOW);
    digitalWrite(led_auto,HIGH);
    delay(200);
    digitalWrite(led_auto,LOW);
    digitalWrite(led_menu,HIGH);
    delay(200);
    digitalWrite(led_menu,LOW);
    digitalWrite(PWM_pin,HIGH);
    delay(200);
    digitalWrite(PWM_pin,LOW);
  }
  delay(300);
  Serial.readString();
  Serial.println();
  
  
}

void SetPWM(int dutyCicle, int time){
  
  analogWrite(PWM_pin,dutyCicle);
  
  boolean setHIGH=false;
  if( digitalRead(led_write) == HIGH ) setHIGH=true;
  for(int i=0; i <5 ;++i){
  digitalWrite(led_write,HIGH);      //
  delay(200);
  digitalWrite(led_write,LOW);       //
  delay(200); 
  }
  if( setHIGH ) digitalWrite(led_write,HIGH);

}

void ReadData(double &_Mean, double &_Sigma){
  double Sample;

  _Mean=0;
  _Sigma=0;
  for(int numberMesure=0; numberMesure<Sample_length; ++numberMesure){
    Sample=analogRead(A0);
    _Mean+=Sample*1./Sample_length;
    _Sigma+=Sample*Sample*1./Sample_length;
  }
  _Sigma=sqrt(_Sigma-_Mean*_Mean);
}


 void setPwmFrequency(int pin, int divisor) {
  byte mode;
  if(pin == 5 || pin == 6 || pin == 9 || pin == 10) {
    switch(divisor) {
      case 1: mode = 0x01; break;
      case 8: mode = 0x02; break;
      case 64: mode = 0x03; break;
      case 256: mode = 0x04; break;
      case 1024: mode = 0x05; break;
      default: return;
    }
    if(pin == 5 || pin == 6) {
      TCCR0B = TCCR0B & 0b11111000 | mode;
    } else {
      TCCR1B = TCCR1B & 0b11111000 | mode;
    }
  }else if(pin == 3 || pin == 11) {
    switch(divisor) {
      case 1: mode = 0x01; break;
      case 8: mode = 0x02; break;
      case 32: mode = 0x03; break;
      case 64: mode = 0x04; break;
      case 128: mode = 0x05; break;
      case 256: mode = 0x06; break;
      case 1024: mode = 0x7; break;
      default: return;
    }
    TCCR2B = TCCR2B & 0b11111000 | mode;
  }
}


  
