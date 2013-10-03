#ifndef MCP_h     
#define MCP_h
#include "Arduino.h"

#define MCP_SDI 46 // PIOC PIO_PC4
#define MCP_CLK 45 // PIOC PIO_PC5
#define MCP_CS1 47 // PIOC PIO_PC6
#define MCP_CS2 48 // PIOC PIO_PC7,
#define MCP_CS3 49 // PIOC PIO_PC8
#define MCP_CS4 50 // PIOC PIO_PC9

class MCP
{
public:
  void Setup();
  void Send (byte MCP_num, uint16_t sample);
  
private:
  
};


extern MCP S4822;

#endif // Fin si


