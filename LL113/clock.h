// -----------------------------------------------
// LL113
// -----------------------------------------------
// Historique:
// (Baloran) laurent.lecatelier@free.fr, version 0.0.1  10/02/12013
// -----------------------------------------------
// OutPut.h DÃƒÂ©finition Class gestion des sorties


#ifndef _Clock_H_
#define _Clock_H_

#include <MIDIMul.h>
#include "utilld.h"



#define EVENT_NONE    0
#define EVENT_CLOCK   1
#define EVENT_START   2
#define EVENT_CONTINU 3
#define EVENT_STOP    4
#define EVENT_CLOCKSTART    5


#define CLOCK_INT      0
#define CLOCK_MIDI_1   1
#define CLOCK_MIDI_2   2
#define CLOCK_MIDI_3   3
#define CLOCK_TRIG     4
#define CLOCK_DYN      5
#define CLOCK_CVIN     6

#define PLAYED  1
#define STOPPED 2
#define PAUSED  3

#define DELAI_TRIG_MIN    8333
#define DELAI_TRIG_MAX 4000000

#define DELAI_CLOCK_OUT 1000000

class SysClock
{

public:
  // Constructor and Destructor
  SysClock();
  ~SysClock();

  void Traite(uint32_t reftime);

  void DiffuseLenEvent(uint32_t lenClock);

  void PushEvent(byte);


  void ReceptTrig(uint32_t reftime);
  void ReceptDyn(uint32_t reftime);
  void SetModeClock(byte mode, byte div);
  
  void HandleClock(int mid);
  void HandleStart(int mid); 
  void HandleContinue(int mid);
  void HandleStop(int mid); 

  void Pause(); 
  void Stop(); 
  void Start(); 
  void Continue(); 

  void ChangeTempo();

  void PostStart();
  void PostStop();

   byte Event;
 
  uint32_t mtime, utime;
  uint32_t delta_event, next_event;

  byte Mode; 
  byte Divise; 
  
protected:

  byte NbStart;
  byte midiclock;
  byte  nbAutoTrig;
  uint32_t memoTrig;
  uint32_t memoClock;
//  uint32_t nextTrigEvent;
//  uint32_t incTrigEvent;
  uint32_t lastEvent;
  uint32_t Trig_Min;
  
  boolean IsPlayed;
  
};


#endif // _Clock_H_


