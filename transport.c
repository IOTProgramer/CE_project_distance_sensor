/*
 * File:   transport.c
 * Author: ShadowThrone
 *
 * Created on November 3, 2016, 12:23 PM
 */

#include <xc.h>
#include <pic16f877a.h>
#include <xc.h>

// Some Constants definition
#define _XTAL_FREQ  20000000
#define TMR2PRESCALE    4
#define MOTOR1  RB1     // Clockwise motor1 = 1 motor2 = 0
#define MOTOR2  RB2     // Anti Clockwise motor1 = 0 motor 2 = 1
#define outLed  RD0  
#define DIST_ER 10
// End of constants block

#pragma config  FOSC = HS
#pragma config  LVP = OFF
#pragma config  BOREN = ON
#pragma config  WDTE = OFF
#pragma config  CPD =   OFF
#pragma config  CP  =   OFF
#pragma config PWRTE    = OFF

long freq;
unsigned int duty1 = 500;
int aux;
void interrupt echo(){
    if(RBIF == 1){
        RBIE =  0;
        if(RB4 == 1)
            TMR1ON = 1;
        if(RB4 == 0){
            TMR1ON = 0;
            aux = (TMR1L | (TMR1H<<8))/36.76;
        }
    }
    RBIF = 0;
    RBIE = 1;
}
int PWMMaxDuty(){
    return _XTAL_FREQ/(freq * TMR2PRESCALE );
}
void PWM1Init(long fre){
    PR2 = (_XTAL_FREQ/(4*fre*TMR2PRESCALE))-1;
    freq = fre;
}
void PWM1Duty(unsigned int duty){
    int d;
    if(duty < 1024){
        d = ((float)duty/1023)*PWMMaxDuty();
        CCP1X = duty & 2;
        CCP1Y = duty & 1;
        CCPR1L = (duty>>2);
    }          
}

void PWM1Start(){
    CCP1M2  = 1;
    CCP1M3  = 1;
    #if TMR2PRESCALE == 1
        T2CKPS0 = 0;
        T2CKPS1 = 0;
    #elif TMR2PRESCALE == 4
        T2CKPS0 = 1;
        T2CKPS1 = 0;
    #elif TMR2PRESCALE == 16
        T2CKPS0 = 1;
        T2CKPS1 = 1;
    #endif
        TMR2ON = 1;
        TRISC2 = 0;
}

void PWM1Stop(){
    CCP1M2 = 0;
    CCP1M3 = 0;
}

void distSensorInit(){
    TRISD = 0x00;
    GIE = 1;
    RBIF = 0;
    RBIE = 1;
    // __delay_ms(3000);    
    T1CON = 0x30;
}
void setup(){
    TRISB = 0xF0;
    
    distSensorInit();
    PWM1Init(5000); 
    
    // PORTB = 0x02; // motor direction
    MOTOR1 = 0;
    MOTOR2 = 1;
    PWM1Duty(500);
    PWM1Start();
}
void loop(){
    // TO do   
}
void lcdOutput(int dist,char message[]){
    // To do, dont know hwo strings work here
    // Cant find the lcd.h library
}

void errorSound(){
    // To do
}
void main(void) {   
    setup();
    while(1){
        TMR1H = 0;
        TMR1L = 0;
        RB0 = 1;
        __delay_us(10);
        RB0 = 0;
        
       __delay_ms(100);
        aux = aux + 1;
        
        if(aux>=2 && aux<=400){
            if(aux <= DIST_ER){
                outLed = 1;
                PWM1Stop();
            }else{
                outLed = 0;
            }
        }else{
            outLed = 0;
        }      
         __delay_ms(400);
    }
    
    return;   
}
