#include "mbed.h"
#include "math.h"
#include "MotorDriver.h"
#include <PS3BT.h>
#include <usbhub.h>

#define MAX_DUTY 0.25
#define STICK_DEADBAND 500

Serial pc(USBTX, USBRX, 115200);
DigitalOut LED_0(PA_0);
PwmOut LED_1(PA_6);

//Nucleo f303k8用
USB Usb(D11, D12, D13, A3, A2); // mosi, miso, sclk, ssel, intr
BTD Btd(&Usb);
PS3BT PS3(&Btd);

int Stick_vel_converter(int dead_band,int max_vel,int stick_input){    
    int dead_band_min;
    int dead_band_max;
    int output_data;

    double slope,intercept;

    dead_band_min = 127 - dead_band/2 - 1; 
    dead_band_max = 128 + dead_band/2 - 1;

    slope = max_vel / (255 - dead_band_max);
    intercept = max_vel - slope * 255;

    if(stick_input < dead_band_min){
        output_data = slope * stick_input - max_vel;
    }else if(stick_input > dead_band_max){
        output_data = slope * stick_input + intercept;
    }else{
        output_data = 0;
    }

    return output_data;    
}

void motor_single_stick_mode(){
                 
    float x,y;
                 
    x = (float)PS3.getAnalogHat(RightHatX)/128-1;
    y = 1-(float)PS3.getAnalogHat(RightHatY)/128;
                
    m(MAX_DUTY*(x+y),MAX_DUTY*(y-x));
}

void motor_double_stick_mode(){
    
    int stick_L,stick_R;
    double duty_L,duty_R;
    
    LED_1.period_us(50);
    A_PWM.period_us(50);
    B_PWM.period_us(50);
    
    stick_L = PS3.getAnalogHat(LeftHatY);
    stick_R = PS3.getAnalogHat(RightHatY);
    
    duty_L = fabsf(((double)stick_L/255-0.5)*2*MAX_DUTY);
    duty_R = fabsf(((double)stick_R/255-0.5)*2*MAX_DUTY);
    
    printf("%f %f\r\n",duty_L,duty_R);
    
    A_PWM = duty_L;
    B_PWM = duty_R;
    //LED_1 = duty_R;
    
    if(stick_L >= 128){
        A_IN1 = 1;
        A_IN2 = 0;
    }else{
        A_IN1 = 0;
        A_IN2 = 1;
    }
    
    if(stick_R >= 128){
        B_IN1 = 1;
        B_IN2 = 0;
    }else{
        B_IN1 = 0;
        B_IN2 = 1;
    }
    
}

int main(){
        
    bool printAngle = false;
 
    if (Usb.Init() == -1)
    {
        pc.printf("\r\nOSC did not start");
        while (1); // Halt
    }
    pc.printf("\r\nPS3 USB Library Started");
    
    while(1){
        Usb.Task();
        if (PS3.PS3Connected || PS3.PS3NavigationConnected) {
            //motor_single_stick_mode();
            motor_double_stick_mode();
        }else{
            pc.printf("not connect\r\n");
        }
    }
}


//PS3 controller test
#if 0
int main(){
    if (Usb.Init() == -1){
        pc.printf("\r\nOSC did not start");
        while (1); // Halt
    }
    
    pc.printf("\r\nPS3 USB Library Started");
 
    while (1){
        Usb.Task();
        
        if (PS3.PS3Connected) {
            printf("\n\r%3d %3d %3d %3d",PS3.getAnalogHat(LeftHatX),PS3.getAnalogHat(LeftHatY),PS3.getAnalogHat(RightHatX),PS3.getAnalogHat(RightHatY));
            printf(" %3d %3d %3d %3d",PS3.getAnalogButton(UP),PS3.getAnalogButton(DOWN),PS3.getAnalogButton(RIGHT),PS3.getAnalogButton(LEFT));
            printf(" %3d %3d %3d %3d",PS3.getAnalogButton(TRIANGLE),PS3.getAnalogButton(CIRCLE),PS3.getAnalogButton(CROSS),PS3.getAnalogButton(SQUARE));
            printf(" %3d %3d %3d %3d",PS3.getAnalogButton(L1),PS3.getAnalogButton(R1),PS3.getAnalogButton(L2),PS3.getAnalogButton(R2));
            //printf(" %3d %3d",PS3.getAnalogButton(SELECT),PS3.getAnalogButton(START));
            //printf(" %3d %3d",PS3.getAnalogButton(L3),PS3.getAnalogButton(R3));
            if(PS3.getAnalogButton(CROSS) > 50){
                led = 1;
            }else{
                led = 0;
            }
            
            
        }else{
            pc.printf("not connect\n");
        }
    }
}
#endif