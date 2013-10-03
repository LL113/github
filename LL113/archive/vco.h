// -----------------------------------------------
// LL113
// -----------------------------------------------
// Historique:
// (Baloran) laurent.lecatelier@free.fr, version 0.0.1  10/02/12013
// -----------------------------------------------
// VCO.h DÃƒÂ©finition Class gestion des VCOs et Enveloppes

#ifndef _VCO_H_
#define _VCO_H_

#include <MIDIMul.h> 
#include "Arduino.h"
#include "VCO.h"
#include "utilld.h"
#include "onde.h"


#define NB_VCO 2


#define VCO_SAMPLES 512
#define VCO_RESO 0x5000
#define VCO_FRACTIONAL 1024
#define VCO_FRACTIONAL_SHIFT 10
#define VCO_FRACTIONAL_MASQ 1023

#define SAMPLES_LIM VCO_SAMPLES * VCO_FRACTIONAL  

#define SMOOTH_TIME         410   // 41000 * 10 / 1000 = 10ms
#define SMOOTH_TIME_SHORT    41   // 41000 * 10 / 1000 = 1ms

#define SMOOTH_VALUE  128

struct VCOTrack {        
  int16_t     Shap;          
  int16_t     Freq;          
  int16_t     Ampl;  
  uint8_t     Modu;  
  uint8_t     ModDest;    
  int16_t     ModAmpl;  
  int16_t     Fine;  
  uint8_t     Sync;
  int16_t     AmSync;  
//  uint32_t    PerioMicros;  
  int16_t     Out;
  int16_t     LastNote;
  boolean     Retrig;
  uint32_t    StartTime;
  uint32_t    inc;
  uint32_t    index;
  int16_t     fFiltre;  
  int16_t     fReso;  
};


class VCO {

public:
  // Constructor and Destructor
  VCO(byte);
  ~VCO();

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
  
  void SetNote(int pos,int note);
  void UpdateNote(int pos);
  
  
  
  void ExStart();
  void ExStop();
  
public:

  boolean is_played;
  VCOTrack VCOs[NB_VCO];
  
protected:
	
  byte NumFunc;
  char FileName[10];
  char tmpCh[10]; 
  char tmpVal[10]; 

  uint32_t lastreftime;

  mMenu *Menu;
  
  int lastChangeValue;
  
  int32_t last_n;
  int32_t Smooth; // 12 bits
  

#ifdef _DEBUG_MODE_
  int16_t     Wave[VCO_SAMPLES];
  void SetWave(int);
#endif

  
};

#endif // _VCO_H_





