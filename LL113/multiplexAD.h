// -----------------------------------------------
// LL113
// -----------------------------------------------
// Historique:
// (Baloran) laurent.lecatelier@free.fr, version 0.0.1  10/02/12013
// -----------------------------------------------
// multiplex.h DÃƒÂ©finition Class gestion du matÃƒÂ©riel et des multiplexeurs


#ifndef _multiplexAD_H_
#define _multiplexAD_H_

#include "Arduino.h"

// Adresses des ports de sÃƒÆ’Ã‚Â©lection lignes AD
#define ADDR_MIXAD_0 22
#define ADDR_MIXAD_1 24
#define ADDR_MIXAD_2 26
#define ADDR_MIXAD_3 28
#define ADDR_MIXAD_4 30

#define NB_AD 34


#define DELTA_BTN  30
#define DELTA_PAGE 15
#define DELTA_ITEM 4

/*
#define DELTA_BTN  120
#define DELTA_PAGE 60
#define DELTA_ITEM 16
*/

#define MAD_POTAR 0
#define MAD_ECHEL 1
#define MAD_RIEN  2


struct paramAD {        
  int     Value;          
  int     Old;          
  int     Old_1;          
  int     Old_2;          
  byte    DeltaChange;      
  int     incDeltaChange;      
  byte    Type;      
};


class multiplexAD {

public:
  // Constructor and Destructor
  multiplexAD();
  ~multiplexAD();
  void  Setup();
  void  PageChanged();
  boolean ScanNextAD();

  int last_inc_scan;
  paramAD mm[NB_AD];

protected:
  int inc_scan; 

  long  refA4;
  long  refA5;

  void  IncAD();
};


#endif _multiplexAD_H_




