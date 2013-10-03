// -----------------------------------------------
// LL113
// -----------------------------------------------
// Historique:
// (Baloran) laurent.lecatelier@free.fr, version 0.0.1  10/02/12013
// -----------------------------------------------
// seqeuclid.h DÃƒÂ©finition Class gestion du sequenceur euclidien

#ifndef _seqeuclid_H_
#define _seqeuclid_H_

#include "Arduino.h"
#include "utilld.h"
#include "output.h"

#define NB_EU_STEP_MAX 24
#define NB_TRACK 8
#define NB_EU_PATTERN 16
#define NB_EU_SONG 16

#define MODE_PATT  0
#define MODE_SONG  1
#define MODE_PARAM 2

struct euTrack {        
  byte    Lg;          
  byte    Div;          
  int     Note;  
  int     Played;
  int     Veloci;  
  int     Duree;      
  byte    Modul;      
  int     Ampli;     
  byte    Dest1;
  byte    Dest2;
  byte    Chanel;  
  byte    Pat[NB_EU_STEP_MAX];
  boolean IsOn;
  uint32_t TimeOff;  
};


struct euSongs {
  byte    Track;
  byte    Repeat;
  int     Transpose;
  byte    Next;  
};


class SeqEuclid 
{

public:
  // Constructor and Destructor
  SeqEuclid(byte);
  ~SeqEuclid();

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

  void EventOff(int pos);
  void EventOn(int pos, uint32_t reftime);

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

  void ChangeCurPlaySong(int value) ;

public:
  int CurPage;
  int CurPiste, CurPattern, NextPattern;
  byte is_played;
  uint32_t delta_event;
  int mesure, nb24, cpt24;
  int CurTrSong;
  int CurPlaySong;
  boolean is_played_Mode_Song;
  byte Mode;
  int nbSteep;
  boolean is_echo;
  
protected:

  byte NumFunc;
  char FileName[10];

  char tmpCh[10]; 
  char tmpVal[10]; 
  void UpdatePattern(int pos);
  int steep;
  euTrack Tracks[NB_EU_PATTERN][NB_TRACK];

  euSongs Songs[NB_EU_SONG];

  mMenu *Menu;
  
  int lastChangeValue;
  byte LastOffset;
};

#endif // _seqeuclid_H_






