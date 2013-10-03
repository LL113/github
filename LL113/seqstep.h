 // -----------------------------------------------
// LL113
// -----------------------------------------------
// Historique:
// (Baloran) laurent.lecatelier@free.fr, version 0.0.1  10/02/12013
// -----------------------------------------------
// seqstep.h DÃƒÂ©finition Class gestion du sequenceur pas a pas

#ifndef _SeqStep_H_
#define _SeqStep_H_


#include "Arduino.h"
#include "utilld.h"
#include "output.h"

#define NB_STEP_MAX 24
#define NB_STEP_TRACK 16
#define NB_STEP_SONG 16

#define MODE_PATT  0
#define MODE_SONG  1
#define MODE_PARAM 2

struct stStep {        
  int     Note;  
  byte    Action;      
  int     V1;  
  int     V2;  
};

struct stTrack {        
  byte    Ctrl1;
  byte    Ctrl2;
  int     Played;
  int     Duree;  
  byte    Veloci;  
  byte    Modul;      
  int     Ampli;     
  byte    Dest1;
  byte    Dest2;
  byte    Chanel;  
  boolean IsOn;
  uint32_t    TimeOff;  
  stStep  Step[NB_STEP_MAX];
  byte    Trig[8];
};


struct stSongs {
  byte    Track;
  byte    Repeat;
  int     Transpose;
  byte    Next;  
};

class SeqStep 
{

public:
  // Constructor and Destructor
  SeqStep(byte);
  ~SeqStep();

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
  void TraiteEvent(uint32_t reftime);
  void Encoder(byte Value);

  void EventOff();
  void EventOn(int pos, uint32_t reftime);

  void Backup();
  void Restore();

  void ExStart(boolean);
  void ExStop();

  void ChangeCurPlaySong(int value) ;
  void ChangeCurStepIndex(int value) ;

  void HandleNoteOn(byte Interface, byte channel, byte pitch, byte velocity) ;
  void HandleNoteOff(byte Interface, byte channel, byte pitch, byte velocity) ;
  void HandlePitchBend(byte Interface, byte channel, int bend) ;
  void HandleControlChange(byte Interface, byte channel, byte number, byte value) ;
  void HandleAfterTouch(byte Interface, byte channel, byte pressure) ;
  void HandleProgramChange(byte Interface, byte channel, byte number) ;
  

public:
  int CurStep;
  int CurStepIndex;
  int CurPattern;
  int NextPattern;
  int nbSteep;
  int CurTrSong;
  int CurPlaySong;
  byte is_played;
  boolean is_played_Mode_Song;
  boolean is_recorded;
  int pos_record;
  boolean is_echo;
  uint32_t delta_event;
  int mesure, nb24, cpt24;
  
  byte Mode;
  
protected:

  byte NumFunc;
  char FileName[10];

  char tmpCh[10]; 
  char tmpVal[10]; 
  void UpdatePattern(int pos);
  int  Steep;
  stTrack Tracks[NB_STEP_TRACK];
  stSongs Songs[NB_STEP_SONG];

  stTrack stMem;
  
  mMenu *Menu;
  int lastChangeValue;
  byte LastOffset;
  
};

#endif // _SeqStep_H_






