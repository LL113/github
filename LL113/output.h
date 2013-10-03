// -----------------------------------------------
// LL113
// -----------------------------------------------
// Historique:
// (Baloran) laurent.lecatelier@free.fr, version 0.0.1  10/02/12013
// -----------------------------------------------
// OutPut.h DÃƒÂ©finition Class gestion des sorties


#ifndef _OutPut_H_
#define _OutPut_H_

#include <MIDIMul.h>
#include "utilld.h"




#define NOUT_NONE  0

#define NB_OUTS   28
extern const char *listDestin;

#define NOUT_MIDI1  1
#define NOUT_MIDI2  2
#define NOUT_MIDI3  3

#define NOUT_CVALL  4

#define NOUT_CVG2   5
#define NOUT_CVG3   6
#define NOUT_CVG4   7
#define NOUT_CVG5   8
#define NOUT_CVG6   9
#define NOUT_CVG7   10
#define NOUT_CVG8   11

#define NOUT_CV1    12
#define NOUT_CV2    13
#define NOUT_CV3    14
#define NOUT_CV4    15
#define NOUT_CV5    16
#define NOUT_CV6    17
#define NOUT_CV7    18
#define NOUT_CV8    19

#define NOUT_TRIG1  20
#define NOUT_TRIG2  21
#define NOUT_TRIG3  22
#define NOUT_TRIG4  23
#define NOUT_TRIG5  24
#define NOUT_TRIG6  25
#define NOUT_TRIG7  26
#define NOUT_TRIG8  27

#define SN_OFF      0
#define SN_ON       1

class SuperOut
{
public:
  // Constructor and Destructor
  SuperOut();
  ~SuperOut();

  void Setup();
  void Traite(uint32_t reftime);

  void NoteOn(byte nOut, byte pitch, byte velo, byte chanel);
  void NoteOff(byte nOut, byte pitch, byte velo, byte chanel);
  void PitchBend(byte nOut, int bend, byte channel) ;
  void AfterTouch(byte nOut, byte pressure, byte channel) ;
  void ControlChange(byte nOut, byte number, byte value, byte channel ) ;
  void ProgramChange(byte nOut, byte number, byte channel ) ;
  void AutoOff(byte nOut);
  
protected:

/*
  byte state_GateO;
  byte gated_Pitch0;
  byte state_Gate1;
  byte gated_Pitch1;
  
  byte delai_Trig0;
  byte delai_Trig1;
*/  
  
  int norm_DAC1;
  int bend_DAC1;
  byte gated_Pitch;

//  int32_t modu_DAC0;

  uint32_t next_event;

  uint8_t SNote[128];
};

/*
class OutPut 
{

public:
  // Constructor and Destructor
  OutPut();
  ~OutPut();
  
  void NoteOn(byte pitch, byte velo, byte chanel);
  void NoteOff(byte pitch, byte velo, byte chanel);
  void PitchBend(int bend, byte channel) ;
  void AfterTouch(byte pressure, byte channel) ;
  void ControlChange(byte number, byte value, byte channel ) ;

protected:
  byte state_GateO;
  byte gated_Pitch;
  
};

*/

class InPut 
{

public:
  // Constructor and Destructor
  InPut();
  ~InPut();

  void Setup();
  
  void Traite(uint32_t reftime);

  byte ActionTrigIn();
  byte StateTrigIn();

  byte ActionDynClk();
  byte StateDynClk();

  byte ActionDynStartStop();
  byte StateDynStartStop();

  byte Encoder;

protected:

  byte state_DynClk;
  byte action_DynClk;

  byte state_DynStartStop;
  byte action_DynStartStop;

  byte state_TrigIn0;
  byte action_TrigIn0;

  byte state_A;
  byte action_A;

  byte state_B;
  byte action_B;

  byte state_P;
  byte action_P;

  byte old_action_AB;
  byte old_state_AB;
  
 
};


#endif // _OutPut_H_


