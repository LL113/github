// -----------------------------------------------
// LL113
// -----------------------------------------------
// Historique:
// (Baloran) laurent.lecatelier@free.fr, version 0.0.1  10/02/12013
// -----------------------------------------------
// LFO.h DÃƒÂ©finition Class gestion des LFOs et Enveloppes

#ifndef _LFo_H_
#define _LFo_H_

#include <MIDIMul.h> 
#include "Arduino.h"
#include "lfo.h"
#include "utilld.h"


#define NB_LFO 2
#define NB_ENV 2


#define LFO_SAMPLES 1024

#define LFO_RESO 0x7000

const uint32_t PERIO_LIM = (uint32_t)0xFFFFFFFF / LFO_SAMPLES;

extern float *cv_INT_HZ;

#define LFO_SMOOTH_TIME         1000
#define LFO_SMOOTH_TIME_SHORT    100 
#define LFO_SMOOTH_VALUE  128


#define ENV_SMOOTH_TIME          300
#define ENV_SMOOTH_TIME_SHORT     30
#define ENV_SMOOTH_VALUE  128


struct lfoTrack {        
  uint8_t     Shap;          
  int16_t     Freq;          
  int16_t     Ampl;  
  uint8_t     Modu;  
  uint8_t     ModDest;    
  int16_t     ModAmpl;  
  int16_t     Phase;  
  uint8_t     Trig;
  uint8_t     Destination;
  int32_t     PerioMicros;  
  int16_t     Out;
  boolean     Retrig;
  uint32_t    StartTime;
  uint32_t    Index ;  
  int32_t     last_n;
  int32_t     Smooth; // 12 bits
  
};


struct envTrack {        
  int16_t     Attack;          
  int16_t     Decay;          
  int16_t     Sustain;  
  int16_t     Release;  
  int16_t     Etape;  
  int16_t     Ampl;  
  int16_t     Delay;  
  uint8_t     Log;  
  int32_t     LevelRef;  
  int32_t     EtapeRef;  
  uint8_t     Trig;
  uint8_t     Destination;
  int16_t     Out;
  boolean     Retrig;
  uint32_t    StartTime;
  int32_t     last_n;
  int32_t     Smooth; // 12 bits

};


class LFo {

public:
  // Constructor and Destructor
  LFo(byte);
  ~LFo();

  void Init();
  
  void DessinPage();
  void UpdateCtrl(int pos);

  boolean ChangeValue(int pos, int value, byte Encoder);
  boolean ChangePage(int value, byte Encoder);

  boolean ChangeM1(int value, byte Encoder);
  boolean ChangeM2(int value, byte Encoder);
  boolean ChangeM3(int value, byte Encoder);
  boolean ChangeS1S5(int value);
  boolean ChangePAT(int value);


  void Interface(int Num, int Value);
  void Traite(uint32_t reftime);
  void Encoder(byte Value);
  void ChangeFreq(int pos, int value);

  void Backup();
  void Restore();

  void ExStart();
  void ExStop();

  void HandleNoteOn(byte Interface, byte channel, byte pitch, byte velocity) ;
  void HandleNoteOff(byte Interface, byte channel, byte pitch, byte velocity) ;
  
public:

  boolean is_played;
  lfoTrack LFos[NB_LFO];
  envTrack Envs[NB_ENV];
  
protected:
	
  byte NumFunc;
  char FileName[10];
  char tmpCh[10]; 
  char tmpVal[10]; 
  void SetWave(int, int);
  int  CurLfoEnv;
  uint32_t lastreftime, next_event, delta_event;

  mMenu *Menu;
  
  int lastChangeValue;
  
};

#endif // _LFo_H_





