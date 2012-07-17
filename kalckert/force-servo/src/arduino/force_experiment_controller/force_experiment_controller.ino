/*


 */
 
#include <Servo.h>

// Analog in pins
int flexpin1 = 1;  // analog pin used to connect the flexiforce
int flexpin0 = 0;  // analog pin used to connect the flexiforce
int pospin = 2;    // analog pin used to connect the position potentiometer
int sliderPin = 3; // analog pin used to connect the slider potentiometer

// Digital IO pins
int speakerPin = 4; // 7
int servoPin = 9;
int ledPin = 7; // 5
int redledPin = 6; // 4
int buttonPin = 3;

int sendVal = 0;
int fieldIndex = 0;
const int NUMBER_OF_FIELDS = 2;
int value; //[NUMBER_OF_FIELDS];
float value_float; //[NUMBER_OF_FIELDS];
int set_value;
int reciveValue = 0;
char command;
int ready = 0;
int floatflag = 0;
float dec;
int buttonState = 0;         // variable for reading the pushbutton status


// Variable that the user can change
int RunExp = 0;
int RunMeasure = 0;
int RunCal = 0;
int SensorOrArrayInput = 0; // 0 is sensor
int NoOfRepetions = 1;
int Duration = 3000;
int Delay = 0;
int PauseDuration = 1500;
int ElemetIndex = 0;
int ForceInElement = 0;
int NumberOfForceElements = 24;
float ForceArray[128];
int Debug = 0;

// Contoller parameters
Servo myservo;  // create servo object to control a servo 
int pos_set;
int pos_in;
unsigned long gtime;
unsigned long gtime_old;
unsigned long dt;

void run_controller(int duration, int force_des)
{
  digitalWrite(redledPin,LOW);
  gtime = millis();
  pos_in = analogRead(pospin);
  
  unsigned long t0=gtime;
  unsigned long acc = 0;
  
  while( acc < duration)
  {
    gtime_old = gtime;
    int pos_old = pos_in;
    pos_in = analogRead(pospin);
    gtime = millis();
    acc = gtime-t0;
    dt = gtime - gtime_old;  
    
    int tau_des;
    if (force_des == -1)
    {
      tau_des= analogRead(flexpin0);
    }
    else
    {
      tau_des = force_des; 
    }
    int tau = analogRead(flexpin1);
    int e_tau = tau_des-tau;
    
    float q_dot = (float)(pos_old - pos_in)/(float)dt;
    float k_d = 2.2; //1.7;
    float k_p = 0.0035;
    float e_tau_f = (float)e_tau;
    float u = 3.3*k_p*e_tau_f-k_d*q_dot;
    
    pos_set = pos_set + (int)u;
    myservo.write(pos_set);
    if( (duration-acc) < 500 )
    { 
      digitalWrite(ledPin ,HIGH);
    }
    Serial.print(gtime);
    Serial.print(" ");
    Serial.print(tau_des);
    Serial.print(" ");
    Serial.println(tau);
    delay(10);
  }
  digitalWrite(ledPin,LOW);
  digitalWrite(redledPin,HIGH);

}

void beep(int dur)
{
  int a_tone = 1136; // a
  //int dur = 500;
  for(long int i=0;i < dur * 1000L; i += a_tone*2)
  {
     digitalWrite(speakerPin,HIGH);
     delayMicroseconds(a_tone);
     digitalWrite(speakerPin,LOW);
     delayMicroseconds(a_tone);
  }
}

void run_experiment(){
  int i = 0;
  int warningDuration = 300;
  for(int j =0; j < NoOfRepetions; j++)
  {
    for(i = 0; i < NumberOfForceElements; i++)
    {
      if( 1 == Debug)
      {
        Serial.print("Force is ");
        Serial.println(ForceArray[i]);
      }
      beep(warningDuration);
      // Match sensor of force set from array
      if( SensorOrArrayInput == 1)
        run_controller(Duration, ForceArray[i]);
      else
        run_controller(Duration, -1);
      delay(PauseDuration);
    }
    delay(PauseDuration);
  }
}

void run_calibration(){
  
  gtime = millis();  
  unsigned long t0=gtime;
  unsigned long acc = 0;
  int firstroundflag = 1;
  int average0, average1;
  int iter=10;
  
  /*
  int a0 = analogRead(flexpin0);
  int a1 = analogRead(flexpin1);
  // Dont start until we have some value
  while( a0 < 5 || a1 < 5 )
  {  
    a0 = analogRead(flexpin0);
    a1 = analogRead(flexpin1);
    delay(500); // Avoid too much error
  }*/
  
  //while( acc < Duration)
  for( int i = 0; i < 150 ; i++)
  {
    // start with 10 measurments
    if ( firstroundflag == 1)
    {
      average0 = 0.1*analogRead(flexpin0);
      average1 = 0.1*analogRead(flexpin1);
      for(int i=1; i < 10;i++)
      {
        average0 =  average0 + 0.1*analogRead(flexpin0);
        average1 =  average1 + 0.1*analogRead(flexpin1);
      }
      firstroundflag = 0;
    }
   //continue with a moving average

    average0 =  0.9*average0 + 0.1*analogRead(flexpin0);
    average1 =  0.9*average1 + 0.1*analogRead(flexpin1);
    
    gtime = millis();
    acc = gtime-t0;
    //Serial.print("C ");
    Serial.print(average0);
    Serial.print(" ");
    Serial.println(average1);
    delay(20);
  }
  Serial.println(" ");
}

void run_measure(){
  
  int m0 = analogRead(flexpin0);
  int m1 = analogRead(flexpin1);
  Serial.print(m0);
  Serial.print(" ");
  Serial.println(m1);
}

void setup_servo()
{
  int pos_out_min;
  int pos_out_max;
  int pos_set = 30;
  int pos_in = 0;
  int pos_in_min = 1000;
  int pos_in_max = -1000;
  
  myservo.attach(servoPin);  // attaches the servo on pin 9 to the servo object
  //int counter = 0;
  //isSending=1;
  //Serial.begin(9600);
  
  // Find the min position
  //Serial.println("Start, go to min pos");
  pos_set = 30;
  myservo.write(pos_set);
  delay(1500);  
  pos_in = analogRead(pospin);
  while( pos_in < pos_in_min )
  {
     pos_in_min = pos_in;
     pos_set = pos_set - 1; //
     myservo.write(pos_set);
     delay(200);
     pos_in = analogRead(pospin);
  }
  pos_out_min = pos_set;
  //Serial.print("Pos out min is: ");
  //Serial.println(pos_out_min);
  //Serial.println("Go to max pos");
  
  // Find the max position
  pos_set = 160;
  myservo.write(pos_set);
  delay(1500);  
  pos_in = analogRead(pospin);
  while( pos_in > pos_in_max )
  {
     pos_in_max = pos_in;
     pos_set = pos_set + 1; //
     myservo.write(pos_set);
     delay(200);
     pos_in = analogRead(pospin);
  }
  pos_out_max = pos_set;
  //Serial.print("Pos out max is: ");
  //Serial.println(pos_out_max);
  
  int val=(pos_in_max-pos_in_min)/2+pos_in_min;
  pos_set = map(val,pos_in_min,pos_in_max,pos_out_min,pos_out_max);
  //Serial.print("Pos_set is: ");
  //Serial.println(pos_set);
  myservo.write(pos_set);
  delay(1500);
  //Serial.println("Ready");
  gtime = millis();
  
}

int intLength(int lvalue)
{
  int valLen = 0;

  if(lvalue > 9999)
    valLen = 5;
  else if(lvalue > 999)
    valLen = 4;
  else if(lvalue > 99)
    valLen = 3;
  else if(lvalue > 9)
    valLen = 2;
  else
    valLen = 1; 
  
  return valLen;

}

// the setup routine runs once when you press reset:
void setup() {

  for(int i = 0; i < 128; i++)
  {
    ForceArray[i] = 0;  
  }
  for(int i = 0; i < NumberOfForceElements; i++)
  {
    ForceArray[i] = i * 0.1;  
  }
  pinMode(speakerPin,OUTPUT);
  pinMode(ledPin,OUTPUT);
  pinMode(redledPin,OUTPUT);  
  digitalWrite(ledPin,LOW);
  digitalWrite(redledPin,LOW);
  pinMode(buttonPin, INPUT);   

  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  reciveValue = 0;
  // Initialze the controller
  setup_servo();
}

// the loop routine runs over and over again forever:
void loop() {

  /*buttonState = digitalRead(buttonPin);
  
  if (buttonState == HIGH) {     
    run_calibration();
  } */
  
  if ( Serial.available() )
  {
    char ch = Serial.read();
    if( ch == ';')
    {
      reciveValue = 0;
      set_value = value;
      value = 0;   
      ready = 1;
      if (floatflag == 1)
      {
	dec = value_float/( pow(10,intLength(value_float) ) );
        floatflag = 0;
      }
      value_float = 0;
    }   
    else if( ch >= '0' && ch <= '9' )
    {
      if(reciveValue == 1)
      {
        if (floatflag == 1)
	  {
	    value_float = (value_float*10)+(ch - '0');	    
	  }
	else
	  {
	    value = (value*10)+(ch - '0');
	  }
      }
    }
    else if( ch == '.' )
    {
      floatflag = 1;
    }
    else if( ch >= 'A' && ch <= 'Z' ) // Capital letter
    {
      command = ch;
      reciveValue = 1;
    }
  }
  if( ready ==  1)
  {
    ready = 0;
    switch (command){
      case 'S':
        RunExp = 1;
        break;
      case 'M':
        RunMeasure = 1;
        break;
      case 'A': // Use force array
        if (SensorOrArrayInput == 0)
          SensorOrArrayInput = 1;
        break;
      case 'a': // Use force sensor
        if (SensorOrArrayInput == 1)
          SensorOrArrayInput = 0;
        break;
      case 'J': // Dummy fuction, just for debugging float
        float dummy; 
        dummy = (float) set_value + dec;
        Serial.print("Float is ");       
        Serial.println(dummy);
        break;
      case 'C': // Dummy fuction, just for debugging float
        RunCal = 1;
        break;
      case 'N':
        NoOfRepetions = set_value;
        if( 1 == Debug)
        {
          Serial.print("NoOfRepetions is ");
          Serial.println(NoOfRepetions);
        }
        break;
      case 'P':
        PauseDuration = set_value;
        if( 1 == Debug)
        {
          Serial.print("PauseDuration is ");
          Serial.println(PauseDuration);
        }
        break;
      case 'D':
        Duration = set_value;
        if( 1 == Debug)
        {
          Serial.print("Duration is ");
          Serial.println(Duration);
        }
        break;
      case 'L':
        Delay = set_value;
        if( 1 == Debug)
        {
          Serial.print("Delay is ");
          Serial.println(Delay);
        }
        break;
      case 'E':
        ElemetIndex = set_value;
        if( 1 == Debug)
        {
          Serial.print("ElemetIndex is ");
          Serial.println(ElemetIndex);
        }
        break;
      case 'F':
        ForceInElement = set_value;
        if( 1 == Debug)
        {
          Serial.print("ForceInElement is ");
          Serial.println(ForceInElement);
        }
        break;
      case 'I':
        NumberOfForceElements = set_value;
        if( 1 == Debug)
        {
          Serial.print("NumberOfForceElements is ");
          Serial.println(NumberOfForceElements);
        }
        break;
      case 'Q':
        Debug = 1;
        Serial.println("Debugging info is turned on");
        break;
      case 'q':
        Debug = 0;
        break;

    }
  }
  if( RunExp == 1)
  {
    run_experiment();
    RunExp = 0;
  }
  if( RunMeasure == 1)
  {
    run_measure();
    RunMeasure = 0;
  }
  if( RunCal == 1)
  {
    run_calibration();
    RunCal = 0;
  }
  
  delay(1);        // delay in between reads for stability
  
}


