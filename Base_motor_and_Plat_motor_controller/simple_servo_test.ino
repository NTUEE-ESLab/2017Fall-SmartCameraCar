#include <Wire.h>
#include <Servo.h>

#define SLAVE_ADDRESS 0x04

int inputDirection = 0;
int forward_backward = 0;
int right_left = 0;
// motor one
int enA = 11;
int in1 = 10;
int in2 = 9;
// motor two
int in3 = 7;
int in4 = 6;
int enB = 5;

Servo myservo;
int degree = 0;
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
  myservo.attach(4);
}

// callback for received data
void receiveData(int byteCount){ 
  while(Wire.available()) {
    inputDirection = Wire.read();
    Serial.print(inputDirection);
    forward_backward = (inputDirection)/3;
    right_left = (inputDirection)%3;
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
void forward()
{
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  analogWrite(enA, moveSpeed-deltaSpeed);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
  analogWrite(enB, moveSpeed);
}
void backward()
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
  for(degree = 0; degree < 180; degree += 1)
  {
    //myservo.write(degree);
    delay(15); 
  } 
  for(degree = 180; degree>=1; degree-=1)
  {
    //myservo.write(degree);
    delay(15);
  } 
}
