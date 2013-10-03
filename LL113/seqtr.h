// -----------------------------------------------
// LL113
// -----------------------------------------------
// Historique:
// (Baloran) laurent.lecatelier@free.fr, version 0.0.1  10/02/12013
// -----------------------------------------------
// seqtr.h DÃƒÂ©finition Class gestion du sÃƒÂ©quenceur TRxxx


#ifndef _SeqTrxx_H_
#define _SeqTrxx_H_


#include "Arduino.h"
#include "utilld.h"
#include "output.h"

#define NB_TR_INST 9
#define NB_TR_PATTERN 16
#define NB_TR_SONG 16

#define MODE_PATT  0
#define MODE_SONG  1
#define MODE_PARAM 2


struct trInstru {
  uint16_t Pattern;  
  int     Active;      
  int     Note;      
  int     Duree;  
  byte    Veloci;  
  byte    Modul;      
  int     Ampli;     
  byte    Dest1;
  byte    Dest2;
  byte    Chanel;  
  int     Played;
  boolean IsOn;
  uint32_t    TimeOff;  
};


struct trTrack {        
  int       Lg;
  int       Accent;
  trInstru  Instru[NB_TR_INST];
};


struct trSongs {
  byte    Track;
  byte    Repeat;
  int     Transpose;
  byte    Next;  
};

class SeqTrxx 
{

public:
  // Constructor and Destructor
  SeqTrxx(byte);
  ~SeqTrxx();
  
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

  void EventOff(int);
  void EventOn(int pos, uint32_t reftime, boolean acc);


  void HandleNoteOn(byte Interface, byte channel, byte pitch, byte velocity) ;
  void HandleNoteOff(byte Interface, byte channel, byte pitch, byte velocity) ;
  void HandlePitchBend(byte Interface, byte channel, int bend) ;
  void HandleControlChange(byte Interface, byte channel, byte number, byte value) ;
  void HandleAfterTouch(byte Interface, byte channel, byte pressure) ;
  void HandleProgramChange(byte Interface, byte channel, byte number) ;

  void Backup();
  void Restore();

  void ExStart(boolean);
  void ExStop();
  void ChangeCurPlaySong(int value);

public:
  int CurInstr;
  int CurPattern;
  int NextPattern;
  int nbSteep;
  int CurTrSong;
  int CurPlaySong;
  byte is_played;
  boolean is_played_Mode_Song, is_mode_write;
  uint32_t delta_event;
  int mesure, nb24, cpt24;
  byte Mode;
  boolean is_echo;

protected:

  byte NumFunc;
  char FileName[10];

  char tmpCh[10]; 
  char tmpVal[10]; 
  void UpdatePattern(int pos);
  int  Steep;
  trTrack Tracks[NB_TR_PATTERN];
  trSongs Songs[NB_TR_SONG];

  
  mMenu *Menu;
  int lastChangeValue;
  byte LastOffset;
};

#endif // _SeqTrxx_H_






