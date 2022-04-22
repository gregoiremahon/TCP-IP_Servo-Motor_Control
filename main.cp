#include "mbed.h"
 
Serial    pc(USBTX, USBRX);
 PwmOut servo(D5);

const int kMaxBufferSize = 100;
char      buffer[kMaxBufferSize];
int       len = 0;
int VitessePourcent=0; //val en pourcent
int ValRecue;
int ValMot=2000;


int main() {
     servo.period_ms(20);
    buffer[0] = '\0';
    servo.pulsewidth_us(ValMot);
    pc.printf("Start...\n\n");
    while (true) {
            
        while (pc.readable()) {
            char new_char = pc.getc(); 
            if (new_char != '$') {
                buffer[len++] = new_char; 
            }
            else {
                //pc.printf("%s",buffer);
                VitessePourcent = atoi(buffer);
                pc.printf("%d",VitessePourcent);
                memset(buffer, 0, kMaxBufferSize);
                len=0;
                 if(VitessePourcent>100 || VitessePourcent<-100) {
                    ValMot=1490;
                    } 
                else {
                    ValMot=1490+(5*VitessePourcent);
                }
                servo.pulsewidth_us(ValMot);
            }
        }
    }
}