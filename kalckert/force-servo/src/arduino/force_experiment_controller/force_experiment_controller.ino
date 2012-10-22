
/*
    
    Force contoller for a sevo motor.
    Copyright (C) 2012 Alexander Skoglund, Karolinska Institute
    
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

 */
//#include <forceservoclass.h>
#include <cominterfaceclass.h>
#include <Servo.h>
#include <Serial.h>


// Analog in pins
int flexpin1 = 1;  // analog pin used to connect the flexiforce
int flexpin0 = 0;  // analog pin used to connect the flexiforce
int pospin = 2;    // analog pin used to connect the position potentiometer
int sliderPin = 3; // analog pin used to connect the slider potentiometer

// Digital IO pins
int speakerPin = 4; // 7
int servoPin = 3;
int greenledPin = 7; // 5
int redledPin = 6; // 4
//int buttonPin = 3;

int fieldIndex = 0;
int value_int; //[NUMBER_OF_FIELDS];
float value_float; //[NUMBER_OF_FIELDS];
char command;
int g_READY = 0;

// Software set min and max position
int pos_in_min;
int pos_in_max;
int pos_in_min_s;
int pos_in_max_s;

// Variable that the user can change
int RunExp = 0;
int RunMeasure = 0;
int RunCal = 0;
int SensorOrArrayInput = 1; // 0 is sensor
int NoOfRepetions = 1;
int Duration = 3000;
int Delay = 0;
int PauseDuration = 1500;
int ElemetIndex = 0;
int ForceInElement = 0;
int NumberOfForceElements = 24;
int ForceArray[128];
int Debug = 0;

ComInterfaceClass comint;
//ForceServoClass fc(servoPin, pospin, flexpin1, 10 );

// The "dynamic" arrays for force position correspondence
int sensor_array[20];
int position_array[20];
int size_array = 0;

// Contoller parameters
Servo myservo;  // create servo object to control a servo 
int pos_set;
int pos_in;
unsigned long gtime;
unsigned long gtimestart;
//unsigned long gtime_old;
unsigned long dt;
//float k_p, k_d;

void beep(int dur)
{
  int a_tone = 1136; // a
  //int dur = 500;
  for(long int i=0;i < dur * 1000L; i += a_tone*2)
  {
     run_measure();
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

      //Serial.print("Force is ");
      //Serial.println(ForceArray[i]);
        
      //fc.attach_servo();
      myservo.attach(servoPin);  // attaches the servo on pin 9 to the servo object
      digitalWrite(redledPin,LOW);
      digitalWrite(greenledPin ,HIGH);
      
      unsigned long t0=millis();
      unsigned long acc = 0;
      int pos = force2pos(ForceArray[i]);
      
      while( acc < Duration)
      { 
        run_measure();
        acc = millis() - t0;
        if( SensorOrArrayInput == 1)
        {
          //fc.run_controller(ForceArray[i]);
          //Serial.print("Force is ");
          //Serial.print(ForceArray[i]);
          //Serial.print("  Pos is ");
          myservo.write(pos);
         
        }
        else
        {
          int tau_des = analogRead(flexpin0);
          //fc.run_controller(tau_des);
        }
        delay(10);
      }
      digitalWrite(greenledPin,LOW);
      digitalWrite(redledPin,HIGH);
      //fc.detach_servo();
      myservo.detach();  // detaches the servo on pin 9 to the servo object
      
      delay(PauseDuration);
      
    }
    delay(PauseDuration);
  }
}

int new_calibration(){
  myservo.attach(servoPin);  // attaches the servo on pin 9 to the servo object
  
  int start_set = 50;
  int go_to_pos;
  myservo.write(start_set);
  
  int tau = analogRead(flexpin1);
  int i = 0;
  // Make contact
  unsigned long t0 = millis();
  while( tau < 50) // Prevent noise
  {
    Serial.print("In while( tau < 1): ");
    go_to_pos = start_set+i;
    if( go_to_pos > 170)
      go_to_pos = 170;
    if( go_to_pos < 10)
      go_to_pos = 10;
    Serial.println(go_to_pos);
    myservo.write(go_to_pos); 
    tau = analogRead(flexpin1);
    //Serial.print("  ");
    //Serial.println(tau);
    delay(15);
    i++;
    if( (millis() - t0) > 4500)
    {
      myservo.write(10);
      Serial.print(millis());
      Serial.print("  ");
      Serial.print(t0);
      Serial.println("  Time out");
      delay(500);
      myservo.detach();
      return -1;
    }
  }
  // Retract 5 steps, should be enough
  go_to_pos = go_to_pos -5;
  start_set = go_to_pos;
  myservo.write(go_to_pos);
  delay(150); // Wait for servo
  
  int stauration = 1023; // Sensor is saturated
  int margin = 50;      // Arbitrary number, derived empirically
  
  int mean_tau = 0;
  
  long tau_acc = 0;
  i = 0;
  int k = 0;
  tau = analogRead(flexpin1);
 
  while( tau < (stauration - margin) )
  {
    //Serial.print("In tau < (stauration - margin): ");
    go_to_pos = start_set+i;
    myservo.write(go_to_pos);
    if( go_to_pos > 170)
      go_to_pos = 170;
    if( go_to_pos < 10)
      go_to_pos = 10;
    Serial.print("Go to pos: ");
    Serial.println(go_to_pos);
    for(int j =0; j < 999; j++)
    {
      tau_acc = tau_acc + analogRead(flexpin1);
      delay(1);
    }
    mean_tau = tau_acc / 1000;
    if( mean_tau > 0 )
    {
       sensor_array[k] = mean_tau;
       position_array[k] = go_to_pos;
       size_array = k;
       k++;
    }
    delay(15);
    tau_acc = 0;
    i++;
    tau = analogRead(flexpin1);
    Serial.print("Tau: ");
    Serial.print(tau);
    Serial.print("  ");
  }
  myservo.write(0);
  delay(1000);
  myservo.detach();  // detaches the servo on pin 9 to the servo object
  
  /*for(i=0; i <= size_array ; i++)
  {
    Serial.print(sensor_array[i]);
    Serial.print("  ");
    Serial.println(position_array[i]);
  }*/
   return 1;
}

int force2pos(int tau)
{
  int pos;
  int i = 0;
  int e0, e1;
  int tau_a = sensor_array[i];
  
  while( tau > tau_a )
  {
    i++;
    tau_a = sensor_array[i];    
    // ERROR: prevent overflow
    if( i > size_array )
      return position_array[size_array]+1;
     
  }
  if( i>0 )
  {  
    e0 = tau - sensor_array[i-1];
    e1 = sensor_array[i] - tau;
    if( e0 < e1 )
      pos = position_array[i-1];
    else
      pos = position_array[i];
  }
  else
  {
    e0 = tau;
    e1 = sensor_array[0] - tau;
    if( e0 < e1 )
      pos = position_array[0]-1;
    else
      pos = position_array[0];
  }

  return pos;
  
}



void run_measure(){
  
  unsigned long t = millis();
  int m0 = analogRead(flexpin0);
  int m1 = analogRead(flexpin1);
  Serial.print(t-gtimestart);
  Serial.print(" ");
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
  
  pos_in_min = 1000;
  pos_in_max = -1000;
  
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
  pos_in_min = pos_in;
  pos_out_min = pos_set;
  //Serial.print("Pos in min is: ");
  //Serial.println(pos_in_min);
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
  pos_in_max = pos_in;
  pos_out_max = pos_set;
  //Serial.print("Pos in max is: ");
  //Serial.println(pos_in_max);
  
  int val=(pos_in_max-pos_in_min)/2+pos_in_min;
  pos_set = map(val,pos_in_min,pos_in_max,pos_out_min,pos_out_max);
  
  // Set Range limits to 20% of full
  double d = sq((double)pos_in_max - (double)pos_in_min);
  int range=(int) sqrt(d); ///2+pos_in_min;
  //Serial.print("range: ");
  //Serial.println(range);
  int mid = range/2+pos_in_max;
  //Serial.print("mid: ");
  //Serial.println(mid);
  range = range*0.1; 
  //Serial.print("range: ");
  //Serial.println(range);
  pos_in_max_s = mid+range; 
  pos_in_min_s = mid-range;
  Serial.print("Pos in max soft is: ");
  Serial.println(pos_in_max_s);
  Serial.print("Pos in min soft is: ");
  Serial.println(pos_in_min_s);
  
  //Serial.print("Pos_set is: ");
  //Serial.println(pos_set);
  myservo.write(pos_set);
  delay(1500);
  //Serial.println("g_READY");
  gtime = millis();
  myservo.detach();  // attaches the servo on pin 9 to the servo object

  // Init gains  
  //k_d = 0.00; //1.7;
  //k_p = 0.012; //0.012; //0.018; // 0.012
}


// the setup routine runs once when you press reset:
void setup() {

  gtimestart = millis();
  Serial.begin(9600);
  //Serial.println("setup");
  for(int i = 0; i < 128; i++)
    ForceArray[i] = 0;  

  // Remove this later...
  for(int i = 0; i < NumberOfForceElements; i++)
    ForceArray[i] = i * 3;  

  
  pinMode(speakerPin,OUTPUT);
  pinMode(greenledPin,OUTPUT);
  pinMode(redledPin,OUTPUT);  
  digitalWrite(greenledPin,LOW);
  digitalWrite(redledPin,LOW);

}

// the loop routine runs over and over again forever:
void loop() {
  
  float dummy;
  g_READY = comint.ReadCommand();
  if( g_READY !=  0)
  {
    command = comint.getChar();
    value_int = comint.getInt();
    value_float = comint.getDouble();
  }
  
  if( g_READY ==  1)
  {
    g_READY = 0;
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
         //dummy; 
        //dummy = (float) set_value + dec;
        Serial.print("Float is ");       
        Serial.println(dummy);
        break;
      case 'C': 
        RunCal = 1;
        break;
      case 'c':
        Serial.print("Desire force ");
        Serial.print(value_int);
        Serial.print(" is  pos ");
        Serial.println( force2pos(value_int) );
        break;
      case 'N':
        NoOfRepetions = value_int; //set_value;
        if( 1 == Debug)
        {
          Serial.print("NoOfRepetions is ");
          Serial.println(NoOfRepetions);
        }
        break;
      case 'P':
        PauseDuration = value_int; //set_value;
        if( 1 == Debug)
        {
          Serial.print("PauseDuration is ");
          Serial.println(PauseDuration);
        }
        break;
      case 'D':
        Duration = value_int; //set_value;
        if( 1 == Debug)
        {
          Serial.print("Duration is ");
          Serial.println(Duration);
        }
        break;
      case 'L':
        Delay = value_int; //set_value;
        if( 1 == Debug)
        {
          Serial.print("Delay is ");
          Serial.println(Delay);
        }
        break;
      case 'E':
        ElemetIndex = value_int; //set_value;
        if( 1 == Debug)
        {
          Serial.print("ElemetIndex is ");
          Serial.println(ElemetIndex);
        }
        break;
      case 'F':
        ForceInElement = value_int; //set_value;
        ForceArray[ElemetIndex] = ForceInElement;
        if( 1 == Debug)
        {
          Serial.print("ForceInElement is ");
          Serial.println(ForceInElement);
        }
        break;
      case 'I':
        NumberOfForceElements = value_int; //set_value;
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
      /*case 'p':
        k_p = (float) value_int + value_float; //set_value;
        if( 1 == Debug)
        {
          Serial.print("k_p is");
          Serial.println(k_p);
        }
        break;
     case 'd':
        k_d = (float) value_int + value_float; //set_value;
        if( 1 == Debug)
        {
          Serial.print("k_d is");
          Serial.println(k_d);
        }
        break;*/
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
    //run_calibration();
    new_calibration();
    RunCal = 0;
  }
  
  delay(1);        // delay in between reads for stability
  
}

// Remove below code... later

/*
void run_calibration(){
  
  gtime = millis();  
  unsigned long t0=gtime;
  unsigned long acc = 0;
  int firstroundflag = 1;
  int average0, average1;
  int iter=10;
  
  
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
*/

/*
void run_controller(int duration, int force_des)
{
  myservo.attach(servoPin);  // attaches the servo on pin 9 to the servo object
  digitalWrite(redledPin,LOW);
  digitalWrite(greenledPin ,HIGH);
  
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
    float e_tau_f = (float)e_tau;
    float u = 3.3*k_p*e_tau_f-k_d*q_dot;
    
    int old_pos_set = pos_set;
    if(  pos_in > pos_in_max_s)
    {
      Serial.println("Over max");
      u = 1;
    }
    if(  pos_in < pos_in_min_s)
    {
      Serial.println("Under min");
      u = -1;
    }
    if( u > 10 )
      u= 10;
    if( u < -10 )
      u= -10;
    if( u > 0.5 | u < -0.5 )
    {
      pos_set = pos_set + (int)u;
      myservo.write(pos_set);
    }

    Serial.print(gtime);
    Serial.print(" ");
    Serial.print(tau_des);
    Serial.print(" ");
    Serial.print(tau);
    Serial.print(" ");
    Serial.println(u);
    
    
    delay(10);
  }
  digitalWrite(greenledPin,LOW);
  digitalWrite(redledPin,HIGH);
  myservo.detach();  // attaches the servo on pin 9 to the servo object
  
}
*/


