#include <18F4520.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#fuses HS,NOWDT,NOPROTECT,NOLVP                       
#use delay(clock = 8000000)
#use fast_io(B)
#use rs232(baud=9600,xmit=pin_C6,rcv=pin_C7) //Comunica��o Serial com console
#define DHT11_PIN PIN_B4    // connection pin between DHT11 and mcu


 
short Time_out;
unsigned int8 T_byte1, T_byte2, RH_byte1, RH_byte2, CheckSum;
char valueC[2];
int value;
void start_signal(){
  output_drive(DHT11_PIN);    // configure connection pin as output
  output_low(DHT11_PIN);      // connection pin output low
  delay_ms(25);
  output_high(DHT11_PIN);     // connection pin output high
  delay_us(30);
  output_float(DHT11_PIN);    // configure connection pin as input
}
short check_response(){
  delay_us(40);
  if(!input(DHT11_PIN)){      // read and test if connection pin is low
    delay_us(80);
    if(input(DHT11_PIN)){     // read and test if connection pin is high
      delay_us(50);
      return 1;
    }
  }
}
unsigned int8 Read_Data(){
  unsigned int8 i, k, _data = 0;        // k is used to count 1 bit reading duration
  if(Time_out)
    break;
  for(i = 0; i < 8; i++){
    k = 0;
    while(!input(DHT11_PIN)){           // Wait until DHT11 pin get raised
      k++;
      if(k > 100){
        Time_out = 1;
        break;
      }
      delay_us(1);
    }
    delay_us(30);
    if(!input(DHT11_PIN))
      bit_clear(_data, (7 - i));        // Clear bit (7 - i)
    else{
      bit_set(_data, (7 - i));          // Set bit (7 - i)
      while(input(DHT11_PIN)){          // Wait until DHT11 pin goes low
        k++;
        if(k > 100){
        Time_out = 1;
        break;
      }
      delay_us(1);}
    }
  }
  return _data;
}
void main(){  
while(TRUE){
    Time_out = 0;
    Start_signal();
    if(check_response()){                     // If there is a response from sensor
      RH_byte1 = Read_Data();                 // read RH byte1
      RH_byte2 = Read_Data();                 // read RH byte2
      T_byte1 = Read_Data();                  // read T byte1
      T_byte2 = Read_Data();                  // read T byte2
      Checksum = Read_Data();                 // read checksum
      if(Time_out){                           // If reading takes long time        
        printf("\nTime out!");
      }
      else{
       if(CheckSum == ((RH_Byte1 + RH_Byte2 + T_Byte1 + T_Byte2) & 0xFF)){        
         valueC[0]  = RH_Byte1/10 + 48;
         valueC[1]  = RH_Byte1%10 + 48; 
         value = atoi(valueC);
         printf("%d", value);
         if(value < 40){                     //Determina quando ir� come�ar a ligar a bomba
            output_high(pin_D0);
         }else if(value >= 85){              //Determina quando ir� desligar a bomba
            output_low(pin_D0);
         }
       }
       else{
         printf("\nChecksum Error!");
       }
      }
    }
    else {
     printf("\nSem sinal");
    }
  delay_ms(1000);
  }
}
