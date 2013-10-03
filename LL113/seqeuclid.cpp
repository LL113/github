// -----------------------------------------------
// LL113
// -----------------------------------------------
// Historique:
// (Baloran) laurent.lecatelier@free.fr, version 0.0.1  10/02/12013
// -----------------------------------------------
// seqeuclid.cpp Code Class gestion du sequenceur euclidien

#include <LiquidCrystal.h>
// #include <MIDIMul.h> 
#include "Arduino.h"
#include "seqeuclid.h"
#include "utilld.h"
#include "multiplexAD.h"
#include "lfo.h"
#include "SDFile.h"
#include "clock.h"


// Clock
extern SysClock SClock;



extern mPatLed PatLed;

extern SDFile MySD;
extern LFo LFoBi;
extern multiplexAD mxAD;
extern LiquidCrystal lcdM;
extern LiquidCrystal lcd1;
extern LiquidCrystal lcd2;
extern LiquidCrystal lcd3;
extern LiquidCrystal lcd4;

const char *EuclidTitre="\x01 Euclid Sequencer \x02";

extern const char *listDestin;
extern const char *listMul;

extern SuperOut SPOut;

const char  *mListSource ="InternMidi 1Midi 2Midi 3Trig 1";

paramMenuList MenuListEuclid[] = {
  { 
    "Unit", "From", "To", 1, 1, NB_TRACK,0, 2,1,NB_TRACK,0       }
  , 
  { 
    "Track", "From", "To", 1, 1, NB_EU_PATTERN,0 ,2,1,NB_EU_PATTERN,0   }
  , 
  { 
    "Load", "Name", "", 0, MODE_SEQEUCLID, -1, 0 , 0, 0, 0, 0       }
  ,
  { 
    "Save", "Name", "Indice", 0, 0, NB_STYLEMU, listStyleMenu , 1, 1, 64, 0         }
  , 
  { 
    "Init", "", "", 0, 0, 0, 0 , 0, 0, 0, 0                   }
  , 
  { 
    "", "", "", 0, 0, 0, 0 , 0, 0, 0, 0                         }
};


paramMenu menuEuclid = {
  "",
  "Sng",
  "Play",
  "-In",
  "BPM",          
  "S/Beat",     
  "Track",          
  TEMPO_INIT,TEMPO_MIN,TEMPO_MAX,0,     
  4,1,16,0,     
  1,1,NB_EU_PATTERN,0,     
  MenuListEuclid 
};


SeqEuclid::SeqEuclid(byte pNumFunc)
{ 
  NumFunc = pNumFunc;
  Init();
  Menu = new mMenu(&menuEuclid, (char *)EuclidTitre);
  mesure = menuEuclid.Val_2;
  nb24 =  CalcNb24(mesure);
  cpt24 = 0;
  is_echo = true;
  LastOffset = 0;
}

SeqEuclid::~SeqEuclid()
{
}

void SeqEuclid::Init()
{
  lastChangeValue = CHGT_PAGE;
  CurPage=1;

  for (CurPattern=0;CurPattern<NB_EU_PATTERN;CurPattern++)
  {
    for (int pos=0;pos<NB_TRACK;pos++)
    {
      Tracks[CurPattern][pos].Lg = pos==0 ? 16 : 0;          
      Tracks[CurPattern][pos].Div = 4;          
      Tracks[CurPattern][pos].Note = 48;      
      Tracks[CurPattern][pos].Duree = 9;      
      Tracks[CurPattern][pos].Veloci = 100;
      Tracks[CurPattern][pos].Modul = 0;      
      Tracks[CurPattern][pos].Ampli = 0;      
      Tracks[CurPattern][pos].Dest1 = NOUT_MIDI1;
      Tracks[CurPattern][pos].Dest2 = NOUT_NONE;
      Tracks[CurPattern][pos].Chanel = 1;
      Tracks[CurPattern][pos].Pat[NB_EU_STEP_MAX];    
      Tracks[CurPattern][pos].IsOn = false;
      Tracks[CurPattern][pos].TimeOff = 0;  
      UpdatePattern(pos);
    }
  }
  CurPiste=CurPattern=0;

  steep = 0;
  is_played=STOPPED;
  NextPattern=-1;

  for (CurTrSong=0;CurTrSong<NB_EU_SONG;CurTrSong++)
  {  
    Songs[CurTrSong].Track = CurTrSong+1;      
    Songs[CurTrSong].Repeat = 1;      
    Songs[CurTrSong].Transpose = 0;      
    Songs[CurTrSong].Next = (CurTrSong==NB_EU_SONG)?0:CurTrSong+2;   
  }
  CurTrSong=0;
  Mode=MODE_PATT;
  nbSteep=0;
}

void SeqEuclid::DessinPage()
{
  int pos;

  menuEuclid.Val_1 = ParamGlo.Tempo;
  menuEuclid.Val_2 = mesure;
  menuEuclid.Val_3 = CurPattern+1;

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

    if (Mode==MODE_PATT) {
      lcdM.setCursor(4,3);
      lcdM.print("Sng");
    }
    else
    {
      lcdM.setCursor(4,3);
      lcdM.print("Stp");
    }
    
    if (is_echo) {
      lcdM.setCursor(13,3);
      lcdM.print("-In");
    }
    else
    {
      lcdM.setCursor(13,3);
      lcdM.print("+In");
    }


  }
  Z_Clear();


  switch(Mode)
  {
  case MODE_PATT :

    pos = (CurPage - 1) *2 ;
    sprintf(tmpCh,"\x02Unit %d",pos+1);
    Z_Titre(1,tmpCh);
    if ( pos == 0 )
    {
      if ( Tracks[CurPattern][pos].Lg == 0)
        lcd_Text(tmpVal,"Off");
      else
        lcd_Num(tmpVal,Tracks[CurPattern][pos].Lg);
    }
    else
    {
      if ( Tracks[CurPattern][pos].Lg == 0)
        lcd_Text(tmpVal,"Off");
      else
        if (Tracks[CurPattern][pos].Lg == 1)
          lcd_Text(tmpVal,"TR 1");
        else
          lcd_Num(tmpVal,Tracks[CurPattern][pos].Lg - 1);
    }
    Z_Value(1,tmpVal);

    lcd_Num(tmpVal,Tracks[CurPattern][pos].Div);
    Z_TextValue(2,"Divide",tmpVal);

    lcd_Note(tmpVal,Tracks[CurPattern][pos].Note);
    Z_TextValue(3,"Note",tmpVal);

    lcd_Num(tmpVal,Tracks[CurPattern][pos].Duree);
    Z_TextValue(4,"Durati.",tmpVal);

    lcd_Num(tmpVal,Tracks[CurPattern][pos].Veloci);
    Z_TextValue(5,"Veloci.",tmpVal);

    lcd_Liste(tmpVal,(char *)listMul,Tracks[CurPattern][pos].Modul);
    Z_TextValue(6,"Modula.",tmpVal);

    lcd_Num(tmpVal,Tracks[CurPattern][pos].Ampli);
    Z_TextValue(7,"M.Level",tmpVal);

    lcd_Liste(tmpVal,(char *)listDestin,Tracks[CurPattern][pos].Dest1);
    Z_TextValue(8,"Dest.1",tmpVal);

    lcd_Liste(tmpVal,(char *)listDestin,Tracks[CurPattern][pos].Dest2);
    Z_TextValue(9,"Dest.2",tmpVal);

    lcd_Num(tmpVal,Tracks[CurPattern][pos].Chanel);
    Z_TextValue(10,"Channel",tmpVal);

    pos++;

    sprintf(tmpCh,"\x02Unit %d",pos+1);
    Z_Titre(11,tmpCh);
    if ( Tracks[CurPattern][pos].Lg == 0)
      lcd_Text(tmpVal,"Off");
    else
      if (Tracks[CurPattern][pos].Lg  == 1)
        lcd_Text(tmpVal,"TR 1");
      else
        lcd_Num(tmpVal,Tracks[CurPattern][pos].Lg - 1);
    Z_Value(11,tmpVal);

    lcd_Num(tmpVal,Tracks[CurPattern][pos].Div);
    Z_TextValue(12,"Divide",tmpVal);

    lcd_Note(tmpVal,Tracks[CurPattern][pos].Note);
    Z_TextValue(13,"Note",tmpVal);

    lcd_Num(tmpVal,Tracks[CurPattern][pos].Duree);
    Z_TextValue(14,"Durati.",tmpVal);

    lcd_Num(tmpVal,Tracks[CurPattern][pos].Veloci);
    Z_TextValue(15,"Veloci.",tmpVal);

    lcd_Liste(tmpVal,(char *)listMul,Tracks[CurPattern][pos].Modul);
    Z_TextValue(16,"Modula.",tmpVal);

    lcd_Num(tmpVal,Tracks[CurPattern][pos].Ampli);
    Z_TextValue(17,"M.Level",tmpVal);

    lcd_Liste(tmpVal,(char *)listDestin,Tracks[CurPattern][pos].Dest1);
    Z_TextValue(18,"Dest.1",tmpVal);

    lcd_Liste(tmpVal,(char *)listDestin,Tracks[CurPattern][pos].Dest2);
    Z_TextValue(19,"Dest.2",tmpVal);

    lcd_Num(tmpVal,Tracks[CurPattern][pos].Chanel);
    Z_TextValue(20,"Channel",tmpVal);
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


void SeqEuclid::ChangeCurPlaySong(int value) 
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



void SeqEuclid::UpdateCtrl(int cpt)
{
  int pos = (CurPage - 1) *2 ;

  switch(Mode)
  {
  case MODE_PATT :
    switch(cpt)
    {
    case 1:
      if ( pos == 0 )
      {
        if ( Tracks[CurPattern][pos].Lg == 0)
          lcd_Text(tmpVal,"Off");
        else
          lcd_Num(tmpVal,Tracks[CurPattern][pos].Lg);
      }
      else
      {
        if ( Tracks[CurPattern][pos].Lg == 0)
          lcd_Text(tmpVal,"Off");
        else
          if (Tracks[CurPattern][pos].Lg == 1)
            lcd_Text(tmpVal,"TR 1");
          else
            lcd_Num(tmpVal,Tracks[CurPattern][pos].Lg - 1);
      }
      Z_Value(1,tmpVal);
      break;

    case 2:
      lcd_Num(tmpVal,Tracks[CurPattern][pos].Div);
      Z_Value(2,tmpVal);
      break;

    case 3:
      lcd_Note(tmpVal,Tracks[CurPattern][pos].Note);
      Z_Value(3,tmpVal);
      break;

    case 4:
      lcd_Num(tmpVal,Tracks[CurPattern][pos].Duree);
      Z_Value(4,tmpVal);
      break;

    case 5:
      lcd_Num(tmpVal,Tracks[CurPattern][pos].Veloci);
      Z_Value(5,tmpVal);
      break;

    case 6:
      lcd_Liste(tmpVal,(char *)listMul,Tracks[CurPattern][pos].Modul);
      Z_Value(6,tmpVal);
      break;

    case 7:
      lcd_Num(tmpVal,Tracks[CurPattern][pos].Ampli);
      Z_Value(7,tmpVal);
      break;

    case 8:
      lcd_Liste(tmpVal,(char *)listDestin,Tracks[CurPattern][pos].Dest1);
      Z_Value(8,tmpVal);
      break;

    case 9:
      lcd_Liste(tmpVal,(char *)listDestin,Tracks[CurPattern][pos].Dest2);
      Z_Value(9,tmpVal);
      break;

    case 10:
      lcd_Num(tmpVal,Tracks[CurPattern][pos].Chanel);
      Z_Value(10,tmpVal);
      break;

    case 11:
      if ( Tracks[CurPattern][pos+1].Lg == 0)
        lcd_Text(tmpVal,"Off");
      else
        if (Tracks[CurPattern][pos+1].Lg == 1)
          lcd_Text(tmpVal,"TR 1");
        else
          lcd_Num(tmpVal,Tracks[CurPattern][pos+1].Lg - 1);
      Z_Value(11,tmpVal);
      break;

    case 12:
      lcd_Num(tmpVal,Tracks[CurPattern][pos+1].Div);
      Z_Value(12,tmpVal);
      break;

    case 13:
      lcd_Note(tmpVal,Tracks[CurPattern][pos+1].Note);
      Z_Value(13,tmpVal);
      break;

    case 14:
      lcd_Num(tmpVal,Tracks[CurPattern][pos+1].Duree);
      Z_Value(14,tmpVal);
      break;

    case 15:
      lcd_Num(tmpVal,Tracks[CurPattern][pos+1].Veloci);
      Z_Value(15,tmpVal);
      break;

    case 16:
      lcd_Liste(tmpVal,(char *)listMul,Tracks[CurPattern][pos+1].Modul);
      Z_Value(16,tmpVal);
      break;

    case 17:
      lcd_Num(tmpVal,Tracks[CurPattern][pos+1].Ampli);
      Z_Value(17,tmpVal);
      break;

    case 18:
      lcd_Liste(tmpVal,(char *)listDestin,Tracks[CurPattern][pos+1].Dest1);
      Z_Value(18,tmpVal);
      break;

    case 19:
      lcd_Liste(tmpVal,(char *)listDestin,Tracks[CurPattern][pos+1].Dest2);
      Z_Value(19,tmpVal);
      break;    

    case 20:
      lcd_Num(tmpVal,Tracks[CurPattern][pos+1].Chanel);
      Z_Value(20,tmpVal);
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

boolean SeqEuclid::ChangeValue(int cpt, int value, byte Encoder)
{  
  boolean ret = false;
  int pos = (CurPage - 1) *2 ;
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
      newValue = encnewmap(value, 0, pos==0 ? NB_EU_STEP_MAX : NB_EU_STEP_MAX + 1 , Tracks[CurPattern][pos].Lg,Encoder);
      if (newValue != Tracks[CurPattern][pos].Lg )
      {
        Tracks[CurPattern][pos].Lg = newValue;
        ret = true;
        UpdatePattern(pos);
        UpdateCtrl(cpt);

      };
      break;
    case 2:
      newValue = encnewmap(value, 0, NB_EU_STEP_MAX , Tracks[CurPattern][pos].Div,Encoder );
      if (newValue != Tracks[CurPattern][pos].Div )
      {
        Tracks[CurPattern][pos].Div = newValue;
        ret = true;
        UpdatePattern(pos);
        UpdateCtrl(cpt);
      };
      break;
    case 3:
      newValue = encnewmap(value, -24, 96  , Tracks[CurPattern][pos].Note,Encoder);
      if (newValue != Tracks[CurPattern][pos].Note )
      {
        if ( Tracks[CurPattern][pos].IsOn ) EventOff(pos);
        Tracks[CurPattern][pos].Note = newValue;
        ret = true;
        UpdateCtrl(cpt);
      };
      break;
    case 4:
      newValue = encnewmap(value, 1, 40 , Tracks[CurPattern][pos].Duree,Encoder);
      if (newValue != Tracks[CurPattern][pos].Duree )
      {
        Tracks[CurPattern][pos].Duree = newValue;
        ret = true;
        UpdateCtrl(cpt);
      };
      break;
    case 5:
      newValue = encnewmap(value, 0, 127  , Tracks[CurPattern][pos].Veloci,Encoder);
      if (newValue != Tracks[CurPattern][pos].Veloci )
      {
        Tracks[CurPattern][pos].Veloci = newValue;
        ret = true;
        UpdateCtrl(cpt);
      };
      break;
    case 6:
      newValue = encnewmap(value, 0, 4 , Tracks[CurPattern][pos].Modul,Encoder);
      if (newValue != Tracks[CurPattern][pos].Modul )
      {
        Tracks[CurPattern][pos].Modul = newValue;
        ret = true;
        UpdateCtrl(cpt);
      };
      break;
    case 7:
      newValue = encnewmap(value, -99, 99 ,Tracks[CurPattern][pos].Ampli ,Encoder );
      if (newValue != Tracks[CurPattern][pos].Ampli )
      {
        Tracks[CurPattern][pos].Ampli = newValue;
        ret = true;
        UpdateCtrl(cpt);
      };
      break;
    case 8:
      newValue = encnewmap(value, 0, NB_OUTS  , Tracks[CurPattern][pos].Dest1,Encoder);
      if (newValue != Tracks[CurPattern][pos].Dest1 )
      {
        if ( Tracks[CurPattern][pos].IsOn ) EventOff(pos);
        SPOut.AutoOff( Tracks[CurPattern][pos].Dest1 );
        Tracks[CurPattern][pos].Dest1 = newValue;
        ret = true;
        UpdateCtrl(cpt);
      };
      break;
    case 9:
      newValue = encnewmap(value, 0, NB_OUTS  , Tracks[CurPattern][pos].Dest2,Encoder);
      if (newValue != Tracks[CurPattern][pos].Dest2 )
      {
        if ( Tracks[CurPattern][pos].IsOn ) EventOff(pos);
        SPOut.AutoOff( Tracks[CurPattern][pos].Dest2 );
        Tracks[CurPattern][pos].Dest2 = newValue;
        ret = true;
        UpdateCtrl(cpt);
      };
      break;

    case 10:
      newValue = encnewmap(value, 1, 16  , Tracks[CurPattern][pos].Chanel,Encoder);
      if (newValue != Tracks[CurPattern][pos].Chanel )
      {
        if ( Tracks[CurPattern][pos].IsOn ) EventOff(pos);
        SPOut.AutoOff( Tracks[CurPattern][pos].Dest1 );
        SPOut.AutoOff( Tracks[CurPattern][pos].Dest2 );
        Tracks[CurPattern][pos].Chanel = newValue;
        ret = true;
        UpdateCtrl(cpt);
      };
      break;


    case 11:
      newValue = encnewmap(value, 0, NB_EU_STEP_MAX + 1  , Tracks[CurPattern][pos+1].Lg,Encoder);
      if (newValue != Tracks[CurPattern][pos+1].Lg )
      {
        Tracks[CurPattern][pos+1].Lg = newValue;
        ret = true;
        UpdatePattern(pos+1);
        UpdateCtrl(cpt);
      };
      break;
    case 12:
      newValue = encnewmap(value, 0, NB_EU_STEP_MAX  , Tracks[CurPattern][pos+1].Div,Encoder);
      if (newValue != Tracks[CurPattern][pos+1].Div )
      {
        Tracks[CurPattern][pos+1].Div = newValue;
        ret = true;
        UpdatePattern(pos+1);
        UpdateCtrl(cpt);
      };
      break;
    case 13:
      newValue = encnewmap(value,  -24, 96 , Tracks[CurPattern][pos+1].Note,Encoder);
      if (newValue != Tracks[CurPattern][pos+1].Note )
      {
        if ( Tracks[CurPattern][pos+1].IsOn ) EventOff(pos+1);
        Tracks[CurPattern][pos+1].Note = newValue;
        ret = true;
        UpdateCtrl(cpt);
      };
      break;
    case 14:
      newValue = encnewmap(value, 1, 40 , Tracks[CurPattern][pos+1].Duree,Encoder);
      if (newValue != Tracks[CurPattern][pos+1].Duree )
      {
        Tracks[CurPattern][pos+1].Duree = newValue;
        ret = true;
        UpdateCtrl(cpt);
      };
      break;
    case 15:
      newValue = encnewmap(value, 0, 127  , Tracks[CurPattern][pos+1].Veloci,Encoder);
      if (newValue != Tracks[CurPattern][pos+1].Veloci )
      {
        Tracks[CurPattern][pos+1].Veloci = newValue;
        ret = true;
        UpdateCtrl(cpt);
      };
      break;
    case 16:
      newValue = encnewmap(value, 0, 4  , Tracks[CurPattern][pos+1].Modul,Encoder);
      if (newValue != Tracks[CurPattern][pos+1].Modul )
      {
        Tracks[CurPattern][pos+1].Modul = newValue;
        ret = true;
        UpdateCtrl(cpt);
      };
      break;      
    case 17:
      newValue = encnewmap(value, -99, 99  ,  Tracks[CurPattern][pos+1].Ampli,Encoder);
      if (newValue != Tracks[CurPattern][pos+1].Ampli )
      {
        Tracks[CurPattern][pos+1].Ampli = newValue;
        ret = true;
        UpdateCtrl(cpt);
      };
      break;
    case 18:
      newValue = encnewmap(value, 0, NB_OUTS  , Tracks[CurPattern][pos+1].Dest1,Encoder);
      if (newValue != Tracks[CurPattern][pos+1].Dest1 )
      {
        if ( Tracks[CurPattern][pos+1].IsOn ) EventOff(pos+1);
        SPOut.AutoOff( Tracks[CurPattern][pos+1].Dest1 );
        Tracks[CurPattern][pos+1].Dest1 = newValue;
        ret = true;
        UpdateCtrl(cpt);
      };
      break;
    case 19:
      newValue = encnewmap(value, 0, NB_OUTS  , Tracks[CurPattern][pos+1].Dest2,Encoder);
      if (newValue != Tracks[CurPattern][pos+1].Dest2 )
      {
        if ( Tracks[CurPattern][pos+1].IsOn ) EventOff(pos+1);
        SPOut.AutoOff( Tracks[CurPattern][pos+1].Dest2 );
        Tracks[CurPattern][pos+1].Dest2 = newValue;
        ret = true;
        UpdateCtrl(cpt);
      };
      break;
    case 20:
      newValue = encnewmap(value, 1, 16  , Tracks[CurPattern][pos+1].Chanel,Encoder);
      if (newValue != Tracks[CurPattern][pos+1].Chanel )
      {
        if ( Tracks[CurPattern][pos+1].IsOn ) EventOff(pos+1);
        SPOut.AutoOff( Tracks[CurPattern][pos+1].Dest1 );
        SPOut.AutoOff( Tracks[CurPattern][pos+1].Dest2 );
        Tracks[CurPattern][pos+1].Chanel = newValue;
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
      newValue = encnewmap(value, 1, NB_EU_PATTERN, Songs[pos].Track, Encoder );
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
      newValue = encnewmap(value, 0, NB_EU_PATTERN, Songs[pos].Next, Encoder );
      if (newValue != Songs[pos].Next )
      {
        Songs[pos].Next = newValue;
        ret = true;
        UpdateCtrl(cpt);
      };
      break;  
    }
    break;
  }

  return ret;
}


boolean SeqEuclid::ChangePage(int value, byte Encoder)
{  
  boolean ret = false;
  int newValue;

  switch(Mode)
  {
  case MODE_PATT:
    newValue = encnewmap(value, 1, 4  , CurPage,Encoder);
    if ( newValue != CurPage )
    {
      ret = true;
      CurPage = newValue;
      DessinPage();
    }
    break;
  case MODE_SONG:
    newValue = encnewmap(value, 0, NB_EU_SONG - 1, CurTrSong, Encoder);
    if ( newValue != CurTrSong )
    {
      ret = true;
      ChangeCurPlaySong(newValue);
    }
    break;
  }
  return ret;
}


boolean SeqEuclid::ChangeM1(int value, byte Encoder)
{
  boolean ret = false;
  if ( Menu->ChangeM1(value, Encoder) )
  {
    if ( Menu->FirstPage )
    {  
      ParamGlo.Tempo = Menu->M1;
      ret = true;
      SClock.ChangeTempo();
    }
  }
  return ret;

}

boolean SeqEuclid::ChangeM2(int value, byte Encoder)
{
  boolean ret = false;
  if ( Menu->ChangeM2(value, Encoder) )
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

boolean SeqEuclid::ChangeM3(int value, byte Encoder)
{
  boolean ret = false;
  if ( Menu->ChangeM3(value, Encoder) )
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

void SeqEuclid::ExStart(boolean pSong)
{
  if ( is_played != STOPPED ) return;
  is_played_Mode_Song = pSong;  
  steep = 0;
  if ( is_played_Mode_Song ) {
    ChangeCurPlaySong(0);
    nbSteep=-1;
    CurPattern = Songs[CurPlaySong].Track-1;
    if ( ActiFunc == MODE_SEQEUCLID && Menu->FirstPage) Menu->ShowM3(CurPattern+1);
  }
  is_played = PLAYED;
  SClock.PostStart();
}

void SeqEuclid::ExStop()
{
  if ( is_played != STOPPED ) {
    SClock.PostStop();
    is_played = STOPPED;
  }
}



boolean SeqEuclid::ChangeS1S5(int value)
{
  boolean ret = false;

  if ( Menu->ChangeS1S5(value) )
  {
    if ( Menu->FirstPage )
    {  
      switch (Menu->S1S5)
      {
      case 2:
        Mode = (Mode==MODE_SONG)? MODE_PATT : MODE_SONG;
        DessinPage();
        break;

      case 3 :
        if (is_played==STOPPED) {
          ExStart(Mode==MODE_SONG);
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
        
      case 4:
        is_echo = !is_echo;
        DessinPage();
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
        case 0:  /* Unit */
          memcpy( &Tracks[CurPattern][Menu->V2-1], &Tracks[CurPattern][Menu->V1-1], sizeof (euTrack) ) ;
          
          if ( Menu->V1-1 == 0 && Menu->V2-1 > 0 ) // si source = UNit 0
              Tracks[CurPattern][Menu->V2-1].Lg = 1; // Longeur de source

          if ( Menu->V1-1 > 0 && Menu->V2-1 == 0 ) // si destin = Unit 0
              if ( Tracks[CurPattern][Menu->V2-1].Lg > 1 ) Tracks[CurPattern][Menu->V2-1].Lg--; // Longeur de source
          
          PatLed.Show(0);
          DessinPage();
          break;

        case 1:  /* Track */
          for (int pos=0;pos<NB_TRACK;pos++)
          {
              memcpy( &Tracks[Menu->V2-1][pos], &Tracks[Menu->V1-1][pos], sizeof (euTrack) ) ;
          }
          PatLed.Show(0);
          DessinPage();
          break;

        case 2:  /* Load */
          if ( MySD.OpenFileForRead(MODE_SEQEUCLID, Menu->GetFName(FileName) ) )
          {
            Restore();
            MySD.CloseFile();
          }
          PatLed.Show(0);
          DessinPage();
          break;

        case 3:  /* Save */
          if ( MySD.OpenFileForWrite(MODE_SEQEUCLID, Menu->MenuFName(FileName) ) )
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

        case 5:
          break;
        }         
      }
    }
  }
  return ret;
}


void SeqEuclid::Backup()
{
  MySD.WriteStream((byte *)Tracks, sizeof(Tracks) ) ;
}

void SeqEuclid::Restore()
{
  MySD.ReadStream((byte *)Tracks, sizeof(Tracks) ) ;
}

boolean SeqEuclid::ChangePAT(int value)
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
  return ret;
}


void SeqEuclid::Encoder(byte Value)
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

void SeqEuclid::Interface(int Num, int Value)
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
    ChangeValue( Num , Value,0 );
    break;

  case 10:
    lastChangeValue = CHGT_PAGE;
    if ( ChangePage(Value,0) ) mxAD.PageChanged();
    break;

  case 11:
    lastChangeValue = CHGT_M1;
    ChangeM1(Value,0);
    break;

  case 12:
    lastChangeValue = CHGT_M2;
    ChangeM2(Value,0);
    break;

  case 13:
    lastChangeValue = CHGT_M3;
    ChangeM3(Value,0);
    break;

  case 32:
    ChangeS1S5(Value);
    break;  

  case 33:
    ChangePAT(Value);
    break;  

  }
}

void SeqEuclid::Traite(uint32_t reftime)
{
  for (int pos=0;pos<NB_TRACK;pos++)
  {
    if ( Tracks[CurPattern][pos].IsOn && Tracks[CurPattern][pos].TimeOff < reftime )
    {
      EventOff(pos);
    }
  }
}


void SeqEuclid::TraiteEvent(uint32_t reftime)
{
  int PosTrack, pos;
  if (cpt24==0  && is_played==PLAYED)
  {
    int modsteep  = (steep % Tracks[CurPattern][0].Lg) ;
    if ( ActiFunc == MODE_SEQEUCLID )  PatLed.SetStep( modsteep + 1);
    
    if ( is_played_Mode_Song )
    {
      if (modsteep==0)
      {
        for (pos=0;pos<NB_TRACK;pos++)
          if ( Tracks[CurPattern][pos].Lg > 0 && Tracks[CurPattern][pos].IsOn ) EventOff(pos);
        nbSteep++;
        if ( nbSteep >= Songs[CurPlaySong].Repeat ) 
        {
          if ( Songs[CurPlaySong].Next > 0 ) {
            ChangeCurPlaySong( Songs[CurPlaySong].Next-1 );
            CurPattern = Songs[CurPlaySong].Track-1;
            if ( ActiFunc == MODE_SEQEUCLID && Menu->FirstPage) Menu->ShowM3(CurPattern+1);
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
      if ( modsteep==0 && NextPattern > -1 )
      {
        for (pos=0;pos<NB_TRACK;pos++)
          if ( Tracks[CurPattern][pos].Lg > 0 && Tracks[CurPattern][pos].IsOn ) EventOff(pos);
        CurPattern=NextPattern;
        DessinPage();
        NextPattern=-1;
      }    
    }
    for (pos=0;pos<NB_TRACK;pos++)
    {  
      if ( Tracks[CurPattern][pos].Lg > 0 )
      {
        if ( pos == 0 ) 
        {
          PosTrack = ( steep % Tracks[CurPattern][pos].Lg );
          if ( Tracks[CurPattern][pos].Pat[PosTrack] > 0 )
          {
            EventOn(pos, reftime);
          } 
        }  
        else
        {
          PosTrack = Tracks[CurPattern][pos].Lg==1 ? ( steep % Tracks[CurPattern][0].Lg ) : ( steep % Tracks[CurPattern][pos].Lg - 1 );
          if ( Tracks[CurPattern][pos].Pat[PosTrack] > 0 )
          {
            EventOn(pos, reftime);
          } 
        }
      }
    }
    steep++;
  } 
  else
  {
    if ( ActiFunc==MODE_SEQEUCLID && is_played==STOPPED )  PatLed.SetStep(CurPattern+1);
  } 
  //  next_event = next_event + delta_event + ( groove * ( random(delta_event) - delta_event / 2 ) ) / 50;
  if (++cpt24>=nb24) cpt24=0;
}


void SeqEuclid::UpdatePattern(int pos)
{
  if ( pos==0 )
  {
    EuclidGenerator(Tracks[CurPattern][pos].Pat, Tracks[CurPattern][pos].Lg, Tracks[CurPattern][pos].Div);
    for (int ct=1;ct<NB_TRACK;ct++)
      if (  Tracks[CurPattern][ct].Lg == 1) EuclidGenerator(Tracks[CurPattern][ct].Pat, Tracks[CurPattern][0].Lg, Tracks[CurPattern][ct].Div);
  }
  else
  {
    switch (Tracks[CurPattern][pos].Lg)
    {
    case 0 :
      EuclidGenerator(Tracks[CurPattern][pos].Pat, Tracks[CurPattern][pos].Lg, Tracks[CurPattern][pos].Div);
      break;
    case 1 :
      EuclidGenerator(Tracks[CurPattern][pos].Pat, Tracks[CurPattern][0].Lg, Tracks[CurPattern][pos].Div);
      break;
    default :
      EuclidGenerator(Tracks[CurPattern][pos].Pat, Tracks[CurPattern][pos].Lg-1, Tracks[CurPattern][pos].Div);
      break; 
    }
  }
}

void SeqEuclid::EventOff(int pos)
{

  SPOut.NoteOff(Tracks[CurPattern][pos].Dest1,Tracks[CurPattern][pos].Played,0,Tracks[CurPattern][pos].Chanel);
  SPOut.NoteOff(Tracks[CurPattern][pos].Dest2,Tracks[CurPattern][pos].Played,0,Tracks[CurPattern][pos].Chanel);
  Tracks[CurPattern][pos].IsOn = false;
}

void SeqEuclid::EventOn(int pos, uint32_t reftime)
{
  int Veloci;
  if ( Tracks[CurPattern][pos].IsOn ) EventOff(pos);


  Veloci = Tracks[CurPattern][pos].Veloci;

  Tracks[CurPattern][pos].Played = Tracks[CurPattern][pos].Note;

  if (Tracks[CurPattern][pos].Played < 24 ) {
    
      switch( ParamGlo.EuSrc )
      {
        case 0 : Tracks[CurPattern][pos].Played = Tracks[CurPattern][pos].Note + ParamGlo.LastNote; LastOffset = ParamGlo.LastNote; break;
        case 1 : Tracks[CurPattern][pos].Played = Tracks[CurPattern][pos].Note + ParamGlo.LastUpperNote; LastOffset = ParamGlo.LastUpperNote; break;
        case 2 : Tracks[CurPattern][pos].Played = Tracks[CurPattern][pos].Note + ParamGlo.LastLowerNote; LastOffset = ParamGlo.LastLowerNote; break; 
        case 3 : Tracks[CurPattern][pos].Played = Tracks[CurPattern][pos].Note + LastOffset; break; 
      }
  }


  if ( Tracks[CurPattern][pos].Modul )
  {
    //  char *listMul    ="Off    LF1*VelLF1*NotLF2*VelLF2*Not";
    switch( Tracks[CurPattern][pos].Modul )
    {
    case 1:
      Veloci+= ( LFoBi.LFos[0].Out * ( ( 127 * Tracks[CurPattern][pos].Ampli ) / 100 ) / LFO_RESO ) ;
      break;

    case 2:
      Tracks[CurPattern][pos].Played += ( LFoBi.LFos[0].Out * ( ( 24 * Tracks[CurPattern][pos].Ampli ) / 100 ) / LFO_RESO ) ;
      break;

    case 3:
      Veloci+= ( LFoBi.LFos[1].Out * ( ( 127 * Tracks[CurPattern][pos].Ampli ) / 100 ) / LFO_RESO ) ;
      break;

    case 4:
      Tracks[CurPattern][pos].Played += ( LFoBi.LFos[1].Out * ( ( 24 * Tracks[CurPattern][pos].Ampli ) / 100 ) / LFO_RESO ) ;
      break;

    }
    if (Veloci<0) Veloci=0;
    if (Veloci>127) Veloci=127;
    if (Tracks[CurPattern][pos].Played<1) Tracks[CurPattern][pos].Played=1;
    if (Tracks[CurPattern][pos].Played>127) Tracks[CurPattern][pos].Played=127;
  }

  SPOut.NoteOn(Tracks[CurPattern][pos].Dest1,Tracks[CurPattern][pos].Played,Veloci,Tracks[CurPattern][pos].Chanel);
  SPOut.NoteOn(Tracks[CurPattern][pos].Dest2,Tracks[CurPattern][pos].Played,Veloci,Tracks[CurPattern][pos].Chanel);
  Tracks[CurPattern][pos].IsOn = true;
  Tracks[CurPattern][pos].TimeOff = reftime + ( delta_event * Tracks[CurPattern][pos].Duree ) / 10 ;
}


void SeqEuclid::HandleNoteOn(byte Interface, byte channel, byte pitch, byte velocity)
{
  if ( is_echo && is_played==STOPPED && ActiFunc==NumFunc )
  {
    SPOut.NoteOn(Tracks[CurPattern][0].Dest1,pitch,velocity,Tracks[CurPattern][0].Chanel);
    SPOut.NoteOn(Tracks[CurPattern][0].Dest2,pitch,velocity,Tracks[CurPattern][0].Chanel);
  }
}


void SeqEuclid::HandleNoteOff(byte Interface, byte channel, byte pitch, byte velocity)
{
  if ( is_echo && is_played==STOPPED && ActiFunc==NumFunc )
  {
    SPOut.NoteOff(Tracks[CurPattern][0].Dest1, pitch, velocity,Tracks[CurPattern][0].Chanel);
    SPOut.NoteOff(Tracks[CurPattern][0].Dest2, pitch, velocity,Tracks[CurPattern][0].Chanel);
  }
}


void SeqEuclid::HandlePitchBend(byte Interface, byte channel, int bend)
{
  if ( is_echo && is_played==STOPPED && ActiFunc==NumFunc )
  {
    SPOut.PitchBend(Tracks[CurPattern][0].Dest1, bend, Tracks[CurPattern][0].Chanel);
    SPOut.PitchBend(Tracks[CurPattern][0].Dest2, bend, Tracks[CurPattern][0].Chanel);
  }
}

void SeqEuclid::HandleControlChange(byte Interface, byte channel, byte number, byte value)
{
  if ( is_echo && is_played==STOPPED && ActiFunc==NumFunc )
  {
    SPOut.ControlChange(Tracks[CurPattern][0].Dest1, number, value, Tracks[CurPattern][0].Chanel);
    SPOut.ControlChange(Tracks[CurPattern][0].Dest2, number, value, Tracks[CurPattern][0].Chanel);
  }

}

void SeqEuclid::HandleProgramChange(byte Interface, byte channel, byte number)
{
  if ( is_echo && is_played==STOPPED && ActiFunc==NumFunc )
  {
    SPOut.ProgramChange(Tracks[CurPattern][0].Dest1, number, Tracks[CurPattern][0].Chanel);
    SPOut.ProgramChange(Tracks[CurPattern][0].Dest2, number, Tracks[CurPattern][0].Chanel);
  }

}

void SeqEuclid::HandleAfterTouch(byte Interface, byte channel, byte value)
{
  if ( is_echo && is_played==STOPPED && ActiFunc==NumFunc )
  {
    SPOut.AfterTouch(Tracks[CurPattern][0].Dest1, value, Tracks[CurPattern][0].Chanel);
    SPOut.AfterTouch(Tracks[CurPattern][0].Dest2, value, Tracks[CurPattern][0].Chanel);
  }
}









