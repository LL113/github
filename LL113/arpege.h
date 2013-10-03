// -----------------------------------------------
// LL113
// -----------------------------------------------
// Historique:
// (Baloran) laurent.lecatelier@free.fr, version 0.0.1  10/02/12013
// -----------------------------------------------
// Arpege.h DÃƒÂ©finition Class gestion de l'arpegiateur


#ifndef _Arpege_H_
#define _Arpege_H_

#include <MIDIMul.h> 
#include "Arduino.h"
#include "seqeuclid.h"
#include "utilld.h"
#include "output.h"

#define MAX_NOTE_OCTA 10

#define NB_EU_STEP_MAX 24
#define NB_ARP_TRACK 16

#define NB_TR_SONG 16
#define MODE_PATT  0
#define MODE_SONG  1


struct arSongs {
  byte    Track;
  byte    Repeat;
  int     Transpose;
  byte    Next;  
};

struct bufNote {        
  int     Note;          
  int     Velo;          
  long    rTime;  
};


struct arpTrack {        
  byte    Direction;          
  byte    Octave;          
  byte    Note;  
  byte    Velomod;  
  int     Veloci;  
  int     Duree;   
  byte    Motif;
  byte    Lg;          
  byte    Div;          
  byte    Maintien;
  byte    Transpose;
  int     NMin;   
  int     NMax;   
  byte    Modul;      
  int     Ampli;     
  byte    Dest1;
  byte    Dest2;
  byte    Chanel;  
  byte    FillAuto;
  int     Tonale;  
  byte    Accord;
  uint16_t Pattern;  
  bufNote Memo[MAX_NOTE_OCTA]; 
  byte    nbMemo;

};

struct TrSongs {
  byte    Track;
  byte    Repeat;
  int     Transpose;
  byte    Next;  
};

class Arpege {

public:
  // Constructor and Destructor
  Arpege(byte);
  ~Arpege();

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
  
  void HandleNoteOn(byte Interface, byte channel, byte pitch, byte velocity) ;
  void HandleNoteOff(byte Interface, byte channel, byte pitch, byte velocity) ;
  void HandlePitchBend(byte Interface, byte channel, int bend) ;
  void HandleControlChange(byte Interface, byte channel, byte number, byte value) ;
  void HandleProgramChange(byte Interface, byte channel, byte number) ;
  void HandleAfterTouch(byte Interface, byte channel, byte pressure) ;
  
  void Backup();
  void Restore();

  void ExStart(boolean);
  void ExStop();

  void ChangeCurPlaySong(int value) ;

public:
  int CurArp;
  int NextArp;
  int nbSteep;
  byte is_played;
  boolean is_played_Mode_Song;
  boolean is_mode_write;
  boolean is_echo;
  uint32_t delta_event;
  int mesure, nb24, cpt24;
  
protected:
  byte NumFunc;
  char FileName[10];
	
  histInNote MemoNote;
  bufNote preBuf[MAX_NOTE_OCTA*6];
  bufNote tmpBuf[MAX_NOTE_OCTA*6];
  bufNote reelBuf[MAX_NOTE_OCTA*6];
  int reelLen;

  char tmpCh[10]; 
  char tmpVal[10]; 
  void UpdatePattern();
  int steep;
  arpTrack Tracks[NB_ARP_TRACK];
  arSongs Songs[NB_TR_SONG];

  int mV_MIDI1, mV_MIDI2, mV_MIDI3;
  
  int    CurNote;
  int    CurVelo;
  boolean IsOn;
  uint32_t    TimeOff;  
  byte    Pat[NB_EU_STEP_MAX];
  byte    RefNote;
  byte    AllIsOff;
  mMenu *Menu;
  
  int PosTrack;

  int CurTrSong;
  int CurPlaySong;
  byte Mode;
  
  int lastChangeValue;  
  byte LastOffset;
};




#endif // _Arpege_H_





