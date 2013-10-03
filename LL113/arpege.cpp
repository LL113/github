// -----------------------------------------------
// LL113
// -----------------------------------------------
// Historique:
// (Baloran) laurent.lecatelier@free.fr, version 0.0.1  10/02/12013
// -----------------------------------------------
// Arpege.cpp Code Class gestion de l'arpegiateur


#include <LiquidCrystal.h>
#include "Arduino.h"
#include "Arpege.h"
#include "utilld.h"
#include "lfo.h"
#include "SDFile.h"
#include "multiplexAD.h"
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

extern SuperOut SPOut;

const char *ArpTitre="\x01 Arpeggiator      \x02";

extern int *NbClock;

extern const char *listOffOn;
extern const char *listDestin;
const char *listOffOnLock="Off    On     Lock   ";

const char *listAccord="Maj    min    7      Maj7   min7   Maj7augMaj7b5 mMaj7  sus4   sus2   aug    aug7   b5     dim    dim7   0      add9   6eme   min6   4/6    5      9eme   min9   7#9    min9b5 min9#5 Maj9   mMaj9  6/9    11eme  min11  13eme  min13  ";
const AccordNote NotetAccord[]= {
  { 
    0,4,7,0,0,0    }
  , { 
    0,3,7,0,0,0    }
  , { 
    0,4,7,10,0,0    }
  , { 
    0,4,7,11,0,0    }
  , { 
    0,3,7,10,0,0    }
  , { 
    0,4,8,11,0,0    }
  , { 
    0,4,6,11,0,0    }
  , { 
    0,3,7,11,0,0    }
  , { 
    0,5,7,0,0,0    }
  , { 
    0,2,7,0,0,0    }
  , { 
    0,4,8,0,0,0    }
  , { 
    0,4,8,10,0,0    }
  , { 
    0,4,6,0,0,0    }
  , { 
    0,3,6,0,0,0    }
  , { 
    0,3,6,9,0,0    }
  , { 
    0,3,6,10,0,0    }
  , { 
    0,4,7,14,0,0    }
  , { 
    0,4,7,9,0,0    }
  , { 
    0,3,7,9,0,0    }
  , { 
    0,5,9,0,0,0    }
  , { 
    0,7,12,0,0,0    }
  , { 
    0,4,7,10,14,0    }
  , { 
    0,3,7,10,14,0    }
  , { 
    0,4,7,10,15,0    }
  , { 
    0,3,6,10,14,0    }
  , { 
    0,3,8,10,14,0    }
  , { 
    0,4,7,11,14,0    }
  , { 
    0,3,7,11,14,0    }
  , { 
    0,4,7,9,14,0    }
  , { 
    0,4,7,10,14,17    }
  , { 
    0,3,7,10,14,17    }
  , { 
    0,4,7,10,14,0    }
  , { 
    0,3,7,10,14,0    }
};


const char *listMul ="Off    LF1*VelLF1*NotLF2*VelLF2*Not";
const char *listDirection=" /      \x04      /\x04     /\x05\x04_   \x04/     \x04_/\x05  "; 
const char *listNote     ="Sort   Played VelocitRand   13243..15243..12131..43424..";
const char *listVelmod   ="Real   Const  ";
const char *listMotif    ="Const  Euclid Pattern";


paramMenuList MenuListArp[] = {
  { 
    "Copy", "From", "To", 1, 1, NB_ARP_TRACK,0,2,1,NB_ARP_TRACK,0                   }
  , 
  { 
    "Load", "Name", "", 0, MODE_ARPEGE, -1, 0 , 0, 0, 0, 0             }
  ,
  { 
    "Save", "Name", "Indice", 0, 0, NB_STYLEMU, listStyleMenu , 1, 1, 64, 0               }
  , 
  { 
    "Init", "", "", 0, 0, 0, 0 , 0, 0, 0, 0                 }
  , 
  { 
    "", "", "", 0, 0, 0, 0 , 0, 0, 0, 0                                 }
};



paramMenu menuArp = {
  "Ply",
  "Sng",
  "Play",
  "-In",
  "BPM",          
  "S/Beat",     
  "Arpeg",          
  TEMPO_INIT,TEMPO_MIN,TEMPO_MAX,0,     
  4,1,16,0,     
  1,1,NB_ARP_TRACK,0,     
  MenuListArp 
};



Arpege::Arpege(byte pNumFunc)
{ 
  NumFunc = pNumFunc;
  Init();
  Menu = new mMenu(&menuArp, ArpTitre);
  mesure = menuArp.Val_2;
  nb24 =  CalcNb24(mesure);
  cpt24 = 0;
  is_echo = true;
  LastOffset = 0;
}

Arpege::~Arpege()
{

}

void Arpege::Init()
{
  CurArp=0;
  CurTrSong=0;
  CurPlaySong=0;
  NextArp=-1;
  for (int pos=0;pos<NB_ARP_TRACK;pos++)
  {
    Tracks[pos].Direction=0;          
    Tracks[pos].Octave=1;          
    Tracks[pos].Note=0;  
    Tracks[pos].Velomod=1;  
    Tracks[pos].Veloci=127;  
    Tracks[pos].Duree=9;   
    Tracks[pos].Motif=0;
    Tracks[pos].Lg=16;          
    Tracks[pos].Div=4;          
    Tracks[pos].Maintien=0;
    Tracks[pos].Transpose=0;
    Tracks[pos].NMin=0;   
    Tracks[pos].NMax=127;   
    Tracks[pos].Modul=0;      
    Tracks[pos].Ampli=0;     
    Tracks[pos].Dest1=NOUT_MIDI1;
    Tracks[pos].Dest2=NOUT_NONE;
    Tracks[pos].Chanel=1;  
    Tracks[pos].FillAuto=0;
    Tracks[pos].Tonale=48;  
    Tracks[pos].Accord=0;
    Tracks[pos].Pattern = 0xFFF;
  }
  steep = 0;
  PosTrack=0;
  is_played=STOPPED;
  is_mode_write=true;
  is_played_Mode_Song=false;

  for (CurTrSong=0;CurTrSong<NB_TR_SONG;CurTrSong++)
  {  
    Songs[CurTrSong].Track = ( CurTrSong % NB_ARP_TRACK ) + 1;      
    Songs[CurTrSong].Repeat = 1;      
    Songs[CurTrSong].Transpose = 0;      
    Songs[CurTrSong].Next = (CurTrSong+1==NB_TR_SONG)?0:CurTrSong+2;   
  }
  CurTrSong=0;
  mV_MIDI1=0;
  mV_MIDI2=0;
  mV_MIDI3=0;
  IsOn=false;
  TimeOff=0;    
  CurNote=0;
  CurVelo=0;
  Mode=MODE_PATT;
  UpdatePattern();
  AllIsOff=true;

  lastChangeValue = CHGT_PAGE;

}


void Arpege::DessinPage()
{
  if (ActiFunc != NumFunc) return;

  int pos=CurArp;
  
  menuArp.Val_1 = ParamGlo.Tempo;
  menuArp.Val_2 = mesure;
  menuArp.Val_3 = CurArp+1;

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

    if (is_mode_write) {
      lcdM.setCursor(0,3);
      lcdM.print("Ply");
    }
    else
    {
      lcdM.setCursor(0,3);
      lcdM.print("Drw");
    }

    if (Mode==MODE_PATT) {
      lcdM.setCursor(4,3);
      lcdM.print("Sng");
    }
    else
    {
      lcdM.setCursor(4,3);
      lcdM.print("Arp");
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

    sprintf(tmpCh,"\x02\Arpg %d",pos+1);
    Z_Titre(1,tmpCh);

    lcd_Liste(tmpVal,(char *)listDirection,Tracks[pos].Direction);
    Z_TextValue(2,"Evolu.",tmpVal);

    lcd_Num(tmpVal,Tracks[pos].Octave);
    Z_TextValue(3,"Octave",tmpVal);

    lcd_Liste(tmpVal,(char *)listNote,Tracks[pos].Note);
    Z_TextValue(4,"Note",tmpVal);

    lcd_Num(tmpVal,Tracks[pos].Duree);
    Z_TextValue(5,"Durati.",tmpVal);

    lcd_Liste(tmpVal,(char *)listVelmod,Tracks[pos].Velomod);
    Z_TextValue(6,"Vel Mod",tmpVal);

    lcd_Num(tmpVal,Tracks[pos].Veloci);
    Z_TextValue(7,"Velocit",tmpVal);

    lcd_Liste(tmpVal,(char *)listMotif,Tracks[pos].Motif);
    Z_TextValue(8,"Motif",tmpVal);

    lcd_Num(tmpVal,Tracks[pos].Lg);      
    Z_TextValue(9,"Steps",tmpVal);

    lcd_Num(tmpVal,Tracks[pos].Div);
    Z_TextValue(10,"Divide",tmpVal);
    /*
    lcd_Num(tmpVal,Tracks[pos].NMin);
     Z_TextValue(11,"Not.Min",tmpVal);
     
     lcd_Num(tmpVal,Tracks[pos].NMax);
     Z_TextValue(12,"Not.Max",tmpVal);
     */

    lcd_Liste(tmpVal,(char *)listOffOn,Tracks[pos].FillAuto);
    Z_TextValue(11,"AutoFil",tmpVal);

    lcd_NoteAbs(tmpVal,Tracks[pos].Tonale);
    Z_TextValue(12,"Dominan",tmpVal);

    lcd_Liste(tmpVal,(char *)listAccord,Tracks[pos].Accord);
    Z_TextValue(13,"Tonalit",tmpVal);

    lcd_Liste(tmpVal,(char *)listOffOnLock,Tracks[pos].Maintien);
    Z_TextValue(14,"Hold",tmpVal);

    lcd_Liste(tmpVal,(char *)listOffOn,Tracks[pos].Transpose);
    Z_TextValue(15,"Transpo",tmpVal);

    lcd_Liste(tmpVal,(char *)listMul,Tracks[pos].Modul);
    Z_TextValue(16,"Modula.",tmpVal);

    lcd_Num(tmpVal,Tracks[pos].Ampli);
    Z_TextValue(17,"M.Level",tmpVal);

    lcd_Liste(tmpVal,(char *)listDestin,Tracks[pos].Dest1);
    Z_TextValue(18,"Dest.1",tmpVal);

    lcd_Liste(tmpVal,(char *)listDestin,Tracks[pos].Dest2);
    Z_TextValue(19,"Dest.2",tmpVal);

    lcd_Num(tmpVal,Tracks[pos].Chanel);
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


void Arpege::ChangeCurPlaySong(int value) 
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

void Arpege::UpdateCtrl(int cpt)
{

  if (ActiFunc != NumFunc) return;

  int pos = CurArp;

  switch(Mode)
  {
  case MODE_PATT :

    switch(cpt)
    {
    case 2:
      lcd_Liste(tmpVal,(char *)listDirection,Tracks[pos].Direction);
      Z_Value(cpt,tmpVal);
      break;

    case 3:
      lcd_Num(tmpVal,Tracks[pos].Octave);
      Z_Value(cpt,tmpVal);
      break;

    case 4:
      lcd_Liste(tmpVal,(char *)listNote,Tracks[pos].Note);
      Z_Value(cpt,tmpVal);
      break;

    case 5:
      lcd_Num(tmpVal,Tracks[pos].Duree);
      Z_Value(cpt,tmpVal);
      break;

    case 6:
      lcd_Liste(tmpVal,(char *)listVelmod,Tracks[pos].Velomod);
      Z_Value(cpt,tmpVal);
      break;

    case 7:
      lcd_Num(tmpVal,Tracks[pos].Veloci);
      Z_Value(cpt,tmpVal);
      break;

    case 8:
      lcd_Liste(tmpVal,(char *)listMotif,Tracks[pos].Motif);
      Z_Value(cpt,tmpVal);
      break;

    case 9:
      lcd_Num(tmpVal,Tracks[pos].Lg);
      Z_Value(cpt,tmpVal);
      break;

    case 10:
      lcd_Num(tmpVal,Tracks[pos].Div);
      Z_Value(cpt,tmpVal);
      break;



    case 11:
      lcd_Liste(tmpVal,(char *)listOffOn,Tracks[pos].FillAuto);
      Z_Value(cpt,tmpVal);
      break;

    case 12:
      lcd_NoteAbs(tmpVal,Tracks[pos].Tonale);
      Z_Value(cpt,tmpVal);
      break;

    case 13:
      lcd_Liste(tmpVal,(char *)listAccord,Tracks[pos].Accord);
      Z_Value(cpt,tmpVal);
      break;

      /*
    case 11:
       lcd_Num(tmpVal,Tracks[pos].NMin);
       Z_Value(cpt,tmpVal);
       break;
       
       case 12:
       lcd_Num(tmpVal,Tracks[pos].NMax);
       Z_Value(cpt,tmpVal);
       break;
       
       case 13:
       break;
       */

    case 14:
      lcd_Liste(tmpVal,(char *)listOffOnLock,Tracks[pos].Maintien);
      Z_Value(cpt,tmpVal);
      break;

    case 15:
      lcd_Liste(tmpVal,(char *)listOffOn,Tracks[pos].Transpose);
      Z_Value(cpt,tmpVal);
      break;

    case 16:
      lcd_Liste(tmpVal,(char *)listMul,Tracks[pos].Modul);
      Z_Value(cpt,tmpVal);
      break;

    case 17:
      lcd_Num(tmpVal,Tracks[pos].Ampli);
      Z_Value(cpt,tmpVal);
      break;

    case 18:
      lcd_Liste(tmpVal,(char *)listDestin,Tracks[pos].Dest1);
      Z_Value(cpt,tmpVal);
      break;

    case 19:
      lcd_Liste(tmpVal,(char *)listDestin,Tracks[pos].Dest2);
      Z_Value(cpt,tmpVal);
      break;    

    case 20:
      lcd_Num(tmpVal,Tracks[pos].Chanel);
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

boolean Arpege::ChangeValue(int cpt, int value, byte Encoder)
{  
  int newValue;
  int pos;
  int ret=false;

  // cpt compte de 1 ÃƒÆ’  ...
  if (cpt >=16) cpt-=6;
  cpt++;


  switch(Mode)
  {
  case MODE_PATT :


    switch(cpt)
    {
/*      
    case 1:
      newValue = encnewmap(value, 0, NB_ARP_TRACK-1, CurArp, Encoder);
      if (newValue != CurArp )
      {
        if ( IsOn ) EventOff();
        ret = true;
        CurArp = newValue;
        DessinPage();
        UpdatePattern();
      }
      break;      
*/
    case 2:
      newValue = encnewmap(value, 0, 5, Tracks[CurArp].Direction, Encoder);
      if (newValue != Tracks[CurArp].Direction )
      {
        Tracks[CurArp].Direction = newValue;
        ret = true;
        UpdateCtrl(cpt);
        UpdatePattern();
      };
      break;
    case 3:
      newValue = encnewmap(value, 1, 6, Tracks[CurArp].Octave , Encoder );
      if (newValue != Tracks[CurArp].Octave )
      {
        Tracks[CurArp].Octave = newValue;
        ret = true;
        UpdateCtrl(cpt);
        UpdatePattern();
      };
      break;
    case 4:
      newValue = encnewmap(value, 0, 6, Tracks[CurArp].Note , Encoder );
      if (newValue != Tracks[CurArp].Note )
      {
        Tracks[CurArp].Note = newValue;
        ret = true;
        UpdateCtrl(cpt);
        UpdatePattern();
      };
      break;
    case 5:
      newValue = encnewmap(value, 1, 40, Tracks[CurArp].Duree , Encoder );
      if (newValue != Tracks[CurArp].Duree )
      {
        Tracks[CurArp].Duree = newValue;
        ret = true;
        UpdateCtrl(cpt);
        UpdatePattern();
      };
      break;
    case 6:
      newValue = encnewmap(value, 0, 1,  Tracks[CurArp].Velomod , Encoder );
      if (newValue != Tracks[CurArp].Velomod )
      {
        Tracks[CurArp].Velomod = newValue;
        ret = true;
        UpdateCtrl(cpt);
      };
      break;
    case 7:
      newValue = encnewmap(value, 0, 127, Tracks[CurArp].Veloci , Encoder );
      if (newValue != Tracks[CurArp].Veloci )
      {
        Tracks[CurArp].Veloci = newValue;
        ret = true;
        UpdateCtrl(cpt);
        UpdatePattern();
      };
      break;
    case 8:
      newValue = encnewmap(value, 0, 2, Tracks[CurArp].Motif , Encoder );
      if (newValue != Tracks[CurArp].Motif )
      {
        Tracks[CurArp].Motif = newValue;
        ret = true;
        UpdateCtrl(cpt);
        UpdatePattern();
      };
      break;
    case 9:
      newValue = encnewmap(value, 1, NB_EU_STEP_MAX, Tracks[CurArp].Lg , Encoder );
      if (newValue != Tracks[CurArp].Lg )
      {
        Tracks[CurArp].Lg = newValue;
        ret = true;
        UpdateCtrl(cpt);
        UpdatePattern();
      };
      break;

    case 10:
      newValue = encnewmap(value, 1, NB_EU_STEP_MAX, Tracks[CurArp].Div , Encoder );
      if (newValue != Tracks[CurArp].Div )
      {
        Tracks[CurArp].Div = newValue;
        ret = true;
        UpdateCtrl(cpt);
        UpdatePattern();
      };
      break;

    case 11:
      newValue = encnewmap(value, 0, 1, Tracks[CurArp].FillAuto , Encoder );
      if (newValue != Tracks[CurArp].FillAuto )
      {
        Tracks[CurArp].FillAuto = newValue;
        ret = true;
        UpdateCtrl(cpt);
        UpdatePattern();
      };
      break;

    case 12:
      newValue = encnewmap(value, 12, 96, Tracks[CurArp].Tonale , Encoder );
      if (newValue != Tracks[CurArp].Tonale )
      {
        Tracks[CurArp].Tonale = newValue;
        ret = true;
        UpdateCtrl(cpt);
        UpdatePattern();
      };
      break;

    case 13:
      newValue = encnewmap(value, 0, 32, Tracks[CurArp].Accord , Encoder );
      if (newValue != Tracks[CurArp].Accord )
      {
        Tracks[CurArp].Accord = newValue;
        ret = true;
        UpdateCtrl(cpt);
        UpdatePattern();
      };
      break;

      /*
  case 11:
       newValue = newmap(value, 0, 127 );
       if (newValue != Tracks[CurArp].NMin )
       {
       Tracks[CurArp].NMin = newValue;
       ret = true;
       UpdateCtrl(cpt);
       UpdatePattern();
       };
       break;
       
       case 12:
       newValue = newmap(value, 0, 127 );
       if (newValue != Tracks[CurArp].NMax )
       {
       Tracks[CurArp].NMax = newValue;
       ret = true;
       UpdateCtrl(cpt);
       UpdatePattern();
       };
       break;
       
       case 13:
       break;
       */
    case 14:
      newValue = encnewmap(value, 0, 2, Tracks[CurArp].Maintien , Encoder );
      if (newValue != Tracks[CurArp].Maintien )
      {
        Tracks[CurArp].Maintien = newValue;
        ret = true;
        UpdateCtrl(cpt);
      };
      break;

    case 15:
      newValue = encnewmap(value, 0, 1, Tracks[CurArp].Transpose , Encoder );
      if (newValue != Tracks[CurArp].Transpose )
      {
        Tracks[CurArp].Transpose = newValue;
        if ( newValue == 1) {
            switch( ParamGlo.ArpSrc )
            {
              case 0:  ParamGlo.LastNote = reelBuf[0].Note; break;
              case 1:  ParamGlo.LastUpperNote = reelBuf[0].Note; break;
              case 2:  ParamGlo.LastLowerNote = reelBuf[0].Note; break;
              case 3:  LastOffset = 0; break;
            }
        }
        ret = true;
        UpdateCtrl(cpt);
      };
      break;

    case 16:
      newValue = encnewmap(value, 0, 4, Tracks[CurArp].Modul , Encoder );
      if (newValue != Tracks[CurArp].Modul )
      {
        Tracks[CurArp].Modul = newValue;
        ret = true;
        UpdateCtrl(cpt);
      };
      break;      

    case 17:
      newValue = encnewmap(value, -99, 99, Tracks[CurArp].Ampli , Encoder );
      if (newValue != Tracks[CurArp].Ampli )
      {
        Tracks[CurArp].Ampli = newValue;
        ret = true;
        UpdateCtrl(cpt);
      };
      break;

    case 18:
      newValue = encnewmap(value, 0, NB_OUTS, Tracks[CurArp].Dest1 , Encoder );
      if (newValue != Tracks[CurArp].Dest1 )
      {
        if ( IsOn ) EventOff();
        SPOut.AutoOff( Tracks[CurArp].Dest1 );
        Tracks[CurArp].Dest1 = newValue;
        ret = true;
        UpdateCtrl(cpt);
      };
      break;

    case 19:
      newValue = encnewmap(value, 0, NB_OUTS, Tracks[CurArp].Dest2 , Encoder );
      if (newValue != Tracks[CurArp].Dest2 )
      {
        if ( IsOn ) EventOff();
        SPOut.AutoOff( Tracks[CurArp].Dest2 );
        Tracks[CurArp].Dest2 = newValue;
        ret = true;
        UpdateCtrl(cpt);
      };
      break;

    case 20:
      newValue = encnewmap(value, 1, 16, Tracks[CurArp].Chanel , Encoder );
      if (newValue != Tracks[CurArp].Chanel )
      {
        if ( IsOn ) EventOff();
        SPOut.AutoOff( Tracks[CurArp].Dest1 );
        SPOut.AutoOff( Tracks[CurArp].Dest2 );
        Tracks[CurArp].Chanel = newValue;
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
      newValue = encnewmap(value, 1, NB_ARP_TRACK, Songs[pos].Track , Encoder );
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
      newValue = encnewmap(value, 1, 16, Songs[pos].Repeat , Encoder );
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
      newValue = encnewmap(value, -12, +12, Songs[pos].Transpose , Encoder );
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
      newValue = encnewmap(value, 0, NB_TR_SONG, Songs[pos].Next , Encoder );
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


boolean Arpege::ChangePage(int value, byte Encoder)
{  
  boolean ret = false;
  int newValue;
  switch(Mode)
  {
  case MODE_PATT:
    newValue = encnewmap(value, 0, NB_ARP_TRACK-1, CurArp, Encoder);
    if (newValue != CurArp )
    {
      if ( IsOn ) EventOff();
      ret = true;
      CurArp = newValue;
      DessinPage();
      UpdatePattern();
    }
    break;
  case MODE_SONG:
    newValue = encnewmap(value, 0, NB_TR_SONG - 1, CurPlaySong, Encoder);
    if ( newValue != CurPlaySong )
    {
      ret = true;
      ChangeCurPlaySong(newValue);
    }
    break;
  }
  return ret;
}


boolean Arpege::ChangeM1(int value, byte Encoder)
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

boolean Arpege::ChangeM2(int value, byte Encoder)
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

boolean Arpege::ChangeM3(int value, byte Encoder)
{
  boolean ret = false;
  if ( Menu->ChangeM3(value, Encoder) )
  {
    if ( Menu->FirstPage )
    {  
      int newValue =  Menu->M3-1;
      if (is_played==PLAYED)
      {
        NextArp = newValue;
      }
      else
      {
        if ( IsOn ) EventOff();
        ret = true;
        CurArp = newValue;
        DessinPage();
        UpdatePattern();
      }
      ret = true;
    }
  }
  return ret;
}


void Arpege::ExStart(boolean pSong)
{
  if ( is_played != STOPPED ) return;
  ParamGlo.MidiInput->Reset(1);
  AllIsOff = true;
  is_played_Mode_Song = pSong;
  steep = 0;
  PosTrack = 0;
  NextArp=-1;
  if ( is_played_Mode_Song ) {
    ChangeCurPlaySong(0);
    nbSteep=-1;
    CurArp = Songs[CurPlaySong].Track-1;
    if ( ActiFunc == NumFunc && Menu->FirstPage) Menu->ShowM3(CurArp+1);

  }
  UpdatePattern();
  is_played = PLAYED;
  SClock.PostStart();
}

void Arpege::ExStop()
{
  if ( is_played != STOPPED ) {
      SClock.PostStop();
      is_played = STOPPED;
  }
}


boolean Arpege::ChangeS1S5(int value)
{
  boolean ret = false;
  if ( Menu->ChangeS1S5(value) )
  {
    if ( Menu->FirstPage )
    {  
      switch (Menu->S1S5)
      {
      case 1:
        is_mode_write = !is_mode_write;
        DessinPage();
        break;

      case 2:
        Mode = (Mode==MODE_SONG)? MODE_PATT : MODE_SONG;
        DessinPage();
        break;

      case 3 :
        if ( is_played == STOPPED  ) {
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
        case 0:  /* copy */

          memcpy( &Tracks[Menu->V2-1], &Tracks[Menu->V1-1], sizeof (arpTrack) ) ;
          PatLed.Show(0);
          DessinPage();
          break;

        case 1:  /* Load */


          SPOut.AutoOff( ParamGlo.UpperDest );
          SPOut.AutoOff( ParamGlo.LowerDest );

          if ( MySD.OpenFileForRead(MODE_ARPEGE, Menu->GetFName(FileName) ) )
          {
            Restore();
            MySD.CloseFile();
          }
          PatLed.Show(0);
          DessinPage();
          break;

        case 2:  /* Save */

          if ( MySD.OpenFileForWrite(MODE_ARPEGE, Menu->MenuFName(FileName) ) )
          {
            Backup();
            MySD.CloseFile();
          }
          PatLed.Show(0);
          break;

        case 3:  /* Init */

          Init();
          PatLed.Show(0);
          DessinPage();
          UpdatePattern();
          break;
        }         
      }
    }
  }

  return ret;
}

void Arpege::Backup()
{
  MySD.WriteStream((byte *)Tracks, sizeof(Tracks) ) ;
  MySD.WriteStream((byte *)Songs, sizeof(Songs) ) ;

}

void Arpege::Restore()
{
  MySD.ReadStream((byte *)Tracks, sizeof(Tracks) ) ;
  MySD.ReadStream((byte *)Songs, sizeof(Songs) ) ;
  UpdatePattern();
}


boolean Arpege::ChangePAT(int value)
{
  boolean ret = false;
  int newValue;

  switch(Mode)
  {
  case MODE_PATT:

    newValue = PatLed.Map(value);
    if ( newValue > 0 )
    {
      newValue--;
      if ( is_mode_write )
      {
        uint16_t masq =  1 << newValue;
        if ( ( Tracks[CurArp].Pattern & masq ) == masq )
          Tracks[CurArp].Pattern &= ~masq;
        else
          Tracks[CurArp].Pattern |= masq;
      }
      else
      {
        if ( newValue < NB_ARP_TRACK)
        {
          if (is_played==PLAYED)
          {
            NextArp = newValue;
          }
          else
          {
            if ( IsOn ) EventOff();
            ret = true;
            CurArp = newValue;
            DessinPage();
            UpdatePattern();
          }
        }
      }
    }
    break;

  case MODE_SONG:

    newValue = newmap(value, 0, NB_TR_SONG - 5);
    if ( newValue != CurTrSong )
    {
      ret = true;
      CurTrSong = newValue;
      DessinPage();
    }
    break;
  }


}


void Arpege::Encoder(byte Value)
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

void Arpege::Interface(int Num, int Value)
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
    ChangeValue( Num , Value , 0);
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



void iSortN(bufNote *preBuf, int nb)
{
  bufNote temp;
  int ct=0;
  while ( ct < nb-1 )
  {   
    int lg = ct;
    while ( preBuf[lg+1].Note < preBuf[lg].Note && lg>=0)
    {
      memcpy( &temp , &preBuf[lg], sizeof(bufNote));
      memcpy( &preBuf[lg], &preBuf[lg+1], sizeof(bufNote));
      memcpy( &preBuf[lg+1], &temp ,sizeof(bufNote));
      lg--;
    }
    ct++;
  }   
}

void iSortV(bufNote *preBuf, int nb)
{
  bufNote temp;
  int ct=0;
  while ( ct < nb-1 )
  {   
    int lg = ct;
    while ( preBuf[lg+1].Velo < preBuf[lg].Velo && lg>=0)
    {
      memcpy( &temp , &preBuf[lg], sizeof(bufNote));
      memcpy( &preBuf[lg], &preBuf[lg+1], sizeof(bufNote));
      memcpy( &preBuf[lg+1], &temp ,sizeof(bufNote));
      lg--;
    }
    ct++;
  }   
}

void iSortT(bufNote *preBuf, int nb)
{
  bufNote temp;
  int ct=0;
  while ( ct < nb-1 )
  {   
    int lg = ct;
    while ( preBuf[lg+1].rTime < preBuf[lg].rTime && lg>=0 )
    {
      memcpy( &temp , &preBuf[lg], sizeof(bufNote));
      memcpy( &preBuf[lg], &preBuf[lg+1], sizeof(bufNote));
      memcpy( &preBuf[lg+1], &temp ,sizeof(bufNote));
      lg--;
    }
    ct++;
  }   
}


void Arpege::UpdatePattern()
{

  int inSert = 0, ct, oct, rpos; 
  //   memset(&preBuf,0,sizeof(bufNote)*HIST_NOTE_LENGTH*4);
  
  if ( Tracks[CurArp].Maintien==2 ) return;
  memset(preBuf,0,sizeof(preBuf));
  long lAst = 0;

  if ( AllIsOff &&  !ParamGlo.MidiInput->AllIsOff ) {
    steep = 0;
    PosTrack=0;
  }
  AllIsOff =  ParamGlo.MidiInput->AllIsOff;

  if ( Tracks[CurArp].Motif == 1 )
  {
    EuclidGenerator(Pat, Tracks[CurArp].Lg, Tracks[CurArp].Div);
    Tracks[CurArp].Pattern = EuclidToPat(Pat, Tracks[CurArp].Lg);
  }

  if ( Tracks[CurArp].FillAuto==1)
  {
    for (oct=0; oct< Tracks[CurArp].Octave; oct++) 
    {
      byte ava=0;
      while ( ava < 6 )
      {
#ifdef _DEBUG_MODE_

        Serial.print(inSert);
        Serial.print(",");
        Serial.print(NotetAccord[Tracks[CurArp].Accord].Note[ava]);
        Serial.print(",");
        Serial.print(Tracks[CurArp].Accord);
        Serial.print(",");
        Serial.print(oct*12 );
        Serial.print(",");
        Serial.println(Tracks[CurArp].Tonale);

#endif

        preBuf[inSert].Note = NotetAccord[Tracks[CurArp].Accord].Note[ava] + oct*12 + Tracks[CurArp].Tonale;
        preBuf[inSert].Velo = Tracks[CurArp].Veloci ;
        preBuf[inSert].rTime = inSert;
        ava++;
        inSert++;
        if ( NotetAccord[Tracks[CurArp].Accord].Note[ava]==0 )  break;


      }
    }
  }
  else
  {
    if (  Tracks[CurArp].Transpose || ( ParamGlo.MidiInput->AllIsOff && Tracks[CurArp].Maintien==1 ) ) 
    {
      for (oct=0; oct< Tracks[CurArp].Octave; oct++) 
      {
        for (ct=0; ct< Tracks[CurArp].nbMemo; ct++) 
        {
          preBuf[inSert].Note = Tracks[CurArp].Memo[ct].Note + oct * 12;
          preBuf[inSert].Velo = Tracks[CurArp].Memo[ct].Velo;
          if ( oct==0 )
          {
            preBuf[inSert].rTime = Tracks[CurArp].Memo[ct].rTime;
            if (lAst <  preBuf[inSert].rTime ) lAst =preBuf[inSert].rTime; 
          }
          else
          {
            preBuf[inSert].rTime = lAst * oct + MemoNote.rTime[ct];
          }
          inSert++;   
        }
      }  
    }
    else
    {
      Tracks[CurArp].nbMemo=0;
      MemoNote = ParamGlo.MidiInput->hNote;
      memset(Tracks[CurArp].Memo, 0, sizeof(bufNote) * MAX_NOTE_OCTA);
      for (oct=0; oct< Tracks[CurArp].Octave; oct++) 
      {
        for (ct=0; ct< HIST_NOTE_LENGTH; ct++) 
        {
          if ( ( Tracks[CurArp].Maintien || ParamGlo.MidiInput->hNote.Etat[ct]==1 ) && ParamGlo.MidiInput->hNote.Note [ct] > 0 && ( ParamGlo.ArpSrc == 0 || ( ParamGlo.MidiInput->hNote.Note[ct] >= ParamGlo.NoteSplit && ParamGlo.ArpSrc == 1 ) || ( ParamGlo.MidiInput->hNote.Note[ct] < ParamGlo.NoteSplit && ParamGlo.ArpSrc == 2 )  ) )
          {
            preBuf[inSert].Note = ParamGlo.MidiInput->hNote.Note[ct] + oct * 12;

            if ( ParamGlo.ArpSrc == 1 ) preBuf[inSert].Note += ParamGlo.UpperTrans * 12;
            else if ( ParamGlo.ArpSrc == 2 ) preBuf[inSert].Note += ParamGlo.LowerTrans * 12;

            preBuf[inSert].Velo = ParamGlo.MidiInput->hNote.Velo[ct] ;
            if ( oct==0 )
            {
              preBuf[inSert].rTime = ParamGlo.MidiInput->hNote.rTime[ct];
              if (lAst <  ParamGlo.MidiInput->hNote.rTime[ct]) lAst =  ParamGlo.MidiInput->hNote.rTime[ct]; 
            }
            else
            {
              preBuf[inSert].rTime = lAst * oct + ParamGlo.MidiInput->hNote.rTime[ct];
            }

            if (oct==0 && inSert<MAX_NOTE_OCTA) 
            {
              Tracks[CurArp].Memo[Tracks[CurArp].nbMemo++] = preBuf[inSert];

            }

            inSert++;       
          }   
        }
      }  
    }
  }
  if (inSert==0) 
  {
    reelLen = 0;
    return;
  }   
  // char *listNote     ="Sort   Play   VelocitRand   13243..15243..12131..43424..";
  rpos=0;
  ct=0; 
  iSortN(preBuf, inSert);
  RefNote = preBuf[0].Note;
  switch(Tracks[CurArp].Note)
  {
  case 0 : // Sort
    break;
  case 1 : // Play
    iSortT(preBuf, inSert);
    break;
  case 2 : // Velocit
    iSortV(preBuf, inSert);
    break;
  case 3 : // Rand
    while ( ct < inSert )
    {
      preBuf[ct].rTime = random(100);
      ct++;
    }
    iSortT(preBuf, inSert);
    break;
  case 4 : // 13243
    while ( ct < inSert - 2 )
    {
      tmpBuf[rpos]=preBuf[ct];
      tmpBuf[rpos].rTime =rpos++;
      tmpBuf[rpos]=preBuf[ct+2];
      tmpBuf[rpos].rTime =rpos++;
      ct++;
    }
    inSert = rpos;
    ct=0;
    while ( ct < inSert)
    {
      preBuf[ct]=tmpBuf[ct];
      ct++;
    }
    iSortT(preBuf, inSert);
    break;
  case 5 : // 15243
    while ( ct < inSert / 2)
    {
      preBuf[ct].rTime =rpos++;
      preBuf[inSert-1-ct].rTime = rpos++;
      ct++;
    }
    iSortT(preBuf, inSert);
    break;
  case 6 : // 12131
    ct++;
    while ( ct < inSert  )
    {
      tmpBuf[rpos]=preBuf[0];
      tmpBuf[rpos].rTime =rpos++;
      tmpBuf[rpos]=preBuf[ct];
      tmpBuf[rpos].rTime =rpos++;
      ct++;
    }
    inSert = rpos;
    ct=0;
    while ( ct < inSert)
    {
      preBuf[ct]=tmpBuf[ct];
      ct++;
    }
    iSortT(preBuf, inSert);
    break;
  case 7 : // 43424
    while ( ct < inSert - 2)
    {
      tmpBuf[rpos]=preBuf[inSert - 1];
      tmpBuf[rpos].rTime =rpos++;
      tmpBuf[rpos]=preBuf[inSert - 2 - ct];
      tmpBuf[rpos].rTime =rpos++;
      ct++;
    }
    inSert = rpos;
    ct=0;
    while ( ct < inSert)
    {
      preBuf[ct]=tmpBuf[ct];
      ct++;
    }
    iSortT(preBuf, inSert);
    break;
  case 8 :
    break;

  }

  // char *listDirection=" /      \x04      /\x04     /\x05\x04_   \x04/     \x04_/\x05  "; 

  rpos=0;
  switch(Tracks[CurArp].Direction)
  {
  case 0:
    for (ct=0;ct<inSert; ct++) reelBuf[rpos++] = preBuf[ct];
    break;
  case 1:
    for (ct=0;ct<inSert; ct++) reelBuf[rpos++] = preBuf[inSert-1-ct];
    break;
  case 2:
    for (ct=0;ct<inSert; ct++) reelBuf[rpos++] = preBuf[ct];
    for (ct=1;ct<inSert-1; ct++) reelBuf[rpos++] = preBuf[inSert-1-ct];
    break;
  case 3:
    for (ct=0;ct<inSert; ct++) reelBuf[rpos++] = preBuf[ct];
    for (ct=0;ct<inSert; ct++) reelBuf[rpos++] = preBuf[inSert-1-ct];
    break;
  case 4:
    for (ct=0;ct<inSert; ct++) reelBuf[rpos++] = preBuf[inSert-1-ct];
    for (ct=1;ct<inSert-1; ct++) reelBuf[rpos++] = preBuf[ct];
    break;
  case 5:
    for (ct=0;ct<inSert; ct++) reelBuf[rpos++] = preBuf[inSert-1-ct];
    for (ct=0;ct<inSert; ct++) reelBuf[rpos++] = preBuf[ct];
    break;

  }    
  reelLen = rpos;      

}


void Arpege::Traite(uint32_t reftime)
{
  if ( IsOn && TimeOff < reftime )
  {
    EventOff();
  }
  return;    
} 


void Arpege::TraiteEvent(uint32_t reftime)
{
  if (  cpt24==0 )  
  { 
    if ( ParamGlo.MidiInput->hNote.Version != mV_MIDI1 ) // Midi1 a changÃƒÂ© 
    {
      mV_MIDI1 = ParamGlo.MidiInput->hNote.Version; 
      UpdatePattern();
    }


    if (is_played==PLAYED && reelLen)
    {
  
      if ( ActiFunc==NumFunc && Tracks[CurArp].Motif > 0 )  
      {
        PatLed.SetPatt( Tracks[CurArp].Pattern );
      }
  
  
      if ( is_played_Mode_Song )
      {
        if ( steep==0)
        {
          EventOff();
          nbSteep++;
          if ( nbSteep >= Songs[CurPlaySong].Repeat ) 
          {
            if ( Songs[CurPlaySong].Next > 0 ) {
              ChangeCurPlaySong( Songs[CurPlaySong].Next-1 );
              CurArp = Songs[CurPlaySong].Track-1;
              if ( ActiFunc == NumFunc && Menu->FirstPage) Menu->ShowM3(CurArp+1);
              UpdatePattern();
              NextArp=-1;
              PosTrack=0;
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
        if ( steep==0 && NextArp > -1 )
        {
          if ( IsOn ) EventOff();
          CurArp=NextArp;
          DessinPage();
          UpdatePattern();
          NextArp=-1;
          PosTrack=0;
        }    
      }
      if ( Tracks[CurArp].Motif == 0 || ( Tracks[CurArp].Motif == 1 && Pat[ steep ]  ) || ( Tracks[CurArp].Motif == 2 && ( Tracks[CurArp].Pattern & ( 1 << steep ) ) != 0  ))
      { 
        EventOn(PosTrack, reftime);
  
        if ( ActiFunc==NumFunc ) 
        {
          if ( Tracks[CurArp].Motif == 0 ) 
            PatLed.SetStep(PosTrack+1);
          else
            PatLed.SetStep(steep+1);
        }  
        PosTrack++;
  
        PosTrack %= reelLen;
      }   
      else
      {
        if ( ActiFunc==NumFunc && Tracks[CurArp].Motif > 0 )   PatLed.SetStep(steep+1);
      }  
  
      steep++;
      if ( Tracks[CurArp].Motif == 0 ) // Si pas motif on prend la longueur reel de l'arpege pour fin, sinon la longueur du motif 
        steep %= reelLen;
      else
        steep %= Tracks[CurArp].Lg;
    } 
    else
    {
      if ( ActiFunc==NumFunc && Tracks[CurArp].Motif > 0 )  
      {
        PatLed.SetPatt( Tracks[CurArp].Pattern );
      }
      else
      {
        if ( ActiFunc==NumFunc )  PatLed.SetStep(CurArp+1);
      }
    }
  }
  if (++cpt24>=nb24) cpt24=0;
}



void Arpege::EventOff()
{
  SPOut.NoteOff(Tracks[CurArp].Dest1, CurNote,0,Tracks[CurArp].Chanel);
  SPOut.NoteOff(Tracks[CurArp].Dest2, CurNote,0,Tracks[CurArp].Chanel);
  IsOn = false;
}

void Arpege::EventOn(int pos, uint32_t reftime)
{
  if ( IsOn ) EventOff();
  CurNote=reelBuf[pos].Note;
//  if ( Tracks[CurArp].Transpose && ( ParamGlo.ArpSrc == 0 || ( ParamGlo.MidiInput->LastNote >= ParamGlo.NoteSplit && ParamGlo.ArpSrc == 1 ) || ( ParamGlo.MidiInput->LastNote < ParamGlo.NoteSplit && ParamGlo.ArpSrc == 2 ) ) )
  if ( Tracks[CurArp].Transpose ) 
  {
//    CurNote += ( ParamGlo.MidiInput->LastNote - RefNote ); 
    switch( ParamGlo.ArpSrc )
    {
      case 0 : CurNote += ( ParamGlo.LastNote - RefNote ); LastOffset = ParamGlo.LastNote; break;
      case 1 : CurNote += ( ParamGlo.LastUpperNote - RefNote ); LastOffset = ParamGlo.LastUpperNote; break;
      case 2 : CurNote += ( ParamGlo.LastLowerNote - RefNote ); LastOffset = ParamGlo.LastLowerNote; break; 
      case 3 : CurNote += ( LastOffset - RefNote ); break; 
    }

  }

  if ( is_played_Mode_Song ) CurNote += Songs[CurPlaySong].Transpose;


  switch( Tracks[CurArp].Velomod )
  {
    //      char *listVelmod   ="Real   Const  Up     Down   UpDown Euclid Random ";
  case 0:
    CurVelo=reelBuf[pos].Velo;
    break;
  case 1:  
    CurVelo= Tracks[CurArp].Veloci;
    break;
  }

  if ( Tracks[CurArp].Modul )
  {
    //  char *listMul    ="Off    LF1*VelLF1*NotLF2*VelLF2*Not";
    switch( Tracks[CurArp].Modul )
    {
    case 1:
      CurVelo+= ( LFoBi.LFos[0].Out * ( ( 127 * Tracks[CurArp].Ampli ) / 100 ) / LFO_RESO ) ;
      break;

    case 2:
      CurNote += ( LFoBi.LFos[0].Out * ( ( 24 * Tracks[CurArp].Ampli ) / 100 ) / LFO_RESO ) ;
      break;

    case 3:
      CurVelo+= ( LFoBi.LFos[1].Out * ( ( 127 * Tracks[CurArp].Ampli ) / 100 ) / LFO_RESO ) ;
      break;

    case 4:
      CurNote += ( LFoBi.LFos[1].Out * ( ( 24 * Tracks[CurArp].Ampli ) / 100 ) / LFO_RESO ) ;
      break;

    }
    if (CurVelo<0) CurVelo=0;
    if (CurVelo>127) CurVelo=127;
    if (CurNote<1) CurNote=1;
    if (CurNote>127) CurNote=127;
  }


  SPOut.NoteOn(Tracks[CurArp].Dest1,CurNote,CurVelo,Tracks[CurArp].Chanel);
  SPOut.NoteOn(Tracks[CurArp].Dest2,CurNote,CurVelo,Tracks[CurArp].Chanel);


  IsOn = true;
  TimeOff = reftime + ( delta_event * Tracks[CurArp].Duree ) / 10 ;
}


void Arpege::HandleNoteOn(byte Interface, byte channel, byte pitch, byte velocity)
{
  if ( is_echo &&  is_played==STOPPED && ActiFunc==NumFunc )
  {
    SPOut.NoteOn(Tracks[CurArp].Dest1,pitch,velocity,Tracks[CurArp].Chanel);
    SPOut.NoteOn(Tracks[CurArp].Dest2,pitch,velocity,Tracks[CurArp].Chanel);
  }
}


void Arpege::HandleNoteOff(byte Interface, byte channel, byte pitch, byte velocity)
{
  if (  is_echo &&  is_played==STOPPED && ActiFunc==NumFunc )
  {
    SPOut.NoteOff(Tracks[CurArp].Dest1, pitch, velocity,Tracks[CurArp].Chanel);
    SPOut.NoteOff(Tracks[CurArp].Dest2, pitch, velocity,Tracks[CurArp].Chanel);
  }
}


void Arpege::HandlePitchBend(byte Interface, byte channel, int bend)
{
  if (  is_echo &&  is_played==STOPPED && ActiFunc==NumFunc )
  {
    SPOut.PitchBend(Tracks[CurArp].Dest1, bend, Tracks[CurArp].Chanel);
    SPOut.PitchBend(Tracks[CurArp].Dest2, bend, Tracks[CurArp].Chanel);
  }
}

void Arpege::HandleControlChange(byte Interface, byte channel, byte number, byte value)
{
  if (   is_echo && is_played==STOPPED && ActiFunc==NumFunc )
  {
    SPOut.ControlChange(Tracks[CurArp].Dest1, number, value, Tracks[CurArp].Chanel);
    SPOut.ControlChange(Tracks[CurArp].Dest2, number, value, Tracks[CurArp].Chanel);
  }

}

void Arpege::HandleProgramChange(byte Interface, byte channel, byte number)
{
  if (   is_echo && is_played==STOPPED && ActiFunc==NumFunc )
  {
    SPOut.ProgramChange(Tracks[CurArp].Dest1, number, Tracks[CurArp].Chanel);
    SPOut.ProgramChange(Tracks[CurArp].Dest2, number, Tracks[CurArp].Chanel);
  }

}

void Arpege::HandleAfterTouch(byte Interface, byte channel, byte value)
{
  if ( is_echo &&  is_played==STOPPED && ActiFunc==NumFunc )
  {
    SPOut.AfterTouch(Tracks[CurArp].Dest1, value, Tracks[CurArp].Chanel);
    SPOut.AfterTouch(Tracks[CurArp].Dest2, value, Tracks[CurArp].Chanel);
  }
}








