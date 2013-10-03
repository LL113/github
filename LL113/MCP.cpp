/*
  MCP.cpp - Library for DAC MCP4822 for Arduino DUE
 Created by xarolium
 Released into the public domain.
 */

#include "Arduino.h"
#include "MCP.h"
#include "utilld.h"


void MCP::Setup()
{
  pinMode(MCP_CLK,OUTPUT);//Clock Pin
  pinMode(MCP_SDI,OUTPUT);//Serial Pin
  pinMode(MCP_CS1,OUTPUT);//CS first MCP4822
  pinMode(MCP_CS2,OUTPUT);//CS second MCP4822
  pinMode(MCP_CS3,OUTPUT);//CS third MCP4822
  pinMode(MCP_CS4,OUTPUT);//CS forth MCP4822
  digitalWrite(MCP_CS1,HIGH);//CS first MCP4822
  digitalWrite(MCP_CS2,HIGH);//CS second MCP4822
  digitalWrite(MCP_CS3,HIGH);//CS third MCP4822
  digitalWrite(MCP_CS4,HIGH);//CS forth MCP4822

}




void MCP::Send(byte MCP_num, uint16_t sample)
{


        uint8_t dacSPI0 = 0;
        uint8_t dacSPI1 = 0;
        byte selector;
 
 #ifdef _HARDWARE_1_
        switch (MCP_num)
        {
        case 0:
            selector = MCP_CS1;
            sample = sample | 0b0111000000000000; 
            break;
        case 4:
            selector = MCP_CS1;
            sample = sample | 0b1111000000000000; 
            break;
        case 1:
            selector = MCP_CS2;
            sample = sample | 0b0111000000000000; 
            break;
        case 5:
            selector = MCP_CS2;
            sample = sample | 0b1111000000000000; 
            break;
        case 2:
            selector = MCP_CS3;
            sample = sample | 0b0111000000000000; 
            break;
        case 6:
            selector = MCP_CS3;
            sample = sample | 0b1111000000000000; 
            break;
        case 3:
            selector = MCP_CS4;
            sample = sample | 0b0111000000000000; 
            break;
        case 7:
            selector = MCP_CS4;
            sample = sample | 0b1111000000000000; 
            break;
        }
#endif

#ifdef _HARDWARE_2_
        switch (MCP_num)
        {
        case 0:
            selector = MCP_CS4;
            sample = sample | 0b0111000000000000; 
            break;
        case 1:
            selector = MCP_CS4;
            sample = sample | 0b1111000000000000; 
            break;
        case 2:
            selector = MCP_CS1;
            sample = sample | 0b0111000000000000; 
            break;
        case 3:
            selector = MCP_CS1;
            sample = sample | 0b1111000000000000; 
            break;
        case 4:
            selector = MCP_CS2;
            sample = sample | 0b0111000000000000; 
            break;
        case 5:
            selector = MCP_CS2;
            sample = sample | 0b1111000000000000; 
           break;
        case 6:
            selector = MCP_CS3;
            sample = sample | 0b0111000000000000; 
            break;
        case 7:
            selector = MCP_CS3;
            sample = sample | 0b1111000000000000; 
            break;
        }
#endif

        dacSPI0 = (sample >> 8) & 0x00FF;
        dacSPI1 = sample & 0x00FF;

        digitalWrite(selector,LOW);
        shiftOut(MCP_SDI, MCP_CLK, MSBFIRST, dacSPI0);  
        shiftOut(MCP_SDI, MCP_CLK, MSBFIRST, dacSPI1);  
        digitalWrite(selector,HIGH);
}
	

 


