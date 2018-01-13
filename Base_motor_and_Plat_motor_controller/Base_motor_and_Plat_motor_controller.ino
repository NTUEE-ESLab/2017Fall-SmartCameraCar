#include <Wire.h>
#include <Servo.h>
#include <PID_v1.h>

#define SLAVE_ADDRESS 0x04

//I2C input
int input[3];
int inputDirection = 0;
int inputVerticalSpeed = 0;
int inputHorizontalSpeed = 0;
int inputCount = 0;
int inputMode = 0;
int forward_backward = 0;
int right_left = 0;
bool first = true;

//L298n
// motor one
int enA = 11;
int in1 = 10;
int in2 = 9;
// motor two
int in3 = 7;
int in4 = 6;
int enB = 5;

//servo
Servo myservo;

//Define Variables we'll be connecting to
//double Setpoint, Input, Output;
//float Kp = 0.5;
//float Ki = 0.0;
//float Kd = 0.0;

//PID
//PID myPID(&Input, &Output, &Setpoint, Kp, Ki, Kd, DIRECT);;

//parameters
//motor
int minmoveSpeed = 140;
int moveSpeed = 140;
int minrotateSpeed = 90;
int rotateSpeed = 110;
int deltaSpeed = 10;
int SurroundmoveSpeed = 120;
int SurrounddeltaSpeed = 70;
//threshold
int verticalThreshold = 100;
int horizontalThreshold = 100;

//servo
int degree = 100;
int servoSpeed = 100;



void setup()
{
  // 車體馬達
  pinMode(enA, OUTPUT);
  pinMode(enB, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);

  // I2C to RPi
  pinMode(13, OUTPUT);
  Serial.begin(9600);
  Wire.begin(SLAVE_ADDRESS);
  Wire.onReceive(receiveData);

  // 雲台馬達
  myservo.attach(12);
  
  //pid
  //Setpoint = 0;  //dont care in our case
  //Input = 0;
  //myPID.SetMode(AUTOMATIC);
}

// callback for received data
void receiveData(int byteCount){
  while(Wire.available()) {
    int inputData = Wire.read();
    //Serial.print(inputCount);
    //Serial.print(' ');
    //Serial.print(inputData);
    //Serial.println();
    
    if (inputData == 255)
    {
      inputCount = 0;
    }
    input[inputCount] = inputData;
    inputCount++;
    if (inputCount == 4)
    {
      inputCount = 0;
      inputDirection = input[1];
      inputVerticalSpeed = input[2];
      inputHorizontalSpeed = input[3];
      inputMode = (inputDirection)/9;
      forward_backward = ((inputDirection)/3)%3;
      right_left = (inputDirection)%3;
      first = true;
    }
  }
}
void hardstop()
{
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
  analogWrite(enA, 0);
  analogWrite(enB, 0);
}
void backward()
{
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
  analogWrite(enA, moveSpeed-deltaSpeed);
  analogWrite(enB, moveSpeed);
}
void forward()
{
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
  analogWrite(enA, moveSpeed-deltaSpeed);
  analogWrite(enB, moveSpeed);
}
void left()
{
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
  analogWrite(enA, rotateSpeed);
  analogWrite(enB, rotateSpeed);
}
void right()
{
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
  analogWrite(enA, rotateSpeed);
  analogWrite(enB, rotateSpeed);
}
void surround ()
{
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  analogWrite(enA, moveSpeed-deltaSpeed);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
  analogWrite(enB, moveSpeed+SurrounddeltaSpeed);
}
void loop()
{
  if (inputMode == 0) // follow mode
  {
    myservo.write(98);
    //moveSpeed = (inputVerticalSpeed < verticalThreshold)? minmoveSpeed+(255-minmoveSpeed)*inputVerticalSpeed/verticalThreshold : 255;
    //rotateSpeed = (inputHorizontalSpeed < horizontalThreshold)? minrotateSpeed+(255-minrotateSpeed)*inputHorizontalSpeed/horizontalThreshold : 255;
    //Serial.print(inputDirection);
    //Serial.print(' ');
    //Serial.print(moveSpeed);
    //Serial.print(' ');
    //Serial.print(rotateSpeed);
    //Serial.println();
    
    
    if (right_left == 1)
    {
      if (first)
      {
        for (int i = 0; i < rotateSpeed; ++i)
        {
          analogWrite(enA, i);
          analogWrite(enB, i);
          delay (10);
        }
      }
      first = false;
      left ();
    }
    else if (right_left == 2)
    {
      if (first)
      {
        for (int i = 0; i < rotateSpeed; ++i)
        {
          analogWrite(enA, i);
          analogWrite(enB, i);
          delay (10);
        }
      }
      first = false;
      right ();
    }
    else if (forward_backward == 1)
    {
      forward ();
    }
    else if (forward_backward == 2)
    {
      backward ();
    }
    else
    {
      hardstop();
    }
  }
  else if (inputMode == 1) // surround mode
  {
    
    myservo.write(8);
    //surround();
  }
  else if (inputMode == 2) // 360 mode
  {
    rotateSpeed = minrotateSpeed;
    right ();
  }
  else
  {
    hardstop();
  }
}
