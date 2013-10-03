// -----------------------------------------------
// LL113
// -----------------------------------------------
// Historique:
// (Baloran) laurent.lecatelier@free.fr, version 0.0.1  10/02/12013
// -----------------------------------------------
// output.cpp Code Class gestion des outputs


#include "Arduino.h"
#include "utilld.h"
#include "output.h"
#include "lfo.h"
#include "mcp.h"
#include "global.h"

extern MIDIMul_Class MIDI1;
extern MIDIMul_Class MIDI2;
extern MIDIMul_Class MIDI3;

// extern VCO VCoBi;

const char *listDestin ="Off    Midi 1 Midi 2 Midi 3 CV All CV+G 2 CV+G 3 CV+G 4 CV+G 5 CV+G 6 CV+G 7 CV+G 8 CV 1   CV 2   CV 3   CV 4   CV 5   CV 6   CV 7   CV 8   Trig 1 Trig 2 Trig 3 Trig 4 Trig 5 Trig 6 Trig 7 Trig 8 ";
const char *listUpDown ="Off    Up     Down   ";

extern strGlobalParam ParamGlo;

byte CV_Type[8];
byte TRIG_Type[8];

#define CV_BYTE (uint16_t)32 

SuperOut::SuperOut()
{
  norm_DAC1 = 0;
  bend_DAC1 = 0;
  gated_Pitch = 0;

  memset(CV_Type,0,8);
  memset(TRIG_Type,0,8);

  memset(SNote,SN_OFF,128);
/*
  state_GateO = LOW;
  gated_Pitch0 = 0;

  state_Gate1 = LOW;
  gated_Pitch1 = 0;

  norm_DAC0 = 0;
  bend_DAC0 = 0;
  
  modu_DAC0 = 0;
  delai_Trig0=0;
  delai_Trig1=0;
*/  
}


SuperOut::~SuperOut()
{

}

void SuperOut::Setup()
{
  
  S4822.Send(CV_1, 0 );
  S4822.Send(CV_2, 0 );
  S4822.Send(CV_3, 0 );
  S4822.Send(CV_4, 0 );
  S4822.Send(CV_5, 0 );
  S4822.Send(CV_6, 0 );
  S4822.Send(CV_7, 0 );
  S4822.Send(CV_8, 0 );

  digitalWrite(TRIG_1, TrigToTrig(LOW,TRIG_1) );
  digitalWrite(TRIG_2, TrigToTrig(LOW,TRIG_2) );
  digitalWrite(TRIG_3, TrigToTrig(LOW,TRIG_3) );
  digitalWrite(TRIG_4, TrigToTrig(LOW,TRIG_4) );
  digitalWrite(TRIG_5, TrigToTrig(LOW,TRIG_5) );
  digitalWrite(TRIG_6, TrigToTrig(LOW,TRIG_6) );
  digitalWrite(TRIG_7, TrigToTrig(LOW,TRIG_7) );
  digitalWrite(TRIG_8, TrigToTrig(LOW,TRIG_8) );

  norm_DAC1 = 0;
  bend_DAC1 = 0;
  gated_Pitch = 0;

  memset(SNote,SN_OFF,128);
}


// Sequenceur Step
extern LFo LFoBi;

void SuperOut::Traite(uint32_t reftime)
{
  if ( reftime < next_event ) return;    

/*
  if ( modu_DAC0 != 0 )
  {
//    S4822.Send(0,  norm_DAC0 + bend_DAC0 +  modu_DAC0 * LFoBi.LFos[0].Out / LFO_RESO );
      S4822.Send(0,  norm_DAC0 + bend_DAC0 );
  }

  if ( delai_Trig0 > 0 )
  { 
    delai_Trig0--;
    if ( delai_Trig0 == 0 )
    {
      digitalWrite(GATE_0, HIGH );
    } 
  }

  if ( delai_Trig1 > 0 )
  { 
    delai_Trig1--;
    if ( delai_Trig1 == 0 )
    {
      digitalWrite(GATE_1, HIGH );
    } 
  }
*/
  next_event += 1000;

}

void SuperOut::NoteOn(byte nOut, byte pitch, byte velo, byte chanel)
{
    byte ct;
#ifdef _DEBUG_MODE_
    Serial.print("NoteOn=");
    Serial.print(pitch);
    Serial.print(",");
    Serial.print(velo);
    Serial.print(",");
    Serial.println(chanel);
#endif    

  switch (nOut)
  {
  case NOUT_NONE:
    break;

  case NOUT_MIDI1:
    MIDI1.sendNoteOn(pitch,velo,chanel);
    break;

  case NOUT_MIDI2:
    MIDI2.sendNoteOn(pitch,velo,chanel);
    break;

  case NOUT_MIDI3:
    MIDI3.sendNoteOn(pitch,velo,chanel);
    break;

  case  NOUT_CVALL: // Gestion de la prioritÃƒÂ© pour ce mode 
  
    // ="Single Legato Re low Re Upp Poly 2 Poly 3 Poly 4 Poly 5 Poly 6 Poly 7 Poly 8 ";
 
    switch( ParamGlo.CVTrig )
    {
      case 0 : // Single, cherche si note dÃƒÂ©jÃƒÂ  On,  pas de trig dans ce cas...
           norm_DAC1 = NoteToCv(pitch, CV_1) ;
           S4822.Send(CV_1, norm_DAC1 + bend_DAC1 );
           S4822.Send(CV_2, NoteToCv(velo, CV_2) );
           for (ct=0 ; ct<128 && SNote[ct]==SN_OFF ;ct++);
           if ( ct >= 128 )
           {
              digitalWrite(TRIG_1, TrigToTrig(HIGH,TRIG_1) );
           }
           SNote[pitch]=SN_ON;
           break;
      case 1 : // Mutiple
           norm_DAC1 = NoteToCv(pitch, CV_1) ;
           S4822.Send(CV_1, norm_DAC1 + bend_DAC1 );
           S4822.Send(CV_2, NoteToCv(velo, CV_2) );
           if (  SNote[gated_Pitch] == SN_OFF )           
                 for (ct=0 ; ct<128 && SNote[ct]==SN_OFF ;ct++);
           else
                 ct = gated_Pitch;
           if ( ct < 128 ) 
           {
               digitalWrite(TRIG_1, TrigToTrig(LOW,TRIG_1) );
           }
           digitalWrite(TRIG_1, TrigToTrig(HIGH,TRIG_1) );
           SNote[pitch]=SN_ON;
           gated_Pitch = pitch;
           break;
    }

    break;



  case  NOUT_CVG2:
    S4822.Send(CV_2, NoteToCv(pitch, CV_2));
    digitalWrite(TRIG_2, TrigToTrig(HIGH,TRIG_2) );
    break;

  case  NOUT_CVG3:
    S4822.Send(CV_3, NoteToCv(pitch, CV_3) );
    digitalWrite(TRIG_3, TrigToTrig(HIGH,TRIG_3) );
    break;

  case  NOUT_CVG4:
    S4822.Send(CV_4, NoteToCv(pitch, CV_4) );
    digitalWrite(TRIG_4, TrigToTrig(HIGH,TRIG_4) );
    break;

  case  NOUT_CVG5:
    S4822.Send(CV_5, NoteToCv(pitch, CV_5) );
    digitalWrite(TRIG_5, TrigToTrig(HIGH,TRIG_5) );
    break;

  case  NOUT_CVG6:
    S4822.Send(CV_6, NoteToCv(pitch, CV_6) );
    digitalWrite(TRIG_6, TrigToTrig(HIGH,TRIG_6) );
    break;

  case  NOUT_CVG7:
    S4822.Send(CV_7, NoteToCv(pitch, CV_7) );
    digitalWrite(TRIG_7, TrigToTrig(HIGH,TRIG_7) );
    break;

  case  NOUT_CVG8:
    S4822.Send(CV_8, NoteToCv(pitch, CV_8) );
    digitalWrite(TRIG_8, TrigToTrig(HIGH,TRIG_8) );
    break;

  case  NOUT_CV1:
    S4822.Send(CV_1, NoteToCv(pitch, CV_1) );
    break;

  case  NOUT_CV2:
    S4822.Send(CV_2, NoteToCv(pitch, CV_2) );
    break;

  case  NOUT_CV3:
    S4822.Send(CV_3,NoteToCv(pitch, CV_3) );
    break;

  case  NOUT_CV4:
    S4822.Send(CV_4, NoteToCv(pitch, CV_4) );
    break;

  case  NOUT_CV5:
    S4822.Send(CV_5, NoteToCv(pitch, CV_5) );
    break;

  case  NOUT_CV6:
    S4822.Send(CV_6, NoteToCv(pitch, CV_6) );
    break;

  case  NOUT_CV7:
    S4822.Send(CV_7, NoteToCv(pitch, CV_7) );
    break;

  case  NOUT_CV8:
    S4822.Send(CV_8, NoteToCv(pitch, CV_8) );
    break;


  case  NOUT_TRIG1:
    digitalWrite(TRIG_1, TrigToTrig(HIGH,TRIG_1) );
    break;

  case  NOUT_TRIG2:
    digitalWrite(TRIG_2, TrigToTrig(HIGH,TRIG_2) );
    break;

  case  NOUT_TRIG3:
    digitalWrite(TRIG_3, TrigToTrig(HIGH,TRIG_3) );
    break;

  case  NOUT_TRIG4:
    digitalWrite(TRIG_4, TrigToTrig(HIGH,TRIG_4) );
    break;

  case  NOUT_TRIG5:
    digitalWrite(TRIG_5, TrigToTrig(HIGH,TRIG_5) );
    break;

  case  NOUT_TRIG6:
    digitalWrite(TRIG_6, TrigToTrig(HIGH,TRIG_6) );
    break;

  case  NOUT_TRIG7:
    digitalWrite(TRIG_7, TrigToTrig(HIGH,TRIG_7) );
    break;

  case  NOUT_TRIG8:
    digitalWrite(TRIG_8, TrigToTrig(HIGH,TRIG_8) );
    break;

  }
}

void SuperOut::NoteOff(byte nOut,byte pitch, byte velo, byte chanel)
{
    byte ct;
#ifdef _DEBUG_MODE_
    Serial.print("NoteOff=");
    Serial.print(pitch);
    Serial.print(",");
    Serial.print(velo);
    Serial.print(",");
    Serial.println(chanel);
#endif    
  switch (nOut)
  {
  case NOUT_NONE:
    break;

  case NOUT_MIDI1:
    MIDI1.sendNoteOff(pitch,velo,chanel);
    break;

  case NOUT_MIDI2:
    MIDI2.sendNoteOff(pitch,velo,chanel);
    break;

  case NOUT_MIDI3:
    MIDI3.sendNoteOff(pitch,velo,chanel);
    break;


  case  NOUT_CVALL: // Gestion de la prioritÃƒÂ© pour ce mode 
    switch( ParamGlo.CVTrig )
    {
      case 0 : // Single, cherche si note dÃƒÂ©jÃƒÂ  On,  pas de trig dans ce cas...
           SNote[pitch]=SN_OFF;
           for (ct=0 ; ct<128 && SNote[ct]==SN_OFF ;ct++);
           if ( ct >= 128 )
           {
              digitalWrite(TRIG_1, TrigToTrig(LOW,TRIG_1) );
           }
           else
           {
               norm_DAC1 = NoteToCv(ct, CV_1) ;
               S4822.Send(CV_1, norm_DAC1 + bend_DAC1 );
           }
           break;
      case 1 : // Multipl...
           SNote[pitch]=SN_OFF;
           if (  SNote[gated_Pitch] == SN_OFF )           
                 for (ct=0 ; ct<128 && SNote[ct]==SN_OFF ;ct++);
           else
                 ct = gated_Pitch;
           if ( ct < 128  ) 
           {
               if ( ct != gated_Pitch )
               {
                 digitalWrite(TRIG_1, TrigToTrig(LOW,TRIG_1) );
                 norm_DAC1 = NoteToCv(ct, CV_1) ;
                 S4822.Send(CV_1, norm_DAC1 + bend_DAC1 );
                 digitalWrite(TRIG_1, TrigToTrig(HIGH,TRIG_1) );
//                 gated_Pitch = ct;
               }
           }
           else
           {
               gated_Pitch=0;
               digitalWrite(TRIG_1, TrigToTrig(LOW,TRIG_1) );
           }
           break;
    }
    break;

  case  NOUT_TRIG1:
    digitalWrite(TRIG_1, TrigToTrig(LOW,TRIG_1) );
    break;

  case  NOUT_TRIG2:
  case  NOUT_CVG2:
    digitalWrite(TRIG_2, TrigToTrig(LOW,TRIG_2) );
    break;

  case  NOUT_TRIG3:
  case  NOUT_CVG3:
    digitalWrite(TRIG_3, TrigToTrig(LOW,TRIG_3) );
    break;

  case  NOUT_TRIG4:
  case  NOUT_CVG4:
    digitalWrite(TRIG_4, TrigToTrig(LOW,TRIG_4) );
    break;

  case  NOUT_TRIG5:
  case  NOUT_CVG5:
    digitalWrite(TRIG_5, TrigToTrig(LOW,TRIG_5) );
    break;

  case  NOUT_TRIG6:
  case  NOUT_CVG6:
    digitalWrite(TRIG_6, TrigToTrig(LOW,TRIG_6) );
    break;

  case  NOUT_TRIG7:
  case  NOUT_CVG7:
    digitalWrite(TRIG_7, TrigToTrig(LOW,TRIG_7) );
    break;

  case  NOUT_TRIG8:
  case  NOUT_CVG8:
    digitalWrite(TRIG_8, TrigToTrig(LOW,TRIG_8) );
    break;


  }
}

void SuperOut::PitchBend(byte nOut,int bend, byte channel)
{
  switch (nOut)
  {
  case NOUT_NONE:
    break;

  case NOUT_MIDI1:
    MIDI1.sendPitchBend(bend, channel);
    break;

  case NOUT_MIDI2:
    MIDI2.sendPitchBend(bend, channel);
    break;

  case NOUT_MIDI3:
    MIDI3.sendPitchBend(bend, channel);
    break;

  case  NOUT_CVALL:

    bend_DAC1 = ( (int32_t)bend * 400 * ParamGlo.Bend / 12 ) / 8192;
    S4822.Send(CV_1, norm_DAC1 + bend_DAC1 );
    break;

  }  

}

void SuperOut::ControlChange(byte nOut,byte number, byte value, byte channel)
{
  switch (nOut)
  {
  case NOUT_NONE:
    break;

  case NOUT_MIDI1:
    MIDI1.sendControlChange(number,value,channel);
    break;

  case NOUT_MIDI2:
    MIDI2.sendControlChange(number,value,channel);
    break;

  case NOUT_MIDI3:
    MIDI3.sendControlChange(number,value,channel);
    break;

  case  NOUT_CV1:
    S4822.Send(CV_1, CV_BYTE * value  );
    break;

  case  NOUT_CV2:
    S4822.Send(CV_2, CV_BYTE * value  );
    break;

  case  NOUT_CV3:
    S4822.Send(CV_3, CV_BYTE * value );
    break;

  case  NOUT_CV4:
    S4822.Send(CV_4, CV_BYTE * value  );
    break;

  case  NOUT_CV5:
    S4822.Send(CV_5, CV_BYTE * value  );
    break;

  case  NOUT_CV6:
    S4822.Send(CV_6, CV_BYTE * value  );
    break;

  case  NOUT_CV7:
    S4822.Send(CV_7, CV_BYTE * value  );
    break;

  case  NOUT_CV8:
    S4822.Send(CV_8, CV_BYTE * value  );
    break;

  case  NOUT_CVALL:
/*  
    if ( number == 1 )
    {
      modu_DAC0 = (int32_t)value * LFO_RESO / 10000;
    } 
*/
    break;

  }
}

void SuperOut::ProgramChange(byte nOut,byte number, byte channel)
{
  switch (nOut)
  {
  case NOUT_NONE:
    break;

  case NOUT_MIDI1:
    MIDI1.sendProgramChange(number,channel);
    break;

  case NOUT_MIDI2:
    MIDI2.sendProgramChange(number,channel);
    break;

  case NOUT_MIDI3:
    MIDI3.sendProgramChange(number,channel);
    break;

  }
}


void SuperOut::AfterTouch(byte nOut,byte value, byte channel)
{
  switch (nOut)
  {
  case NOUT_NONE:
    break;

  case NOUT_MIDI1:
    MIDI1.sendAfterTouch(value,channel);
    break;

  case NOUT_MIDI2:
    MIDI2.sendAfterTouch(value,channel);
    break;

  case NOUT_MIDI3:
    MIDI3.sendAfterTouch(value,channel);
    break;

  case  NOUT_CV1:
    S4822.Send(CV_1, CV_BYTE * value );
    break;

  case  NOUT_CV2:
    S4822.Send(CV_2, CV_BYTE * value  );
    break;

  case  NOUT_CV3:
    S4822.Send(CV_3, CV_BYTE * value );
    break;

  case  NOUT_CV4:
    S4822.Send(CV_4, CV_BYTE * value  );
    break;

  case  NOUT_CV5:
    S4822.Send(CV_5, CV_BYTE * value  );
    break;

  case  NOUT_CV6:
    S4822.Send(CV_6, CV_BYTE * value  );
    break;

  case  NOUT_CV7:
    S4822.Send(CV_7, CV_BYTE * value  );
    break;

  case  NOUT_CV8:
    S4822.Send(CV_8, CV_BYTE * value  );
    break;

  case  NOUT_CVALL:
/*  
    if ( number == 1 )
    {
      modu_DAC0 = (int32_t)value * LFO_RESO / 10000;
    } 
*/
    break;

  }
}



void SuperOut::AutoOff(byte nOut)
{
  switch (nOut)
  {
  case NOUT_MIDI1:
    MIDI1.Reset(0);
    break;

  case NOUT_MIDI2:
    MIDI2.Reset(0);
    break;

  case NOUT_MIDI3:
    MIDI3.Reset(0);
    break;

  case  NOUT_CVALL:
    digitalWrite(TRIG_1, TrigToTrig(LOW,TRIG_1) );
    break;

  case  NOUT_TRIG1:
    digitalWrite(TRIG_1, TrigToTrig(LOW,TRIG_1) );
    break;

  case  NOUT_TRIG2:
  case  NOUT_CVG2:
    digitalWrite(TRIG_2, TrigToTrig(LOW,TRIG_2) );
    break;

  case  NOUT_TRIG3:
  case  NOUT_CVG3:
    digitalWrite(TRIG_3, TrigToTrig(LOW,TRIG_3) );
    break;

  case  NOUT_TRIG4:
  case  NOUT_CVG4:
    digitalWrite(TRIG_4, TrigToTrig(LOW,TRIG_4) );
    break;

  case  NOUT_TRIG5:
  case  NOUT_CVG5:
    digitalWrite(TRIG_5, TrigToTrig(LOW,TRIG_5) );
    break;

  case  NOUT_TRIG6:
  case  NOUT_CVG6:
    digitalWrite(TRIG_6, TrigToTrig(LOW,TRIG_6) );
    break;

  case  NOUT_TRIG7:
  case  NOUT_CVG7:
    digitalWrite(TRIG_7, TrigToTrig(LOW,TRIG_7) );
    break;

  case  NOUT_TRIG8:
  case  NOUT_CVG8:
    digitalWrite(TRIG_8, TrigToTrig(LOW,TRIG_8) );
    break;

  }
}


InPut::InPut()
{
  state_DynStartStop = digitalRead(DYNSTARTSTOP_IN);
  action_DynStartStop = NONE;

  state_DynClk = digitalRead(DYNCLK_IN);
  action_DynClk = NONE;

  state_TrigIn0 = digitalRead(TRIGIN_0);
  action_TrigIn0 = NONE;

  state_A = digitalRead(ENCODER_A);
  action_A = NONE;

  state_B = digitalRead(ENCODER_B);
  action_B = NONE;

  state_P = digitalRead(ENCODER_P);
  action_P = NONE;

  old_action_AB = NONE;
  old_state_AB = NONE;
}


InPut::~InPut()
{

}

void InPut::Setup()
{

}

void InPut::Traite(uint32_t reftime)
{
  byte state;

  action_DynStartStop = action_DynClk = action_TrigIn0 = action_A = action_B = action_P = Encoder = NONE;

  state=digitalRead(DYNCLK_IN);
  if ( state != state_DynClk )
  {
    action_DynClk = ( state > state_DynClk )? DOWN2UP:UP2DOWN;
    state_DynClk = state;
/*    
#ifdef _DEBUG_MODE_
    Serial.print("DYNCLK_IN=");
    Serial.println(action_DynClk);
#endif    
*/
  }

  state=digitalRead(DYNSTARTSTOP_IN);
//  state=digitalRead(DYNCLK_IN);

  if ( state != state_DynStartStop )
  {
    action_DynStartStop = ( state > state_DynStartStop )? DOWN2UP:UP2DOWN;
    state_DynStartStop = state;
#ifdef _DEBUG_MODE_
    Serial.print("DYNSTARTSTOP_IN=");
    Serial.println(action_DynStartStop);
#endif    
  }

  state=digitalRead(TRIGIN_0);
  if ( state != state_TrigIn0 )
  {
    action_TrigIn0 = ( state > state_TrigIn0 )? DOWN2UP:UP2DOWN;
    state_TrigIn0 = state;
  }

  state=digitalRead(ENCODER_A);
  if ( state != state_A )
  {
    action_A = ( state > state_A )? DOWN2UP:UP2DOWN;
    state_A = state;
  }
  
  state=digitalRead(ENCODER_B);
  if ( state != state_B )
  {
    action_B = ( state > state_B )? DOWN2UP:UP2DOWN;
    state_B = state;
  }

  state=digitalRead(ENCODER_P);
  if ( state != state_P )
  {
    action_P = ( state > state_P )? DOWN2UP:UP2DOWN;
    state_P = state;
  }

  if ( action_P != NONE || action_B != NONE  || action_A != NONE   )
  {
    
    byte action_AB = ( action_A << 2 ) | action_B;
    byte state_AB = ( state_A << 2 ) | state_B;
    if ( action_P == UP2DOWN ) Encoder = ENCODER_PUSH;
    if ( ( old_action_AB == 8 && action_AB == 1 ) || ( old_action_AB == 4 && action_AB == 2 ) || ( old_action_AB == 8 && action_AB == 4 ) ) Encoder = ENCODER_PLUS;
    if ( ( old_action_AB == 2 && action_AB == 4 ) || ( old_action_AB == 1 && action_AB == 8 ) ) Encoder = ENCODER_MOINS;
    old_action_AB = action_AB ;
    old_state_AB =  state_AB ;

#ifdef _DEBUG_MODE_
    Serial.print( action_AB );
    Serial.print(action_P);
    Serial.print(",");
    Serial.print( state_AB );
    Serial.print(state_P);
    Serial.print(",");
    Serial.println(Encoder);
#endif    

    old_action_AB = action_AB ;
    old_state_AB =  state_AB ;
  
  }


}


byte InPut::ActionTrigIn()
{
  return action_TrigIn0;
}

byte InPut::StateTrigIn()
{
  return state_TrigIn0;
}


byte InPut::ActionDynClk()
{
  return action_DynClk;
}

byte InPut::StateDynClk()
{
  return state_DynClk;
}

byte InPut::ActionDynStartStop()
{
  return action_DynStartStop;
}

byte InPut::StateDynStartStop()
{
  return state_DynStartStop;
}







