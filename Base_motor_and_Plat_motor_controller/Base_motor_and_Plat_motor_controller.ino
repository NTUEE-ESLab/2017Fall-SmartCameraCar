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
int forward_backward = 0;
int right_left = 0;

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
double Setpoint, Input, Output;
float Kp = 0.1;
float Ki = 0.0;
float Kd = 0.1;

//PID
PID myPID(&Input, &Output, &Setpoint, Kp, Ki, Kd, DIRECT);;

//parameters
int degree = 90;
int deltaSpeed = 40;
int moveSpeed = 150;
int rotateSpeed = 90;
int servoSpeed = 30;


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
  Setpoint = 1;  //dont care in our case
  Input = 0;
  myPID.SetMode(AUTOMATIC);
}

// callback for received data
void receiveData(int byteCount){
  while(Wire.available()) {
    int inputData = Wire.read();
    Serial.print(inputCount);
    Serial.print(' ');
    Serial.print(inputData);
    Serial.println();
    
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
      forward_backward = (inputDirection)/3;
      right_left = (inputDirection)%3;
      Serial.println();
    }
  }
}
void hardstop()
{
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  analogWrite(enA, 0);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
  analogWrite(enB, 0);
}
void backward()
{
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  analogWrite(enA, moveSpeed-deltaSpeed);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
  analogWrite(enB, moveSpeed);
}
void forward()
{
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  analogWrite(enA, moveSpeed-deltaSpeed);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
  analogWrite(enB, moveSpeed);
}
void left()
{
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  // set speed to 200 out of possible range 0~255
  analogWrite(enA, rotateSpeed);
  // turn on motor B
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
  // set speed to 200 out of possible range 0~255
  analogWrite(enB, rotateSpeed);
}
void right()
{
  // this function will run the motors in both directions at a fixed speed
  // turn on motor A
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  // set speed to 200 out of possible range 0~255
  analogWrite(enA, rotateSpeed);
  // turn on motor B
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
  // set speed to 200 out of possible range 0~255
  analogWrite(enB, rotateSpeed);
}
void loop()
{
    myservo.write(97);
    
    if (right_left == 1)
    {
      left ();
    }
    else if (right_left == 2)
    {
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
