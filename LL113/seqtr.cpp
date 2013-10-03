// -----------------------------------------------
// LL113
// -----------------------------------------------
// Historique:
// (Baloran) laurent.lecatelier@free.fr, version 0.0.1  10/02/12013
// -----------------------------------------------
// seqtr.cpp Code Class gestion du sÃƒÂ©quenceur TRxxx

#include <LiquidCrystal.h>
// #include <MIDIMul.h> 
#include "Arduino.h"
#include "Seqtr.h"
#include "utilld.h"
#include "multiplexAD.h"
#include "lfo.h"
#include "SDFile.h"
#include "clock.h"


// Clock
extern SysClock SClock;

// Sequenceur Step
extern SDFile MySD;
extern mPatLed PatLed;
extern LFo LFoBi;
extern multiplexAD mxAD;
extern LiquidCrystal lcdM;
extern LiquidCrystal lcd1;
extern LiquidCrystal lcd2;
extern LiquidCrystal lcd3;
extern LiquidCrystal lcd4;

const char *TrTitre  ="\x01 TRxxx Sequencer  \x02";

extern const char *listDestin;
extern const char *listSource;
extern const char *listMul;
extern const char *listAction;
extern const char *listTTrig;
extern const char *listCtrl;
extern const char  *mListSource;
extern const char *listOffOn;

extern SuperOut SPOut;


paramMenuList MenuListTr[] = {
  { 
    "Copy", "From", "To", 1, 1, 16,0,2,1,16,0     }
  , 
  { 
    "Load", "Name", "", 0, MODE_SEQTR, -1, 0 , 0, 0, 0, 0 }
  ,
  { 
    "Save", "Name", "Indice", 0, 0, NB_STYLEMU, listStyleMenu , 1, 1, 64, 0   }
  , 
  { 
    "Init", "", "", 0, 0, 0, 0 , 0, 0, 0, 0             }
  , 
  { 
    "", "", "", 0, 0, 0, 0 , 0, 0, 0, 0                   }
};


paramMenu menuTr = {
  "Ply",
  "Sng",
  "Play",
  "-In",
  "BPM",          
  "S/Beat",     
  "Track",          
  TEMPO_INIT,TEMPO_MIN,TEMPO_MAX,0,     
  4,1,16,0,     
  1,1,NB_TR_PATTERN,0,     
  MenuListTr 
};


SeqTrxx::SeqTrxx(byte pNumFunc)
{ 
  NumFunc=pNumFunc;
  Init();
  Menu = new mMenu(&menuTr, TrTitre);
  mesure = menuTr.Val_2;
  nb24 =  CalcNb24(mesure);
  cpt24 = 0;
  is_echo=true;
  LastOffset = 0;
}

SeqTrxx::~SeqTrxx()
{

}

void SeqTrxx::Init()
{
  lastChangeValue = CHGT_PAGE;

  CurInstr=0;
  CurPattern=0;
  CurTrSong=0;
  
  for (CurPattern=0;CurPattern<NB_TR_PATTERN;CurPattern++)
  {  
    for (CurInstr=0;CurInstr<NB_TR_INST;CurInstr++)
    {
      Tracks[CurPattern].Instru[CurInstr].Active=1;
      if ( CurInstr==0 )
        Tracks[CurPattern].Instru[CurInstr].Pattern = 0x1111;
      else
        Tracks[CurPattern].Instru[CurInstr].Pattern = 0x0000;

      Tracks[CurPattern].Instru[CurInstr].Note = 48;      
      Tracks[CurPattern].Instru[CurInstr].Veloci = 100;
      Tracks[CurPattern].Instru[CurInstr].Duree = 5;      
      Tracks[CurPattern].Instru[CurInstr].Modul = 0;      
      Tracks[CurPattern].Instru[CurInstr].Ampli = 0;      
      Tracks[CurPattern].Instru[CurInstr].Dest1 = NOUT_MIDI1;
      Tracks[CurPattern].Instru[CurInstr].Dest2 = NOUT_NONE;
      Tracks[CurPattern].Instru[CurInstr].Chanel = 1;
      Tracks[CurPattern].Instru[CurInstr].IsOn = false;
      Tracks[CurPattern].Instru[CurInstr].TimeOff = 0;  

    }

    Tracks[CurPattern].Lg = 16;      
    Tracks[CurPattern].Accent = 8;      
  }
  CurPattern=0;
  NextPattern=-1;
  CurInstr=0;
  nbSteep=0;

  for (CurTrSong=0;CurTrSong<NB_TR_SONG;CurTrSong++)
  {  
    Songs[CurTrSong].Track = CurTrSong+1;      
    Songs[CurTrSong].Repeat = 1;      
    Songs[CurTrSong].Transpose = 0;      
    Songs[CurTrSong].Next = (CurTrSong==NB_TR_SONG)?0:CurTrSong+2;   
  }
  CurTrSong=0;
  Steep = 0;
  is_played=STOPPED;
  is_mode_write=true;
  is_played_Mode_Song=false;
  Mode=MODE_PATT;

}

void SeqTrxx::DessinPage()
{
  int pos;
  
  menuTr.Val_1 = ParamGlo.Tempo;
  menuTr.Val_2 = mesure;
  menuTr.Val_3 = CurPattern+1;

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
      lcdM.print("Trk");
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

    if (CurInstr==NB_TR_INST - 1)
      sprintf(tmpCh,"\x02\Accent");
    else
      sprintf(tmpCh,"\x02Instr %d",CurInstr+1);
    Z_Titre(1,tmpCh);

    lcd_Liste(tmpVal,(char *)listOffOn,Tracks[CurPattern].Instru[CurInstr].Active);
    Z_TextValue(2,"Active",tmpVal);

    if ( CurInstr < NB_TR_INST - 1 )
    {
      lcd_Note(tmpVal,Tracks[CurPattern].Instru[CurInstr].Note);
      Z_TextValue(3,"Note",tmpVal);

      lcd_Num(tmpVal,Tracks[CurPattern].Instru[CurInstr].Duree);
      Z_TextValue(4,"Durati.",tmpVal);

      lcd_Num(tmpVal,Tracks[CurPattern].Instru[CurInstr].Veloci);
      Z_TextValue(5,"Veloci.",tmpVal);
    }

    lcd_Liste(tmpVal,(char *)listMul,Tracks[CurPattern].Instru[CurInstr].Modul);
    Z_TextValue(6,"Modula.",tmpVal);

    lcd_Num(tmpVal,Tracks[CurPattern].Instru[CurInstr].Ampli);
    Z_TextValue(7,"M.Level",tmpVal);

    lcd_Liste(tmpVal,(char *)listDestin,Tracks[CurPattern].Instru[CurInstr].Dest1);
    Z_TextValue(8,"Dest.1",tmpVal);

    lcd_Liste(tmpVal,(char *)listDestin,Tracks[CurPattern].Instru[CurInstr].Dest2);
    Z_TextValue(9,"Dest.2",tmpVal);

    lcd_Num(tmpVal,Tracks[CurPattern].Instru[CurInstr].Chanel);
    Z_TextValue(10,"Channel",tmpVal);


    lcd_Num(tmpVal,CurPattern+1);
    Z_TextValue(11,"Pattern",tmpVal);

    lcd_Num(tmpVal,Tracks[CurPattern].Lg);
    Z_TextValue(12,"Length",tmpVal);

    lcd_Num(tmpVal,Tracks[CurPattern].Accent);
    Z_TextValue(13,"Accent",tmpVal);

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


void SeqTrxx::ChangeCurPlaySong(int value) 
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

void SeqTrxx::UpdateCtrl(int cpt)
{
  int pos;

  switch(Mode)
  {
  case MODE_PATT :
    switch(cpt)
    {
    case 1:
      break;

    case 2:
      lcd_Liste(tmpVal,(char *)listOffOn,Tracks[CurPattern].Instru[CurInstr].Active);
      Z_Value(2,tmpVal);
      break;

    case 3:
      lcd_Note(tmpVal,Tracks[CurPattern].Instru[CurInstr].Note);
      Z_Value(3,tmpVal);
      break;

    case 4:
      lcd_Num(tmpVal,Tracks[CurPattern].Instru[CurInstr].Duree);
      Z_Value(cpt,tmpVal);
      break;

    case 5:
      lcd_Num(tmpVal,Tracks[CurPattern].Instru[CurInstr].Veloci);
      Z_Value(cpt,tmpVal);
      break;

    case 6:
      lcd_Liste(tmpVal,(char *)listMul,Tracks[CurPattern].Instru[CurInstr].Modul);
      Z_Value(cpt,tmpVal);
      break;

    case 7:
      lcd_Num(tmpVal,Tracks[CurPattern].Instru[CurInstr].Ampli);
      Z_Value(cpt,tmpVal);
      break;

    case 8:
      lcd_Liste(tmpVal,(char *)listDestin,Tracks[CurPattern].Instru[CurInstr].Dest1);
      Z_Value(cpt,tmpVal);
      break;

    case 9:
      lcd_Liste(tmpVal,(char *)listDestin,Tracks[CurPattern].Instru[CurInstr].Dest2);
      Z_Value(cpt,tmpVal);
      break;    

    case 10:
      lcd_Num(tmpVal,Tracks[CurPattern].Instru[CurInstr].Chanel);
      Z_Value(cpt,tmpVal);
      break;

    case 11:
      lcd_Num(tmpVal,CurPattern+1);
      Z_Value(cpt,tmpVal);
      break;

    case 12:
      lcd_Num(tmpVal,Tracks[CurPattern].Lg);
      Z_Value(cpt,tmpVal);
      break;

    case 13:
      lcd_Num(tmpVal,Tracks[CurPattern].Accent);
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

boolean SeqTrxx::ChangeValue(int cpt, int value, byte Encoder)
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
      newValue = encnewmap(value, 0, NB_TR_INST - 1,CurInstr ,Encoder);
      if ( newValue != CurInstr )
      {
        ret = true;
        CurInstr = newValue;
        DessinPage();
      }
      break;
    case 2:
      newValue = encnewmap(value, 0, 1, Tracks[CurPattern].Instru[CurInstr].Active,Encoder);
      if (newValue != Tracks[CurPattern].Instru[CurInstr].Active )
      {
        Tracks[CurPattern].Instru[CurInstr].Active = newValue;
        ret = true;
        UpdateCtrl(cpt);
      };
      break;
    case 3:
      newValue = encnewmap(value, -24, 96 , Tracks[CurPattern].Instru[CurInstr].Note,Encoder);
      if (newValue != Tracks[CurPattern].Instru[CurInstr].Note )
      {
        Tracks[CurPattern].Instru[CurInstr].Note = newValue;
        ret = true;
        UpdateCtrl(cpt);
      };
      break;
    case 4:
      newValue = encnewmap(value, 1, 40, Tracks[CurPattern].Instru[CurInstr].Duree,Encoder);
      if (newValue != Tracks[CurPattern].Instru[CurInstr].Duree )
      {
        Tracks[CurPattern].Instru[CurInstr].Duree = newValue;
        ret = true;
        UpdateCtrl(cpt);
      };
      break;
    case 5:
      newValue = encnewmap(value, 0, 127,Tracks[CurPattern].Instru[CurInstr].Veloci ,Encoder);
      if (newValue != Tracks[CurPattern].Instru[CurInstr].Veloci )
      {
        Tracks[CurPattern].Instru[CurInstr].Veloci = newValue;
        ret = true;
        UpdateCtrl(cpt);
      };
      break;
    case 6:
      newValue = encnewmap(value, 0, 4 , Tracks[CurPattern].Instru[CurInstr].Modul,Encoder);
      if (newValue != Tracks[CurPattern].Instru[CurInstr].Modul )
      {
        Tracks[CurPattern].Instru[CurInstr].Modul = newValue;
        ret = true;
        UpdateCtrl(cpt);
      };
      break;
    case 7:
      newValue = encnewmap(value, -99, 99 , Tracks[CurPattern].Instru[CurInstr].Ampli,Encoder);
      if (newValue != Tracks[CurPattern].Instru[CurInstr].Ampli )
      {
        Tracks[CurPattern].Instru[CurInstr].Ampli = newValue;
        ret = true;
        UpdateCtrl(cpt);
      };
      break;

    case 8:
      newValue = encnewmap(value, 0, NB_OUTS , Tracks[CurPattern].Instru[CurInstr].Dest1,Encoder);
      if (newValue != Tracks[CurPattern].Instru[CurInstr].Dest1 )
      {
        if ( Tracks[CurPattern].Instru[CurInstr].IsOn  ) EventOff(CurInstr);
        SPOut.AutoOff( Tracks[CurPattern].Instru[CurInstr].Dest1 );
        Tracks[CurPattern].Instru[CurInstr].Dest1 = newValue;
        ret = true;
        UpdateCtrl(cpt);
      };
      break;

    case 9:
      newValue = encnewmap(value, 0, NB_OUTS , Tracks[CurPattern].Instru[CurInstr].Dest2,Encoder);
      if (newValue != Tracks[CurPattern].Instru[CurInstr].Dest2 )
      {
        if ( Tracks[CurPattern].Instru[CurInstr].IsOn  ) EventOff(CurInstr);
        SPOut.AutoOff( Tracks[CurPattern].Instru[CurInstr].Dest2 );
        Tracks[CurPattern].Instru[CurInstr].Dest2 = newValue;
        ret = true;
        UpdateCtrl(cpt);
      };
      break;

    case 10:
      newValue = encnewmap(value, 1, 16 , Tracks[CurPattern].Instru[CurInstr].Chanel,Encoder);
      if (newValue != Tracks[CurPattern].Instru[CurInstr].Chanel )
      {
        if ( Tracks[CurPattern].Instru[CurInstr].IsOn  ) EventOff(CurInstr);
        SPOut.AutoOff( Tracks[CurPattern].Instru[CurInstr].Dest1 );
        SPOut.AutoOff( Tracks[CurPattern].Instru[CurInstr].Dest2 );
        Tracks[CurPattern].Instru[CurInstr].Chanel = newValue;
        ret = true;
        UpdateCtrl(cpt);
      };
      break;

    case 11:
      newValue = encnewmap(value, 0, NB_TR_PATTERN-1 , CurPattern,Encoder);
      if (newValue != CurPattern )
      {
        CurPattern = newValue;
        ret = true;
        DessinPage();
      }
      break;

    case 12:
      newValue = encnewmap(value, 1, 16 , Tracks[CurPattern].Lg,Encoder);
      if (newValue != Tracks[CurPattern].Lg )
      {
        Tracks[CurPattern].Lg = newValue;
        ret = true;
        UpdateCtrl(cpt);
      };
      break;

    case 13:
      newValue = encnewmap(value, 0, 16 , Tracks[CurPattern].Accent,Encoder);
      if (newValue != Tracks[CurPattern].Accent )
      {
        Tracks[CurPattern].Accent = newValue;
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
      newValue = encnewmap(value, 1, NB_TR_PATTERN , Songs[pos].Track,Encoder);
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
      newValue = encnewmap(value, 1, 16 , Songs[pos].Repeat,Encoder);
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
      newValue = encnewmap(value, -12, +12 , Songs[pos].Transpose,Encoder);
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
      newValue = encnewmap(value, 0, NB_TR_SONG, Songs[pos].Next,Encoder );
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


boolean SeqTrxx::ChangePage(int value, byte Encoder)
{  
  boolean ret = false;
  int newValue;

  switch(Mode)
  {
  case MODE_PATT:
    newValue = encnewmap(value, 0, NB_TR_INST - 1, CurInstr,Encoder);
    if ( newValue != CurInstr )
    {
      ret = true;
      CurInstr = newValue;
      DessinPage();
    }
    break;
  case MODE_SONG:
    newValue = encnewmap(value, 0, NB_TR_SONG - 1,CurPlaySong ,Encoder);
    if ( newValue != CurPlaySong )
    {
      ret = true;
      ChangeCurPlaySong(newValue);
    }
    break;
  }
  return ret;
}


boolean SeqTrxx::ChangeM1(int value, byte Encoder)
{
  boolean ret = false;
  if ( Menu->ChangeM1(value,Encoder) )
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

boolean SeqTrxx::ChangeM2(int value, byte Encoder)
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

boolean SeqTrxx::ChangeM3(int value, byte Encoder)
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


void SeqTrxx::ExStart(boolean pSong)
{
  is_played_Mode_Song = pSong;
  Steep = 0;
  if ( is_played_Mode_Song ) {
    ChangeCurPlaySong(0);
    nbSteep=-1;
    CurPattern = Songs[CurPlaySong].Track-1;
    if ( ActiFunc == MODE_SEQTR && Menu->FirstPage) Menu->ShowM3(CurPattern+1);
  }
  is_played = PLAYED;
  SClock.PostStart();
}

void SeqTrxx::ExStop()
{
  if ( is_played != STOPPED ) {
      SClock.PostStop();
      is_played = STOPPED;
  }
}

boolean SeqTrxx::ChangeS1S5(int value)
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
      case 3:
        if (is_played==STOPPED) 
        {
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
      
#ifdef _DEBUG_MODE_
        Serial.print( "SeqTrxx::ChangeS1S5 ");
        Serial.print( Menu->curCmd );
        Serial.print( "," );
        Serial.print( Menu->V1 );
        Serial.print( "," );
        Serial.println( Menu->V2 );
#endif
        switch( Menu->curCmd )
        {
        case 0:  /* copy */

          memcpy( &Tracks[Menu->V2-1], &Tracks[Menu->V1-1], sizeof (trTrack) ) ;
          PatLed.Show(0);
          DessinPage();
          break;

        case 1:  /* Load */

          if ( MySD.OpenFileForRead(MODE_SEQTR, Menu->GetFName(FileName) ) )
          {
            Restore();
            MySD.CloseFile();
          }
          PatLed.Show(0);
          DessinPage();
          break;

        case 2:  /* Save */

          if ( MySD.OpenFileForWrite(MODE_SEQTR, Menu->MenuFName(FileName) ) )
          {
            Backup();
            MySD.CloseFile();
          }
          PatLed.Show(0);
          break;

        case 3:
          Init();
          PatLed.Show(0);
          DessinPage();
          break;
        }         
      }
    }
  }

  return ret;
}


void  SeqTrxx::Backup()
{
            MySD.WriteStream((byte *)Tracks, sizeof(Tracks) ) ;
            MySD.WriteStream((byte *)Songs, sizeof(Songs) ) ;
}

void  SeqTrxx::Restore()
{
            MySD.ReadStream((byte *)Tracks, sizeof(Tracks) ) ;
            MySD.ReadStream((byte *)Songs, sizeof(Songs) ) ;
}



boolean SeqTrxx::ChangePAT(int value)
{
  boolean ret = false;
  int newValue = PatLed.Map(value);
  if ( newValue > 0 )
  {
    newValue--;
    if ( is_mode_write )
    {
      uint16_t masq =  1 << newValue;
      if ( ( Tracks[CurPattern].Instru[CurInstr].Pattern & masq ) == masq )
        Tracks[CurPattern].Instru[CurInstr].Pattern &= ~masq;
      else
        Tracks[CurPattern].Instru[CurInstr].Pattern |= masq;

#ifdef _DEBUG_MODE_
      Serial.print("SeqTrxx::ChangePAT");
      Serial.print(newValue);
      Serial.print(",");
      sprintf(tmpVal,"%4X",masq);
      Serial.print(tmpVal);
      Serial.print(",");
      sprintf(tmpVal,"%4X",Tracks[CurPattern].Instru[CurInstr].Pattern);
      Serial.println(tmpVal);

#endif

    }
    else
    {
      if (is_played==PLAYED)
        NextPattern = newValue;
      else
      {
        CurPattern = newValue;
        ret = true;
        DessinPage();
      }
    }
  }
}

void SeqTrxx::Encoder(byte Value)
{
   switch(lastChangeValue)
   {
   case CHGT_PAGE:
     ChangePage(0, Value); break;
   case CHGT_M1:
     ChangeM1(0, Value); break;
   case CHGT_M2:
     ChangeM2(0, Value); break;
   case CHGT_M3:
     ChangeM3(0, Value); break;
   default :
     ChangeValue(lastChangeValue,0, Value );break;
   }
}

void SeqTrxx::Interface(int Num, int Value)
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

void SeqTrxx::Traite(uint32_t reftime)
{
  int ct;
  for (ct=0 ; ct < NB_TR_INST -1 ; ct++)
  {
    if ( Tracks[CurPattern].Instru[ct].IsOn && Tracks[CurPattern].Instru[ct].TimeOff < reftime )
    {
      EventOff(ct);
    }
  }
}

void SeqTrxx::TraiteEvent(uint32_t reftime)
{
  int ct;
  if (cpt24==0  && is_played==PLAYED)
  {
    if ( ActiFunc==MODE_SEQTR )  
    {
          PatLed.SetPatt( Tracks[CurPattern].Instru[CurInstr].Pattern );
         PatLed.SetStep(Steep+1);
    }
    if ( is_played_Mode_Song )
    {
      if (Steep==0)
      {
        for (ct=0 ; ct < NB_TR_INST -1 ; ct++)
        {
          if ( Tracks[CurPattern].Instru[ct].IsOn && Tracks[CurPattern].Instru[ct].TimeOff < reftime )
          {
            EventOff(ct);
          }
        }
        nbSteep++;
        if ( nbSteep >= Songs[CurPlaySong].Repeat ) 
        {
          if ( Songs[CurPlaySong].Next > 0 ) {
            ChangeCurPlaySong(Songs[CurPlaySong].Next-1);
            CurPattern = Songs[CurPlaySong].Track-1;
            if ( ActiFunc == MODE_SEQTR && Menu->FirstPage) Menu->ShowM3(CurPattern+1);
            nbSteep=0;
          }
          else { 
            ExStop();;
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
        for (ct=0 ; ct < NB_TR_INST -1 ; ct++)
        {
          if ( Tracks[CurPattern].Instru[ct].IsOn && Tracks[CurPattern].Instru[ct].TimeOff < reftime )
          {
            EventOff(ct);
          }
        }
        CurPattern=NextPattern;
        DessinPage();
        NextPattern=-1;
      }    
    }

    uint16_t masq =  1 << Steep;
    boolean isacc = (  ( Tracks[CurPattern].Instru[NB_TR_INST-1].Pattern & masq ) == masq );
    for (ct=0 ; ct < NB_TR_INST - 2 ; ct++)
    {
      if ( ( Tracks[CurPattern].Instru[ct].Pattern & masq ) == masq )
      {
        EventOn(ct, reftime, isacc ); 
      }
    }

    Steep++;
    if (Steep>=Tracks[CurPattern].Lg) Steep=0;

  }
  else
  {
    if ( ActiFunc==MODE_SEQTR && is_played==STOPPED ) PatLed.SetPatt( Tracks[CurPattern].Instru[CurInstr].Pattern );
  } 
  //  next_event = next_event + delta_event + ( groove * ( random(delta_event) - delta_event / 2 ) ) / 50;
  if (++cpt24>=nb24) cpt24=0;
}



void SeqTrxx::EventOff(int num)
{
  SPOut.NoteOff(Tracks[CurPattern].Instru[num].Dest1,Tracks[CurPattern].Instru[num].Played,0,Tracks[CurPattern].Instru[num].Chanel);
  SPOut.NoteOff(Tracks[CurPattern].Instru[num].Dest2,Tracks[CurPattern].Instru[num].Played,0,Tracks[CurPattern].Instru[num].Chanel);
  Tracks[CurPattern].Instru[num].IsOn = false;
}

void SeqTrxx::EventOn(int pos, uint32_t reftime, boolean accent)
{
  int Veloci;  
  if ( Tracks[CurPattern].Instru[pos].IsOn ) EventOff(pos);

 
  Tracks[CurPattern].Instru[pos].Played = Tracks[CurPattern].Instru[pos].Note;
  if (Tracks[CurPattern].Instru[pos].Played < 24  )
  {
    
      switch( ParamGlo.TrSrc )
      {
        case 0 : Tracks[CurPattern].Instru[pos].Played = Tracks[CurPattern].Instru[pos].Note + ParamGlo.LastNote; LastOffset = ParamGlo.LastNote;  break;
        case 1 : Tracks[CurPattern].Instru[pos].Played = Tracks[CurPattern].Instru[pos].Note + ParamGlo.LastUpperNote; LastOffset = ParamGlo.LastUpperNote;  break;
        case 2 : Tracks[CurPattern].Instru[pos].Played = Tracks[CurPattern].Instru[pos].Note + ParamGlo.LastLowerNote; LastOffset = ParamGlo.LastLowerNote; break; 
        case 3 : Tracks[CurPattern].Instru[pos].Played = Tracks[CurPattern].Instru[pos].Note + LastOffset; break; 
      }
  }
  
  
  
//  if (Tracks[CurPattern].Instru[pos].Played < 24 ) Tracks[CurPattern].Instru[pos].Played += MIDI1.LastNote;
  if ( is_played_Mode_Song && Songs[CurPlaySong].Transpose != 0 ) Tracks[CurPattern].Instru[pos].Played+= Songs[CurPlaySong].Transpose;

  Veloci = Tracks[CurPattern].Instru[pos].Veloci;

  if ( Tracks[CurPattern].Instru[pos].Modul )
  {
    //  char *listMul    ="Off    LF1*VelLF1*NotLF2*VelLF2*Not";
    switch( Tracks[CurPattern].Instru[pos].Modul )
    {
    case 1:
      Veloci+= ( LFoBi.LFos[0].Out * ( ( 127 * Tracks[CurPattern].Instru[pos].Ampli ) / 100 ) / LFO_RESO ) ;
      break;

    case 2:
      Tracks[pos].Instru[pos].Played += ( LFoBi.LFos[0].Out * ( ( 24 * Tracks[CurPattern].Instru[pos].Ampli ) / 100 ) / LFO_RESO ) ;
      break;

    case 3:
      Veloci+= ( LFoBi.LFos[1].Out * ( ( 127 * Tracks[CurPattern].Instru[pos].Ampli ) / 100 ) / LFO_RESO ) ;
      break;

    case 4:
      Tracks[CurPattern].Instru[pos].Played += ( LFoBi.LFos[1].Out * ( ( 24 * Tracks[CurPattern].Instru[pos].Ampli ) / 100 ) / LFO_RESO ) ;
      break;

    }
    if (Veloci<0) Veloci=0;
    if (Veloci>127) Veloci=127;
    if (Tracks[CurPattern].Instru[pos].Played<1) Tracks[CurPattern].Instru[pos].Played=1;
    if (Tracks[CurPattern].Instru[pos].Played>127) Tracks[CurPattern].Instru[pos].Played=127;
  }   
  if (accent) Veloci += (127-Veloci)*Tracks[CurPattern].Accent / 16;

  SPOut.NoteOn(Tracks[CurPattern].Instru[pos].Dest1,Tracks[CurPattern].Instru[pos].Played,Veloci,Tracks[CurPattern].Instru[pos].Chanel);
  SPOut.NoteOn(Tracks[CurPattern].Instru[pos].Dest2,Tracks[CurPattern].Instru[pos].Played,Veloci,Tracks[CurPattern].Instru[pos].Chanel);
  Tracks[CurPattern].Instru[pos].IsOn = true;
  Tracks[CurPattern].Instru[pos].TimeOff = reftime + ( delta_event * Tracks[CurPattern].Instru[pos].Duree ) / 10 ;
}



void SeqTrxx::HandleNoteOn(byte Interface, byte channel, byte pitch, byte velocity)
{
   if ( is_echo && is_played==STOPPED && ActiFunc==NumFunc )
   {
       SPOut.NoteOn(Tracks[CurPattern].Instru[CurInstr].Dest1,pitch,velocity,Tracks[CurPattern].Instru[CurInstr].Chanel);
       SPOut.NoteOn(Tracks[CurPattern].Instru[CurInstr].Dest2,pitch,velocity,Tracks[CurPattern].Instru[CurInstr].Chanel);
   }
}


void SeqTrxx::HandleNoteOff(byte Interface, byte channel, byte pitch, byte velocity)
{
   if ( is_echo && is_played==STOPPED && ActiFunc==NumFunc )
   {
       SPOut.NoteOff(Tracks[CurPattern].Instru[CurInstr].Dest1, pitch, velocity,Tracks[CurPattern].Instru[CurInstr].Chanel);
       SPOut.NoteOff(Tracks[CurPattern].Instru[CurInstr].Dest2, pitch, velocity,Tracks[CurPattern].Instru[CurInstr].Chanel);
   }
}


void SeqTrxx::HandlePitchBend(byte Interface, byte channel, int bend)
{
   if ( is_echo && is_played==STOPPED && ActiFunc==NumFunc )
   {
       SPOut.PitchBend(Tracks[CurPattern].Instru[CurInstr].Dest1, bend, Tracks[CurPattern].Instru[CurInstr].Chanel);
       SPOut.PitchBend(Tracks[CurPattern].Instru[CurInstr].Dest2, bend, Tracks[CurPattern].Instru[CurInstr].Chanel);
   }
}

void SeqTrxx::HandleControlChange(byte Interface, byte channel, byte number, byte value)
{
   if ( is_echo && is_played==STOPPED && ActiFunc==NumFunc )
   {
       SPOut.ControlChange(Tracks[CurPattern].Instru[CurInstr].Dest1, number, value, Tracks[CurPattern].Instru[CurInstr].Chanel);
       SPOut.ControlChange(Tracks[CurPattern].Instru[CurInstr].Dest2, number, value, Tracks[CurPattern].Instru[CurInstr].Chanel);
   }

}

void SeqTrxx::HandleProgramChange(byte Interface, byte channel, byte number)
{
   if ( is_echo && is_played==STOPPED && ActiFunc==NumFunc )
   {
       SPOut.ProgramChange(Tracks[CurPattern].Instru[CurInstr].Dest1, number,Tracks[CurPattern].Instru[CurInstr].Chanel);
       SPOut.ProgramChange(Tracks[CurPattern].Instru[CurInstr].Dest2, number,Tracks[CurPattern].Instru[CurInstr].Chanel);
   }

}

void SeqTrxx::HandleAfterTouch(byte Interface, byte channel, byte value)
{
   if ( is_echo && is_played==STOPPED && ActiFunc==NumFunc )
   {
       SPOut.AfterTouch(Tracks[CurPattern].Instru[CurInstr].Dest1, value, Tracks[CurPattern].Instru[CurInstr].Chanel);
       SPOut.AfterTouch(Tracks[CurPattern].Instru[CurInstr].Dest2, value, Tracks[CurPattern].Instru[CurInstr].Chanel);
   }
}







