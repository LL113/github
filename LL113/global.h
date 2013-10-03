// -----------------------------------------------
// LL113
// -----------------------------------------------
// Historique:
// (Baloran) laurent.lecatelier@free.fr, version 0.0.1  10/02/12013
// -----------------------------------------------
// Global.h DÃƒÂ©finition Class gestion du global


#ifndef _Global_H_
#define _Global_H_

#include <MIDIMul.h> 
#include "Arduino.h"
#include "utilld.h"
#include "output.h"

struct strRoutes {        
  byte      Type;          
  char      Name[10];
  byte      Perim;
  byte      Route;
  byte      Source;
  uint16_t  Chanel;
  int16_t   MidiType;
};


#define r_NoteOn  1
#define r_NoteOff 2


#define RECORD_OFF 0
#define RECORD_ON 1
#define RECORD_PLAY 2

#define NB_RECORD 1000

struct strRecord {        
  byte      Type;
  byte      Interface;
  byte      pitch;
  byte      velocity;
  byte      channel;
  uint32_t  rtime;
};

struct EntRecord {        
  strRecord record[NB_RECORD];
  int cur_Rec_Record;
  int cur_Play_Record;
  byte mode_Record;
  int nb_Rec_Record;
};

#define RT_MIDI    0
#define RT_CV      0
#define RT_TRIG    0

#define PER_NONE 0
#define PER_GLOBAL 1
#define PER_ALWAY 2

#define TAPTEMPO_MAX  4

class Global {

public:
  // Constructor and Destructor
  Global();
  ~Global();


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
  void Encoder(byte Value);

  void Traite(uint32_t reftime);

/*
  void Traite(uint32_t reftime);

  void EventOff();
  void EventOn(int pos, uint32_t reftime);
  
  void HandleNoteOn(byte Interface, byte channel, byte pitch, byte velocity) ;
  void HandleNoteOff(byte Interface, byte channel, byte pitch, byte velocity) ;
*/

  void Backup();
  void Restore();

  void ExStart(boolean);
  void ExStop();

  void UpdateRoute(byte Route);

  void UpdateSplit();

  void HandleNoteOn(byte Interface, byte channel, byte pitch, byte velocity) ;
  void HandleNoteOff(byte Interface, byte channel, byte pitch, byte velocity) ;
  void HandlePitchBend(byte Interface, byte channel, int bend) ;
  void HandleControlChange(byte Interface, byte channel, byte number, byte value) ;
  void HandleProgramChange(byte Interface, byte channel, byte number) ;
  void HandleAfterTouch(byte Interface, byte channel, byte pressure) ;



public:

  boolean is_played;
  uint32_t delta_event, next_event, clock_count;
 
  char tmpCh[10]; 
  char tmpVal[10]; 
  
  EntRecord Recorder;
  
 
protected:

  byte CurPage;
  byte LastOffset;
  
  char FileName[10];
  mMenu *Menu;

  int PosTrack;
  
  int lastChangeValue;
  
  uint32_t tapTempo[TAPTEMPO_MAX];
 
};

#endif // _Global_H_




