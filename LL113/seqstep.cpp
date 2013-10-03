// -----------------------------------------------
// LL113
// -----------------------------------------------
// Historique:
// (Baloran) laurent.lecatelier@free.fr, version 0.0.1  10/02/12013
// -----------------------------------------------
// seqstep.cpp Code Class gestion du sequenceur pas a pas

#include <LiquidCrystal.h>
// #include <MIDIMul.h> 
#include "Arduino.h"
#include "SeqStep.h"
#include "utilld.h"
#include "multiplexAD.h"
#include "lfo.h"
#include "SDFile.h"
#include "clock.h"


// Clock
extern SysClock SClock;


extern SDFile MySD;
extern mPatLed PatLed;
extern LFo LFoBi;
extern multiplexAD mxAD;
extern LiquidCrystal lcdM;
extern LiquidCrystal lcd1;
extern LiquidCrystal lcd2;
extern LiquidCrystal lcd3;
extern LiquidCrystal lcd4;

const char *StepTitre  ="\x01 Step Sequencer   \x02";

extern const char *listDestin;
extern const char *listSource;
extern const char *listMul;

const char *listAction ="Play   Skip   Blank  Stop   ";
const char *listTTrig ="Off    All    Step/2 Step/3 Step/4 Raz    Step 1 Step 2 Step 3 Step 4 Step 5 Step 6 Step 7 Step 8 Step 9 Step 10Step 11Step 12Step 13Step 14Step 15Step 16Step 17Step 18Step 19Step 20Step 21Step 22Step 23Step 24";
const char *listCtrl  ="Off    Veloci.Pressu.CV 1   CV 2   CC 1   CC 2   CC 3   CC 4   ";
const char *listMOffOn  ="Off   On    ";

extern SuperOut SPOut;
extern const char  mListSource[];

paramMenuList MenuListStep[] = {
  { 
    "Input", "Echo", "", 0, 0, 1, listMOffOn, 0, 0 ,0 ,0       }
  , 
  { 
    "Copy", "From", "To", 1, 1, 16,0 ,2,1,16,0       }
  , 
  { 
    "Load", "Name", "", 0, MODE_SEQSTEP, -1, 0 , 0, 0, 0, 0     }
  ,
  { 
    "Save", "Name", "Indice", 0, 0, NB_STYLEMU, listStyleMenu , 1, 1, 64, 0       }
  , 
  { 
    "Init", "", "", 0, 0, 0, 0 , 0, 0, 0, 0                 }
  , 
  { 
    "Re Key", "", "", 0, 0, 0, 0 , 0, 0, 0, 0       }
  , 
  { 
    "", "", "", 0, 0, 0, 0 , 0, 0, 0, 0                     }
};


paramMenu menuStep = {
  "Par",
  "Sng",
  "Play",
  "Enr",
  "BPM",          
  "S/Beat",     
  "Track",          
  TEMPO_INIT,TEMPO_MIN,TEMPO_MAX,0,     
  4,1,16,0,     
  1,1,NB_STEP_TRACK,0,     
  MenuListStep 
};


SeqStep::SeqStep(byte pNumFunc)
{ 
  NumFunc=pNumFunc;
  Init();
  Menu = new mMenu(&menuStep, (char *)StepTitre);
  mesure = menuStep.Val_2;
  nb24 =  CalcNb24(mesure);
  cpt24 = 0;
  is_echo=true;
  LastOffset = 0;
}

SeqStep::~SeqStep()
{

}

void SeqStep::Init()
{
  lastChangeValue = CHGT_PAGE;

  CurStep=0;
  CurPattern=0;
  CurTrSong=0;

  for (CurPattern=0;CurPattern<NB_STEP_TRACK;CurPattern++)
  {  
    for (CurStep=0;CurStep<NB_STEP_MAX;CurStep++)
    {
      Tracks[CurPattern].Step[CurStep].Note = 48;      
      Tracks[CurPattern].Step[CurStep].V1 = 100;
      Tracks[CurPattern].Step[CurStep].V2 = 100;
      Tracks[CurPattern].Step[CurStep].Action = 0;      
    }

    Tracks[CurPattern].Ctrl1 = 1;      
    Tracks[CurPattern].Ctrl2 = 2;   
    Tracks[CurPattern].Modul = 0;      
    Tracks[CurPattern].Duree = 5;      
    Tracks[CurPattern].Modul = 0;      
    Tracks[CurPattern].Ampli = 0;      
    Tracks[CurPattern].Dest1 = NOUT_MIDI1;
    Tracks[CurPattern].Dest2 = NOUT_NONE;
    Tracks[CurPattern].Chanel = 1;
    Tracks[CurPattern].IsOn = false;
    Tracks[CurPattern].TimeOff = 0;  
    memset( Tracks[CurPattern].Trig, 0, 8 );
  }
  CurPattern=0;
  NextPattern=-1;
  CurStep=0;
  CurStepIndex=0;
  nbSteep=0;

  for (CurTrSong=0;CurTrSong<NB_STEP_SONG;CurTrSong++)
  {  
    Songs[CurTrSong].Track = CurTrSong+1;      
    Songs[CurTrSong].Repeat = 1;      
    Songs[CurTrSong].Transpose = 0;      
    Songs[CurTrSong].Next = (CurTrSong==NB_STEP_SONG)?0:CurTrSong+2;   
  }
  CurTrSong=0;


  Steep = 0;
  is_played=STOPPED;
  Mode=MODE_PATT;

}


void SeqStep::DessinPage()
{
  int pos;

  menuStep.Val_1 = ParamGlo.Tempo;
  menuStep.Val_2 = mesure;
  menuStep.Val_3 = CurPattern+1;

  Menu->DessinPage();

  if ( Menu->FirstPage )
  {
    if (is_played==PLAYED) {
      lcdM.setCursor(8,3);
      lcdM.print("Stop");
    }
    else
    {
      lcdM.setCursor(8,3);
      lcdM.print("Play");
    }


    if (is_recorded) {
      lcdM.setCursor(13,3);
      lcdM.print("End");
    }
    else
    {
      lcdM.setCursor(13,3);
      lcdM.print("Rec");
    }

    if (Mode==MODE_PARAM) {
      lcdM.setCursor(0,3);
      lcdM.print("Stp");
    }
    else
    {
      lcdM.setCursor(0,3);
      lcdM.print("Par");
    }
    
    if (Mode==MODE_PATT || Mode==MODE_PARAM) 
    {
      lcdM.setCursor(4,3);
      lcdM.print("Sng");
    }
    else
    {
      lcdM.setCursor(4,3);
      lcdM.print("Stp");
    }



  }

  Z_Clear();

  switch(Mode)
  {
  case MODE_PATT :
    for (pos=0;pos<5;pos++)
    {

      if ( pos+CurStep == CurStepIndex )
        lcd1.Carac(pos*8,0,2);
      else
        if ( pos > 0 )            
          lcd1.Carac(pos*8,0,6);
        else
          lcd1.Carac(0,0,32);

      sprintf(tmpCh,"Step %d",pos+CurStep+1);
      Z_Text(pos+1,tmpCh);

      lcd_Note(tmpVal,Tracks[CurPattern].Step[pos+CurStep].Note);
      Z_Value(pos+1,tmpVal);

      lcd_Liste(tmpVal,(char *)listAction,Tracks[CurPattern].Step[pos+CurStep].Action);
      Z_TextValue(pos+6,"Action",tmpVal);

      lcd_Liste(tmpCh,(char *)listCtrl,Tracks[CurPattern].Ctrl1);
      lcd_Num(tmpVal,Tracks[CurPattern].Step[pos+CurStep].V1);
      Z_TextValue(pos+11,tmpCh,tmpVal);

      lcd_Liste(tmpCh,(char *)listCtrl,Tracks[CurPattern].Ctrl2);
      lcd_Num(tmpVal,Tracks[CurPattern].Step[pos+CurStep].V2);
      Z_TextValue(pos+16,tmpCh,tmpVal);

    }
    break;

  case MODE_PARAM:

    lcd_Liste(tmpVal,(char *)listCtrl,Tracks[pos].Ctrl1);
    Z_TextValue(1,"Ctrl.1",tmpVal);

    lcd_Liste(tmpVal,(char *)listCtrl,Tracks[pos].Ctrl2);
    Z_TextValue(2,"Ctrl.2",tmpVal);


    lcd_Num(tmpVal,Tracks[pos].Duree);
    Z_TextValue(4,"Durati.",tmpVal);

    lcd_Num(tmpVal,Tracks[pos].Veloci);
    Z_TextValue(5,"Veloci.",tmpVal);

    lcd_Liste(tmpVal,(char *)listMul,Tracks[pos].Modul);
    Z_TextValue(6,"Modula.",tmpVal);

    lcd_Num(tmpVal,Tracks[pos].Ampli);
    Z_TextValue(7,"M.Level",tmpVal);

    lcd_Liste(tmpVal,(char *)listDestin,Tracks[pos].Dest1);
    Z_TextValue(8,"Dest.1",tmpVal);

    lcd_Liste(tmpVal,(char *)listDestin,Tracks[pos].Dest2);
    Z_TextValue(9,"Dest.2",tmpVal);

    lcd_Num(tmpVal,Tracks[pos].Chanel);
    Z_TextValue(10,"Channel",tmpVal);  


    lcd_Liste(tmpVal,(char *)listTTrig, Tracks[pos].Trig[0]);
    Z_TextValue(11,"Trig 1",tmpVal);

    lcd_Liste(tmpVal,(char *)listTTrig, Tracks[pos].Trig[1]);
    Z_TextValue(12,"Trig 2",tmpVal);

    lcd_Liste(tmpVal,(char *)listTTrig, Tracks[pos].Trig[2]);
    Z_TextValue(13,"Trig 3",tmpVal);

    lcd_Liste(tmpVal,(char *)listTTrig, Tracks[pos].Trig[3]);
    Z_TextValue(14,"Trig 4",tmpVal);

    lcd_Liste(tmpVal,(char *)listTTrig, Tracks[pos].Trig[4]);
    Z_TextValue(16,"Trig 5",tmpVal);

    lcd_Liste(tmpVal,(char *)listTTrig, Tracks[pos].Trig[5]);
    Z_TextValue(17,"Trig 6",tmpVal);

    lcd_Liste(tmpVal,(char *)listTTrig, Tracks[pos].Trig[6]);
    Z_TextValue(18,"Trig 7",tmpVal);

    lcd_Liste(tmpVal,(char *)listTTrig, Tracks[pos].Trig[7]);
    Z_TextValue(19,"Trig 8",tmpVal);

    break;

  case MODE_SONG :



    for (pos=0;pos<5;pos++)
    {

      if ( pos+CurTrSong == CurPlaySong )
        lcd1.Carac(pos*8,0,2);
      else
        if ( pos > 0 )            
          lcd1.Carac(pos*8,0,6);
        else
          lcd1.Carac(0,0,32);

      sprintf(tmpCh,"Song %d",pos+CurTrSong+1);
      Z_Text(pos+1,tmpCh);

      lcd_Num(tmpVal,Songs[pos+CurTrSong].Track);
      Z_Value(pos+1,tmpVal);

      lcd_Num(tmpVal,Songs[pos+CurTrSong].Repeat);
      Z_TextValue(pos+6,"Repeat",tmpVal);

      lcd_Num(tmpVal,Songs[pos+CurTrSong].Transpose);
      Z_TextValue(pos+11,"Transp.",tmpVal);

      if ( Songs[pos+CurTrSong].Next == 0)
        sprintf(tmpVal,"Stop");
      else
        lcd_Num(tmpVal,Songs[pos+CurTrSong].Next);
      Z_TextValue(pos+16,"Next",tmpVal);

    }
    break;

  }    


}


void SeqStep::ChangeCurPlaySong(int value) 
{
  if (value == CurPlaySong ) return;
  boolean mustDes=false;
  int pos = CurPlaySong - CurTrSong;
  while ( value < CurTrSong ) 
  {
    CurTrSong--;
    mustDes=true;
  }
  while ( value >= CurTrSong + 5 ) 
  {
    CurTrSong++;
    mustDes=true;
  }
  CurPlaySong = value;
  if ( ActiFunc==NumFunc ) 
  {
    if (mustDes)
    {
      DessinPage();
    }
    else
    {
      if ( pos > 0 )            
        lcd1.Carac(pos*8,0,6);
      else
        lcd1.Carac(0,0,32);
      pos = value - CurTrSong;
      lcd1.Carac(pos*8,0,2);
    }
  }
}



void SeqStep::ChangeCurStepIndex(int value) 
{
  if ( value == CurStepIndex ) return;
  boolean mustDes=false;
  while ( value < CurStep ) 
  {
    CurStep--;
    mustDes=true;
  }
  while ( value >= CurStep + 5 ) 
  {
    CurStep++;
    mustDes=true;
  }
  if (mustDes)
  {
    CurStepIndex = value;
    DessinPage();
  }
  else
  {
    int pos = CurStepIndex - CurStep;
    if ( pos > 0 )            
      lcd1.Carac(pos*8,0,6);
    else
      lcd1.Carac(0,0,32);
    pos = value - CurStep;
    lcd1.Carac(pos*8,0,2);
    CurStepIndex = value;
  }
}


void SeqStep::UpdateCtrl(int cpt)
{
  int pos;

  switch(Mode)
  {
  case MODE_PATT :
    switch(cpt)
    {
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
      pos=CurStep+cpt-1;
      lcd_Note(tmpVal,Tracks[CurPattern].Step[pos].Note);
      Z_Value(cpt,tmpVal);
      break;

    case 6:
    case 7:
    case 8:
    case 9:
    case 10:
      pos=CurStep+cpt-6;
      lcd_Liste(tmpVal,(char *)listAction,Tracks[CurPattern].Step[pos].Action);
      Z_Value(cpt,tmpVal);
      break;

    case 11:
    case 12:
    case 13:
    case 14:
    case 15:
      pos=CurStep+cpt-11;
      lcd_Num(tmpVal,Tracks[CurPattern].Step[pos].V1);
      Z_Value(cpt,tmpVal);
      break;

    case 16:
    case 17:
    case 18:
    case 19:
    case 20:
      pos=CurStep+cpt-16;
      lcd_Num(tmpVal,Tracks[CurPattern].Step[pos].V2);
      Z_Value(cpt,tmpVal);
      break;
    }
    break;

  case MODE_PARAM:

    switch(cpt)
    {
    case 1:
      lcd_Liste(tmpVal,(char *)listCtrl,Tracks[CurPattern].Ctrl1);
      Z_Value(cpt,tmpVal);
      break;

    case 2:
      lcd_Liste(tmpVal,(char *)listCtrl,Tracks[CurPattern].Ctrl2);
      Z_Value(cpt,tmpVal);
      break;

    case 4:
      lcd_Num(tmpVal,Tracks[CurPattern].Duree);
      Z_Value(cpt,tmpVal);
      break;

    case 5:
      lcd_Num(tmpVal,Tracks[CurPattern].Veloci);
      Z_Value(cpt,tmpVal);
      break;

    case 6:
      lcd_Liste(tmpVal,(char *)listMul,Tracks[CurPattern].Modul);
      Z_Value(cpt,tmpVal);
      break;

    case 7:
      lcd_Num(tmpVal,Tracks[CurPattern].Ampli);
      Z_Value(cpt,tmpVal);
      break;

    case 8:
      lcd_Liste(tmpVal,(char *)listDestin,Tracks[CurPattern].Dest1);
      Z_Value(cpt,tmpVal);
      break;

    case 9:
      lcd_Liste(tmpVal,(char *)listDestin,Tracks[CurPattern].Dest2);
      Z_Value(cpt,tmpVal);
      break;    

    case 10:
      lcd_Num(tmpVal,Tracks[CurPattern].Chanel);
      Z_Value(cpt,tmpVal);
      break;

    case 11:
      lcd_Liste(tmpVal,(char *)listTTrig,Tracks[CurPattern].Trig[0]);
      Z_Value(cpt,tmpVal);
      break;

    case 12:
      lcd_Liste(tmpVal,(char *)listTTrig,Tracks[CurPattern].Trig[1]);
      Z_Value(cpt,tmpVal);
      break;
    case 13:
      lcd_Liste(tmpVal,(char *)listTTrig,Tracks[CurPattern].Trig[2]);
      Z_Value(cpt,tmpVal);
      break;

    case 14:
      lcd_Liste(tmpVal,(char *)listTTrig,Tracks[CurPattern].Trig[3]);
      Z_Value(cpt,tmpVal);
      break;

    case 16:
      lcd_Liste(tmpVal,(char *)listTTrig,Tracks[CurPattern].Trig[4]);
      Z_Value(cpt,tmpVal);
      break;

    case 17:
      lcd_Liste(tmpVal,(char *)listTTrig,Tracks[CurPattern].Trig[5]);
      Z_Value(cpt,tmpVal);
      break;

    case 18:
      lcd_Liste(tmpVal,(char *)listTTrig,Tracks[CurPattern].Trig[6]);
      Z_Value(cpt,tmpVal);
      break;

    case 19:
      lcd_Liste(tmpVal,(char *)listTTrig,Tracks[CurPattern].Trig[7]);
      Z_Value(cpt,tmpVal);
      break;

    }
    break;

  case MODE_SONG :
    switch(cpt)
    {
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
      pos=CurTrSong+cpt-1;
      lcd_Num(tmpVal,Songs[pos].Track);
      Z_Value(cpt,tmpVal);
      break;

    case 6:
    case 7:
    case 8:
    case 9:
    case 10:
      pos=CurTrSong+cpt-6;
      lcd_Num(tmpVal,Songs[pos].Repeat);
      Z_Value(cpt,tmpVal);
      break;

    case 11:
    case 12:
    case 13:
    case 14:
    case 15:
      pos=CurTrSong+cpt-11;
      lcd_SNum(tmpVal,Songs[pos].Transpose);
      Z_Value(cpt,tmpVal);
      break;

    case 16:
    case 17:
    case 18:
    case 19:
    case 20:
      pos=CurTrSong+cpt-16;
      if ( Songs[pos].Next == 0)
        sprintf(tmpVal,"Stop");
      else
        lcd_Num(tmpVal,Songs[pos].Next);
      Z_Value(cpt,tmpVal);
      break;
    }
    break;
  }

}

boolean SeqStep::ChangeValue(int cpt, int value, byte Encoder)
{  
  boolean ret = false;
  int pos;
  int newValue;

  // cpt compte de 1 ÃƒÆ’  ...
  if (cpt >=16) cpt-=6;
  cpt++;

  switch(Mode)
  {
  case MODE_PATT :
    switch(cpt)
    {

    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
      pos=CurStep+cpt-1;
      newValue = encnewmap(value, -24, 96, Tracks[CurPattern].Step[pos].Note, Encoder);
      if (newValue != Tracks[CurPattern].Step[pos].Note )
      {
        Tracks[CurPattern].Step[pos].Note = newValue;
        if (is_played==STOPPED) EventOn(pos, micros());
        ret = true;
        UpdateCtrl(cpt);
      };
      break;  

    case 6:
    case 7:
    case 8:
    case 9:
    case 10:
      pos=CurStep+cpt-6;
      newValue = encnewmap(value, 0, 3, Tracks[CurPattern].Step[pos].Action, Encoder);
      if (newValue != Tracks[CurPattern].Step[pos].Action )
      {
        Tracks[CurPattern].Step[pos].Action = newValue;
        ret = true;
        UpdateCtrl(cpt);
      };
      break;  

    case 11:
    case 12:
    case 13:
    case 14:
    case 15:
      pos=CurStep+cpt-11;
      newValue = encnewmap(value, 0, 127, Tracks[CurPattern].Step[pos].V1, Encoder) ;
      if (newValue != Tracks[CurPattern].Step[pos].V1 )
      {
        //      if ( Tracks[pos].IsOn ) EventOff(pos);
        Tracks[CurPattern].Step[pos].V1 = newValue;
        ret = true;
        UpdateCtrl(cpt);
      };
      break;  

    case 16:
    case 17:
    case 18:
    case 19:
    case 20:
      pos=CurStep+cpt-16;
      newValue = encnewmap(value, 0, 127, Tracks[CurPattern].Step[pos].V2, Encoder);
      if (newValue != Tracks[CurPattern].Step[pos].V2 )
      {
        //      if ( Tracks[pos].IsOn ) EventOff(pos);
        Tracks[CurPattern].Step[pos].V2 = newValue;
        ret = true;
        UpdateCtrl(cpt);
      };
      break;  
    }
    break;
  case MODE_PARAM :

    switch(cpt)
    {
    case 1:
      newValue = encnewmap(value, 0, 9,Tracks[CurPattern].Ctrl1 , Encoder);
      if (newValue != Tracks[CurPattern].Ctrl1 )
      {
        Tracks[CurPattern].Ctrl1 = newValue;
        ret = true;
        UpdateCtrl(cpt);
      };
      break;
    case 2:
      newValue = encnewmap(value, 0, 9, Tracks[CurPattern].Ctrl2, Encoder);
      if (newValue != Tracks[CurPattern].Ctrl2 )
      {
        Tracks[CurPattern].Ctrl2 = newValue;
        ret = true;
        UpdateCtrl(cpt);
      };
      break;
    case 4:
      newValue = encnewmap(value, 1, 40, Tracks[CurPattern].Duree, Encoder);
      if (newValue != Tracks[CurPattern].Duree )
      {
        Tracks[CurPattern].Duree = newValue;
        ret = true;
        UpdateCtrl(cpt);
      };
      break;
    case 5:
      newValue = encnewmap(value, 0, 127, Tracks[CurPattern].Veloci , Encoder);
      if (newValue != Tracks[CurPattern].Veloci )
      {
        Tracks[CurPattern].Veloci = newValue;
        ret = true;
        UpdateCtrl(cpt);
      };
      break;
    case 6:
      newValue = encnewmap(value, 0, 4 ,  Tracks[CurPattern].Modul, Encoder);
      if (newValue != Tracks[CurPattern].Modul )
      {
        Tracks[CurPattern].Modul = newValue;
        ret = true;
        UpdateCtrl(cpt);
      };
      break;
    case 7:
      newValue = encnewmap(value, -99, 99, Tracks[CurPattern].Ampli, Encoder );
      if (newValue != Tracks[CurPattern].Ampli )
      {
        Tracks[CurPattern].Ampli = newValue;
        ret = true;
        UpdateCtrl(cpt);
      };
      break;

    case 8:
      newValue = encnewmap(value, 0, NB_OUTS, Tracks[CurPattern].Dest1, Encoder );
      if (newValue != Tracks[CurPattern].Dest1 )
      {
        if ( Tracks[CurPattern].IsOn  ) EventOff();
        SPOut.AutoOff( Tracks[CurPattern].Dest1 );
        Tracks[CurPattern].Dest1 = newValue;
        ret = true;
        UpdateCtrl(cpt);
      };
      break;

    case 9:
      newValue = encnewmap(value, 0, NB_OUTS, Tracks[CurPattern].Dest2, Encoder);
      if (newValue != Tracks[CurPattern].Dest2 )
      {
        if ( Tracks[CurPattern].IsOn  ) EventOff();
        SPOut.AutoOff( Tracks[CurPattern].Dest2 );
        Tracks[CurPattern].Dest2 = newValue;
        ret = true;
        UpdateCtrl(cpt);
      };
      break;

    case 10:
      newValue = encnewmap(value, 1, 16 , Tracks[CurPattern].Chanel, Encoder);
      if (newValue != Tracks[CurPattern].Chanel)
      {
        if ( Tracks[CurPattern].IsOn  ) EventOff();
        SPOut.AutoOff( Tracks[CurPattern].Dest1 );
        SPOut.AutoOff( Tracks[CurPattern].Dest2 );
        Tracks[CurPattern].Chanel = newValue;
        ret = true;
        UpdateCtrl(cpt);
      };
      break;

    case 11:
      newValue = encnewmap(value, 0, 30 , Tracks[CurPattern].Trig[0], Encoder);
      if (newValue != Tracks[CurPattern].Trig[0] )
      {
        Tracks[CurPattern].Trig[0] = newValue;
        ret = true;
        UpdateCtrl(cpt);
      };
      break;

    case 12:
      newValue = encnewmap(value, 0, 30 , Tracks[CurPattern].Trig[1], Encoder);
      if (newValue != Tracks[CurPattern].Trig[1] )
      {
        Tracks[CurPattern].Trig[1] = newValue;
        ret = true;
        UpdateCtrl(cpt);
      };
      break;

    case 13:
      newValue = encnewmap(value, 0, 30 , Tracks[CurPattern].Trig[2], Encoder);
      if (newValue != Tracks[CurPattern].Trig[2] )
      {
        Tracks[CurPattern].Trig[2] = newValue;
        ret = true;
        UpdateCtrl(cpt);
      };
      break;

    case 14:
      newValue = encnewmap(value, 0, 30 , Tracks[CurPattern].Trig[3], Encoder);
      if (newValue != Tracks[CurPattern].Trig[3] )
      {
        Tracks[CurPattern].Trig[3] = newValue;
        ret = true;
        UpdateCtrl(cpt);
      };
      break;

    case 16:
      newValue = encnewmap(value, 0, 30 ,  Tracks[CurPattern].Trig[4], Encoder);
      if (newValue != Tracks[CurPattern].Trig[4] )
      {
        Tracks[CurPattern].Trig[4] = newValue;
        ret = true;
        UpdateCtrl(cpt);
      };
      break;

    case 17:
      newValue = encnewmap(value, 0, 30, Tracks[CurPattern].Trig[5], Encoder );
      if (newValue != Tracks[CurPattern].Trig[5] )
      {
        Tracks[CurPattern].Trig[5] = newValue;
        ret = true;
        UpdateCtrl(cpt);
      };
      break;

    case 18:
      newValue = encnewmap(value, 0, 30 , Tracks[CurPattern].Trig[6], Encoder);
      if (newValue != Tracks[CurPattern].Trig[6] )
      {
        Tracks[CurPattern].Trig[6] = newValue;
        ret = true;
        UpdateCtrl(cpt);
      };
      break;

    case 19:
      newValue = encnewmap(value, 0, 30 , Tracks[CurPattern].Trig[7], Encoder);
      if (newValue != Tracks[CurPattern].Trig[7] )
      {
        Tracks[CurPattern].Trig[7] = newValue;
        ret = true;
        UpdateCtrl(cpt);
      };
      break;


    }
    break;
  case MODE_SONG :
    switch(cpt)
    {

    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
      pos=CurTrSong+cpt-1;
      newValue = encnewmap(value, 1, NB_STEP_TRACK, Songs[pos].Track, Encoder );
      if (newValue != Songs[pos].Track )
      {
        Songs[pos].Track = newValue;
        ret = true;
        UpdateCtrl(cpt);
      };
      break;  

    case 6:
    case 7:
    case 8:
    case 9:
    case 10:
      pos=CurTrSong+cpt-6;
      newValue = encnewmap(value, 1, 16 , Songs[pos].Repeat, Encoder);
      if (newValue != Songs[pos].Repeat )
      {
        Songs[pos].Repeat = newValue;
        ret = true;
        UpdateCtrl(cpt);
      };
      break;  

    case 11:
    case 12:
    case 13:
    case 14:
    case 15:
      pos=CurTrSong+cpt-11;
      newValue = encnewmap(value, -12, +12 , Songs[pos].Transpose, Encoder);
      if (newValue != Songs[pos].Transpose )
      {
        Songs[pos].Transpose = newValue;
        ret = true;
        UpdateCtrl(cpt);
      };
      break;  

    case 16:
    case 17:
    case 18:
    case 19:
    case 20:
      pos=CurTrSong+cpt-16;
      newValue = encnewmap(value, 0, NB_STEP_TRACK, Songs[pos].Next, Encoder );
      if (newValue != Songs[pos].Next )
      {
        Songs[pos].Next = newValue;
        ret = true;
        UpdateCtrl(cpt);
      };
      break;  
    }


  }
  return ret;
}


boolean SeqStep::ChangePage(int value, byte Encoder)
{  
  boolean ret = false;
  int newValue;

  switch(Mode)
  {
  case MODE_PATT:
    newValue = encnewmap(value, 0, NB_STEP_MAX - 5, CurStepIndex, Encoder);
    if ( newValue != CurStepIndex )
    {
      ret = true;
      ChangeCurStepIndex(newValue);
    }
    break;
  case MODE_SONG:
    newValue = encnewmap(value, 0, NB_STEP_SONG - 1, CurTrSong, Encoder);
    if ( newValue != CurTrSong )
    {
      ret = true;
      ChangeCurPlaySong(newValue);
    }
    break;
  }
  return ret;
}


boolean SeqStep::ChangeM1(int value, byte Encoder)
{
  boolean ret = false;
  if ( Menu->ChangeM1(value,Encoder) )
  {
    if ( Menu->FirstPage )
    {  
      ParamGlo.Tempo = Menu->M1;
      SClock.ChangeTempo();
      ret = true;
    }
  }
  return ret;

}

boolean SeqStep::ChangeM2(int value, byte Encoder)
{
  boolean ret = false;
  if ( Menu->ChangeM2(value,Encoder) )
  {
    if ( Menu->FirstPage )
    {  
      mesure = Menu->M2;
      nb24 = CalcNb24(mesure);
      ret = true;
    }
  }
  return ret;
}

boolean SeqStep::ChangeM3(int value, byte Encoder)
{
  boolean ret = false;
  if ( Menu->ChangeM3(value,Encoder) )
  {
    if ( Menu->FirstPage )
    {  
      int newValue =  Menu->M3-1;
      if (is_played==PLAYED)
      {
        NextPattern = newValue;
      }
      else
      {      
        switch(Mode)
        {
        case MODE_PATT:
          if ( newValue != CurPattern )
          {
            ret = true;
            CurPattern = newValue;
            DessinPage();
          }
          break;
        }
      }
      ret = true;
    }
  }
  return ret;
}

void SeqStep::ExStart(boolean pSong)
{
  if ( is_played != STOPPED ) return;
  is_played_Mode_Song = pSong;
  if (is_recorded) is_recorded=false;
  Steep = 0;
  if ( is_played_Mode_Song ) {
    ChangeCurPlaySong(0);
    nbSteep=-1;
    CurPattern = Songs[CurPlaySong].Track-1;
    if ( ActiFunc == MODE_SEQSTEP && Menu->FirstPage) Menu->ShowM3(CurPattern+1);
  }
  is_played = PLAYED;
  SClock.PostStart();
}

void SeqStep::ExStop()
{
  if ( is_played != STOPPED ) {
    SClock.PostStop();
    is_played = STOPPED;
  }
}


boolean SeqStep::ChangeS1S5(int value)
{
  boolean ret = false;
  if ( Menu->ChangeS1S5(value) )
  {
    if ( Menu->FirstPage )
    {  
      switch (Menu->S1S5)
      {
      case 1:
        Mode = (Mode==MODE_PARAM)? MODE_PATT : MODE_PARAM;
        DessinPage();
        break;
      case 2:
        Mode = (Mode==MODE_SONG)? MODE_PATT : MODE_SONG;
        DessinPage();
        break;
      case 3:
        if (is_played==STOPPED) 
        {
          ExStart(Mode==MODE_SONG);
        }
        else
        {
          ExStop();
          if (is_recorded) {
            is_recorded=false;
            ChangeCurStepIndex(0);
          }
        }
        DessinPage();
        break;

      case 4:
        is_recorded = !is_recorded;
        if (is_recorded) {
          pos_record=0;
          ChangeCurStepIndex(0);
          ExStop();
        }
        else
        {
          ChangeCurStepIndex(0);
        }       
        DessinPage();
        break;

      }
    }
    else
    {
      switch (Menu->S1S5)
      {
      case 3:

#ifdef _DEBUG_MODE_
        Serial.print( Menu->curCmd );
        Serial.print( "," );
        Serial.print( Menu->V1 );
        Serial.print( "," );
        Serial.print( Menu->V2 );
        Serial.print( "\n" );
#endif       
        switch( Menu->curCmd )
        {
        case 0:  /* Echo */

          is_echo  = ( Menu->V1==1 ) ;
          break;

        case 1:  /* copy */

          memcpy( &Tracks[Menu->V2-1], &Tracks[Menu->V1-1], sizeof (stTrack) ) ;
          PatLed.Show(0);
          DessinPage();
          break;

        case 2:  /* Load */

          if ( MySD.OpenFileForRead(MODE_SEQSTEP, Menu->GetFName(FileName) ) )
          {
            Restore();
            MySD.CloseFile();
          }
          PatLed.Show(0);
          DessinPage();
          break;

        case 3:  /* Save */

          if ( MySD.OpenFileForWrite(MODE_SEQSTEP, Menu->MenuFName(FileName) ) )
          {
            Backup();
            MySD.CloseFile();
          }
          PatLed.Show(0);
          break;

        case 4:  /* Init */

          Init();
          PatLed.Show(0);
          DessinPage();
          break;

        case 5:  // Re Key
          for (int ct=1; ct < NB_STEP_MAX; ct++ ) 
          {
            Tracks[CurPattern].Step[ct].Note = Tracks[CurPattern].Step[ct].Note - Tracks[CurPattern].Step[0].Note  ;
          }
          Tracks[CurPattern].Step[0].Note = 0;
          PatLed.Show(0);
          DessinPage();
          break;
        }         
      }
    }
  }

  return ret;
}

void  SeqStep::Backup()
{
  MySD.WriteStream((byte *)Tracks, sizeof(Tracks) ) ;
  MySD.WriteStream((byte *)Songs, sizeof(Songs) ) ;
}

void  SeqStep::Restore()
{
  MySD.ReadStream((byte *)Tracks, sizeof(Tracks) ) ;
  MySD.ReadStream((byte *)Songs, sizeof(Songs) ) ;
}



boolean SeqStep::ChangePAT(int value)
{
  boolean ret = false;
  int newValue;
  newValue = PatLed.Map(value);
  if ( newValue > 0 )
  {
    newValue--;
    if (is_played==PLAYED)
    {
      NextPattern = newValue;
    }
    else
    {      
      switch(Mode)
      {
      case MODE_PATT:
        if ( newValue != CurPattern )
        {
          ret = true;
          CurPattern = newValue;
          DessinPage();
        }
        break;
      }
    }
  }
}


void SeqStep::Encoder(byte Value)
{
  switch(lastChangeValue)
  {
  case CHGT_PAGE:
    ChangePage(0, Value); 
    break;
  case CHGT_M1:
    ChangeM1(0, Value); 
    break;
  case CHGT_M2:
    ChangeM2(0, Value); 
    break;
  case CHGT_M3:
    ChangeM3(0, Value); 
    break;
  default :
    ChangeValue(lastChangeValue,0, Value );
    break;
  }
}

void SeqStep::Interface(int Num, int Value)
{
  switch(Num)
  {
  case 0:
  case 1:
  case 2:
  case 3:
  case 4:
  case 5:
  case 6:
  case 7:
  case 8:
  case 9:
  case 16:
  case 17:
  case 18:
  case 19:
  case 20:
  case 21:
  case 22:
  case 23:
  case 24:
  case 25:
    lastChangeValue = Num;
    ChangeValue( Num , Value, 0 );
    break;

  case 10:
    lastChangeValue = CHGT_PAGE;
    if ( ChangePage(Value, 0) ) mxAD.PageChanged();
    break;

  case 11:
    lastChangeValue = CHGT_M1;
    ChangeM1(Value, 0);
    break;

  case 12:
    lastChangeValue = CHGT_M2;
    ChangeM2(Value, 0);
    break;

  case 13:
    lastChangeValue = CHGT_M3;
    ChangeM3(Value, 0);
    break;

  case 32:
    ChangeS1S5(Value);
    break;  

  case 33:
    ChangePAT(Value);
    break;  

  }
}

void SeqStep::Traite(uint32_t reftime)
{
  if ( Tracks[CurPattern].IsOn && Tracks[CurPattern].TimeOff < reftime )
  {
    EventOff();
  }
}

void SeqStep::TraiteEvent(uint32_t reftime)
{
  if (cpt24==0  && is_played==PLAYED)
  {
    if ( ActiFunc==MODE_SEQSTEP )  PatLed.SetStep(Steep+1);

    if ( is_played_Mode_Song )
    {
      if (Steep==0)
      {

        if ( Tracks[CurPattern].IsOn ) EventOff();
        nbSteep++;
        if ( nbSteep >= Songs[CurPlaySong].Repeat ) 
        {
          if ( Songs[CurPlaySong].Next > 0 ) {
            ChangeCurPlaySong( Songs[CurPlaySong].Next-1 );
            CurPattern = Songs[CurPlaySong].Track-1;

            if ( ActiFunc == MODE_SEQSTEP && Menu->FirstPage) Menu->ShowM3(CurPattern+1);
            nbSteep=0;
          }
          else { 
            ExStop();
            lcdM.setCursor(8,3);
            lcdM.print("Play");
          }
        }

      }    
    }
    else
    {
      if (Steep==0 && NextPattern > -1 )
      {
        if ( Tracks[CurPattern].IsOn ) EventOff();
        CurPattern=NextPattern;
        DessinPage();
        NextPattern=-1;
      }    
    }
    switch( Tracks[CurPattern].Step[Steep].Action )
    {
    case 0: /* play */

      EventOn(Steep, reftime);
      Steep++;
      break;

    case 1: /* skip */
      while (  Tracks[CurPattern].Step[Steep].Action==1 )
      {
        Steep++;
        if (Steep==NB_STEP_MAX) {
          Steep=0;
          break;
        }
        else if ( Tracks[CurPattern].Step[Steep].Action==3 ) {
          Steep=0;
          break;
        }
      }
      EventOn(Steep, reftime);
      Steep++;
      break;

    case 2: /* blank */
      Steep++;
      break;
    }   
    if (Steep>=NB_STEP_MAX) 
      Steep=0;
    else 
      if ( Tracks[CurPattern].Step[Steep].Action==3 ) 
      Steep=0;

  }
  else
  {
    if ( ActiFunc==MODE_SEQSTEP && is_played==STOPPED )  PatLed.SetStep(CurPattern+1);
  } 
  //  next_event = next_event + delta_event + ( groove * ( random(delta_event) - delta_event / 2 ) ) / 50;
  if (++cpt24>=nb24) cpt24=0;
}



void SeqStep::EventOff()
{
  SPOut.NoteOff(Tracks[CurPattern].Dest1,Tracks[CurPattern].Played,0,Tracks[CurPattern].Chanel);
  SPOut.NoteOff(Tracks[CurPattern].Dest2,Tracks[CurPattern].Played,0,Tracks[CurPattern].Chanel);
  Tracks[CurPattern].IsOn = false;
}

void SeqStep::EventOn(int pos, uint32_t reftime)
{
  int Veloci;  
  if ( Tracks[CurPattern].IsOn ) EventOff();

  Tracks[CurPattern].Played = Tracks[CurPattern].Step[pos].Note;

  if (Tracks[CurPattern].Played < 24 ) {

    switch( ParamGlo.StSrc )
    {
    case 0 : 
      Tracks[CurPattern].Played = Tracks[CurPattern].Step[pos].Note + ParamGlo.LastNote; 
      LastOffset = ParamGlo.LastNote;
      break;
    case 1 : 
      Tracks[CurPattern].Played = Tracks[CurPattern].Step[pos].Note + ParamGlo.LastUpperNote; 
      LastOffset = ParamGlo.LastUpperNote;
      break;
    case 2 : 
      Tracks[CurPattern].Played = Tracks[CurPattern].Step[pos].Note + ParamGlo.LastLowerNote; 
      LastOffset = ParamGlo.LastLowerNote;
      break; 
    case 3 :
      Tracks[CurPattern].Played = Tracks[CurPattern].Step[pos].Note + LastOffset; 
      break; 
    }
  }

  //  if (Tracks[CurPattern].Played < 24 ) Tracks[CurPattern].Played += MIDI1.LastNote;

  if ( is_played_Mode_Song && Songs[CurPlaySong].Transpose != 0 ) Tracks[CurPattern].Played+= Songs[CurPlaySong].Transpose;

  Veloci = Tracks[CurPattern].Step[pos].V1;

  if ( Tracks[CurPattern].Modul )
  {
    //  char *listMul    ="Off    LF1*VelLF1*NotLF2*VelLF2*Not";
    switch( Tracks[CurPattern].Modul )
    {
    case 1:
      Veloci+= ( LFoBi.LFos[0].Out * ( ( 127 * Tracks[CurPattern].Ampli ) / 100 ) / LFO_RESO ) ;
      break;

    case 2:
      Tracks[CurPattern].Played += ( LFoBi.LFos[0].Out * ( ( 24 * Tracks[CurPattern].Ampli ) / 100 ) / LFO_RESO ) ;
      break;

    case 3:
      Veloci+= ( LFoBi.LFos[1].Out * ( ( 127 * Tracks[CurPattern].Ampli ) / 100 ) / LFO_RESO ) ;
      break;

    case 4:
      Tracks[CurPattern].Played += ( LFoBi.LFos[1].Out * ( ( 24 * Tracks[CurPattern].Ampli ) / 100 ) / LFO_RESO ) ;
      break;

    }
    if (Veloci<0) Veloci=0;
    if (Veloci>127) Veloci=127;
    if (Tracks[CurPattern].Played<1) Tracks[CurPattern].Played=1;
    if (Tracks[CurPattern].Played>127) Tracks[CurPattern].Played=127;
  }   

  SPOut.NoteOn(Tracks[CurPattern].Dest1,Tracks[CurPattern].Played,Veloci,Tracks[CurPattern].Chanel);
  SPOut.NoteOn(Tracks[CurPattern].Dest2,Tracks[CurPattern].Played,Veloci,Tracks[CurPattern].Chanel);
  Tracks[CurPattern].IsOn = true;
  Tracks[CurPattern].TimeOff = reftime + ( delta_event * Tracks[CurPattern].Duree ) / 10 ;
}



void SeqStep::HandleNoteOn(byte Interface, byte channel, byte pitch, byte velocity)
{
  if (is_recorded)
  {
    Tracks[CurPattern].Step[pos_record].Note = pitch;
    Tracks[CurPattern].Step[pos_record].V1 = velocity;
    Tracks[CurPattern].Step[pos_record].Action = 0;
    pos_record++;
    if (pos_record>=NB_STEP_MAX) 
    {
      pos_record--;
      is_recorded=false;
    }
    else
    {
      Tracks[CurPattern].Step[pos_record].Action = 3;
    }
    ChangeCurStepIndex(pos_record);
    DessinPage();
  }
  if ( is_echo && is_played==STOPPED && ActiFunc==NumFunc )
  {
    SPOut.NoteOn(Tracks[CurPattern].Dest1,pitch,velocity,Tracks[CurPattern].Chanel);
    SPOut.NoteOn(Tracks[CurPattern].Dest2,pitch,velocity,Tracks[CurPattern].Chanel);
  }
}


void SeqStep::HandleNoteOff(byte Interface, byte channel, byte pitch, byte velocity)
{
  if ( is_echo && is_played==STOPPED && ActiFunc==NumFunc )
  {
    SPOut.NoteOff(Tracks[CurPattern].Dest1,pitch,velocity,Tracks[CurPattern].Chanel);
    SPOut.NoteOff(Tracks[CurPattern].Dest2,pitch,velocity,Tracks[CurPattern].Chanel);
  }
}


void SeqStep::HandlePitchBend(byte Interface, byte channel, int bend)
{
  if ( is_echo && is_played==STOPPED && ActiFunc==NumFunc )
  {
    SPOut.PitchBend(Tracks[CurPattern].Dest1, bend, Tracks[CurPattern].Chanel);
    SPOut.PitchBend(Tracks[CurPattern].Dest2, bend, Tracks[CurPattern].Chanel);
  }
}

void SeqStep::HandleControlChange(byte Interface, byte channel, byte number, byte value)
{
  if ( is_echo && is_played==STOPPED && ActiFunc==NumFunc )
  {
    SPOut.ControlChange(Tracks[CurPattern].Dest1, number, value, Tracks[CurPattern].Chanel);
    SPOut.ControlChange(Tracks[CurPattern].Dest2, number, value, Tracks[CurPattern].Chanel);
  }
}

void SeqStep::HandleProgramChange(byte Interface, byte channel, byte number)
{
  if ( is_echo && is_played==STOPPED && ActiFunc==NumFunc )
  {
    SPOut.ProgramChange(Tracks[CurPattern].Dest1, number, Tracks[CurPattern].Chanel);
    SPOut.ProgramChange(Tracks[CurPattern].Dest2, number, Tracks[CurPattern].Chanel);
  }

}

void SeqStep::HandleAfterTouch(byte Interface, byte channel, byte value)
{
  if ( is_echo && is_played==STOPPED && ActiFunc==NumFunc )
  {
    SPOut.AfterTouch(Tracks[CurPattern].Dest1, value, Tracks[CurPattern].Chanel);
    SPOut.AfterTouch(Tracks[CurPattern].Dest2, value, Tracks[CurPattern].Chanel);
  }
}






