// -----------------------------------------------
// LL113
// -----------------------------------------------
// Historique:
// (Baloran) laurent.lecatelier@free.fr, version 0.0.1  10/02/12013
// -----------------------------------------------
// output.cpp Code Class gestion des outputs


#include "Arduino.h"
#include "utilld.h"
#include "clock.h"

#include "arpege.h"
#include "lfo.h"
#include "seqeuclid.h"
#include "seqstep.h"
#include "seqtr.h"
#include "clock.h"

extern Arpege Arp;
extern SeqEuclid SeqEu;
extern SeqStep SeqSt;
extern SeqTrxx SeqTr;
extern LFo LFoBi;

SysClock::SysClock()
{
  next_event = lastEvent = memoTrig = memoClock = micros();
  nbAutoTrig = NbStart = midiclock=0;

}

SysClock::~SysClock()
{

}

void SysClock::Traite(uint32_t reftime)
{
  mtime = reftime;
  
  switch (Mode)
  {
  case CLOCK_INT : 
    if ( reftime < next_event  )
    {
      Arp.Traite(reftime);
      SeqSt.Traite(reftime);
      SeqEu.Traite(reftime);
      SeqTr.Traite(reftime);
      return;
    }
    Arp.TraiteEvent(next_event);
    SeqSt.TraiteEvent(next_event);
    SeqEu.TraiteEvent(next_event);
    SeqTr.TraiteEvent(next_event);
    next_event += delta_event;
    break;


  case CLOCK_MIDI_1 : 
  case CLOCK_MIDI_2 : 
  case CLOCK_MIDI_3 : 
      if ( Event == EVENT_STOP  ) break;
      if ( Event == EVENT_START  ) break;
      if ( Event == EVENT_CONTINU  ) break;
      if ( Event == EVENT_NONE  )
      {
        Arp.Traite(reftime);
        SeqSt.Traite(reftime);
        SeqEu.Traite(reftime);
        SeqTr.Traite(reftime);
        return;
      }
      Arp.TraiteEvent(reftime);
      SeqSt.TraiteEvent(reftime);
      SeqEu.TraiteEvent(reftime);
      SeqTr.TraiteEvent(reftime);
      Event = EVENT_NONE;
      break;

  case CLOCK_TRIG : 
    if ( reftime < next_event || nbAutoTrig == 0 )
    {
      Arp.Traite(reftime);
      SeqSt.Traite(reftime);
      SeqEu.Traite(reftime);
      SeqTr.Traite(reftime);
      return;
    }
    Arp.TraiteEvent(next_event);
    SeqSt.TraiteEvent(next_event);
    SeqEu.TraiteEvent(next_event);
    SeqTr.TraiteEvent(next_event);
    
    nbAutoTrig--;
    if ( nbAutoTrig > 0 ) next_event += delta_event;
    break;

  case CLOCK_DYN : 
    if ( Event == EVENT_NONE  )
    {
      Arp.Traite(reftime);
      SeqSt.Traite(reftime);
      SeqEu.Traite(reftime);
      SeqTr.Traite(reftime);
      return;
    }
    Arp.TraiteEvent(reftime);
    SeqSt.TraiteEvent(reftime);
    SeqEu.TraiteEvent(reftime);
    SeqTr.TraiteEvent(reftime);
    Event = EVENT_NONE;
    break;


  }

} 

void SysClock::ChangeTempo()
{
  if ( Mode != CLOCK_INT ) return;
  delta_event =  ( 60000000 ) / (ParamGlo.Tempo * 24 );
  DiffuseLenEvent( delta_event ); 
}


void SysClock::DiffuseLenEvent(uint32_t mEvent)
{
  delta_event = mEvent;
  if (delta_event < 8333) delta_event = 8333;
  Arp.delta_event = Arp.nb24 * delta_event;
  SeqSt.delta_event = SeqSt.nb24 * delta_event;
  SeqEu.delta_event = SeqEu.nb24 * delta_event;
  SeqTr.delta_event = SeqTr.nb24 * delta_event;
}


void SysClock::SetModeClock(byte mod, byte div)
{
  Mode = mod;
  Divise = div;
  
  delta_event =  ( 60000000 ) / (ParamGlo.Tempo * 24 );
  DiffuseLenEvent( delta_event ); 

  switch (Mode)
  {
  case CLOCK_INT : 
    break;

  case CLOCK_MIDI_1 : 
  case CLOCK_MIDI_2 : 
  case CLOCK_MIDI_3 : 
    midiclock=0;  
    Event = EVENT_NONE;
    break;

  case CLOCK_TRIG : 
    Trig_Min = DELAI_TRIG_MIN * 24 / Divise ;
    nbAutoTrig=0;
    break;  

  case CLOCK_DYN : 
    midiclock=0;  
    Event = EVENT_NONE;
    break;  

  }

}


void SysClock::ReceptTrig(uint32_t reftime)
{
  if ( Mode != CLOCK_TRIG ) return;
  uint32_t delta = reftime - memoTrig ;
  if ( delta > DELAI_TRIG_MAX || delta < Trig_Min ) { 
    memoTrig = reftime;    
    return; 
  }
  DiffuseLenEvent( (reftime - memoTrig) * Divise / 24 ); 
  memoTrig = reftime;    
  next_event =  reftime;
  nbAutoTrig=24;
}


void SysClock::ReceptDyn(uint32_t reftime)
{
  if ( Mode != CLOCK_DYN ) return;
  midiclock++;  
  if (midiclock>=24 )
  {
      uint32_t ttime = micros(); 
      DiffuseLenEvent( (ttime - memoClock) / midiclock ); 
      memoClock = ttime;   
      midiclock=0; 
  }

  Event = EVENT_CLOCK;
}


void SysClock::HandleClock(int mid)
{
  if ( Mode != mid ) return;

  midiclock++;  
  if (midiclock>=24 )
  {
      uint32_t ttime = micros(); 
      DiffuseLenEvent( (ttime - memoClock) / midiclock ); 
      memoClock = ttime;   
      midiclock=0; 
#ifdef _DEBUG_MODE_
      Serial.println("");
#endif    
  }

  Event = EVENT_CLOCK;

}

void SysClock::HandleStart(int mid)
{
  if ( Mode != mid ) return;

#ifdef _DEBUG_MODE_
  Serial.println("HandleStart");
#endif    


  Start();

}

void SysClock::HandleContinue(int mid)
{
  if ( Mode != mid ) return;

#ifdef _DEBUG_MODE_
  Serial.println("HandleContinue");
#endif    

  Continue();
}


void SysClock::HandleStop(int mid)
{
  if ( Mode != mid ) return;
#ifdef _DEBUG_MODE_
  Serial.println("HandleStop");
#endif    
  IsPlayed=false;
  Stop();
}

void SysClock::PushEvent(byte eVent)
{
  next_event = mtime;
  Event = eVent;
}

void SysClock::Pause()
{
  PushEvent(EVENT_STOP);
}

void SysClock::Continue()
{
  PushEvent(EVENT_CONTINU);
}

void SysClock::Start()
{
  PushEvent(EVENT_START);
}

void SysClock::Stop()
{
  PushEvent(EVENT_STOP);
}

void SysClock::PostStart()
{
  // Si un des objet est Started
  if  (NbStart==0) {
      next_event = mtime; 
      PushEvent(EVENT_START);
  }    
  NbStart++;
/*  
#ifdef _DEBUG_MODE_
  Serial.print(NbStart);
  Serial.print(",");
  Serial.print(next_event);
  Serial.print(NbStart);
  Serial.print(NbStart);
  Serial.print(NbStart);
#endif    
*/
}

void SysClock::PostStop()
{
  // Si un des objet est Started
  if ( NbStart > 0 ) NbStart--;
}





