// -----------------------------------------------
// LL113
// -----------------------------------------------
// Historique:
// (Baloran) laurent.lecatelier@free.fr, version 0.0.1  10/02/12013
// -----------------------------------------------
// LFO.cpp Code Class gestion des LFOs et Enveloppes


#include <LiquidCrystal.h>
#include "Arduino.h"
#include "lfo.h"
#include "utilld.h"
#include "SDFile.h"
#include "onde.h"
#include "mcp.h"

#include "multiplexAD.h"


extern SDFile MySD;
extern mPatLed PatLed;


extern multiplexAD mxAD;
extern LiquidCrystal lcdM;
extern LiquidCrystal lcd1;
extern LiquidCrystal lcd2;
extern LiquidCrystal lcd3;
extern LiquidCrystal lcd4;

const char *LFoTitre="\x01 Modules LFO/ENV  \x02";

extern const char *mListSource;
extern const char *mListMesure;
extern const int  *NbClock;

const char *listShape    ="Sinus  Triang SawUp  SawDownSquare ";
const char *listShape2   ="Sinus  Triang SawUp  SawDownSquare S&H    ";
const char *listlModul   ="Off    LFO1/2 Env 1  Env 2  CV In  CC 1   CC 2   CC 3   CC 4   ";
const char *listlModDest ="AmplituFreq   ";

const char *listlTrig    ="Off    Midi 1 Midi 2 Midi 3 Trig in";
const char *listlDest    ="Off    CV 1   CV 2   CV 3   CV 4   CV 5   CV 6   CV 7   CC 1   CC 2   CC 3   CC 4   ";
const char *listlLog     ="Lin    Log    ";


paramMenuList MenuListLFo[] = {
  { 
    "Load", "Name", "", 0, MODE_LFO, -1, 0 , 0, 0, 0, 0     }
  ,
  { 
    "Save", "Name", "Indice", 0, 0, NB_STYLEMU, listStyleMenu , 1, 1, 64, 0       }
  , 
  { 
    "Init", "", "", 0, 0, 0, 0 , 0, 0, 0, 0                 }
  , 
  {   
    "", "", "", 0, 0, 0, 0 , 0, 0, 0, 0                     }
};

paramMenu menuLFo = {
  "P/S",
  "",
  "Play",
  "",
  "BPM",          
  "S/Beat",     
  "Groove",          
  100,30,300,0,     
  4,1,16,0,     
  0,0,9,0,     
  MenuListLFo 
};



LFo::LFo(byte pNumFunc)
{ 
  NumFunc=pNumFunc;
  Init();
  Menu = new mMenu(&menuLFo, LFoTitre);
}

LFo::~LFo()
{

}

void LFo::Init()
{
  lastChangeValue = CHGT_PAGE;
  lastreftime = micros();
  delta_event = 333;
  next_event=lastreftime;

  for (int pos=0;pos<NB_LFO;pos++)
  {


    LFos[pos].Shap=0;          
    LFos[pos].Freq=80;        
    LFos[pos].Ampl=100;  
    LFos[pos].Modu=0;  
    LFos[pos].ModDest=0;
    LFos[pos].ModAmpl=0;  
    LFos[pos].Phase=0;
    LFos[pos].Trig=0;
    LFos[pos].Destination=0;
    LFos[pos].Out=0;
    LFos[pos].StartTime = lastreftime;
    LFos[pos].Index=0;
    LFos[pos].Smooth = LFO_SMOOTH_TIME ;
    LFos[pos].last_n = 0;

    Envs[pos].Etape=0;   
    Envs[pos].Attack=1;          
    Envs[pos].Decay=100;        
    Envs[pos].Sustain=90;  
    Envs[pos].Release=100;  
    Envs[pos].Ampl=99;  
    Envs[pos].Delay=0;
    Envs[pos].Log=0;
    Envs[pos].Trig=1;
    Envs[pos].Destination=0;
    Envs[pos].Out=0;
    Envs[pos].Retrig=true;
    Envs[pos].StartTime = lastreftime;
    Envs[pos].Smooth = ENV_SMOOTH_TIME ;
    Envs[pos].last_n = 0;

  }
  CurLfoEnv=0;
}

void LFo::DessinPage()
{
  Menu->DessinPage();
  Z_Clear();

  int pos=0;

  if ( CurLfoEnv == 0 )
  {
    sprintf(tmpCh,"\x02LFO %d", pos+1);
    Z_Titre(1,tmpCh);
    lcd_Liste(tmpVal,(char *)listShape,LFos[pos].Shap);
    Z_Value(1,tmpVal);

    lcd_Num(tmpVal,LFos[pos].Ampl);
    Z_TextValue(2,"Level",tmpVal);

    lcd_Liste(tmpVal,(char *)listlModul,LFos[pos].Modu);
    Z_TextValue(3,"Modula.",tmpVal);

    lcd_Liste(tmpVal,(char *)listlModDest,LFos[pos].ModDest);
    Z_TextValue(4,"Mod.Dst",tmpVal);

    lcd_Liste(tmpVal,(char *)listlTrig,LFos[pos].Trig);
    Z_TextValue(5,"Sync",tmpVal);

    lcd_HZ(tmpVal,LFos[pos].Freq);
    Z_TextValue(6,"Freq.",tmpVal);

    lcd_Num(tmpVal,LFos[pos].Phase);
    Z_TextValue(7,"Phase",tmpVal);

    lcd_Num(tmpVal,LFos[pos].ModAmpl);
    Z_TextValue(8,"Mod.Lev",tmpVal);

    lcd_Liste(tmpVal,(char *)listlDest,LFos[pos].Destination);
    Z_TextValue(10,"Destin.",tmpVal);

    pos++;

    sprintf(tmpCh,"\x02LFO %d", pos+1);
    Z_Titre(11,tmpCh);
    lcd_Liste(tmpVal,(char *)listShape,LFos[pos].Shap);
    Z_Value(11,tmpVal);

    lcd_Num(tmpVal,LFos[pos].Ampl);
    Z_TextValue(12,"Level",tmpVal);

    lcd_Liste(tmpVal,(char *)listShape2,LFos[pos].Modu);
    Z_TextValue(13,"Modula.",tmpVal);

    lcd_Liste(tmpVal,(char *)listlModDest,LFos[pos].ModDest);
    Z_TextValue(14,"Mod.Dst",tmpVal);

    lcd_Liste(tmpVal,(char *)listlTrig,LFos[pos].Trig);
    Z_TextValue(15,"Sync",tmpVal);

    lcd_HZ(tmpVal,LFos[pos].Freq);
    Z_TextValue(16,"Freq.",tmpVal);

    lcd_Num(tmpVal,LFos[pos].Phase);
    Z_TextValue(17,"Phase",tmpVal);

    lcd_Num(tmpVal,LFos[pos].ModAmpl);
    Z_TextValue(18,"Mod.Lev",tmpVal);

    lcd_Liste(tmpVal,(char *)listlDest,LFos[pos].Destination);
    Z_TextValue(20,"Destin",tmpVal);
  }
  else
  {
    sprintf(tmpCh,"\x02\Attak %d", pos+1);
    Z_Titre(1,tmpCh);
    lcd_Num(tmpVal,Envs[pos].Attack);
    Z_Value(1,tmpVal);

    lcd_Num(tmpVal,Envs[pos].Decay);
    Z_TextValue(2,"Decay",tmpVal);

    lcd_Num(tmpVal,Envs[pos].Sustain);
    Z_TextValue(3,"Sustain",tmpVal);

    lcd_Num(tmpVal,Envs[pos].Release);
    Z_TextValue(4,"Release",tmpVal);

    lcd_Liste(tmpVal,(char *)listlTrig,Envs[pos].Trig);
    Z_TextValue(5,"Sync",tmpVal);

    lcd_Num(tmpVal,Envs[pos].Ampl);
    Z_TextValue(6,"Level",tmpVal);

    lcd_Num(tmpVal,Envs[pos].Delay);
    Z_TextValue(7,"Delay",tmpVal);

    lcd_Liste(tmpVal,(char *)listlLog,Envs[pos].Log);
    Z_TextValue(8,"Profil",tmpVal);

    lcd_Liste(tmpVal,(char *)listlDest,Envs[pos].Destination);
    Z_TextValue(10,"Destin",tmpVal);


    pos++;

    sprintf(tmpCh,"\x02\Attak %d", pos+1);
    Z_Titre(11,tmpCh);
    lcd_Num(tmpVal,Envs[pos].Attack);
    Z_Value(11,tmpVal);

    lcd_Num(tmpVal,Envs[pos].Decay);
    Z_TextValue(12,"Decay",tmpVal);

    lcd_Num(tmpVal,Envs[pos].Sustain);
    Z_TextValue(13,"Sustain",tmpVal);

    lcd_Num(tmpVal,Envs[pos].Release);
    Z_TextValue(14,"Release",tmpVal);

    lcd_Liste(tmpVal,(char *)listlTrig,Envs[pos].Trig);
    Z_TextValue(15,"Sync",tmpVal);

    lcd_Num(tmpVal,Envs[pos].Ampl);
    Z_TextValue(16,"Level",tmpVal);

    lcd_Num(tmpVal,Envs[pos].Delay);
    Z_TextValue(17,"Delay",tmpVal);

    lcd_Liste(tmpVal,(char *)listlLog,Envs[pos].Log);
    Z_TextValue(18,"Profil",tmpVal);

    lcd_Liste(tmpVal,(char *)listlDest,Envs[pos].Destination);
    Z_TextValue(20,"Destin",tmpVal);


  }

}


void LFo::UpdateCtrl(int cpt)
{
  int pos=0;

  if ( CurLfoEnv == 0 )
  {
    switch(cpt)
    {
    case 1:
      lcd_Liste(tmpVal,(char *)listShape,LFos[pos].Shap);
      Z_Value(cpt,tmpVal);
      break;

    case 2:
      lcd_Num(tmpVal,LFos[pos].Ampl);
      Z_Value(cpt,tmpVal);
      break;

    case 3:
      lcd_Liste(tmpVal,(char *)listlModul,LFos[pos].Modu);
      Z_Value(cpt,tmpVal);
      break;

    case 4:
      lcd_Liste(tmpVal,(char *)listlModDest,LFos[pos].ModDest);
      Z_Value(cpt,tmpVal);
      break;

    case 5:
      lcd_Liste(tmpVal,(char *)listlTrig,LFos[pos].Trig);
      Z_Value(cpt,tmpVal);
      break;

    case 6:
      lcd_HZ(tmpVal,LFos[pos].Freq);
      Z_Value(cpt,tmpVal);
      break;

    case 7:
      lcd_Num(tmpVal,LFos[pos].Phase);
      Z_Value(cpt,tmpVal);
      break;

    case 8:
      lcd_Num(tmpVal,LFos[pos].ModAmpl);
      Z_Value(cpt,tmpVal);
      break;

    case 9:
      break;

    case 10:
      lcd_Liste(tmpVal,(char *)listlDest,LFos[pos].Destination);
      Z_Value(cpt,tmpVal);
      break;

    case 11:
      lcd_Liste(tmpVal,(char *)listShape,LFos[pos+1].Shap);
      Z_Value(cpt,tmpVal);
      break;

    case 12:
      lcd_Num(tmpVal,LFos[pos+1].Ampl);
      Z_Value(cpt,tmpVal);
      break;

    case 13:
      lcd_Liste(tmpVal,(char *)listlModul,LFos[pos+1].Modu);
      Z_Value(cpt,tmpVal);
      break;

    case 14:
      lcd_Liste(tmpVal,(char *)listlModDest,LFos[pos+1].ModDest);
      Z_Value(cpt,tmpVal);
      break;

    case 15:
      lcd_Liste(tmpVal,(char *)listlTrig,LFos[pos+1].Trig);
      Z_Value(cpt,tmpVal);
      break;

    case 16:
      lcd_HZ(tmpVal,LFos[pos+1].Freq);
      Z_Value(cpt,tmpVal);
      break;

    case 17:
      lcd_Num(tmpVal,LFos[pos+1].Phase);
      Z_Value(cpt,tmpVal);
      break;

    case 18:
      lcd_Num(tmpVal,LFos[pos+1].ModAmpl);
      Z_Value(cpt,tmpVal);
      break;

    case 19:
      break;

    case 20:
      lcd_Liste(tmpVal,(char *)listlDest,LFos[pos+1].Destination);
      Z_Value(cpt,tmpVal);
      break;
    }
  }
  else
  {

    switch(cpt)
    {
    case 1:
      lcd_Num(tmpVal,Envs[pos].Attack);
      Z_Value(cpt,tmpVal);
      break;

    case 2:
      lcd_Num(tmpVal,Envs[pos].Decay);
      Z_Value(cpt,tmpVal);
      break;

    case 3:
      lcd_Num(tmpVal,Envs[pos].Sustain);
      Z_Value(cpt,tmpVal);
      break;

    case 4:
      lcd_Num(tmpVal,Envs[pos].Release);
      Z_Value(cpt,tmpVal);
      break;

    case 5:
      lcd_Liste(tmpVal,(char *)listlTrig,Envs[pos].Trig);
      Z_Value(cpt,tmpVal);
      break;

    case 6:
      lcd_Num(tmpVal,Envs[pos].Ampl);
      Z_Value(cpt,tmpVal);
      break;

    case 7:
      lcd_Num(tmpVal,Envs[pos].Delay);
      Z_Value(cpt,tmpVal);
      break;

    case 8:
      lcd_Liste(tmpVal,(char *)listlLog,Envs[pos].Log);
      Z_Value(cpt,tmpVal);
      break;

    case 9:
      break;

    case 10:
      lcd_Liste(tmpVal,(char *)listlDest,Envs[pos].Destination);
      Z_Value(cpt,tmpVal);
      break;

    case 11:
      lcd_Num(tmpVal,Envs[pos+1].Attack);
      Z_Value(cpt,tmpVal);
      break;

    case 12:
      lcd_Num(tmpVal,Envs[pos+1].Decay);
      Z_Value(cpt,tmpVal);
      break;

    case 13:
      lcd_Num(tmpVal,Envs[pos+1].Sustain);
      Z_Value(cpt,tmpVal);
      break;

    case 14:
      lcd_Num(tmpVal,Envs[pos+1].Release);
      Z_Value(cpt,tmpVal);
      break;

    case 15:
      lcd_Liste(tmpVal,(char *)listlTrig,Envs[pos+1].Trig);
      Z_Value(cpt,tmpVal);
      break;

    case 16:
      lcd_Num(tmpVal,Envs[pos+1].Ampl);
      Z_Value(cpt,tmpVal);
      break;

    case 17:
      lcd_Num(tmpVal,Envs[pos+1].Delay);
      Z_Value(cpt,tmpVal);
      break;

    case 18:
      lcd_Liste(tmpVal,(char *)listlLog,Envs[pos].Log);
      Z_Value(cpt,tmpVal);
      break;

    case 19:
      break;

    case 20:
      lcd_Liste(tmpVal,(char *)listlDest,Envs[pos+1].Destination);
      Z_Value(cpt,tmpVal);
      break;
    }    
  }
}

boolean LFo::ChangeValue(int cpt, int value, byte Encoder)
{  
  int newValue;
  int ret=false;

  // cpt compte de 1 ÃƒÆ’  ...
  if (cpt >=16) cpt-=6;
  cpt++;

  int pos=0;

  if ( CurLfoEnv == 0)
  {

    switch(cpt)
    {
    case 1:
      newValue = encnewmap(value, 0, 4, LFos[pos].Shap, Encoder);
      if (newValue != LFos[pos].Shap )
      {
        LFos[pos].Shap = newValue;
        ret = true;
        UpdateCtrl(cpt);
      };
      break;
    case 2:
      newValue = encnewmap(value, 0, 100, LFos[pos].Ampl, Encoder);
      if (newValue != LFos[pos].Ampl )
      {
        LFos[pos].Ampl = newValue;
        ret = true;
        UpdateCtrl(cpt);
      };
      break;
    case 3:
      newValue = encnewmap(value, 0, 8, LFos[pos].Modu, Encoder);
      if (newValue != LFos[pos].Modu )
      {
        LFos[pos].Modu = newValue;
        ret = true;
        UpdateCtrl(cpt);
      };
      break;
    case 4:
      newValue = encnewmap(value, 0, 1 , LFos[pos].ModDest, Encoder);
      if (newValue != LFos[pos].ModDest )
      {
        LFos[pos].ModDest = newValue;
        ret = true;
        UpdateCtrl(cpt);
      };
      break;
    case 5:
      newValue = encnewmap(value, 0, 2, LFos[pos].Trig, Encoder);
      if (newValue != LFos[pos].Trig )
      {
        LFos[pos].Trig = newValue;
        ret = true;
        UpdateCtrl(cpt);
      };
      break;
    case 6:
      newValue = encnewmap(value, 0, 199, LFos[pos].Freq, Encoder);
      if (newValue !=  LFos[pos].Freq  )
      {
        LFos[pos].Freq = newValue;
        ChangeFreq(pos, newValue);
        ret = true;
        UpdateCtrl(cpt);
      };
      break;
    case 7:
      newValue = encnewmap(value, -99, 99, LFos[pos].Phase, Encoder);
      if (newValue !=  LFos[pos].Phase  )
      {
        LFos[pos].Phase = newValue;
        ret = true;
        UpdateCtrl(cpt);
      };
      break;
    case 8:
      newValue = encnewmap(value, -99, 99,  LFos[pos].ModAmpl, Encoder);
      if (newValue !=  LFos[pos].ModAmpl  )
      {
        LFos[pos].ModAmpl = newValue;
        ret = true;
        UpdateCtrl(cpt);
      };
      break;
    case 9:
      break;
    case 10:
      newValue = encnewmap(value, 0, 11, LFos[pos].Destination, Encoder);
      if (newValue != LFos[pos].Destination )
      {
        LFos[pos].Destination = newValue;
        ret = true;
        UpdateCtrl(cpt);
      };
      break;

    case 11:
      newValue = encnewmap(value, 0, 4, LFos[pos+1].Shap, Encoder);
      if (newValue != LFos[pos+1].Shap )
      {
        LFos[pos+1].Shap = newValue;
        ret = true;
        UpdateCtrl(cpt);
      };
      break;
    case 12:
      newValue = encnewmap(value, 0, 100, LFos[pos+1].Ampl, Encoder);
      if (newValue != LFos[pos+1].Ampl )
      {
        LFos[pos+1].Ampl = newValue;
        ret = true;
        UpdateCtrl(cpt);
      };
      break;
    case 13:
      newValue = encnewmap(value, 0, 8 , LFos[pos+1].Modu, Encoder);
      if (newValue != LFos[pos+1].Modu )
      {
        LFos[pos+1].Modu = newValue;
        ret = true;
        UpdateCtrl(cpt);
      };
      break;
    case 14:
      newValue = encnewmap(value, 0, 1 , LFos[pos+1].ModDest, Encoder);
      if (newValue != LFos[pos+1].ModDest )
      {
        LFos[pos+1].ModDest = newValue;
        ret = true;
        UpdateCtrl(cpt);
      };
      break;
    case 15:
      newValue = encnewmap(value, 0, 2, LFos[pos+1].Trig, Encoder);
      if (newValue != LFos[pos+1].Trig )
      {
        LFos[pos+1].Trig = newValue;
        ret = true;
        UpdateCtrl(cpt);
      };
      break;
    case 16:
      newValue = encnewmap(value, 0, 199, LFos[pos+1].Freq, Encoder);
      if (newValue !=  LFos[pos+1].Freq  )
      {
        LFos[pos+1].Freq = newValue;
        ChangeFreq(pos+1, newValue);
        ret = true;
        UpdateCtrl(cpt);
      };
      break;
    case 17:
      newValue = encnewmap(value, -99, 99, LFos[pos+1].Phase, Encoder);
      if (newValue !=  LFos[pos+1].Phase  )
      {
        LFos[pos+1].Phase = newValue;
        ret = true;
        UpdateCtrl(cpt);
      };
      break;
    case 18:
      newValue = encnewmap(value, -99, 99, LFos[pos+1].ModAmpl, Encoder);
      if (newValue !=  LFos[pos+1].ModAmpl  )
      {
        LFos[pos+1].ModAmpl = newValue;
        ret = true;
        UpdateCtrl(cpt);
      };
      break;
    case 19:
      break;
    case 20:
      newValue = encnewmap(value, 0, 11, LFos[pos+1].Destination, Encoder);
      if (newValue != LFos[pos+1].Destination )
      {
        LFos[pos+1].Destination = newValue;
        ret = true;
        UpdateCtrl(cpt);
      };
      break;
    }
  }    
  else
  {
    switch(cpt)
    {
    case 1:
      newValue = encnewmap(value, 1, 500, Envs[pos].Attack, Encoder);
      if (newValue != Envs[pos].Attack )
      {
        Envs[pos].Attack = newValue;
        ret = true;
        UpdateCtrl(cpt);
      };
      break;
    case 2:
      newValue = encnewmap(value, 1, 500, Envs[pos].Decay, Encoder);
      if (newValue != Envs[pos].Decay )
      {
        Envs[pos].Decay = newValue;
        ret = true;
        UpdateCtrl(cpt);
      };
      break;
    case 3:
      newValue = encnewmap(value, 0, 100 , Envs[pos].Sustain, Encoder);
      if (newValue != Envs[pos].Sustain )
      {
        Envs[pos].Sustain = newValue;
        ret = true;
        UpdateCtrl(cpt);
      };
      break;
    case 4:
      newValue = encnewmap(value, 1, 500 , Envs[pos].Release, Encoder);
      if (newValue != Envs[pos].Release )
      {
        Envs[pos].Release = newValue;
        ret = true;
        UpdateCtrl(cpt);
      };
      break;
    case 5:
      newValue = encnewmap(value, 1, 3, Envs[pos].Trig, Encoder);
      if (newValue != Envs[pos].Trig )
      {
        Envs[pos].Trig = newValue;
        ret = true;
        UpdateCtrl(cpt);
      };
      break;
    case 6:
      newValue = encnewmap(value, -99, 99, Envs[pos].Ampl, Encoder);
      if (newValue !=  Envs[pos].Ampl  )
      {
        Envs[pos].Ampl = newValue;
        ret = true;
        UpdateCtrl(cpt);
      };
      break;
    case 7:
      newValue = encnewmap(value, 0, 500, Envs[pos].Delay, Encoder);
      if (newValue !=  Envs[pos].Delay  )
      {
        Envs[pos].Delay = newValue;
        ret = true;
        UpdateCtrl(cpt);
      };
      break;
    case 8:
      newValue = encnewmap(value, 0, 1, Envs[pos].Log, Encoder);
      if (newValue !=  Envs[pos].Log  )
      {
        Envs[pos].Log = newValue;
        ret = true;
        UpdateCtrl(cpt);
      };
      break;
    case 9:
      break;
    case 10:
      newValue = encnewmap(value, 0, 2, Envs[pos].Destination, Encoder);
      if (newValue != Envs[pos].Destination )
      {
        Envs[pos].Destination = newValue;
        ret = true;
        UpdateCtrl(cpt);
      };
      break;
    case 11:
      newValue = encnewmap(value, 1, 500, Envs[pos+1].Attack, Encoder);
      if (newValue != Envs[pos+1].Attack )
      {
        Envs[pos+1].Attack = newValue;
        ret = true;
        UpdateCtrl(cpt);
      };
      break;
    case 12:
      newValue = encnewmap(value, 1, 500, Envs[pos+1].Decay, Encoder);
      if (newValue != Envs[pos+1].Decay )
      {
        Envs[pos+1].Decay = newValue;
        ret = true;
        UpdateCtrl(cpt);
      };
      break;
    case 13:
      newValue = encnewmap(value, 0, 100, Envs[pos+1].Sustain, Encoder );
      if (newValue != Envs[pos+1].Sustain )
      {
        Envs[pos+1].Sustain = newValue;
        ret = true;
        UpdateCtrl(cpt);
      };
      break;
    case 14:
      newValue = encnewmap(value, 1, 500 , Envs[pos+1].Release, Encoder);
      if (newValue != Envs[pos+1].Release )
      {
        Envs[pos+1].Release = newValue;
        ret = true;
        UpdateCtrl(cpt);
      };
      break;
    case 15:
      newValue = encnewmap(value, 0, 2, Envs[pos+1].Trig, Encoder);
      if (newValue != Envs[pos+1].Trig )
      {
        Envs[pos+1].Trig = newValue;
        ret = true;
        UpdateCtrl(cpt);
      };
      break;
    case 16:
      newValue = encnewmap(value, -99, 99,  Envs[pos+1].Ampl, Encoder);
      if (newValue !=  Envs[pos+1].Ampl  )
      {
        Envs[pos+1].Ampl = newValue;
        ret = true;
        UpdateCtrl(cpt);
      };
      break;
    case 17:
      newValue = encnewmap(value, 0, 1000, Envs[pos+1].Delay, Encoder);
      if (newValue !=  Envs[pos+1].Delay  )
      {
        Envs[pos+1].Delay = newValue;
        ret = true;
        UpdateCtrl(cpt);
      };
      break;
    case 18:
      newValue = encnewmap(value, 0, 1, Envs[pos+1].Log, Encoder);
      if (newValue !=  Envs[pos+1].Log  )
      {
        Envs[pos+1].Log = newValue;
        ret = true;
        UpdateCtrl(cpt);
      };
      break;
    case 19:
      break;
    case 20:
      newValue = encnewmap(value, 0, 2, Envs[pos+1].Destination, Encoder);
      if (newValue != Envs[pos+1].Destination )
      {
        Envs[pos+1].Destination = newValue;
        ret = true;
        UpdateCtrl(cpt);
      };
      break;   
    }
  }
  return ret;

}


boolean LFo::ChangePage(int value, byte Encoder)
{  
  boolean ret = false;
  int newValue = encnewmap(value, 0, 1 , CurLfoEnv, Encoder);
  if ( newValue != CurLfoEnv )
  {
    ret = true;
    CurLfoEnv = newValue;
    DessinPage();
  }
  return ret;
}


boolean LFo::ChangeM1(int value, byte Encoder)
{
  boolean ret = false;
  if ( Menu->ChangeM1(value, Encoder) )
  {

  }
  return ret;

}

boolean LFo::ChangeM2(int value, byte Encoder)
{
  boolean ret = false;
  if ( Menu->ChangeM2(value, Encoder) )
  {
  }
  return ret;
}

boolean LFo::ChangeM3(int value, byte Encoder)
{
  boolean ret = false;
  if ( Menu->ChangeM3(value, Encoder) )
  {
  }
  return ret;
}

void LFo::ExStart()
{
  for (int pos=0;pos<NB_LFO;pos++)
  {
    Envs[pos].StartTime=micros();
    Envs[pos].Retrig=false;
    LFos[pos].StartTime=micros();
    LFos[pos].Retrig=false;
  }
}

void LFo::ExStop()
{
}

boolean LFo::ChangeS1S5(int value)
{
  boolean ret = false;
  if ( Menu->ChangeS1S5(value) )
  {
    if ( Menu->FirstPage )
    {  
      switch (Menu->S1S5)
      {
      case 3 :
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

          if ( MySD.OpenFileForRead(MODE_LFO, Menu->GetFName(FileName) ) )
          {
            MySD.ReadStream((byte *)LFos, sizeof(LFos) ) ;
            MySD.ReadStream((byte *)Envs, sizeof(Envs) ) ;
            MySD.CloseFile();
          }
          PatLed.Show(0);
          DessinPage();
          break;

        case 1:  /* Save */

          if ( MySD.OpenFileForWrite(MODE_LFO, Menu->MenuFName(FileName) ) )
          {
            MySD.WriteStream((byte *)LFos, sizeof(LFos) ) ;
            MySD.WriteStream((byte *)Envs, sizeof(Envs) ) ;
            MySD.CloseFile();
          }
          PatLed.Show(0);
          break;

        case 2: /* Init */
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



void LFo::Backup()
{
  MySD.WriteStream((byte *)LFos, sizeof(LFos) ) ;
  MySD.WriteStream((byte *)Envs, sizeof(Envs) ) ;
}

void LFo::Restore()
{
  MySD.ReadStream((byte *)LFos, sizeof(LFos) ) ;
  MySD.ReadStream((byte *)Envs, sizeof(Envs) ) ;
}


void LFo::Encoder(byte Value)
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

void LFo::Interface(int Num, int Value)
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
  }
}





static inline int32_t LinOrLog(byte mode, int32_t value, int32_t elapse, int32_t duree)
{
  if (mode==0)  return ( value * elapse / duree );
  uint32_t pos = elapse * 0xFFFF / duree ;
  uint16_t integral = ( pos >> 8 );
  uint16_t fractional = (  pos & 0xFF  );
  return (((( logPow[ integral ] * ( 0x100 - fractional )  +  logPow[ integral + 1 ] * fractional ) >> 8 ) * value ) >> 16  );
}




static inline int32_t interpolateLfoOnde(const int16_t *Onde, uint32_t index)
{	
  // En table 512 echantillions et en calcul LFO_SAMPLES
  int32_t integral = ( index >> 1 );
  int32_t fractional = ( index & 1  ) ; 
  return ( (int32_t)Onde[ integral ] * ( 2 - fractional )  +  (int32_t)Onde[ integral + 1 ] * fractional ) >> 1; 
}



void LFo::Traite(uint32_t reftime)
{


  int32_t Mouv ;
  lastreftime = reftime;
  uint32_t delta;
  const int16_t *Onde;
  int16_t nvalue;

  /*  
   if ( reftime < next_event ) 
   {
   return;    
   } 
   */
  for (int pos=0;pos<NB_LFO;pos++)
  {
    
    nvalue = Envs[pos].last_n;
    
    switch ( Envs[pos].Etape ) 
    {
    case 1:  // Attack
      delta = ( reftime - Envs[pos].StartTime ) / 10000;
      if ( delta <= Envs[pos].Attack ) 
      {
        Envs[pos].LevelRef = LinOrLog( Envs[pos].Log,  LFO_RESO ,   delta ,  Envs[pos].Attack ) ;
        nvalue =  Envs[pos].LevelRef * Envs[pos].Ampl / 100 ;
      }
      else
      {
        Envs[pos].EtapeRef = Envs[pos].LevelRef ;
        Envs[pos].StartTime = reftime;
        Envs[pos].Etape++;
      }
      break;
    case 2:  // Decay
      delta = ( reftime - Envs[pos].StartTime ) / 10000;
      if ( delta <= Envs[pos].Decay) 
      {
        Envs[pos].LevelRef =  LFO_RESO  * Envs[pos].Sustain / 100 + LinOrLog( Envs[pos].Log,  Envs[pos].EtapeRef - (long)LFO_RESO  * Envs[pos].Sustain / 100,  Envs[pos].Decay - delta , Envs[pos].Decay );
        nvalue =  Envs[pos].LevelRef * Envs[pos].Ampl / 100 ;

      }
      else
      {
        Envs[pos].EtapeRef =  Envs[pos].LevelRef;
        Envs[pos].StartTime = reftime;
        if ( Envs[pos].Sustain == 0 )
          Envs[pos].Etape=0;
        else
          Envs[pos].Etape++;
      }
      break;
    case 3:  // Sustain
      nvalue = Envs[pos].LevelRef = (long)LFO_RESO  * Envs[pos].Sustain / 100 ;
      break;
    case 4:  // Release
      delta = ( reftime - Envs[pos].StartTime ) / 10000;
      if ( delta <= Envs[pos].Release )
      {
        Envs[pos].LevelRef = LinOrLog( Envs[pos].Log,  Envs[pos].EtapeRef , ( Envs[pos].Release - delta ) , Envs[pos].Release ) ;
        nvalue =  Envs[pos].LevelRef * Envs[pos].Ampl / 100 ;
      }
      else
      {
        Envs[pos].Out = 0; 
        Envs[pos].Etape=0;
      }
      break;
    }

     if ( Envs[pos].Smooth > 0 )
     {  
        nvalue =  Envs[pos].last_n + LFO_SMOOTH_VALUE * (nvalue - Envs[pos].last_n) / 0xFF;
        Envs[pos].Smooth--;
     }
     
     Envs[pos].last_n = Envs[pos].Out = nvalue;
 

    /*
#ifdef _DEBUG_MODE_
     
     if ( Envs[pos].Etape > 0 && pos==0)
     {
     Serial.print(pos);
     Serial.print("/");
     Serial.print(Envs[pos].Etape);
     Serial.print("/");
     Serial.print(Envs[pos].LevelRef);
     Serial.print("/");
     Serial.print(Envs[pos].Out);
     Serial.println("");
     }    
     #endif
     */

    // LFOs
    
    if ( LFos[pos].Retrig ) {
      LFos[pos].StartTime=reftime;
      LFos[pos].Retrig=false;
      LFos[pos].Smooth = LFO_SMOOTH_TIME_SHORT;
    }


    switch( LFos[pos].Shap )
    {
    case 0 :
      Onde = Sinus; 
      break;
    case 1 : 
      Onde = Triangle[0]; 
      break;
    case 2 : 
      Onde = SawUp[0]; 
      break;
    case 3 : 
      Onde = SawDown[0]; 
      break;
    case 4 : 
      Onde = Square[0]; 
      break;
    }

    if ( LFos[pos].Modu ) // Si modulation
    {
      // char *listlModul   ="Off    LFO1/2 Env 1  Env 2  CV In  CC 1   CC 2   CC 3   CC 4   ";
      switch( LFos[pos].Modu )
      {
      case 1 :
        Mouv = (pos==1)? LFos[0].Out : LFos[1].Out ; // Amplitude de LFO_RESO
        break;
      case 2 :
        Mouv=Envs[0].Out; // Amplitude de LFO_RESO
        break;
      case 3 :
        Mouv=Envs[1].Out; // Amplitude de &
        break;
      }
      //     Mouv =  ( (Mouv * LFos[pos].ModAmpl)/100 ) * 100 / LFO_RESO ; // Reduit Mouv de l'amplitude de la modulation et ramene ÃƒÂ  -100...0..100
      if ( LFos[pos].ModDest==0 ) // Amplitude
      {
        if ( LFos[pos].PerioMicros < PERIO_LIM  )
          LFos[pos].Index = ( ( ( ( reftime - LFos[pos].StartTime ) %  LFos[pos].PerioMicros ) * LFO_SAMPLES  ) / LFos[pos].PerioMicros) % LFO_SAMPLES ;
        else
          LFos[pos].Index = ( ( ( ( reftime - LFos[pos].StartTime ) %  LFos[pos].PerioMicros ) ) / ( LFos[pos].PerioMicros / LFO_SAMPLES ) ) % LFO_SAMPLES ;
        
        nvalue =  ( interpolateLfoOnde( Onde , LFos[pos].Index ) + Mouv * LFos[pos].ModAmpl / 100 )  * LFos[pos].Ampl / 100;
        if (nvalue >= LFO_RESO) nvalue=LFO_RESO-1;
        else if (nvalue <= -LFO_RESO) nvalue = 1 - LFO_RESO;


      }  
      else // Frequence 
      {
        int newpos =  LFos[pos].Freq + ( Mouv * LFos[pos].Freq ) / LFO_RESO;
        if (newpos<0) newpos=0;
        if (newpos>199) newpos=199;

        ChangeFreq(pos, newpos);

        if ( LFos[pos].PerioMicros < PERIO_LIM  )
          LFos[pos].Index = ( ( ( ( reftime - LFos[pos].StartTime ) %  LFos[pos].PerioMicros ) * LFO_SAMPLES  ) / LFos[pos].PerioMicros) % LFO_SAMPLES ;
        else
          LFos[pos].Index = ( ( ( ( reftime - LFos[pos].StartTime ) %  LFos[pos].PerioMicros ) ) / ( LFos[pos].PerioMicros / LFO_SAMPLES ) ) % LFO_SAMPLES ;

        nvalue =  interpolateLfoOnde( Onde , LFos[pos].Index ) * LFos[pos].Ampl / 100;

 
      }
    } 
    else
    { 
      if ( LFos[pos].PerioMicros < PERIO_LIM  ) // Si la pÃƒÂ©riode n'est pas trop grande on peut faire en 32 bits sans dÃƒÂ©bordement...
        LFos[pos].Index = ( ( ( ( reftime - LFos[pos].StartTime ) %  LFos[pos].PerioMicros ) * LFO_SAMPLES  ) / LFos[pos].PerioMicros) % LFO_SAMPLES ;
      else
        LFos[pos].Index = ( ( ( ( reftime - LFos[pos].StartTime ) %  LFos[pos].PerioMicros ) ) / ( LFos[pos].PerioMicros / LFO_SAMPLES ) ) % LFO_SAMPLES ;

       nvalue = interpolateLfoOnde( Onde , LFos[pos].Index ) * LFos[pos].Ampl / 100;

     }  
/*
     if ( LFos[pos].Smooth > 0 )
     {  
        nvalue =  LFos[pos].last_n + LFO_SMOOTH_VALUE * (nvalue - LFos[pos].last_n) / 0xFF;
        LFos[pos].Smooth--;
     }
*/     
     LFos[pos].last_n = LFos[pos].Out = nvalue;
 
     switch( LFos[pos].Destination )
     {
       case 1 :
       case 2 :
       case 3 :
       case 4 :
       case 5 :
       case 6 :
       case 7 :
       case 8 :
        S4822.Send(LFos[pos].Destination -1 , (uint16_t)( LFos[pos].Out + 0x8000 ) >> 4 );
        break;
     }       
  }


  //  next_event = next_event + delta_event;

}





void LFo::ChangeFreq(int pos, int value)
{

  LFos[pos].PerioMicros = (uint32_t)( 1000000.00 / cv_INT_HZ[value] );
  LFos[pos].StartTime =  lastreftime - ( LFos[pos].Index  *  LFos[pos].PerioMicros ) / LFO_SAMPLES  ;
  LFos[pos].Smooth = LFO_SMOOTH_TIME;
}





void LFo::HandleNoteOn(byte Interface, byte channel, byte pitch, byte velocity)
{
  for (int pos=0;pos<NB_LFO;pos++)
  {
    if ( Envs[pos].Etape==0 && Envs[pos].Trig == Interface)
    {
      Envs[pos].Etape=1;
      Envs[pos].EtapeRef=Envs[pos].LevelRef=0;
      Envs[pos].StartTime=lastreftime;
      Envs[pos].Smooth = ENV_SMOOTH_TIME_SHORT;

      /*
#ifdef _DEBUG_MODE_
       if ( pos==0 )
       {
       Serial.print("LFo:HandleNoteOn=");
       Serial.print(pos);
       Serial.print("=");
       Serial.print(Envs[pos].Etape);
       Serial.print("=");
       Serial.print(Envs[pos].StartTime);
       Serial.print("=");
       Serial.print(Envs[pos].Out);
       Serial.println("");
       }
       #endif
       */

    }
  }
}



void LFo::HandleNoteOff(byte Interface, byte channel, byte pitch, byte velocity)
{
  for (int pos=0;pos<NB_LFO;pos++)
  {
    if ( Envs[pos].Etape>=1 && Envs[pos].Trig == Interface)
    {
      Envs[pos].EtapeRef =  Envs[pos].LevelRef;
      Envs[pos].Etape=(Envs[pos].EtapeRef==0)?0:4;
      Envs[pos].StartTime=lastreftime;
      Envs[pos].Smooth = ENV_SMOOTH_TIME_SHORT;


      /*          
       #ifdef _DEBUG_MODE_
       if ( pos==0 )
       {
       Serial.print("LFo:HandleNoteOff=");
       Serial.print(pos);
       Serial.print("=");
       Serial.print(Envs[pos].Etape);
       Serial.print("=");
       Serial.print(Envs[pos].StartTime);
       Serial.print("=");
       Serial.print(Envs[pos].Out);
       Serial.println("");
       }
       #endif
       */

    }
  }
}





