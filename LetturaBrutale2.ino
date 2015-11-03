/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.
 
  This example code is in the public domain.
 */
 
// Pin 13 has an LED connected on most Arduino boards.
// give it a name:
const int led1 = 13;
const int led2 = 8;
const int Sample_length=1000;
const int PWM_pin=10;


void establishContact(const char *SketchCode);
void SetPWM(int dutyCicle, int time=1000);
// the setup routine runs once when you press reset:
void setup() {                
  // initialize the digital pin as an output.
  pinMode(led1, OUTPUT);   
  pinMode(led2, OUTPUT);
  pinMode(PWM_pin, OUTPUT);
  digitalWrite(led1,HIGH);
  digitalWrite(led2,HIGH);
  digitalWrite(PWM_pin,HIGH);
  delay(500);
  digitalWrite(led2,LOW);
  digitalWrite(led1,LOW);
  digitalWrite(PWM_pin,LOW);
  //Opening SerialPort  
  Serial.begin(115200);
  while( !( Serial ) ){}
  
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
  double MeanMean;
  boolean exit=false;
  establishContact("Test0000");
  digitalWrite(led2,HIGH);
  while(true){
    
    if( Serial.available() ){
      digitalWrite(led1,HIGH);
      
      msg=Serial.read();
      Serial.readString();
      
      switch (msg){
        case 'w':
          
          digitalWrite(led1,HIGH);
          while(  Serial.available() <= 0  ){}
          delay(300);
          
          duty=0;
          arg=Serial.readString();
          arg_length=arg.length();
          while( arg[arg_length-1] == '\r' ||  arg[arg_length-1] == '\n' ) --arg_length; 
          
          for( int i = 1; i< arg_length+1 ; ++i ){
            
              if( arg[arg_length-i] < 48 || arg[arg_length-i]>57){
                 exit=true;
                 break;
               }
              tmp=1;
              for( int j=0; j<i-1; ++j){
                tmp*=10;
              }
              duty+=(arg[arg_length-i]-48)*tmp;

          }
          if(!exit) SetPWM(duty);
          digitalWrite(led1,LOW);
          break;
          
         case 'r':
           Serial.println(duty);
           break;
         case 'a':
           
           for(int duty =0; duty<256; ++duty){
             Mean=0;
             MeanMean=0;
             SetPWM(duty);
             for(int numberMesure=0; numberMesure<Sample_length; ++numberMesure){
              Sample=analogRead(A0)*1./Sample_length;
              Mean+=Sample;
              MeanMean+=Sample*Sample;
             }
           //  Mean/=Sample_length;
            // MeanMean/=Sample_length;
             Serial.print(duty);
             Serial.print(":");
             Serial.print(Mean);
             Serial.print(":");
             Serial.print(MeanMean);
             Serial.print("#");
             Serial.println();
             
           }
           break;
          case 'q':
             exit=true;
             break; 
         default:
         ;
       
     }
   delay(1000);
    
   }
   if(exit) break;
 }
 digitalWrite(led2,LOW);
}
   



void establishContact(const char *SketchCode) {
  while (Serial.available() <= 0) {
    digitalWrite(led1, LOW);         // turn the LED on (HIGH is the voltage level)
    
    delay(300);
    Serial.println(SketchCode);      // send sketchCode
    
    digitalWrite(led1, HIGH);          // turn the LED off by making the voltage LOW
    delay(1000);                     // wait for a second
  }
  delay(300);
  Serial.readString();
  digitalWrite(led1,LOW);
  
}

void SetPWM(int dutyCicle, int time){
  
  //analogWrite(PWM_pin,dutyCicle);
  digitalWrite(PWM_pin,HIGH);      //
  delay(1000);            // TEST: Togliere e mettere la riga prima
  digitalWrite(PWM_pin,LOW);       //
  delay(time);


}


  
