// -----------------------------------------------
// LL113
// -----------------------------------------------
// Historique:
// (Baloran) laurent.lecatelier@free.fr, version 0.0.1  10/02/12013
// -----------------------------------------------
// Edsynth.h DÃ¯Â¿Â½finition Class gestion des Instruments

#ifndef _edSN_H_
#define _edSN_H_

#include <MIDIMul.h> 
#include "Arduino.h"
#include "utilld.h"
#include "output.h"


extern const char *listChanel;
extern const char *LisPrgChange;

struct paramSynth {        
  int16_t    i_Name;          
  int16_t    Page;      
  int16_t    Index;      
  int16_t    Mini;      
  int16_t    Maxi;
  int16_t    Step;
  int16_t    Value;
  int16_t    C1;      
  int16_t    C2;      
  byte		 fAff;  
  byte		 fVA;   
  byte		 fC1;   
  byte		 fC2;   
  byte		 TypeBtn;   
  byte		 EvCond;   
  int16_t    i_List;          
  int16_t    i_Cmd;       
  int16_t    i_Cond;       
};



struct paramSysEx 
{        
  byte    i_Value;   
  byte    i_C1;   
  byte    i_C2;
  byte    i_ID;
  byte    i_CH;   
  byte    i_CKD;      
  byte    i_CKF; 
  byte    pBuf; 
  byte    Byte[128];
};


class edSN {

public:
  // Constructor and Destructor
  edSN(byte pNumFunc);
  ~edSN();
  void Setup();
  void LoadFile(char *FileName);
  void DessinPage();
  boolean ChangeValue(int pos, int value, byte Encoder);
  boolean ChangePage(int value, byte Encoder);
  boolean ChangeM1(int value, byte Encoder);
  boolean ChangeM2(int value, byte Encoder);
  boolean ChangeM3(int value, byte Encoder);
  boolean ChangeS1S15(int value);
  boolean ChangePAT(int value);
  void Interface(int Num, int Value);
  void Traite(long reftime);
  void Encoder(byte Value);

  void FmtNb(int16_t value,  int16_t mini,byte format, byte *dest);
  void midiSend(int pos);
  void UpdateCtrl(int pos);

  void HandleNoteOn(byte Interface, byte channel, byte pitch, byte velocity) ;
  void HandleNoteOff(byte Interface, byte channel, byte pitch, byte velocity) ;
  void HandlePitchBend(byte Interface, byte channel, int bend) ;
  void HandleControlChange(byte Interface, byte channel, byte number, byte value) ;
  void HandleProgramChange(byte Interface, byte channel, byte number) ;
  void HandleAfterTouch(byte Interface, byte channel, byte pressure) ;

  void Send();

public:
  int CurPage;
  int MaxPage;


  MIDIMul_Class *ActiveMidi;
  int MidiChan;
  byte outInterface;
  uint32_t next_event, delta_event;
  boolean is_echo;

protected:

  char tmpCh[10]; 
  char tmpVal[10]; 
  char TitreStr[21];

  byte SysTmp[128]; 

  char FileName[10];
  char LoadFileName[20];



  int16_t i_Name;
  int16_t i_Version;
  int16_t i_PathSD;

  paramSynth *curSynth;
  byte	     *curStr;
  byte	     *curSysEx;
  int16_t    *strIndex;		
  int16_t    *sysExIndex;		


  int16_t    OldC1;		
  int16_t    OldC2;		

  uint8_t    delaiSysEx;

  int nbBtn;
  int nbPush;
  int nbStr;
  int nbSysEx; 
  //  char *Titre;
  byte NumFunc;
  mMenu *Menu;

  int sendPos, maxPos, identity;

  int *Values; 

  const char *SDPath;

  int lastChangeValue;

  byte typeSend;
};

#endif // _edSN_H_






