// GPIO mappings for Arduino Mega 2560
//#######################################
int m1_VR=3;    //VR 
int m1_Signal_hall=5;   // Signal - Hall sensor
int m1_ZF_Direction=4;  // ZF 
int m1_EL_Start_Stop=6;  //EL 

int m2_VR=9;    //VR
int m2_Signal_hall=11;   // Signal - Hall sensor
int m2_ZF_Direction=10;  // ZF 
int m2_EL_Start_Stop=12;  //EL 

long nowTime = millis();
long testMotorTime = millis();
//#######################################
//#######################################
// Direction Defining
#define FORWARD 1
#define BACKWARD 2
#define POINT_TURN_LEFT 3
#define POINT_TURN_RIGHT 4
#define STOP 5
#define NOSIGNAL 0

#define INIT_SPEED 20
#define MAX_SPEED 40

#define JOYSTICK_MODE 1
#define SERIAL_MODE 2

//#######################################
int pos=0;int steps=0;int speed1=0;
String direction1;
int prevDirection = STOP;
int nowDirection = STOP;
int d_state_1 = 1;
int d_state_2 = 1;

char serialBuf;
int set_flag;
//#######################################
//#######################################
void plus() {
  pos++; //count steps
  //Serial.println(pos);
    if(pos>=steps){
    wheelStop();
    pos=0;
  }
}

struct MOTORSPEED {
  int m1_VR_Speed;
  int m2_VR_Speed;
};


void wheelStop(){
  digitalWrite(m1_EL_Start_Stop,HIGH);
  digitalWrite(m2_EL_Start_Stop,HIGH);
}

void Left_Forward(){
      analogWrite(m1_VR, speed1);
      digitalWrite(m1_EL_Start_Stop,LOW);
      d_state_1 = HIGH;
      digitalWrite(m1_ZF_Direction,d_state_1);
}

void Right_Forward(){
      analogWrite(m2_VR, speed1);
      digitalWrite(m2_EL_Start_Stop,LOW);
      d_state_2 = LOW;
      digitalWrite(m2_ZF_Direction,d_state_2);   
}

void Left_Backward(){
      analogWrite(m1_VR, speed1);
      digitalWrite(m1_EL_Start_Stop,LOW);
      d_state_1 = LOW;
      digitalWrite(m1_ZF_Direction,d_state_1); 
}

void Right_Backward(){
      analogWrite(m2_VR, speed1);
      digitalWrite(m2_EL_Start_Stop,LOW);
      d_state_2 = HIGH;
      digitalWrite(m2_ZF_Direction,d_state_2);
}

void printLog(int joy_X, int joy_Y) {
  if(nowTime + 200 < millis()) {
    nowTime = millis();
    /*
    Serial.print("joy_X : ");
    Serial.print(joy_X);
    Serial.print(" --- ");
    Serial.print("joy_Y : ");
    Serial.println(joy_Y);
    */
    Serial.println(prevDirection);
  }
}

int adjustJoystickValue(int value) {
}

void setup(){
    Serial.begin(115200); //USB Serial initialize

    //wheel 1 - Setup pin mode
    pinMode(m1_EL_Start_Stop, OUTPUT);//stop/start - EL 
    pinMode(m1_Signal_hall, INPUT);   //plus       - Signal  
    pinMode(m1_ZF_Direction, OUTPUT); //direction  - ZF 
    
    //Hall sensor detection - Count steps
    attachInterrupt(digitalPinToInterrupt(m1_Signal_hall), plus, CHANGE);

    pinMode(m2_EL_Start_Stop, OUTPUT);//stop/start - EL 
    pinMode(m2_Signal_hall, INPUT);   //plus       - Signal  
    pinMode(m2_ZF_Direction, OUTPUT); //direction  - ZF 
    
    //Hall sensor detection - Count steps
    attachInterrupt(digitalPinToInterrupt(m2_Signal_hall), plus, CHANGE);
    MOTORSPEED motor;
    motor.m1_VR_Speed = 0;
    motor.m2_VR_Speed = 0;

}

void moveRobot_Joystick() {
  int joy_X = map(analogRead(A0),0, 1023, 100, -100); // reverse x 
  int joy_Y = map(analogRead(A1),0, 1023, -100, 100);
  printLog(joy_X, joy_Y);

  if (joy_Y >= 80 && joy_X <= 20 && joy_X >= -20) // forward
  {
    if(prevDirection == FORWARD) {
      if(testMotorTime + 200 < millis() && speed1 <= MAX_SPEED) {
        testMotorTime = millis();
        speed1++;
      }
    }
    else {
      speed1 = INIT_SPEED;
    }
    Left_Forward();
    Right_Forward();
    digitalWrite(m1_EL_Start_Stop,LOW);
    digitalWrite(m2_EL_Start_Stop,LOW);
    analogWrite(m1_VR, speed1);
    analogWrite(m2_VR, speed1);
    prevDirection = FORWARD;
  }
  else if (joy_Y <= -80 && joy_X <= 20 && joy_X >= -20) // backward
  {
    if(prevDirection == BACKWARD) {
      if(testMotorTime + 200 < millis() && speed1 <= MAX_SPEED) {
        testMotorTime = millis();
        speed1++;
      }
    }
    else {
      speed1 = INIT_SPEED;
    }
    Left_Backward();
    Right_Backward();
    digitalWrite(m1_EL_Start_Stop,LOW);
    digitalWrite(m2_EL_Start_Stop,LOW);
    analogWrite(m1_VR, speed1);
    analogWrite(m2_VR, speed1);
    prevDirection = BACKWARD;
  }
  else if (joy_X >= 80 && joy_Y <= 20 && joy_Y >= -20) // point turn right
  {
    if(prevDirection == POINT_TURN_RIGHT) {
      if(testMotorTime + 200 < millis() && speed1 <= MAX_SPEED) {
        testMotorTime = millis();
        speed1++;
      }
    }
    else {
      speed1 = INIT_SPEED;
    }
    Left_Forward();
    Right_Backward();
    digitalWrite(m1_EL_Start_Stop,LOW);
    digitalWrite(m2_EL_Start_Stop,LOW);
    analogWrite(m1_VR, speed1);
    analogWrite(m2_VR, speed1);
    prevDirection = POINT_TURN_RIGHT;
  }
  else if (joy_X <= -80 && joy_Y <= 20 && joy_Y >= -20) // point turn left
  {
    if(prevDirection == POINT_TURN_LEFT) {
      if(testMotorTime + 200 < millis() && speed1 <= MAX_SPEED) {
        testMotorTime = millis();
        speed1++;
      }
    }
    else {
      speed1 = INIT_SPEED;
    }
    Left_Backward();
    Right_Forward();
    digitalWrite(m1_EL_Start_Stop,LOW);
    digitalWrite(m2_EL_Start_Stop,LOW);
    analogWrite(m1_VR, speed1);
    analogWrite(m2_VR, speed1);
    prevDirection = POINT_TURN_LEFT;
  }
  //else if (joy_X >= 80 && joy_Y >= 80)
  //{
    ////direction : num9
  //}
  //else if (joy_X <= -80 && joy_Y >= 80)
  //{
    ////direction : num7
  //}
  //else if (joy_X >= 80 && joy_Y <= -80)
  //{
    ////direction : num3
  //}
  //else if (joy_X <= -80 && joy_Y <= -80)
  //{
    ////direction : num1
  //}
  else
  {
    //stop
    wheelStop();
    prevDirection = STOP;
  }
}

void moveRobot_Serial() {
  if(Serial.available()) {
    serialBuf = Serial.read();
    switch(serialBuf) {
      case '1':
        nowDirection = FORWARD;
        Serial.println("move forward");
        break;
      case '2':
        nowDirection = BACKWARD;
        Serial.println("move backward");
        break;
      case '3':
        nowDirection = POINT_TURN_LEFT;
        Serial.println("point turn left");
        break;
      case '4':
        nowDirection = POINT_TURN_RIGHT;
        Serial.println("point turn right");
        break;
      case '5':
        nowDirection = STOP;
        Serial.println("stop");
        break;
      default:
        break;
    }
    serialBuf ='\0';
  }

  if (nowDirection == FORWARD) // forward
  {
    if(prevDirection == FORWARD) {
      if(testMotorTime + 200 < millis() && speed1 <= MAX_SPEED) {
        testMotorTime = millis();
        speed1++;
      }
    }
    else {
      speed1 = INIT_SPEED;
    }
    Left_Forward();
    Right_Forward();
    digitalWrite(m1_EL_Start_Stop,LOW);
    digitalWrite(m2_EL_Start_Stop,LOW);
    analogWrite(m1_VR, speed1);
    analogWrite(m2_VR, speed1);
    prevDirection = FORWARD;
  }
  else if (nowDirection == BACKWARD) // backward
  {
    if(prevDirection == BACKWARD) {
      if(testMotorTime + 200 < millis() && speed1 <= MAX_SPEED) {
        testMotorTime = millis();
        speed1++;
      }
    }
    else {
      speed1 = INIT_SPEED;
    }
    Left_Backward();
    Right_Backward();
    digitalWrite(m1_EL_Start_Stop,LOW);
    digitalWrite(m2_EL_Start_Stop,LOW);
    analogWrite(m1_VR, speed1);
    analogWrite(m2_VR, speed1);
    prevDirection = BACKWARD;
  }
  else if (nowDirection == POINT_TURN_RIGHT) // point turn right
  {
    if(prevDirection == POINT_TURN_RIGHT) {
      if(testMotorTime + 200 < millis() && speed1 <= MAX_SPEED) {
        testMotorTime = millis();
        speed1++;
      }
    }
    else {
      speed1 = INIT_SPEED;
    }
    Left_Forward();
    Right_Backward();
    digitalWrite(m1_EL_Start_Stop,LOW);
    digitalWrite(m2_EL_Start_Stop,LOW);
    analogWrite(m1_VR, speed1);
    analogWrite(m2_VR, speed1);
    prevDirection = POINT_TURN_RIGHT;
  }
  else if (nowDirection == POINT_TURN_LEFT) // point turn left
  {
    if(prevDirection == POINT_TURN_LEFT) {
      if(testMotorTime + 200 < millis() && speed1 <= MAX_SPEED) {
        testMotorTime = millis();
        speed1++;
      }
    }
    else {
      speed1 = INIT_SPEED;
    }
    Left_Backward();
    Right_Forward();
    digitalWrite(m1_EL_Start_Stop,LOW);
    digitalWrite(m2_EL_Start_Stop,LOW);
    analogWrite(m1_VR, speed1);
    analogWrite(m2_VR, speed1);
    prevDirection = POINT_TURN_LEFT;
  }
  else
  {
    //stop
    wheelStop();
    prevDirection = STOP;
  }
}

void loop() {
  // put your main code here, to run repeatedly:

  moveRobot_Serial();

 

}
