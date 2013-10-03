// -----------------------------------------------
// LL113
// -----------------------------------------------
// Historique:
// (Baloran) laurent.lecatelier@free.fr, version 0.0.1  10/02/12013
// -----------------------------------------------
// utilld.cpp Code Class Menu, Leds et fonctions diverses

#include <LiquidCrystal.h>
#include <MIDIMul.h> 
#include "Arduino.h"
#include "utilld.h"
#include "global.h"
#include "multiplexAD.h"
#include "SDFile.h"
#include "arpege.h"
#include "lfo.h"
#include "seqeuclid.h"
#include "seqstep.h"
#include "seqtr.h"
#include "clock.h"
#include "MCP.h"
// Clock
extern SysClock SClock;

extern SuperOut SPOut;

extern SDFile MySD;
extern mPatLed PatLed;

extern multiplexAD mxAD;

extern MIDIMul_Class MIDI1;
extern MIDIMul_Class MIDI2;
extern MIDIMul_Class MIDI3;

extern Arpege Arp;
extern SeqEuclid SeqEu;
extern SeqStep SeqSt;
extern SeqTrxx SeqTr;
extern LFo LFoBi;

const char *GloTitre="\x01 Main Page (v0.99)\x02";

/*

 enum kRoute {
 	Route_None            = 0x00,  
 	Route_SysEx           = 0x01,  
 	Route_Real            = 0x02,
 	Route_Canal           = 0x04,  
 	Route_All             = 0x07   
 };
 
 enum kCanal {
 	Can_All               = 0xFFFF,  
 	Can_None              = 0x0000,  
 	Can_1            	  = 0x0001,
 	Can_2            	  = 0x0002,
 	Can_3            	  = 0x0004,
 	Can_4            	  = 0x0008,
 	Can_5            	  = 0x0010,
 	Can_6            	  = 0x0020,
 	Can_7            	  = 0x0040,
 	Can_8            	  = 0x0080,
 	Can_9            	  = 0x0100,
 	Can_10            	  = 0x0200,
 	Can_11            	  = 0x0400,
 	Can_12           	  = 0x0800,
 	Can_13           	  = 0x1000,
 	Can_14           	  = 0x2000,
 	Can_15           	  = 0x4000,
 	Can_16           	  = 0x8000
 };
 
 */


#define RT_MIDI    0
#define RT_CV      1
#define RT_TRIG    2


#define PER_NONE 0
#define PER_GLOBAL 1
#define PER_ALWAY 2


const char listMidiType[] ="Notes  VelocitPressur";
const char listcSource[]  ="Intern Midi 1 Midi 2 Midi 3 Trg In Dyn ClkCV In  ";
const char listgSource[]  ="Midi 1 Midi 2 Midi 3 CV In  ";
const char listPerim[]    ="Off    Main   Alway  ";
const char listChan[]     ="All    1      2      3      4      5      6      7      8      9      10     11     12     13     14     15     16     1..4   5..8   9..12  13..16 1..8   9..16  ";
uint16_t chankCanal[]  ={ 
  Can_All, Can_1, Can_2, Can_3, Can_4, Can_5, Can_6, Can_7, Can_8, Can_9, Can_10, Can_11, Can_12, Can_13, Can_14, Can_15, Can_16, 0x000F, 0x00F0,   0x0F00,  0xF000, 0x00FF, 0xFF00 };
const char listRoute[]    ="All    ChannelRealtimChn+ReaSysEx  ";
uint8_t routkRoute[] =     { 
  Route_All, Route_Canal, Route_Canal | Route_Real, Route_SysEx };

const char listSplit[]    ="Off    On     ";
const char listCVTrig[]   ="Single ReTrig Poly 2 Poly 3 Poly 4 Poly 5 Poly 6 Poly 7 Poly 8 ";

const char listgArpSrc[]  ="Input  Upper  Lower  Off    "; 
const char listTypeCV[]   ="V/Oct  V/Hz   ";
const char listTypeTrig[] ="V.Trig S.Trig ";

const char listmInit[]  ="Param All   ";

extern const char *listUpDown;
extern const char *listOffOn;

// char *listgType    ="All    ChannelRealtimChn+ReaSysEx  ";

#define NB_PAGES 28
#define NB_ROUTES (9+8+8)

strRoutes Routes[25]= {

  { 
    RT_MIDI, "MIDI1>1", PER_NONE, 1, 0, 0, 0                 }
  , 
  { 
    RT_MIDI, "MIDI1>2", PER_NONE, 2, 0, 0, 0                 }
  , 
  { 
    RT_MIDI, "MIDI1>3", PER_NONE, 3, 0, 0, 0                 }
  , 
  { 
    RT_MIDI, "MIDI2>1", PER_NONE, 4, 0, 0, 0                 }
  , 
  { 
    RT_MIDI, "MIDI2>2", PER_NONE, 5, 0, 0, 0                 }
  , 
  { 
    RT_MIDI, "MIDI2>3", PER_NONE, 6, 0, 0, 0                 }
  , 
  { 
    RT_MIDI, "MIDI3>1", PER_NONE, 7, 0, 0, 0                 }
  , 
  { 
    RT_MIDI, "MIDI3>2", PER_NONE, 8, 0, 0, 0                 }
  , 
  { 
    RT_MIDI, "MIDI3>3", PER_NONE, 9, 0, 0, 0                 }
  , 

  { 
    RT_CV, "CV 1", PER_NONE, 10, 0, 0, 0                 }
  , 
  { 
    RT_CV, "CV 2", PER_NONE, 11, 0, 0, 0                 }
  , 
  { 
    RT_CV, "CV 3", PER_NONE, 12, 0, 0, 0                 }
  , 
  { 
    RT_CV, "CV 4", PER_NONE, 13, 0, 0, 0                 }
  , 
  { 
    RT_CV, "CV 5", PER_NONE, 14, 0, 0, 0                 }
  , 
  { 
    RT_CV, "CV 6", PER_NONE, 15, 0, 0, 0                 }
  , 
  { 
    RT_CV, "CV 7", PER_NONE, 16, 0, 0, 0                 }
  , 
  { 
    RT_CV, "CV 8", PER_NONE, 17, 0, 0, 0                 }
  , 


  { 
    RT_TRIG, "TRIG 1", PER_NONE, 18, 0, 0, 0                 }
  , 
  { 
    RT_TRIG, "TRIG 2", PER_NONE, 19, 0, 0, 0                 }
  , 
  { 
    RT_TRIG, "TRIG 3", PER_NONE, 20, 0, 0, 0                 }
  , 
  { 
    RT_TRIG, "TRIG 4", PER_NONE, 21, 0, 0, 0                 }
  , 
  { 
    RT_TRIG, "TRIG 5", PER_NONE, 22, 0, 0, 0                 }
  , 
  { 
    RT_TRIG, "TRIG 6", PER_NONE, 23, 0, 0, 0                 }
  , 
  { 
    RT_TRIG, "TRIG 7", PER_NONE, 24, 0, 0, 0                 }
  , 
  { 
    RT_TRIG, "TRIG 8", PER_NONE, 25, 0, 0, 0                 }
  , 

};

paramMenuList MenuListGlo[] = {
  { 
    "Load", "Name", "", 0, MODE_PARAM, -1, 0 , 0, 0, 0, 0                     }
  ,
  { 
    "Save", "Name", "Indice", 0, 0, NB_STYLEMU, listStyleMenu , 1, 1, 64, 0                       }
  , 
  { 
    "Lo.All", "Name", "", 0, MODE_GLOBAL, -1, 0 , 0, 0, 0, 0           }
  ,
  { 
    "Sa.All", "Name", "Indice", 0, 0, NB_STYLEMU, listStyleMenu , 1, 1, 64, 0             }
  , 
  { 
    "Init", "What ?", "", 0, 0, 1, (char *)listmInit , 0, 0, 0, 0                       }
  , 
  { 
    "DAC", "Volt", "", 0, 0, 10, 0 , 0, 0, 0, 0                       }
  , 
  { 
    "Trig", "Stat", "", 0, 0, 1, 0 , 0, 0, 0, 0                       }
  , 
  { 
    "", "", "", 0, 0, 0, 0 , 0, 0, 0, 0                                         }
};


paramMenu menuGlo = {
  "Tap",
  "Sng",
  "Play",
  "",
  "BPM",          
  "",     
  "Groove",          
  100,30,300,0,     
  0,0,0,0,     
  0,0,9,0,     
  MenuListGlo
};




Global::Global()
{
  Menu = new mMenu(&menuGlo, GloTitre);

  Init();

}


Global::~Global()
{
}


void Global::Init()
{

  lastChangeValue = CHGT_PAGE;
  memset(tapTempo,0, sizeof(tapTempo));

  CurPage=0;

  Recorder.nb_Rec_Record = 0;
  Recorder.cur_Rec_Record = 0;
  Recorder.cur_Play_Record = 0;
  Recorder.mode_Record = RECORD_OFF;

  ParamGlo.Tempo = 100;
  ParamGlo.Groove = 0;

  ParamGlo.Active[MODE_SEQEUCLID]=0;
  ParamGlo.Active[MODE_SEQSTEP]=0;
  ParamGlo.Active[MODE_SEQTR]=0;
  ParamGlo.Active[MODE_ARPEGE]=1;

  ParamGlo.keyInput=0;
  ParamGlo.canInput=0;
  ParamGlo.Split=0;
  ParamGlo.LastNote=ParamGlo.LastLowerNote=ParamGlo.LastUpperNote=ParamGlo.NoteSplit=48;
  ParamGlo.UpperDest=NOUT_NONE;
  ParamGlo.UpperTrans=0;
  ParamGlo.LowerDest=NOUT_NONE;
  ParamGlo.LowerTrans=0;
  ParamGlo.LowerChan=1;
  ParamGlo.CVTrig=0;

  ParamGlo.ArpSrc=0;
  ParamGlo.EuSrc=0;
  ParamGlo.StSrc=0;
  ParamGlo.TrSrc=0;

  ParamGlo.ClockSrc=CLOCK_INT;
  ParamGlo.ClockDiv=1;

  ParamGlo.TrigInDir=1;

  ParamGlo.Bend=2;
  memset(ParamGlo.FINE_CV,0,sizeof(int16_t)*8);
  
  SClock.SetModeClock(ParamGlo.ClockSrc, ParamGlo.ClockDiv);

  for (int ct=0;ct<NB_ROUTES;ct++)
  {
    Routes[ct].Perim = PER_NONE;
    Routes[ct].Source = 0;
    Routes[ct].Chanel = 0;
    Routes[ct].MidiType = 0;
  }   

  UpdateSplit();
} 


#define MODE_SEQEUCLID  1
#define MODE_SEQSTEP    2
#define MODE_SEQTR      3
#define MODE_ARPEGE     4
#define MODE_LFO        5
#define MODE_EDITEUR_1  6
#define MODE_EDITEUR_2  7

void Global::DessinPage()
{
  int offset;
  Menu->DessinPage();
  if ( Menu->FirstPage )
  {  
    if (is_played) {
      lcdM.setCursor(8,3);
      lcdM.print("Stop");
    }
    else
    {
      lcdM.setCursor(8,3);
      lcdM.print("Play");
    }

    switch (Recorder.mode_Record)
    {
    case  RECORD_OFF:
      lcdM.setCursor(13,3);
      lcdM.print("Off");
      break;

    case  RECORD_PLAY:
      lcdM.setCursor(13,3);
      lcdM.print("Ply");
      break;

    case  RECORD_ON:
      lcdM.setCursor(13,3);
      lcdM.print("Rec");
      break;
    }
  }
  Z_Clear();

  switch (CurPage)
  {
  case 0 :

    Z_Titre(1,"\x02Main");


    lcd_Liste(tmpVal,(char *)listOffOn,ParamGlo.Active[MODE_SEQEUCLID]);
    Z_TextValue(2,"Euc.Seq",tmpVal);

    lcd_Liste(tmpVal,(char *)listOffOn,ParamGlo.Active[MODE_SEQSTEP]);
    Z_TextValue(3,"Stp.Seq",tmpVal);

    lcd_Liste(tmpVal,(char *)listOffOn,ParamGlo.Active[MODE_SEQTR]);
    Z_TextValue(4,"Trx.Seq",tmpVal);

    lcd_Liste(tmpVal,(char *)listOffOn,ParamGlo.Active[MODE_ARPEGE]);
    Z_TextValue(5,"Arpegia",tmpVal);


    lcd_Liste(tmpVal,(char *)listgArpSrc,ParamGlo.EuSrc);
    Z_TextValue(7,"Key Src",tmpVal);

    lcd_Liste(tmpVal,(char *)listgArpSrc,ParamGlo.StSrc);
    Z_TextValue(8,"Key Src",tmpVal);

    lcd_Liste(tmpVal,(char *)listgArpSrc,ParamGlo.TrSrc);
    Z_TextValue(9,"Key Src",tmpVal);

    lcd_Liste(tmpVal,(char *)listgArpSrc,ParamGlo.ArpSrc);
    Z_TextValue(10,"Key Src",tmpVal);

    lcd_Liste(tmpVal,(char *)listgSource,ParamGlo.keyInput);
    Z_TextValue(11,"Key in",tmpVal);

    lcd_Liste(tmpVal,(char *)listOffOn,ParamGlo.Split);
    Z_TextValue(12,"Split",tmpVal);

    lcd_Liste(tmpVal,(char *)listDestin,ParamGlo.UpperDest);
    Z_TextValue(13,"Upper",tmpVal);

    lcd_Liste(tmpVal,(char *)listDestin,ParamGlo.LowerDest);
    Z_TextValue(14,"Lower",tmpVal);

    lcd_Liste(tmpVal,(char *)listCVTrig,ParamGlo.CVTrig);
    Z_TextValue(15,"CV Mode",tmpVal);


    lcd_Liste(tmpVal,(char *)listChan,ParamGlo.canInput);
    Z_TextValue(16,"Channel",tmpVal);

    lcd_NoteAbs(tmpVal,ParamGlo.NoteSplit);
    Z_TextValue(17,"Sp.Note",tmpVal);

    lcd_Octa(tmpVal,ParamGlo.UpperTrans);
    Z_TextValue(18,"Transp.",tmpVal);

    lcd_Octa(tmpVal,ParamGlo.LowerTrans);
    Z_TextValue(19,"Transp.",tmpVal);

    lcd_Num(tmpVal,ParamGlo.LowerChan);
    Z_TextValue(20,"Low Chn",tmpVal);

    break;

  case 1 :

    Z_Titre(1,"\x02Param");

    lcd_Liste(tmpVal,(char *)listcSource,ParamGlo.ClockSrc);
    Z_TextValue(2,"Clock",tmpVal);

    lcd_Num(tmpVal,ParamGlo.ClockDiv);
    Z_TextValue(3,"Divise",tmpVal);

    lcd_Liste(tmpVal,(char *)listUpDown,ParamGlo.TrigInDir);
    Z_TextValue(4,"Trig In",tmpVal);

    lcd_Num(tmpVal,ParamGlo.Bend);
    Z_TextValue(5,"Bend",tmpVal);

    Z_Titre(11,"\x02TUNE");
    for (int ct=0;ct<8;ct++)
    {
      lcd_SNum(tmpVal,ParamGlo.FINE_CV[ct]);
      sprintf(tmpCh,"CV %d",ct+1);
      if (ct < 4)
        Z_TextValue( 12 + ct ,tmpCh,tmpVal);
      else
        Z_TextValue( 13 + ct ,tmpCh,tmpVal);
    }
    break;

    break;

  case 2 :
    offset = CurPage-2;

    Z_Titre(1,"\x02\CV");
    for (int ct=0;ct<8;ct++)
    {
      lcd_Liste(tmpVal,(char *)listTypeCV,CV_Type[ct]);
      sprintf(tmpCh,"   %d",ct+1);
      if (ct < 4)
        Z_TextValue( 2 + ct ,tmpCh,tmpVal);
      else
        Z_TextValue( 3 + ct ,tmpCh,tmpVal);
    }

    Z_Titre(11,"\x02TRIG");
    for (int ct=0;ct<8;ct++)
    {
      lcd_Liste(tmpVal,(char *)listTypeTrig,TRIG_Type[ct]);
      sprintf(tmpCh,"   %d",ct+1);
      if (ct < 4)
        Z_TextValue( 12 + ct ,tmpCh,tmpVal);
      else
        Z_TextValue( 13 + ct ,tmpCh,tmpVal);
    }
    break;

  default :

    offset = CurPage-3;

    for (int ct=0;ct<5;ct++)
    {

      lcd_Liste(tmpVal,(char *)listPerim,Routes[ct + offset].Perim);
      Z_TextValue(1+ct,Routes[ct + offset].Name,tmpVal);

      switch(Routes[ct + offset].Type )
      {
      case RT_MIDI:
        lcd_Liste(tmpVal,(char *)listRoute,Routes[ct + offset].Source);
        Z_TextValue(6+ct,"Route",tmpVal);
        break;
      default:
        lcd_Liste(tmpVal,(char *)listgSource,Routes[ct + offset].Source);
        Z_TextValue(6+ct,"Source",tmpVal);
        break;
      }

      lcd_Liste(tmpVal,(char *)listChan,Routes[ct + offset].Chanel);
      Z_TextValue(11+ct,"Channel",tmpVal);

      switch(Routes[ct + offset].Type )
      {
      case RT_CV:
      case RT_TRIG:
        if ( Routes[ct + offset].MidiType < 3)
          lcd_Liste(tmpVal,(char *)listMidiType,Routes[ct + offset].MidiType);
        else
          sprintf(tmpVal,"CC %3d ",  Routes[ct + offset].MidiType - 3 ); 
        Z_TextValue(16+ct,"Origine",tmpVal);
        break;
      }

    }
    break;
  }

}

void Global::UpdateCtrl(int cpt)
{
  int pos;
  switch (CurPage)
  {
  case 0 :

    switch(cpt)
    {
    case 2:
      lcd_Liste(tmpVal,(char *)listOffOn,ParamGlo.Active[MODE_SEQEUCLID]);
      Z_Value(cpt,tmpVal);
      break;

    case 3:
      lcd_Liste(tmpVal,(char *)listOffOn,ParamGlo.Active[MODE_SEQSTEP]);
      Z_Value(cpt,tmpVal);
      break;

    case 4:
      lcd_Liste(tmpVal,(char *)listOffOn,ParamGlo.Active[MODE_SEQTR]);
      Z_Value(cpt,tmpVal);
      break;

    case 5:
      lcd_Liste(tmpVal,(char *)listOffOn,ParamGlo.Active[MODE_ARPEGE]);
      Z_Value(cpt,tmpVal);
      break;


    case 7:
      lcd_Liste(tmpVal,(char *)listgArpSrc,ParamGlo.EuSrc);
      Z_Value(cpt,tmpVal);
      break;

    case 8:
      lcd_Liste(tmpVal,(char *)listgArpSrc,ParamGlo.StSrc);
      Z_Value(cpt,tmpVal);
      break;

    case 9:
      lcd_Liste(tmpVal,(char *)listgArpSrc,ParamGlo.TrSrc);
      Z_Value(cpt,tmpVal);
      break;

    case 10:
      lcd_Liste(tmpVal,(char *)listgArpSrc,ParamGlo.ArpSrc);
      Z_Value(cpt,tmpVal);
      break;

    case 11:
      lcd_Liste(tmpVal,(char *)listgSource,ParamGlo.keyInput);
      Z_Value(cpt,tmpVal);
      break;

    case 12:
      lcd_Liste(tmpVal,(char *)listOffOn,ParamGlo.Split);
      Z_Value(cpt,tmpVal);
      break;

    case 13:
      lcd_Liste(tmpVal,(char *)listDestin,ParamGlo.UpperDest);
      Z_Value(cpt,tmpVal);
      break;

    case 14:
      lcd_Liste(tmpVal,(char *)listDestin,ParamGlo.LowerDest);
      Z_Value(cpt,tmpVal);
      break;

    case 15:
      lcd_Liste(tmpVal,(char *)listCVTrig,ParamGlo.CVTrig);
      Z_Value(cpt,tmpVal);
      break;

    case 16:
      lcd_Liste(tmpVal,(char *)listChan,ParamGlo.canInput);
      Z_Value(cpt,tmpVal);
      break;

    case 17:
      lcd_NoteAbs(tmpVal,ParamGlo.NoteSplit);
      Z_Value(cpt,tmpVal);
      break;

    case 18:
      lcd_Octa(tmpVal,ParamGlo.UpperTrans);
      Z_Value(cpt,tmpVal);
      break;

    case 19:
      lcd_Octa(tmpVal,ParamGlo.LowerTrans);
      Z_Value(cpt,tmpVal);
      break;

    case 20:
      lcd_Num(tmpVal,ParamGlo.LowerChan);
      Z_Value(cpt,tmpVal);
      break;


    }    
    break;

  case 1:
  
    switch(cpt)
    {
    case 2:
      lcd_Liste(tmpVal,(char *)listcSource,ParamGlo.ClockSrc);
      Z_Value(cpt,tmpVal);
      break;

    case 3:
      lcd_Num(tmpVal,ParamGlo.ClockDiv);
      Z_Value(cpt,tmpVal);
      break;

    case 4:
      lcd_Liste(tmpVal,(char *)listUpDown,ParamGlo.TrigInDir);
      Z_Value(cpt,tmpVal);
      break;

    case 5:
      lcd_Num(tmpVal,ParamGlo.Bend);
      Z_Value(cpt,tmpVal);
      break;

    case 12:
    case 13:
    case 14:
    case 15:
    case 17:
    case 18:
    case 19:
    case 20:
      pos=(cpt > 15)?cpt-13:cpt-12;
      lcd_SNum(tmpVal,ParamGlo.FINE_CV[pos]);
      Z_Value(cpt,tmpVal);
      break;
    }
    break;



  case 2:

    switch(cpt)
    {
    case 2:
    case 3:
    case 4:
    case 5:
    case 7:
    case 8:
    case 9:
    case 10:
      pos=(cpt > 5)?cpt-3:cpt-2;
      lcd_Liste(tmpVal,(char *)listTypeCV,CV_Type[pos]);
      Z_Value(cpt,tmpVal);
      break;

    case 12:
    case 13:
    case 14:
    case 15:
    case 17:
    case 18:
    case 19:
    case 20:
      pos=(cpt > 15)?cpt-13:cpt-12;
      lcd_Liste(tmpVal,(char *)listTypeTrig,TRIG_Type[pos]);
      Z_Value(cpt,tmpVal);
      break;
    }
    break;

  default:
    int offset = CurPage-3;


    switch(cpt)
    {
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
      pos=cpt-1;
      lcd_Liste(tmpVal,(char *)listPerim,Routes[pos + offset].Perim);
      Z_Value(cpt,tmpVal);
      break;

    case 6:
    case 7:
    case 8:
    case 9:
    case 10:
      pos=cpt-6;

      switch(Routes[pos + offset].Type )
      {
      case RT_MIDI:
        lcd_Liste(tmpVal,(char *)listRoute,Routes[pos + offset].Source);
        break;
      default:
        lcd_Liste(tmpVal,(char *)listgSource,Routes[pos + offset].Source);
        break;
      }
      Z_Value(cpt,tmpVal);
      break;

    case 11:
    case 12:
    case 13:
    case 14:
    case 15:
      pos=cpt-11;
      lcd_Liste(tmpVal,(char *)listChan,Routes[pos + offset].Chanel);
      Z_Value(cpt,tmpVal);
      break;

    case 16:
    case 17:
    case 18:
    case 19:
    case 20:
      pos=cpt-16;
      if ( Routes[pos + offset].MidiType < 3)
        lcd_Liste(tmpVal,(char *)listMidiType,Routes[pos + offset].MidiType);
      else
        sprintf(tmpVal,"CC %3d ",  Routes[pos + offset].MidiType - 3 ); 
      Z_Value(cpt,tmpVal);
      break;

    }
    break;
  }

}

boolean Global::ChangeValue(int cpt, int value, byte Encoder)
{  
  boolean ret = false;
  int newValue;
  int pos;
  int offset;
  
  // cpt compte de 1 ÃƒÆ’  ...
  if (cpt >=16) cpt-=6;
  cpt++;

  switch (CurPage)
  {
  case 0 :
    switch(cpt)
    {
    case 2:
      newValue = encnewmap(value, 0, 2, ParamGlo.Active[MODE_SEQEUCLID] ,Encoder);
      if (newValue != ParamGlo.Active[MODE_SEQEUCLID] )
      {
        ParamGlo.Active[MODE_SEQEUCLID] = newValue;
        if (  ParamGlo.Active[MODE_SEQEUCLID] && is_played && !SeqEu.is_played ) SeqEu.ExStart(false);
        if (  !ParamGlo.Active[MODE_SEQEUCLID] && is_played && SeqEu.is_played ) SeqEu.ExStop();
        ret = true;
        UpdateCtrl(cpt);
      };
      break;

    case 3:
      newValue = encnewmap(value, 0, 2, ParamGlo.Active[MODE_SEQSTEP],Encoder);
      if (newValue != ParamGlo.Active[MODE_SEQSTEP] )
      {
        ParamGlo.Active[MODE_SEQSTEP] = newValue;
        if (  ParamGlo.Active[MODE_SEQSTEP] && is_played && !SeqSt.is_played ) SeqSt.ExStart(false);
        if (  !ParamGlo.Active[MODE_SEQSTEP] && is_played && SeqSt.is_played ) SeqSt.ExStop();
        ret = true;
        UpdateCtrl(cpt);
      };
      break;

    case 4:
      newValue = encnewmap(value, 0, 2, ParamGlo.Active[MODE_SEQTR]  ,Encoder);
      if (newValue != ParamGlo.Active[MODE_SEQTR] )
      {
        ParamGlo.Active[MODE_SEQTR] = newValue;
        if (  ParamGlo.Active[MODE_SEQTR] && is_played && !SeqTr.is_played ) SeqTr.ExStart(false);
        if (  !ParamGlo.Active[MODE_SEQTR] && is_played && SeqTr.is_played ) SeqTr.ExStop();
        ret = true;
        UpdateCtrl(cpt);
      };
      break;

    case 5:
      newValue = encnewmap(value, 0, 2, ParamGlo.Active[MODE_ARPEGE],Encoder);
      if (newValue != ParamGlo.Active[MODE_ARPEGE] )
      {
        ParamGlo.Active[MODE_ARPEGE] = newValue;
        if (  ParamGlo.Active[MODE_ARPEGE] && is_played && !Arp.is_played ) Arp.ExStart(false);
        if (  !ParamGlo.Active[MODE_ARPEGE] && is_played && Arp.is_played ) Arp.ExStop();
        ret = true;
        UpdateCtrl(cpt);
      };
      break;

    case 7:
      newValue = encnewmap(value, 0, 3, ParamGlo.EuSrc,Encoder);
      if (newValue != ParamGlo.EuSrc )
      {
        switch( ParamGlo.EuSrc )
        {
          case 0: LastOffset = ParamGlo.LastNote;break;
          case 1: LastOffset = ParamGlo.LastUpperNote;break;
          case 2: LastOffset = ParamGlo.LastLowerNote;break;
        }
        ParamGlo.EuSrc = newValue;
        switch( ParamGlo.EuSrc )
        {
          case 0: ParamGlo.LastNote = LastOffset; break;
          case 1: ParamGlo.LastUpperNote = LastOffset; break;
          case 2: ParamGlo.LastLowerNote = LastOffset; break;
        }
        ret = true;
        UpdateCtrl(cpt);
        UpdateSplit();
      };
      break;

    case 8:
      newValue = encnewmap(value, 0, 3, ParamGlo.StSrc,Encoder);
      if (newValue != ParamGlo.StSrc )
      {
        switch( ParamGlo.StSrc )
        {
          case 0: LastOffset = ParamGlo.LastNote;break;
          case 1: LastOffset = ParamGlo.LastUpperNote;break;
          case 2: LastOffset = ParamGlo.LastLowerNote;break;
        }
        ParamGlo.StSrc = newValue;
        switch( ParamGlo.StSrc )
        {
          case 0: ParamGlo.LastNote = LastOffset; break;
          case 1: ParamGlo.LastUpperNote = LastOffset; break;
          case 2: ParamGlo.LastLowerNote = LastOffset; break;
        }
        ret = true;
        UpdateCtrl(cpt);
        UpdateSplit();
      };
      break;


    case 9:
      newValue = encnewmap(value, 0, 3, ParamGlo.TrSrc,Encoder);
      if (newValue != ParamGlo.TrSrc )
      {
        switch( ParamGlo.TrSrc )
        {
          case 0: LastOffset = ParamGlo.LastNote;break;
          case 1: LastOffset = ParamGlo.LastUpperNote;break;
          case 2: LastOffset = ParamGlo.LastLowerNote;break;
        }
        ParamGlo.TrSrc = newValue;
        switch( ParamGlo.TrSrc )
        {
          case 0: ParamGlo.LastNote = LastOffset; break;
          case 1: ParamGlo.LastUpperNote = LastOffset; break;
          case 2: ParamGlo.LastLowerNote = LastOffset; break;
        }
        ret = true;
        UpdateCtrl(cpt);
        UpdateSplit();
      };
      break;

    case 10:
      newValue = encnewmap(value, 0, 3, ParamGlo.ArpSrc,Encoder);
      if (newValue != ParamGlo.ArpSrc )
      {
        switch( ParamGlo.ArpSrc )
        {
          case 0: LastOffset = ParamGlo.LastNote;break;
          case 1: LastOffset = ParamGlo.LastUpperNote;break;
          case 2: LastOffset = ParamGlo.LastLowerNote;break;
        }
        ParamGlo.ArpSrc = newValue;
        switch( ParamGlo.ArpSrc )
        {
          case 0: ParamGlo.LastNote = LastOffset; break;
          case 1: ParamGlo.LastUpperNote = LastOffset; break;
          case 2: ParamGlo.LastLowerNote = LastOffset; break;
        }
        ret = true;
        UpdateCtrl(cpt);
        UpdateSplit();
      };
      break;

    case 11:
      newValue = encnewmap(value, 0, 4, ParamGlo.keyInput,Encoder);
      if (newValue != ParamGlo.keyInput )
      {
        ParamGlo.keyInput = newValue;
        ret = true;
        UpdateCtrl(cpt);
        UpdateSplit();
      };
      break;

    case 12:
      newValue = encnewmap(value, 0, 1, ParamGlo.Split,Encoder);
      if (newValue !=ParamGlo.Split )
      {
        SPOut.AutoOff( ParamGlo.UpperDest );
        SPOut.AutoOff( ParamGlo.LowerDest );
        ParamGlo.Split = newValue;
        ret = true;
        UpdateCtrl(cpt);
        UpdateSplit();
      };
      break;

    case 13:
      newValue = encnewmap(value, 0, NB_OUTS , ParamGlo.UpperDest,Encoder);
      if (newValue != ParamGlo.UpperDest )
      {
        SPOut.AutoOff( ParamGlo.UpperDest );
        ParamGlo.UpperDest = newValue;
        ret = true;
        UpdateCtrl(cpt);
      };
      break;

    case 14:
      newValue = encnewmap(value, 0, NB_OUTS, ParamGlo.LowerDest,Encoder );
      if (newValue != ParamGlo.LowerDest )
      {
        SPOut.AutoOff( ParamGlo.LowerDest );
        ParamGlo.LowerDest = newValue;
        ret = true;
        UpdateCtrl(cpt);
      };
      break;

    case 15:
      newValue = encnewmap(value, 0, 6 , ParamGlo.CVTrig,Encoder);
      if (newValue != ParamGlo.CVTrig )
      {
        ParamGlo.CVTrig = newValue;
        ret = true;
        UpdateCtrl(cpt);
      };
      break;

    case 16:
      newValue = encnewmap(value, 0, 20 , ParamGlo.canInput,Encoder);
      if (newValue != ParamGlo.canInput )
      {
        ParamGlo.canInput = newValue;
        ret = true;
        UpdateCtrl(cpt);
      };
      break;

    case 17:
      newValue = encnewmap(value, 12, 72 , ParamGlo.NoteSplit,Encoder);
      if (newValue != ParamGlo.NoteSplit )
      {
        SPOut.AutoOff( ParamGlo.UpperDest );
        SPOut.AutoOff( ParamGlo.LowerDest );
        ParamGlo.NoteSplit = newValue;
        ret = true;
        UpdateCtrl(cpt);
        UpdateSplit();
      };
      break;


    case 18:
      newValue = encnewmap(value, -3, 3 , ParamGlo.UpperTrans,Encoder);
      if (newValue != ParamGlo.UpperTrans )
      {
        SPOut.AutoOff( ParamGlo.UpperDest );
        ParamGlo.UpperTrans = newValue;
        ret = true;
        UpdateCtrl(cpt);
        UpdateSplit();
      };
      break;


    case 19:
      newValue = encnewmap(value, -3, 3 , ParamGlo.LowerTrans,Encoder);
      if (newValue != ParamGlo.LowerTrans )
      {
        SPOut.AutoOff( ParamGlo.LowerDest );
        ParamGlo.LowerTrans = newValue;
        ret = true;
        UpdateCtrl(cpt);
        UpdateSplit();
      };
      break;

    case 20:
      newValue = encnewmap(value, 1, 16, ParamGlo.LowerChan,Encoder );
      if (newValue != ParamGlo.LowerChan )
      {
        SPOut.AutoOff( ParamGlo.LowerDest );
        ParamGlo.LowerChan = newValue;
        ret = true;
        UpdateCtrl(cpt);
      };
      break;
    }
    break;
  case 1 :
    switch(cpt)
    {
    case 2:
      newValue = encnewmap(value, 0, 6, ParamGlo.ClockSrc,Encoder);
      if (newValue != ParamGlo.ClockSrc )
      {
        ParamGlo.ClockSrc = newValue;
        SClock.SetModeClock(ParamGlo.ClockSrc, ParamGlo.ClockDiv);
        ret = true;
        UpdateCtrl(cpt);
      };
      break;

    case 3:
      newValue = encnewmap(value, 1, 8, ParamGlo.ClockDiv,Encoder);
      if (newValue != ParamGlo.ClockDiv )
      {
        ParamGlo.ClockDiv = newValue;
        SClock.SetModeClock(ParamGlo.ClockSrc, ParamGlo.ClockDiv);
        ret = true;
        UpdateCtrl(cpt);
      };
      break;

    case 4:
      newValue = encnewmap(value, 1, 2, ParamGlo.TrigInDir,Encoder);
      if (newValue != ParamGlo.TrigInDir )
      {
        ParamGlo.TrigInDir = newValue;
        ret = true;
        UpdateCtrl(cpt);
      };
      break;

    case 5:
      newValue = encnewmap(value, 1, 12, ParamGlo.Bend,Encoder);
      if (newValue != ParamGlo.Bend )
      {
        ParamGlo.Bend = newValue;
        ret = true;
        UpdateCtrl(cpt);
      };
      break;

    case 12:
    case 13:
    case 14:
    case 15:
    case 17:
    case 18:
    case 19:
    case 20:
      pos=(cpt > 15)?cpt-13:cpt-12;
      newValue = encnewmap(value, -100, +100 , ParamGlo.FINE_CV[pos],Encoder);
      if (newValue != ParamGlo.FINE_CV[pos] )
      {
        ParamGlo.FINE_CV[pos] = newValue;
        ret = true;
        UpdateCtrl(cpt);
      }
      break;

    }
    break;

  case 2:
    offset = CurPage-2;
    switch(cpt)
    {
    case 2:
    case 3:
    case 4:
    case 5:
    case 7:
    case 8:
    case 9:
    case 10:
      pos=(cpt > 6)?cpt-3:cpt-2;
      newValue = encnewmap(value, 0, 1 , CV_Type[pos],Encoder);
      if (newValue != CV_Type[pos] )
      {
        CV_Type[pos] = newValue;
        ret = true;
        UpdateCtrl(cpt);
      }
      break;

    case 12:
    case 13:
    case 14:
    case 15:
    case 17:
    case 18:
    case 19:
    case 20:
      pos=(cpt > 15)?cpt-13:cpt-12;
      newValue = encnewmap(value, 0, 1 , TRIG_Type[pos],Encoder);
      if (newValue != TRIG_Type[pos] )
      {
        TRIG_Type[pos] = newValue;
        ret = true;
        UpdateCtrl(cpt);
        SPOut.AutoOff(NOUT_CVALL+pos);
      }
      break;
    }
    break;
    
  default:
    offset = CurPage-3;
    switch(cpt)
    {

    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
      pos=cpt-1;
      newValue = encnewmap(value, 0, 2 , Routes[pos + offset].Perim,Encoder);
      if (newValue != Routes[pos + offset].Perim )
      {
        Routes[pos + offset].Perim = newValue;
        ret = true;
        UpdateCtrl(cpt);
        UpdateRoute( Routes[pos + offset].Route);
      }
      break;  

    case 6:
    case 7:
    case 8:
    case 9:
    case 10:
      pos=cpt-6;
      newValue = encnewmap(value, 0, ( Routes[pos + offset].Type == RT_MIDI ) ? 4 : 2 , Routes[pos + offset].Source, Encoder);
      if (newValue != Routes[pos + offset].Source )
      {
        Routes[pos + offset].Source = newValue;
        ret = true;
        UpdateCtrl(cpt);
        UpdateRoute( Routes[pos + offset].Route);
      }
      break;  

    case 11:
    case 12:
    case 13:
    case 14:
    case 15:
      pos=cpt-11;
      newValue = encnewmap(value, 0, 23, Routes[pos + offset].Chanel, Encoder );
      if (newValue != Routes[pos + offset].Chanel)
      {
        Routes[pos + offset].Chanel = newValue;
        ret = true;
        UpdateCtrl(cpt);
        UpdateRoute( Routes[pos + offset].Route);
      }
      break;  

    case 16:
    case 17:
    case 18:
    case 19:
    case 20:
      pos=cpt-16;
      if ( Routes[pos + offset].Type != RT_MIDI )
      {
        newValue = encnewmap(value, 0, 127 + 2, Routes[pos + offset].MidiType , Encoder  );
        if (newValue != Routes[pos + offset].MidiType)
        {
          Routes[pos + offset].MidiType = newValue;
          ret = true;
          UpdateCtrl(cpt);
          UpdateRoute( Routes[pos + offset].Route);
        }
      }
      break;  
    }
    break;
  }
  return ret;

}

boolean Global::ChangePage(int value, byte Encoder)
{  
  boolean ret = false;
  int newValue = encnewmap(value, 0, NB_PAGES - 5, CurPage, Encoder);
  if ( newValue != CurPage )
  {
    ret = true;
    CurPage = newValue;
    DessinPage();
  }
  return ret;
}


boolean Global::ChangeM1(int value, byte Encoder)
{
  boolean ret = false;
  if ( Menu->ChangeM1(value, Encoder) )
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

boolean Global::ChangeM2(int value, byte Encoder)
{
  boolean ret = false;
  if ( Menu->ChangeM2(value, Encoder) )
  {
    if ( Menu->FirstPage )
    {  
    }
  }
  return ret;
}

boolean Global::ChangeM3(int value, byte Encoder)
{
  boolean ret = false;
  if ( Menu->ChangeM3(value, Encoder) )
  {
    if ( Menu->FirstPage )
    {  
      ParamGlo.Groove = Menu->M3;
      ret = true;
      SClock.ChangeTempo();
    }
  }
  return ret;
}


void Global::ExStart(boolean ModeSong)
{
  is_played=true;
  clock_count=0;
  if ( Recorder.mode_Record== RECORD_ON )
  {
    Recorder.nb_Rec_Record=0;
    Recorder.cur_Rec_Record=0;
  }
  if ( Recorder.mode_Record== RECORD_PLAY )
  {
    Recorder.cur_Play_Record=0;
  }
  if (ParamGlo.Active[MODE_SEQEUCLID]) SeqEu.ExStart(ModeSong);
  if (ParamGlo.Active[MODE_SEQSTEP]) SeqSt.ExStart(ModeSong);
  if (ParamGlo.Active[MODE_SEQTR]) SeqTr.ExStart(ModeSong);
  if (ParamGlo.Active[MODE_ARPEGE]) Arp.ExStart(ModeSong);
  LFoBi.ExStart();
}


void Global::ExStop()
{
  is_played=false;
  if (ParamGlo.Active[MODE_SEQEUCLID]) SeqEu.ExStop();
  if (ParamGlo.Active[MODE_SEQSTEP]) SeqSt.ExStop();
  if (ParamGlo.Active[MODE_SEQTR]) SeqTr.ExStop();
  if (ParamGlo.Active[MODE_ARPEGE]) Arp.ExStop();
  LFoBi.ExStop();
  if ( Recorder.mode_Record==RECORD_ON)
  {
    Recorder.mode_Record=RECORD_PLAY;
    lcdM.setCursor(13,3);
    lcdM.print("Ply");
  }

}

boolean Global::ChangeS1S5(int value)
{
  boolean ret = false;
  int nbtap, tot, ct;

  if ( Menu->ChangeS1S5(value) )
  {
    if ( Menu->FirstPage )
    {  
      switch (Menu->S1S5)
      {
      case 1 : // Tap Tempo


        for (ct=TAPTEMPO_MAX-1;ct>0;ct--) tapTempo[ct]=tapTempo[ct-1]; 
        tapTempo[ 0 ] = millis();
        nbtap=0;
        tot=0;
        for ( int ct=0;ct<TAPTEMPO_MAX-1; ct++ )
        {
          if ( tapTempo[ ct ] - tapTempo[ ct + 1] < (60000/TEMPO_MIN) )
          {
            tot +=  ( tapTempo[ct] - tapTempo[ct+1] );
            nbtap++;
          }
        }
        if ( nbtap > 0 )  
        { 
          ParamGlo.Tempo = (uint16_t)60000 / ( tot / nbtap );
          if ( ParamGlo.Tempo < TEMPO_MIN ) ParamGlo.Tempo=TEMPO_MIN;
          if ( ParamGlo.Tempo > TEMPO_MAX ) ParamGlo.Tempo=TEMPO_MAX;
          Menu->SetM1(ParamGlo.Tempo);
          SClock.ChangeTempo();
        }
        break;



      case 3 :
        if (!is_played) {
          ExStart(false);
          lcdM.setCursor(8,3);
          lcdM.print("Stop");
        }
        else
        {
          ExStop();
          lcdM.setCursor(8,3);
          lcdM.print("Play");
        }
        break;
        
      case 2 :
        if (!is_played) {
          ExStart(true);
          lcdM.setCursor(8,3);
          lcdM.print("Stop");
        }
        else
        {
          ExStop();
          lcdM.setCursor(8,3);
          lcdM.print("Play");
        }
        break;
        
        
      case 4 :
        switch (Recorder.mode_Record)
        {
        case  RECORD_OFF:
          Recorder.mode_Record=RECORD_PLAY;
          lcdM.setCursor(13,3);
          lcdM.print("Ply");
          Recorder.cur_Play_Record=0;
          clock_count=0;
          break;

        case  RECORD_PLAY:
          if ( !is_played )
          {
            Recorder.mode_Record=RECORD_ON;
            lcdM.setCursor(13,3);
            lcdM.print("Rec");
          }
          else
          {
            Recorder.mode_Record=RECORD_OFF;
            lcdM.setCursor(13,3);
            lcdM.print("Off");
          }
          break;

        case  RECORD_ON:
          Recorder.mode_Record=RECORD_OFF;
          lcdM.setCursor(13,3);
          lcdM.print("Off");
          break;


        }
        break;

      }
    }        
    else
    {
      switch (Menu->S1S5)
      {
      case 3:
        switch( Menu->curCmd )
        {
        case 0:  /* Load */
          SPOut.AutoOff( ParamGlo.UpperDest );
          SPOut.AutoOff( ParamGlo.LowerDest );

          if ( MySD.OpenFileForRead(MODE_PARAM, Menu->GetFName(FileName) ) )
          {
            Restore();
            MySD.CloseFile();
          }
          PatLed.Show(0);
          UpdateRoute(0);
          DessinPage();
          break;

        case 1:  /* Save */

          if ( MySD.OpenFileForWrite(MODE_PARAM, Menu->MenuFName(FileName) ) )
          {
            Backup();
            MySD.CloseFile();
          }
          PatLed.Show(0);
          break;

        case 2:  /* Load All*/
          SPOut.AutoOff( ParamGlo.UpperDest );
          SPOut.AutoOff( ParamGlo.LowerDest );

          read_global(Menu->GetFName(FileName));
          PatLed.Show(0);
          DessinPage();
          break;


        case 3:  /* Save All*/
          write_global(Menu->MenuFName(FileName) );
          PatLed.Show(0);
          break;


        case 4:  /* Init */

          SPOut.AutoOff( ParamGlo.UpperDest );
          SPOut.AutoOff( ParamGlo.LowerDest );

          if ( Menu->V1 == 0 )
            Init();
          else
            init_global();

          PatLed.Show(0);
          DessinPage();        
          break;



        case 5:
          S4822.Send(CV_1, 400*Menu->V1 );
          S4822.Send(CV_2, 400*Menu->V1);
          S4822.Send(CV_3, 400*Menu->V1 );
          S4822.Send(CV_4, 400*Menu->V1 );
          S4822.Send(CV_5, 400*Menu->V1 );
          S4822.Send(CV_6, 400*Menu->V1 );
          S4822.Send(CV_7, 400*Menu->V1 );
          S4822.Send(CV_8, 400*Menu->V1 );
//          analogWrite(DAC0, 400*Menu->V1 );
//            analogWrite(DAC1, 400*Menu->V1 );
          PatLed.Show(0);
          break;


        case 6: 
          digitalWrite(TRIG_1, Menu->V1 );
          digitalWrite(TRIG_2, Menu->V1 );
          digitalWrite(TRIG_3, Menu->V1 );
          digitalWrite(TRIG_4, Menu->V1 );
          digitalWrite(TRIG_5, Menu->V1 );
          digitalWrite(TRIG_6, Menu->V1 );
          digitalWrite(TRIG_7, Menu->V1 );
          digitalWrite(TRIG_8, Menu->V1 );
          PatLed.Show(0);
          break;
        }
        break;
      }
    }
  }
  return ret;
}

boolean Global::ChangePAT(int value)
{
  boolean ret = false;

  return ret;
}

void Global::Encoder(byte Value)
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

void Global::Interface(int Num, int Value)
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
/*  
#ifdef _DEBUG_MODE_
  Serial.print("Global::Interface=");
  Serial.println(Num);
#endif
*/
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

void  Global::Backup()
{
  MySD.WriteStream((byte *)Routes, sizeof(Routes) ) ;
  MySD.WriteStream((byte *)&ParamGlo, sizeof(ParamGlo) ) ;
  MySD.WriteStream((byte *)&Recorder, sizeof(Recorder) ) ;
}

void  Global::Restore()
{
  MySD.ReadStream((byte *)Routes, sizeof(Routes) ) ;
  MySD.ReadStream((byte *)&ParamGlo, sizeof(ParamGlo) ) ;
  MySD.ReadStream((byte *)&Recorder, sizeof(Recorder) ) ;
  UpdateRoute(0);
  UpdateSplit();
  SClock.ChangeTempo();
}


void Global::UpdateRoute(byte Route)
{
  for (int ct=0;ct<NB_ROUTES;ct++)
  {
    if (Route == Routes[ct].Route || Route == 0)
    {
      switch(Routes[ct].Type )
      {
      case RT_MIDI :

        switch(Routes[ct].Route )
        {
        case 1 :
        case 2 :
        case 3 :
          MIDI1.RouteChange(Routes[ct].Route, routkRoute[Routes[ct].Source], chankCanal[Routes[ct].Chanel] , Routes[ct].Perim==2 || ( Routes[ct].Perim==1 & ActiFunc == MODE_GLOBAL ) );
          break;  
        case 4 :
        case 5 :
        case 6 :
          MIDI2.RouteChange(Routes[ct].Route, routkRoute[Routes[ct].Source], chankCanal[Routes[ct].Chanel] , Routes[ct].Perim==2 || ( Routes[ct].Perim==1 & ActiFunc == MODE_GLOBAL ) );
          break;  
        case 7 :
        case 8 :
        case 9 :
          MIDI3.RouteChange(Routes[ct].Route, routkRoute[Routes[ct].Source], chankCanal[Routes[ct].Chanel] , Routes[ct].Perim==2 || ( Routes[ct].Perim==1 & ActiFunc == MODE_GLOBAL ) );
          break;  

        }
        break;

      }
    }
  }
}


void Global::UpdateSplit()
{
  switch (ParamGlo.keyInput)
  {
  case 0:
    ParamGlo.MidiInput = &MIDI1; 
    break;
  case 1:
    ParamGlo.MidiInput = &MIDI2; 
    break;
  case 2:
    ParamGlo.MidiInput = &MIDI3; 
    break;
  }    

//  if ( ParamGlo.Split )
//  {
    switch (ParamGlo.ArpSrc)
    {
    case 0:
      ParamGlo.MidiInput->setNoteMinMax(0,127);
      break;
    case 1:
      ParamGlo.MidiInput->setNoteMinMax(ParamGlo.NoteSplit,127);
      break;
    case 2:
      ParamGlo.MidiInput->setNoteMinMax(0,ParamGlo.NoteSplit-1);
      break;
    }
 /*
  }
  else
  {
    ParamGlo.MidiInput->setNoteMinMax(0,127);
  }
*/  
}


void Global::HandleNoteOn(byte Interface, byte channel, byte pitch, byte velocity)
{
  if ( Recorder.mode_Record==RECORD_ON && is_played )
  {
    Recorder.record[Recorder.nb_Rec_Record].Type = r_NoteOn;
    Recorder.record[Recorder.nb_Rec_Record].Interface = Interface;
    Recorder.record[Recorder.nb_Rec_Record].channel = channel;
    Recorder.record[Recorder.nb_Rec_Record].pitch = pitch;
    Recorder.record[Recorder.nb_Rec_Record].velocity = velocity;
    Recorder.record[Recorder.nb_Rec_Record].rtime =clock_count;
    Recorder.nb_Rec_Record++;
    if ( Recorder.nb_Rec_Record >= NB_RECORD )
    {
      Recorder.mode_Record=RECORD_OFF;
      lcdM.setCursor(13,3);
      lcdM.print("Off");
    }
    lcdM.setCursor(8,2);
    lcd_Num(tmpVal,Recorder.nb_Rec_Record);
    lcdM.print(tmpVal);
  }
  Interface--;  // MIDI 1..3 de liste


  // Gestion des routes CV
  for (int ct=9;ct<NB_ROUTES;ct++)
  {
    if ( Routes[ct].MidiType < 2 )
    {
      if ( Interface == Routes[ct].Source && ( Routes[ct].Perim == 2 || (Routes[ct].Perim==1 && ActiFunc == MODE_GLOBAL) )  && ( ( 1 << ( channel - 1) ) & chankCanal[Routes[ct].Chanel] ))
      {
        if ( Routes[ct].MidiType==0 )
            SPOut.NoteOn(NOUT_CV1 + (ct-9),pitch,0,channel);
        else
            SPOut.NoteOn(NOUT_CV1 + (ct-9),velocity,0,channel);
      }
    }
  }

  // Gestion interface global
  if ( ParamGlo.keyInput == Interface && ( ( 1 << (channel-1) ) & chankCanal[ParamGlo.canInput] ) )
  {
    ParamGlo.LastNote = pitch;

    // Si noteon alors qu'on edite la note de split
/*
    if ( ActiFunc == MODE_GLOBAL ) {
      if ( CurPage == 0 &&  ( lastChangeValue == 22 || lastChangeValue == 17 )  )
      {
          if ( ParamGlo.Split>0 && ParamGlo.NoteSplit != pitch ) {
            SPOut.AutoOff( ParamGlo.UpperDest );
            SPOut.AutoOff( ParamGlo.LowerDest );
            ParamGlo.NoteSplit = pitch; UpdateCtrl(17);
          }
      }
    }
*/
    if ( pitch >=ParamGlo.NoteSplit ) ParamGlo.LastUpperNote = pitch+ParamGlo.UpperTrans*12; 
    else if ( pitch <ParamGlo.NoteSplit ) ParamGlo.LastLowerNote = pitch+ParamGlo.UpperTrans*12; 

    if ( ActiFunc == MODE_GLOBAL ) 
    {
      if (  ParamGlo.UpperDest && ( ParamGlo.Split==0 || ( ParamGlo.Split==1 && pitch >=ParamGlo.NoteSplit )  ) )
      {
         SPOut.NoteOn(ParamGlo.UpperDest, pitch+ParamGlo.UpperTrans*12 ,velocity,channel);
      }
      if ( ParamGlo.LowerDest && ( ParamGlo.Split==0 || ( ParamGlo.Split==1 && pitch < ParamGlo.NoteSplit ) ) )
      {
         SPOut.NoteOn(ParamGlo.LowerDest,pitch+ParamGlo.LowerTrans*12,velocity,ParamGlo.LowerChan);
      }
    }
  }

}


void Global::HandleNoteOff(byte Interface, byte channel, byte pitch, byte velocity)
{
  if ( Recorder.mode_Record==RECORD_ON && is_played)
  {
    Recorder.record[Recorder.nb_Rec_Record].Type = r_NoteOff;
    Recorder.record[Recorder.nb_Rec_Record].Interface = Interface;
    Recorder.record[Recorder.nb_Rec_Record].channel = channel;
    Recorder.record[Recorder.nb_Rec_Record].pitch = pitch;
    Recorder.record[Recorder.nb_Rec_Record].velocity = velocity;
    Recorder.record[Recorder.nb_Rec_Record].rtime = clock_count;
    Recorder.nb_Rec_Record++;
    if ( Recorder.nb_Rec_Record >= NB_RECORD )
    {
      Recorder.mode_Record=RECORD_OFF;
      lcdM.setCursor(13,3);
      lcdM.print("Off");
    }
    lcdM.setCursor(8,2);
    lcd_Num(tmpVal,Recorder.nb_Rec_Record);
    lcdM.print(tmpVal);
  }
  Interface--;  // MIDI 1..3 de liste
  for (int ct=9;ct<NB_ROUTES;ct++)
  {
    if ( Interface == Routes[ct].Source && ( Routes[ct].Perim == 2 || (Routes[ct].Perim==1 && ActiFunc == MODE_GLOBAL) ) && ( ( 1 << ( channel - 1) ) & chankCanal[Routes[ct].Chanel] ) )
    {
      SPOut.NoteOff(NOUT_CV1 + (ct-9),pitch,velocity,channel);
    }
  }

  // Gestion interface global
  if ( ActiFunc == MODE_GLOBAL ) 
  {
    if ( ParamGlo.keyInput == Interface && ( ( 1 << (channel-1) ) & chankCanal[ParamGlo.canInput] ) )
    {
      if ( ParamGlo.UpperDest && ( ParamGlo.Split==0 || ( ParamGlo.Split==1 && pitch >=ParamGlo.NoteSplit ) ) )
      {
        SPOut.NoteOff(ParamGlo.UpperDest,pitch+ParamGlo.UpperTrans*12,velocity,channel);
      }
      if ( ParamGlo.LowerDest && ( ParamGlo.Split==0 || ( ParamGlo.Split==1 && pitch < ParamGlo.NoteSplit ) ) )
      {
        SPOut.NoteOff(ParamGlo.LowerDest,pitch+ParamGlo.LowerTrans*12,velocity,ParamGlo.LowerChan);
      }
    }
  }
}


void Global::HandlePitchBend(byte Interface, byte channel, int bend)
{
  Interface--;  // MIDI 1..3 de liste

  // Gestion des routes 
  for (int ct=9;ct<NB_ROUTES;ct++)
  {
    if ( Interface == Routes[ct].Source && ( Routes[ct].Perim == 2 || (Routes[ct].Perim==1 && ActiFunc == MODE_GLOBAL) )  && ( ( 1 << ( channel - 1) ) & chankCanal[Routes[ct].Chanel] ))
    {
      SPOut.PitchBend(NOUT_CV1 + (ct-9),bend,channel);
    }
  }

  // Gestion interface global
  if ( ActiFunc == MODE_GLOBAL ) 
  {
    if ( ParamGlo.keyInput == Interface && ( ( 1 << (channel-1) ) & chankCanal[ParamGlo.canInput] ) )
    {
      if ( ParamGlo.UpperDest )
      {
        SPOut.PitchBend(ParamGlo.UpperDest,bend, channel);
      }
      if ( ParamGlo.LowerDest )
      {
        SPOut.PitchBend(ParamGlo.LowerDest,bend ,ParamGlo.LowerChan);
      }
    }
  }
}


void Global::HandleControlChange(byte Interface, byte channel, byte number, byte value)
{
  Interface--;  // MIDI 1..3 de liste

  // Gestion des routes 
  for (int ct=9;ct<NB_ROUTES;ct++)
  {

    if ( Routes[ct].MidiType > 2 && number == Routes[ct].MidiType - 3 && Interface == Routes[ct].Source && ( Routes[ct].Perim == 2 || (Routes[ct].Perim==1 && ActiFunc == MODE_GLOBAL) ) && ( ( 1 << ( channel - 1) ) & chankCanal[Routes[ct].Chanel] ) )
    {
      SPOut.ControlChange(NOUT_CV1 + (ct-9), number ,value, channel);
    }
  }

  // Gestion interface global
  if ( ActiFunc == MODE_GLOBAL ) 
  {
    if ( ParamGlo.keyInput == Interface && ( ( 1 << (channel-1) ) & chankCanal[ParamGlo.canInput] ) )
    {
      if ( ParamGlo.UpperDest )
      {
        SPOut.ControlChange(ParamGlo.UpperDest,number ,value, channel);
      }
      if ( ParamGlo.LowerDest )
      {
        SPOut.ControlChange(ParamGlo.LowerDest,number ,value, ParamGlo.LowerChan);
      }
    }
  }
}

void Global::HandleProgramChange(byte Interface, byte channel, byte number)
{
  Interface--;  // MIDI 1..3 de liste

  // Gestion des routes 
  for (int ct=9;ct<NB_ROUTES;ct++)
  {

    if ( Routes[ct].MidiType > 2 && number == Routes[ct].MidiType - 3 && Interface == Routes[ct].Source && ( Routes[ct].Perim == 2 || (Routes[ct].Perim==1 && ActiFunc == MODE_GLOBAL) ) && ( ( 1 << ( channel - 1) ) & chankCanal[Routes[ct].Chanel] ) )
    {
      SPOut.ProgramChange(NOUT_CV1 + (ct-9), number ,channel);
    }
  }

  // Gestion interface global
  if ( ActiFunc == MODE_GLOBAL ) 
  {
    if ( ParamGlo.keyInput == Interface && ( ( 1 << (channel-1) ) & chankCanal[ParamGlo.canInput] ) )
    {
      if ( ParamGlo.UpperDest )
      {
        SPOut.ProgramChange(ParamGlo.UpperDest,number, channel);
      }
      if ( ParamGlo.LowerDest )
      {
        SPOut.ProgramChange(ParamGlo.LowerDest,number , ParamGlo.LowerChan);
      }
    }
  }
}

void Global::HandleAfterTouch(byte Interface, byte channel, byte value)
{
  
  Interface--;  // MIDI 1..3 de liste
  
  // Gestion des routes CV
  for (int ct=9;ct<NB_ROUTES;ct++)
  {
    if ( Routes[ct].MidiType == 2 )
    {
      if ( Interface == Routes[ct].Source && ( Routes[ct].Perim == 2 || (Routes[ct].Perim==1 && ActiFunc == MODE_GLOBAL) )  && ( ( 1 << ( channel - 1) ) & chankCanal[Routes[ct].Chanel] ))
      {
         SPOut.AfterTouch(NOUT_CV1 + (ct-9),value,channel);
      }
    }
  }
  
}

void Global::Traite(uint32_t reftime)
{
  if ( reftime < next_event ) 
  {
    return;    
  } 

  if ( Recorder.mode_Record == RECORD_PLAY && is_played )
  {
    while ( clock_count >= Recorder.record[Recorder.cur_Play_Record].rtime  )
    {
      switch(Recorder.record[Recorder.cur_Play_Record].Type)
      {
      case r_NoteOn:
        MIDI1.insertNoteOn(Recorder.record[Recorder.cur_Play_Record].pitch, Recorder.record[Recorder.cur_Play_Record].velocity,Recorder.record[Recorder.cur_Play_Record].channel);
        break;
      case r_NoteOff:
        MIDI1.insertNoteOff(Recorder.record[Recorder.cur_Play_Record].pitch, Recorder.record[Recorder.cur_Play_Record].velocity,Recorder.record[Recorder.cur_Play_Record].channel);
        break;
      }
      Recorder.cur_Play_Record++;
      if ( Recorder.cur_Play_Record >= Recorder.nb_Rec_Record )
      {
        Recorder.mode_Record=RECORD_OFF;
        Recorder.cur_Play_Record=0;
        lcdM.setCursor(13,3);
        lcdM.print("Off");
        break;
      }

      lcdM.setCursor(8,2);
      lcd_Num(tmpVal,Recorder.cur_Play_Record);
      lcdM.print(tmpVal);

    }
  }

  clock_count++;

  next_event = next_event + delta_event;

}








