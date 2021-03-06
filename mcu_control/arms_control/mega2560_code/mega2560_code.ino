/*
  file: mega2560_code.ino
  date: 20160218
  description :
 
        this version is for demo 
*/

/*
   Created Or Edited on: 14/12/2015
   Author: Zach
           qoogood1234@gmail.com
*/

#include <ros.h>
#include <std_msgs/Float64.h>
#include <std_msgs/Int16.h>
#include <geometry_msgs/Vector3.h>
#include <Arduino.h>
#include <SoftwareSerial.h>
#include <Metro.h>
#define ARM_RIGHT_A0_A1_EN 1
#define ARM_RIGHT_A2_A3_EN 0
#define ARM_LEFT_A0_A1_EN 0
#define ARM_LEFT_A2_A3_EN 0


int move_flag=1;
 
ros::NodeHandle nh;



//cmd Byte to UNO

struct mySerialSendByte{
  char cS=0;
  byte cSH=0;
  byte cSL=0;
  byte cPH=0;
  byte cPL=0;
  byte cF=0; 
  };
mySerialSendByte mySerialSendByte_0,mySerialSendByte_1,mySerialSendByte_2,mySerialSendByte_3;
char commandArray[12];
  int x;
  int V[5]={};
struct mySerialReceiveByte{
  char rS=0;
  char rS2=0;
  byte rV0H=0;
  byte rV0L=0;
  byte rV1H=0;
  byte rV1L=0;
  byte rV2H=0;
  byte rV2L=0;
  byte rV3H=0;
  byte rV3L=0;
  byte rV4H=0;
  byte rV4L=0;
  char rF=0; 
  char rF2=0;
  };
mySerialReceiveByte mySerialReceiveByte_0,mySerialReceiveByte_1,mySerialReceiveByte_2,mySerialReceiveByte_3;


double dT=0.02;
double rad_tick=(double)6.28/32767; //resolution
double degree_tick=(double)360/32767; //resolution
double increment_angle_0=0.2;//max is 0.3
double increment_angle_1=0.2;

double angle_Axis_0=0;
double angle_Axis_1=0;


#if(ARM_RIGHT_A0_A1_EN || ARM_LEFT_A0_A1_EN)
double gearRatio_Axis_0=5;
double gearRatio_Axis_1=3;
#endif

#if(ARM_RIGHT_A2_A3_EN || ARM_LEFT_A2_A3_EN)
double gearRatio_Axis_0=2.5;
double gearRatio_Axis_1=3;
#endif

double cmd_Axis_0=0;
double cmd_Axis_1=0;
double cmd_Axis_1_by_0=0;
double cmd_Axis_1_by_1=0;


double cmd_Motor_0=0;
double cmd_Motor_1=0,cmd_Motor_1_by_0=0,cmd_Motor_1_by_1=0;


double cmd_Motor_0_increment_by_0=0;
double cmd_Motor_1_increment_by_0=0,cmd_Motor_1_increment_by_1=0;



double offset_v_0=0;
double offset_v_1=0;

void messageCallBackHomeArm0(const std_msgs::Float64& msg)
{
    offset_v_0=(int)(msg.data*gearRatio_Axis_0*(double)360/6.28)/0.01;
  
}

void messageCallBackHomeArm1(const std_msgs::Float64& msg)
{
    offset_v_1=(int)(msg.data*gearRatio_Axis_1*(double)360/6.28)/0.01;
  
}



void messageCallBackArm0(const geometry_msgs::Vector3 &msg)
{ 
    //angle_Axis_0=(int)((double)(msg.data/rad_tick)/degree_tick);   
    angle_Axis_0=(int)(msg.x*(double)360/6.28);  
    double T_0=msg.y;
    double L=abs(angle_Axis_0-cmd_Axis_0);
    int N=(int)T_0/dT;
    
    increment_angle_0=((double)L/N>0.2)? 0.2:(double)L/N;
  
}
void messageCallBackArm1(const geometry_msgs::Vector3 &msg)
{
    //angle_Axis_1=(int)((double)(msg.data/rad_tick)/degree_tick);   
    angle_Axis_1=(int)(msg.x*(double)360/6.28);  
    double T_1=msg.y;
    double L=abs(angle_Axis_1-cmd_Axis_1_by_1);
    int N=(int)T_1/dT;
    
    increment_angle_1=((double)L/N>0.2)? 0.2:(double)L/N;
}

  std_msgs::Int16 state_0;
  std_msgs::Int16 state_1;

  std_msgs::Float64 angle_0;
  std_msgs::Float64 angle_1;

  std_msgs::Float64 cmd_0;
  std_msgs::Float64 cmd_1;

  std_msgs::Float64 deb_0;
  std_msgs::Float64 deb_1;
 
#if(ARM_RIGHT_A0_A1_EN)
  ros::Subscriber<std_msgs::Float64> sub_offset_v_0("andbot/joint/R0/cmd/offset_v",messageCallBackHomeArm0);
  ros::Subscriber<std_msgs::Float64> sub_offset_v_1("andbot/joint/R1/cmd/offset_v",messageCallBackHomeArm1);
  ros::Subscriber<geometry_msgs::Vector3> sub_pos_0("andbot/joint/R0/cmd/position",messageCallBackArm0);
  ros::Subscriber<geometry_msgs::Vector3> sub_pos_1("andbot/joint/R1/cmd/position",messageCallBackArm1);
  ros::Publisher pub_state_0("andbot/joint/R0/feedback/state",&state_0);
  ros::Publisher pub_state_1("andbot/joint/R1/feedback/state",&state_1);
  ros::Publisher pub_angle_0("andbot/joint/R0/feedback/angle",&angle_0);
  ros::Publisher pub_angle_1("andbot/joint/R1/feedback/angle",&angle_1);
  ros::Publisher pub_cmd_0("andbot/joint/R0/feedback/cmd",&cmd_0);
  ros::Publisher pub_cmd_1("andbot/joint/R1/feedback/cmd",&cmd_1); 
#endif
#if(ARM_RIGHT_A2_A3_EN)
  ros::Subscriber<std_msgs::Float64> sub_offset_v_0("andbot/joint/R2/cmd/offset_v",messageCallBackHomeArm0);
  ros::Subscriber<std_msgs::Float64> sub_offset_v_1("andbot/joint/R3/cmd/offset_v",messageCallBackHomeArm1);
  ros::Subscriber<geometry_msgs::Vector3> sub_pos_0("andbot/joint/R2/cmd/position",messageCallBackArm0);
  ros::Subscriber<geometry_msgs::Vector3> sub_pos_1("andbot/joint/R3/cmd/position",messageCallBackArm1);   
  ros::Publisher pub_state_0("andbot/joint/R2/feedback/state",&state_0);
  ros::Publisher pub_state_1("andbot/joint/R3/feedback/state",&state_1);
  ros::Publisher pub_angle_0("andbot/joint/R2/feedback/angle",&angle_0);
  ros::Publisher pub_angle_1("andbot/joint/R3/feedback/angle",&angle_1);
  ros::Publisher pub_cmd_0("andbot/joint/R2/feedback/cmd",&cmd_0);
  ros::Publisher pub_cmd_1("andbot/joint/R3/feedback/cmd",&cmd_1);  
#endif
#if(ARM_LEFT_A0_A1_EN)
  ros::Subscriber<std_msgs::Float64> sub_offset_v_0("andbot/joint/L0/cmd/offset_v",messageCallBackHomeArm0);
  ros::Subscriber<std_msgs::Float64> sub_offset_v_1("andbot/joint/L1/cmd/offset_v",messageCallBackHomeArm1);
  ros::Subscriber<geometry_msgs::Vector3> sub_pos_0("andbot/joint/L0/cmd/position",messageCallBackArm0);
  ros::Subscriber<geometry_msgs::Vector3> sub_pos_1("andbot/joint/L1/cmd/position",messageCallBackArm1);
  ros::Publisher pub_state_0("andbot/joint/L0/feedback/state",&state_0);
  ros::Publisher pub_state_1("andbot/joint/L1/feedback/state",&state_1);
  ros::Publisher pub_angle_0("andbot/joint/L0/feedback/angle",&angle_0);
  ros::Publisher pub_angle_1("andbot/joint/L1/feedback/angle",&angle_1);
  ros::Publisher pub_cmd_0("andbot/joint/L0/feedback/cmd",&cmd_0);
  ros::Publisher pub_cmd_1("andbot/joint/L1/feedback/cmd",&cmd_1); 
#endif
#if(ARM_LEFT_A2_A3_EN)
  ros::Subscriber<std_msgs::Float64> sub_offset_v_0("andbot/joint/L2/cmd/offset_v",messageCallBackHomeArm0);
  ros::Subscriber<std_msgs::Float64> sub_offset_v_1("andbot/joint/L3/cmd/offset_v",messageCallBackHomeArm1);
  ros::Subscriber<geometry_msgs::Vector3> sub_pos_0("andbot/joint/L2/cmd/position",messageCallBackArm0);
  ros::Subscriber<geometry_msgs::Vector3> sub_pos_1("andbot/joint/L3/cmd/position",messageCallBackArm1);   
  ros::Publisher pub_state_0("andbot/joint/L2/feedback/state",&state_0);
  ros::Publisher pub_state_1("andbot/joint/L3/feedback/state",&state_1);
  ros::Publisher pub_angle_0("andbot/joint/L2/feedback/angle",&angle_0);
  ros::Publisher pub_angle_1("andbot/joint/L3/feedback/angle",&angle_1);
  ros::Publisher pub_cmd_0("andbot/joint/L2/feedback/cmd",&cmd_0);
  ros::Publisher pub_cmd_1("andbot/joint/L3/feedback/cmd",&cmd_1);  
#endif
int i=0;

Metro mainTimer = Metro(1000*dT); //milisecond
//Metro feedbackTimer = Metro(1); //milisecond
void setup() { 
 nh.initNode();
 
 nh.subscribe(sub_offset_v_0);
 nh.subscribe(sub_offset_v_1);

 nh.subscribe(sub_pos_0);
 nh.subscribe(sub_pos_1);

 nh.advertise(pub_state_0);
 nh.advertise(pub_state_1);

 nh.advertise(pub_angle_0);
 nh.advertise(pub_angle_1);

 nh.advertise(pub_cmd_0);
 nh.advertise(pub_cmd_1);

 
 Serial.begin(57600);
 Serial1.begin(57600);
 Serial2.begin(57600);
 
} 

void loop(){

  if (mainTimer.check() == true){
    increase_0();
    increase_1();

    //cmd_Motor_0
    cmd_Motor_0= cmd_Motor_0 + cmd_Motor_0_increment_by_0;
    //cmd_Axis_0
    cmd_Axis_0=(double)cmd_Motor_0/gearRatio_Axis_0;
    
    //cmd_Motor_1
    if(ARM_RIGHT_A0_A1_EN||ARM_LEFT_A0_A1_EN)
      cmd_Motor_1_by_0=cmd_Motor_1_by_0- cmd_Motor_1_increment_by_0;    
    else if(ARM_RIGHT_A2_A3_EN||ARM_LEFT_A2_A3_EN)
      cmd_Motor_1_by_0=cmd_Motor_1_by_0+ cmd_Motor_1_increment_by_0;    
    cmd_Motor_1_by_1=cmd_Motor_1_by_1+ cmd_Motor_1_increment_by_1;  
    cmd_Motor_1=cmd_Motor_1_by_0+cmd_Motor_1_by_1;
    //cmd_Axis_1
    cmd_Axis_1_by_0=(double)cmd_Motor_1_by_0/ gearRatio_Axis_1;
    cmd_Axis_1_by_1=(double)cmd_Motor_1_by_1/ gearRatio_Axis_1;
    cmd_Axis_1=cmd_Axis_1_by_0+cmd_Axis_1_by_1;    

    cmd_serial_to_uno();
    nh.spinOnce();
  }  
  if (1) {  
  get_cmd_pos_0();
  get_cmd_pos_1();

  }
  //show();
}

void show()
  {
    Serial.print(" ");Serial.print(angle_Axis_0);
    Serial.print(" ");Serial.print(cmd_Axis_0);
    Serial.print(" ");Serial.print(cmd_Motor_0);
    Serial.print(" ");Serial.print(cmd_Motor_0_increment_by_0);

Serial.print("       ");
    Serial.print(" ");Serial.print(angle_Axis_1);
    Serial.print(" ");Serial.print(cmd_Axis_1);    
    Serial.print(" ");Serial.print(cmd_Axis_1_by_0);
    Serial.print(" ");Serial.print(cmd_Axis_1_by_1);
    Serial.print(" ");Serial.print(cmd_Motor_1);
//    Serial.print(" ");Serial.print(cmd_Motor_1_by_0);
//    Serial.print(" ");Serial.print(cmd_Motor_1_by_1);
//    Serial.print(" ");Serial.print(cmd_Motor_1_increment_by_0);
//    Serial.print(" ");Serial.print(cmd_Motor_1_increment_by_1);
    Serial.println(" ");    
    }

    
void increase_0()
{

  if(abs(cmd_Axis_0-angle_Axis_0)>0.01)
  {
    if((angle_Axis_0-cmd_Axis_0)>0){
      cmd_Motor_0_increment_by_0=increment_angle_0*gearRatio_Axis_0;
      if(ARM_RIGHT_A0_A1_EN)
        cmd_Motor_1_increment_by_0=increment_angle_0;//*gearRatio_Axis_1*(double)1/gearRatio_Axis_1; 
      else
        cmd_Motor_1_increment_by_0=-increment_angle_0;//*gearRatio_Axis_1*(double)1/gearRatio_Axis_1; 
    }
    else {
      cmd_Motor_0_increment_by_0=-increment_angle_0*gearRatio_Axis_0;
      if(ARM_RIGHT_A0_A1_EN)
        cmd_Motor_1_increment_by_0=-increment_angle_0;//*gearRatio_Axis_1*(double)1/gearRatio_Axis_1; 
      else
        cmd_Motor_1_increment_by_0=increment_angle_0;//*gearRatio_Axis_1*(double)1/gearRatio_Axis_1; 
    }
  }
  else{
    cmd_Motor_0_increment_by_0=0;
    cmd_Motor_1_increment_by_0=0;
    
    }
    
}

void increase_1()
{
  if(abs(cmd_Axis_1_by_1-angle_Axis_1)>0.01)
  {
    if((angle_Axis_1-cmd_Axis_1_by_1)>0){
      cmd_Motor_1_increment_by_1=increment_angle_1*gearRatio_Axis_1; 
    }
    else {
      cmd_Motor_1_increment_by_1=-increment_angle_1*gearRatio_Axis_1; 
    }
  }
  else{
    cmd_Motor_1_increment_by_1=0;
    
    }
}



void cmd_serial_to_uno(){

    Serial.print("cmd_Motor_0 ");Serial.print(cmd_Motor_0); 
    Serial.print("cmd_Motor_1 ");Serial.println(cmd_Motor_1);
    
    mySerialSendByte_0.cS='{';
    mySerialSendByte_0.cSH =highByte(int(offset_v_0));
    mySerialSendByte_0.cSL =lowByte(int(offset_v_0));
    mySerialSendByte_0.cPH =highByte(int(cmd_Motor_0));
    mySerialSendByte_0.cPL =lowByte(int(cmd_Motor_0));
    mySerialSendByte_0.cF='}'; 
    Serial1.write(mySerialSendByte_0.cS);Serial1.write(mySerialSendByte_0.cSH);Serial1.write(mySerialSendByte_0.cSL);Serial1.write(mySerialSendByte_0.cPH);Serial1.write(mySerialSendByte_0.cPL);Serial1.write(mySerialSendByte_0.cF);
    
    mySerialSendByte_1.cS='{';
    mySerialSendByte_1.cSH =highByte(int(offset_v_1));
    mySerialSendByte_1.cSL =lowByte(int(offset_v_1));
    mySerialSendByte_1.cPH =highByte(int(cmd_Motor_1));
    mySerialSendByte_1.cPL =lowByte(int(cmd_Motor_1));
    mySerialSendByte_1.cF='}';
    Serial2.write(mySerialSendByte_1.cS);Serial2.write(mySerialSendByte_1.cSH);Serial2.write(mySerialSendByte_1.cSL);Serial2.write(mySerialSendByte_1.cPH);Serial2.write(mySerialSendByte_1.cPL);Serial2.write(mySerialSendByte_1.cF);


  }

void get_cmd_pos_0() {
    if (Serial1.available()>0){
      int cmd=Serial1.read();
      if(cmd=='{') {
      byte tmp1[9]={};
      Serial1.readBytes(tmp1,9);
      int value0=(tmp1[0]<<8)+tmp1[1];
      int value1=(tmp1[2]<<8)+tmp1[3];
      int value2=(tmp1[4]<<8)+tmp1[5];
      int cksum=(tmp1[6]<<8)+tmp1[7];
      if(tmp1[8]=='}'&& cksum==value0+value1+value2){
             state_0.data=value0;
             angle_0.data=(double)value1*rad_tick;
             cmd_0.data=(double)value2*rad_tick;             
             pub_state_0.publish(&state_0);
             pub_angle_0.publish(&angle_0);
             pub_cmd_0.publish(&cmd_0);   
        //Serial.print("0");Serial.print(" ");Serial.print(value0);Serial.print(" ");Serial.print(value1);Serial.print(" ");Serial.print(value2);Serial.println("      ");//Serial.println(cksum);
        }
      }
    }
}
void get_cmd_pos_1() {
  //Serial.println(mySerial_0.read());
    if (Serial2.available()>0){
      int cmd=Serial2.read();
      if(cmd=='{') {
      byte tmp1[9]={};
      Serial2.readBytes(tmp1,9);
      int value0=(tmp1[0]<<8)+tmp1[1];
      int value1=(tmp1[2]<<8)+tmp1[3];
      int value2=(tmp1[4]<<8)+tmp1[5];
      int cksum=(tmp1[6]<<8)+tmp1[7];
      if(tmp1[8]=='}'&& cksum==value0+value1+value2){
             state_1.data=value0;
             angle_1.data=(double)value1*rad_tick;
             cmd_1.data=(double)value2*rad_tick;             
             pub_state_1.publish(&state_1);
             pub_angle_1.publish(&angle_1);
             pub_cmd_1.publish(&cmd_1); 
        //Serial.print("1");Serial.print(" ");Serial.print(value0);Serial.print(" ");Serial.print(value1);Serial.print(" ");Serial.println(value2);//Serial.print(" ");Serial.println(cksum);
        }
      }
    }
}





