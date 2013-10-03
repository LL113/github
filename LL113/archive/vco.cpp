// -----------------------------------------------
// LL113
// -----------------------------------------------
// Historique:
// (Baloran) laurent.lecatelier@free.fr, version 0.0.1  10/02/12013
// -----------------------------------------------
// VCO.cpp Code Class gestion des VCOs et Enveloppes


#include <LiquidCrystal.h>
#include "Arduino.h"
#include "VCO.h"
#include "utilld.h"
#include "SDFile.h"
#include "multiplexAD.h"
#include "Audioint.h"
#include "lfo.h"
#include "onde.h"

extern SDFile MySD;
extern mPatLed PatLed;

// Sequenceur Step
extern LFo LFoBi;


extern multiplexAD mxAD;
extern LiquidCrystal lcdM;
extern LiquidCrystal lcd1;
extern LiquidCrystal lcd2;
extern LiquidCrystal lcd3;
extern LiquidCrystal lcd4;

const char *VCOTitre="\x01 Modules VCOs     \x02";

extern const char *mListSource;
extern const char *mListMesure;
extern const int  *NbClock;

const char *listvModul    ="Off    LFO1   LFO2   ENV1   ENV2   ";
const char *listv1ModDest ="Freq   Level  Shape  Filter Reso   ";
const char *listv2ModDest ="Freq   Level  Shape  Sync   ";

const char *listvSync="Off    VCO 1  FM     ";   

LowPassFilter lFp;

paramMenuList MenuListVCO[] = {
  { 
    "Load", "Name", "", 0, MODE_VCO, -1, 0 , 0, 0, 0, 0 }
  ,
  { 
    "Save", "Name", "Indice", 0, 0, NB_STYLEMU, listStyleMenu , 1, 1, 64, 0   }
  , 
  { 
    "Init", "", "", 0, 0, 0, 0 , 0, 0, 0, 0             }
  , 
  {   
    "", "", "", 0, 0, 0, 0 , 0, 0, 0, 0                 }
};

paramMenu menuVCO = {
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
  MenuListVCO 
};



VCO::VCO(byte pNumFunc)
{ 
  NumFunc=pNumFunc;
  Init();
  Menu = new mMenu(&menuVCO, VCOTitre);
  Smooth = SMOOTH_TIME ;
  last_n = 0;
}

VCO::~VCO()
{
}

void VCO::Init()
{
  lastChangeValue = CHGT_PAGE;
  
  for (int pos=0;pos<NB_VCO;pos++)
  {


    VCOs[pos].Shap=0;          
    VCOs[pos].Freq=0;        
    VCOs[pos].Ampl=50;  
    VCOs[pos].Modu=0;  
    VCOs[pos].ModDest=0;
    VCOs[pos].ModAmpl=0;  
    VCOs[pos].Fine=0;
    VCOs[pos].Out=0;
    VCOs[pos].index=0;
    VCOs[pos].fFiltre=0;  
    VCOs[pos].fReso=0;  
    VCOs[pos].Sync=0;  
    VCOs[pos].AmSync=0;  
    SetNote(pos,36);
  }
  lastreftime = micros();

  lFp.setResonance( VCOs[0].fReso );
  lFp.setCutoffFreq( VCOs[0].fFiltre );

}

void VCO::DessinPage()
{
  Menu->DessinPage();
  Z_Clear();

  int pos=0;

  sprintf(tmpCh,"\x02VCO %d", pos+1);
  Z_Titre(1,tmpCh);
  lcd_Num(tmpVal,VCOs[pos].Shap);
  Z_Value(1,tmpVal);

  lcd_Num(tmpVal,VCOs[pos].Ampl);
  Z_TextValue(2,"Level",tmpVal);

  lcd_Liste(tmpVal,(char *)listvModul,VCOs[pos].Modu);
  Z_TextValue(3,"Modula.",tmpVal);

  lcd_Liste(tmpVal,(char *)listv1ModDest,VCOs[pos].ModDest);
  Z_TextValue(4,"Mod.Dst",tmpVal);

  lcd_NumOff(tmpVal,VCOs[pos].fFiltre);
  Z_TextValue(5,"Filtre",tmpVal);

  lcd_Num(tmpVal,VCOs[pos].Freq);
  Z_TextValue(6,"Freq.",tmpVal);

  lcd_Num(tmpVal,VCOs[pos].Fine);
  Z_TextValue(7,"Fine",tmpVal);

  lcd_Num(tmpVal,VCOs[pos].ModAmpl);
  Z_TextValue(8,"Mod.Lev",tmpVal);

  lcd_Num(tmpVal,VCOs[pos].fReso);
  Z_TextValue(10,"Reso",tmpVal);

  pos++;

  sprintf(tmpCh,"\x02VCO %d", pos+1);
  Z_Titre(11,tmpCh);
  lcd_Num(tmpVal,VCOs[pos].Shap);
  Z_Value(11,tmpVal);

  lcd_Num(tmpVal,VCOs[pos].Ampl);
  Z_TextValue(12,"Level",tmpVal);

  lcd_Liste(tmpVal,(char *)listvModul,VCOs[pos].Modu);
  Z_TextValue(13,"Modula.",tmpVal);

  lcd_Liste(tmpVal,(char *)listv2ModDest,VCOs[pos].ModDest);
  Z_TextValue(14,"Mod.Dst",tmpVal);

  lcd_Liste(tmpVal,(char *)listvSync,VCOs[pos].Sync);
  Z_TextValue(15,"Sync",tmpVal);

  lcd_Num(tmpVal,VCOs[pos].Freq);
  Z_TextValue(16,"Freq.",tmpVal);

  lcd_Num(tmpVal,VCOs[pos].Fine);
  Z_TextValue(17,"Fine",tmpVal);

  lcd_Num(tmpVal,VCOs[pos].ModAmpl);
  Z_TextValue(18,"Mod.Lev",tmpVal);

  lcd_Num(tmpVal,VCOs[pos].AmSync);
  Z_TextValue(20,"FM.Lev",tmpVal);

}


void VCO::UpdateCtrl(int cpt)
{
  int pos=0;

  switch(cpt)
  {
  case 1:
    lcd_Num(tmpVal,VCOs[pos].Shap);
    Z_Value(cpt,tmpVal);
    break;

  case 2:
    lcd_Num(tmpVal,VCOs[pos].Ampl);
    Z_Value(cpt,tmpVal);
    break;

  case 3:
    lcd_Liste(tmpVal,(char *)listvModul,VCOs[pos].Modu);
    Z_Value(cpt,tmpVal);
    break;

  case 4:
    lcd_Liste(tmpVal,(char *)listv1ModDest,VCOs[pos].ModDest);
    Z_Value(cpt,tmpVal);
     break;

  case 5:
    lcd_NumOff(tmpVal,VCOs[pos].fFiltre);
    Z_Value(cpt,tmpVal);
   break;

  case 6:
    lcd_Num(tmpVal,VCOs[pos].Freq);
    Z_Value(cpt,tmpVal);
    break;

  case 7:
    lcd_Num(tmpVal,VCOs[pos].Fine);
    Z_Value(cpt,tmpVal);
    break;

  case 8:
    lcd_Num(tmpVal,VCOs[pos].ModAmpl);
    Z_Value(cpt,tmpVal);
    break;

  case 9:
    break;

  case 10:
    lcd_Num(tmpVal,VCOs[pos].fReso);
    Z_Value(cpt,tmpVal);
    break;

  case 11:
    lcd_Num(tmpVal,VCOs[pos+1].Shap);
    Z_Value(cpt,tmpVal);
    break;

  case 12:
    lcd_Num(tmpVal,VCOs[pos+1].Ampl);
    Z_Value(cpt,tmpVal);
    break;

  case 13:
    lcd_Liste(tmpVal,(char *)listvModul,VCOs[pos+1].Modu);
    Z_Value(cpt,tmpVal);
    break;

  case 14:
    lcd_Liste(tmpVal,(char *)listv2ModDest,VCOs[pos+1].ModDest);
    Z_Value(cpt,tmpVal);
    break;

  case 15:
    lcd_Liste(tmpVal,(char *)listvSync,VCOs[pos+1].Sync);
    Z_Value(cpt,tmpVal);
    break;

  case 16:
    lcd_Num(tmpVal,VCOs[pos+1].Freq);
    Z_Value(cpt,tmpVal);
    break;

  case 17:
    lcd_Num(tmpVal,VCOs[pos+1].Fine);
    Z_Value(cpt,tmpVal);
    break;

  case 18:
    lcd_Num(tmpVal,VCOs[pos+1].ModAmpl);
    Z_Value(cpt,tmpVal);
    break;

  case 19:
    break;

  case 20:
    lcd_Num(tmpVal,VCOs[pos+1].AmSync);
    Z_Value(cpt,tmpVal);
    break;
  }
}

// 0..31 32..64 64..128 128..192 192..256

boolean VCO::ChangeValue(int cpt, int value, byte Encoder)
{  
  int newValue;
  int ret=false;

  // cpt compte de 1 ÃƒÆ’  ...
  if (cpt >=16) cpt-=6;
  cpt++;

  int pos=0;

 switch(cpt)
  {
  case 1:
    newValue = encnewmap(value, 0, 159, VCOs[pos].Shap, Encoder);
    if (newValue != VCOs[pos].Shap )
    {
      VCOs[pos].Shap = newValue;
#ifdef _DEBUG_MODE_
      SetWave(VCOs[pos].Shap);   
#endif   
      UpdateNote(pos);
      ret = true;
      UpdateCtrl(cpt);
    };
    break;
  case 2:
    newValue = encnewmap(value, 0, 100, VCOs[pos].Ampl, Encoder);
    if (newValue != VCOs[pos].Ampl )
    {
      VCOs[pos].Ampl = newValue;
      ret = true;
      UpdateCtrl(cpt);
    };
    break;
  case 3:
    newValue = encnewmap(value, 0, 4, VCOs[pos].Modu, Encoder);
    if (newValue != VCOs[pos].Modu )
    {
      VCOs[pos].Modu = newValue;
      ret = true;
      UpdateCtrl(cpt);
    };
    break;
  case 4:
    newValue = encnewmap(value, 0, 5 , VCOs[pos].ModDest, Encoder);
    if (newValue != VCOs[pos].ModDest )
    {
      VCOs[pos].ModDest = newValue;
      ret = true;
      UpdateCtrl(cpt);
    };
    break;
  case 5:
     newValue = encnewmap(value, 0, 255 , VCOs[pos].fFiltre, Encoder);
    if (newValue != VCOs[pos].fFiltre )
    {
      VCOs[pos].fFiltre = newValue;
      lFp.setCutoffFreq(VCOs[pos].fFiltre);
      ret = true;
      UpdateCtrl(cpt);
    };
    break;
  case 6:
    newValue = encnewmap(value, -24, +24, VCOs[pos].Freq, Encoder);
    if (newValue !=  VCOs[pos].Freq  )
    {
      VCOs[pos].Freq = newValue;
      UpdateNote(pos);
      ret = true;
      UpdateCtrl(cpt);
    };
    break;
  case 7:
    newValue = encnewmap(value, -99, 99, VCOs[pos].Fine, Encoder);
    if (newValue !=  VCOs[pos].Fine  )
    {
      VCOs[pos].Fine = newValue;
      UpdateNote(pos);
      ret = true;
      UpdateCtrl(cpt);
    };
    break;
  case 8:
    newValue = encnewmap(value, -99, 99,  VCOs[pos].ModAmpl, Encoder);
    if (newValue !=  VCOs[pos].ModAmpl  )
    {
      VCOs[pos].ModAmpl = newValue;
      ret = true;
      UpdateCtrl(cpt);
    };
    break;
  case 9:
    break;
  case 10:
    newValue = encnewmap(value, 0, 255, VCOs[pos].fReso, Encoder);
    if (newValue != VCOs[pos].fReso )
    {
      VCOs[pos].fReso = newValue;
      lFp.setResonance(VCOs[pos].fReso);
      ret = true;
      UpdateCtrl(cpt);
    };
    break;

  case 11:
    newValue = encnewmap(value, 0, 159, VCOs[pos+1].Shap, Encoder);
    if (newValue != VCOs[pos+1].Shap )
    {
      VCOs[pos+1].Shap = newValue;
#ifdef _DEBUG_MODE_
      SetWave(VCOs[pos+1].Shap);
#endif
      UpdateNote(pos+1);
      ret = true;
      UpdateCtrl(cpt);
    };
    break;
  case 12:
    newValue = encnewmap(value, 0, 100, VCOs[pos+1].Ampl, Encoder);
    if (newValue != VCOs[pos+1].Ampl )
    {
      VCOs[pos+1].Ampl = newValue;
      ret = true;
      UpdateCtrl(cpt);
    };
    break;
  case 13:
    newValue = encnewmap(value, 0, 4 , VCOs[pos+1].Modu, Encoder);
    if (newValue != VCOs[pos+1].Modu )
    {
      VCOs[pos+1].Modu = newValue;
      ret = true;
      UpdateCtrl(cpt);
    };
    break;
  case 14:
    newValue = encnewmap(value, 0, 4 , VCOs[pos+1].ModDest, Encoder);
    if (newValue != VCOs[pos+1].ModDest )
    {
      VCOs[pos+1].ModDest = newValue;
      ret = true;
      UpdateCtrl(cpt);
    };
    break;
  case 15:
    newValue = encnewmap(value, 0, 2, VCOs[pos+1].Sync, Encoder);
    if (newValue != VCOs[pos+1].Sync )
    {
      VCOs[pos+1].Sync = newValue;
      UpdateNote(pos+1);
      ret = true;
      UpdateCtrl(cpt);
    };
    break;
  case 16:
    newValue = encnewmap(value, -24, +24, VCOs[pos+1].Freq, Encoder);
    if (newValue !=  VCOs[pos+1].Freq  )
    {
      VCOs[pos+1].Freq = newValue;
      UpdateNote(pos+1);
      ret = true;
      UpdateCtrl(cpt);
    };
    break;
  case 17:
    newValue = encnewmap(value, -99, 99, VCOs[pos+1].Fine, Encoder);
    if (newValue !=  VCOs[pos+1].Fine  )
    {
      VCOs[pos+1].Fine = newValue;
      UpdateNote(pos+1);
      ret = true;
      UpdateCtrl(cpt);
    };
    break;
  case 18:
    newValue = encnewmap(value, -99, 99, VCOs[pos+1].ModAmpl, Encoder);
    if (newValue !=  VCOs[pos+1].ModAmpl  )
    {
      VCOs[pos+1].ModAmpl = newValue;
      ret = true;
      UpdateCtrl(cpt);
    };
    break;
  case 19:
    break;
  case 20:
    newValue = encnewmap(value, -99, 99, VCOs[pos+1].AmSync, Encoder);
    if (newValue !=  VCOs[pos+1].AmSync  )
    {
      VCOs[pos+1].AmSync = newValue;
      ret = true;
      UpdateCtrl(cpt);
    };
    break;
  }
  return ret;

}


boolean VCO::ChangePage(int value, byte Encoder)
{  
  boolean ret = false;
/*
  int newValue = encnewmap(value, 0, 1 , CurVCOEnv, Encoder);
  if ( newValue != CurVCOEnv )
  {
    ret = true;
    CurVCOEnv = newValue;
    DessinPage();
  }
*/
  return ret;
}


boolean VCO::ChangeM1(int value, byte Encoder)
{
  boolean ret = false;
  if ( Menu->ChangeM1(value, Encoder) )
  {

  }
  return ret;

}

boolean VCO::ChangeM2(int value, byte Encoder)
{
  boolean ret = false;
  if ( Menu->ChangeM2(value, Encoder) )
  {
  }
  return ret;
}

boolean VCO::ChangeM3(int value, byte Encoder)
{
  boolean ret = false;
  if ( Menu->ChangeM3(value, Encoder) )
  {
  }
  return ret;
}

void VCO::ExStart()
{
  for (int pos=0;pos<NB_VCO;pos++)
  {
      VCOs[pos].StartTime=micros();
      VCOs[pos].Retrig=false;
  }
}

void VCO::ExStop()
{
}

boolean VCO::ChangeS1S5(int value)
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

          if ( MySD.OpenFileForRead(MODE_VCO, Menu->GetFName(FileName) ) )
          {
            MySD.ReadStream((byte *)VCOs, sizeof(VCOs) ) ;
            MySD.CloseFile();
          }
          PatLed.Show(0);
          DessinPage();
          break;

        case 1:  /* Save */

          if ( MySD.OpenFileForWrite(MODE_VCO, Menu->MenuFName(FileName) ) )
          {
            MySD.WriteStream((byte *)VCOs, sizeof(VCOs) ) ;
            MySD.CloseFile();
          }
          PatLed.Show(0);
          break;

        case 2: /* Init */

                        
#ifdef _DEBUG_MODE_
    char Tmp[128];
    sprintf(Tmp,"const int16_t Sinus[%d]={", VCO_SAMPLES);
    Serial.println(Tmp);
    for (int y=0;y<VCO_SAMPLES;y+=16)
    {
      for (int x=0;x<16;x++)
      {
          Serial.print(Wave[y+x]);
          Serial.print(",");
      }
      Serial.println("");
    }
    Serial.println("};");
#endif

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



void VCO::Backup()
{
  MySD.WriteStream((byte *)VCOs, sizeof(VCOs) ) ;
}

void VCO::Restore()
{
  MySD.ReadStream((byte *)VCOs, sizeof(VCOs) ) ;
}


void VCO::Encoder(byte Value)
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

void VCO::Interface(int Num, int Value)
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


#ifdef _DEBUG_MODE_

void VCO::SetWave(int type)
{
  float value=0;
  float inc=0;
  const float sinac = 6.283185307 / VCO_SAMPLES;
  float angle;

  switch(type)
  {
  case 0 :
    angle=0;
    for (int ct=0;ct<VCO_SAMPLES;ct++)
    {
      Wave[ct]= (int16_t)( sin( angle ) * VCO_RESO );
      angle+=sinac;
    }
    break;
  case 1 :
    inc = (float)(VCO_RESO * 4 ) / VCO_SAMPLES;
    value = 0;
    for (int ct=0;ct<VCO_SAMPLES;ct++)
    {
      Wave[ct]=value;
      if (value >= VCO_RESO || value <= -VCO_RESO  )  inc=-inc;
      value+=inc;
    }
    break;

  case 2 :
    inc=(float)(VCO_RESO*2)/VCO_SAMPLES;
    value=-(VCO_RESO-1);
    for (int ct=0;ct<VCO_SAMPLES;ct++)
    {
      Wave[ (ct+VCO_SAMPLES/2) % VCO_SAMPLES ]=value;
      value+=inc;
    }
    break;

  case 3 :
    inc=-((float)(VCO_RESO*2)/VCO_SAMPLES);
    value=(VCO_RESO-1);
    for (int ct=0;ct<VCO_SAMPLES;ct++)
    {
      Wave[ (ct+VCO_SAMPLES/2) % VCO_SAMPLES ]=value;
      value+=inc;
    }
    break;

  case 4 :
    for (int ct=0;ct<VCO_SAMPLES;ct++)
    {
      if ( ct < VCO_SAMPLES/2 ) 
        Wave[ct]=VCO_RESO-1;
      else
        Wave[ct]=-(VCO_RESO-1);
    }
    break;
  }
  
}

#endif


static inline int32_t interpolateNone(const int16_t *Onde, uint32_t index)
{	
	int32_t integral = ( index >> VCO_FRACTIONAL_SHIFT );
	return ( (uint32_t)Onde[ integral ] ) ; // Echantillion + fraction du suivant ÃƒÂ  12 bits
}


static inline int32_t interpolateOnde(const int16_t *Onde, uint32_t index)
{	
	int32_t integral = ( index >> VCO_FRACTIONAL_SHIFT );
	int32_t fractional = (  index & VCO_FRACTIONAL_MASQ  ); // ramene ÃƒÂ  12 bits
	return ( (int32_t)Onde[ integral ] * ( VCO_FRACTIONAL - fractional )  + (int32_t)Onde[ integral + 1 ] * fractional  ) >> VCO_FRACTIONAL_SHIFT; // balance 2 echantillons ÃƒÂ  12 bits

}


static inline int32_t interpolateSquare(uint32_t pwmValue, uint32_t index)
{	
	int32_t integral = ( index >> VCO_FRACTIONAL_SHIFT );
	int32_t fractional = (  index & VCO_FRACTIONAL_MASQ  ) ; // ramene ÃƒÂ  12 bits
	return ( ( (integral>=pwmValue)? -20000:20000 ) * ( VCO_FRACTIONAL - fractional )  +  ( (integral + 1 >= pwmValue)?-20000:20000 )* fractional ) >> VCO_FRACTIONAL_SHIFT; // Echantillion + fraction du suivant ÃƒÂ  12 bits
}

static inline int32_t interpolate2OndesWithVol(const int16_t *Onde1, const int16_t *Onde2, uint32_t index, uint16_t vol5b)
{	
	int32_t integral = ( index >> VCO_FRACTIONAL_SHIFT );
	int32_t fractional = (  index & VCO_FRACTIONAL_MASQ ); // ramene ÃƒÂ  12 bits
	return ( ( (int32_t)Onde1[ integral ] * ( VCO_FRACTIONAL - fractional )  +  (int32_t)Onde1[ integral + 1 ] * fractional ) * vol5b  +   ( (int32_t)Onde2[ integral ] * ( VCO_FRACTIONAL - fractional )  +  (int32_t)Onde2[ integral + 1 ] * fractional ) * (32 - vol5b ) ) >> (VCO_FRACTIONAL_SHIFT + 5); // Echantillion + fraction du suivant ÃƒÂ  12 bits
}



void VCO::Traite(uint32_t reftime)
{
  float fNote;
  float fFrequency;
  int32_t n1, n11, n;

  const int16_t *Onde_1;
  const int16_t *Onde_2;
  int16_t vOnde_1;

  const int16_t *Onde_11;
  const int16_t *Onde_21;
  int16_t vOnde_11;
  
  int16_t mod_1; 
  int16_t mod_2; 


  int32_t ampli_1 = VCOs[0].Ampl;
  int32_t ampli_2 = VCOs[1].Ampl;
  
  int32_t shape_1 = VCOs[0].Shap;
  int32_t shape_2 = VCOs[1].Shap;
  
  int32_t filtre;
  int32_t AmSync = VCOs[1].AmSync;


  int16_t octa1 = VCOs[0].LastNote % 12;
  int16_t octa2 = VCOs[1].LastNote % 12;
  
  if ( pt_canwrite )
  {
      uint16_t *buffer = (uint16_t *)&audiBuf[pt_limite_write];



      if ( VCOs[0].Modu > 0 )
      {  
          mod_1 = 0;
          switch ( VCOs[0].Modu  )
          {
           case 1 :
                 mod_1 = LFoBi.LFos[0].Out; break;
           case 2 :
                 mod_1 = LFoBi.LFos[1].Out; break;
           case 3 :
                 mod_1 = LFoBi.Envs[0].Out; break;
           case 4 :
                 mod_1 = LFoBi.Envs[1].Out; break;
          }           
          switch ( VCOs[0].ModDest  )
          {
           case 0 :
                 fNote = (float)VCOs[0].LastNote+(2.0*VCOs[0].Fine/100)+(float)VCOs[0].Freq+(float)mod_1*VCOs[0].ModAmpl/50000;
                 fFrequency = ((pow(2.0,(fNote-69.0)/12.0)) * 440.0); 
                 VCOs[0].inc  = ( (float)VCO_SAMPLES * fFrequency / 41015.625 ) * VCO_FRACTIONAL;
                 break;
           case 1 :
                 ampli_1 += VCOs[0].ModAmpl * mod_1 / LFO_RESO;
                 if ( ampli_1 < 0 ) ampli_1=0;
                 if ( ampli_1 > 100 ) ampli_1=100;
                 break;
           case 2 :
                 shape_1 += VCOs[0].ModAmpl * mod_1 / LFO_RESO; 
                 if ( shape_1 < 1 ) shape_1=1;
                 if ( shape_1 > 158 ) shape_1=158;
                 break;
           case 3 :
                 filtre =  VCOs[0].fFiltre + (128 * VCOs[0].ModAmpl * mod_1) / (100 * LFO_RESO); 
                 if ( filtre < 0 ) filtre=0;
                 if ( filtre > 255 ) filtre=255;
                 lFp.setCutoffFreq( filtre );
                 break;
           case 4 :
                 filtre =  VCOs[0].fReso + (128 * VCOs[0].ModAmpl * mod_1) / (100 * LFO_RESO); 
                 if ( filtre < 0 ) filtre=0;
                 if ( filtre > 255 ) filtre=255;
                 lFp.setResonance( filtre );
                 break;
          }
      }



      if ( VCOs[1].Modu > 0 )
      {  
          mod_2 = 0;
          switch ( VCOs[1].Modu  )
          {
           case 1 :
                 mod_2 = LFoBi.LFos[0].Out; break;
           case 2 :
                 mod_2 = LFoBi.LFos[1].Out; break;
           case 3 :
                 mod_2 = LFoBi.Envs[0].Out; break;
           case 4 :
                 mod_2 = LFoBi.Envs[1].Out; break;
          }           
          switch ( VCOs[1].ModDest  )
          {
           case 0 :
                 fNote = (float)VCOs[1].LastNote+(2.0*VCOs[1].Fine/100)+(float)VCOs[1].Freq+(float)mod_2 * VCOs[1].ModAmpl/50000;
                 fFrequency = ((pow(2.0,(fNote-69.0)/12.0)) * 440.0); 
                 VCOs[1].inc  = ( (float)VCO_SAMPLES * fFrequency / 41015.625 ) * VCO_FRACTIONAL;
                 break;
           case 1 :
                 ampli_2 += VCOs[1].ModAmpl * mod_2 / LFO_RESO;
                 if ( ampli_2 < 0 ) ampli_1=0;
                 if ( ampli_2 > 100 ) ampli_2=100;
                 break;
           case 2 :
                 shape_2 += VCOs[1].ModAmpl * mod_2 / LFO_RESO; 
                 if ( shape_2 < 1 ) shape_2=1;
                 if ( shape_2 > 158 ) shape_2=158;
                 break;
           case 4 :
                 AmSync += VCOs[1].ModAmpl * mod_2 / LFO_RESO;
                 if ( AmSync < -99 ) AmSync=-99;
                 if ( AmSync > 99 ) AmSync=99;
                 break;
          }
      }

      if ( shape_1 < 32 )
      {
          Onde_1 = Sinus; Onde_2 = SawUp[octa1]; vOnde_1 = 32;
      }
      else if ( shape_1 < 64 )
      {
          Onde_1 = SawUp[octa1]; Onde_2 = Triangle[octa1]; vOnde_1 = 64 - shape_1;
      }
      else if ( shape_1 < 96 )
      {
          Onde_1 = Triangle[octa1]; Onde_2 = SawDown[octa1]; vOnde_1 = 96 - shape_1;
      }
      else if ( shape_1 < 128 )
      {
          Onde_1 = SawDown[octa1]; Onde_2 = Square[octa1];  vOnde_1 = 128 - shape_1;
      }
      else if ( shape_1 < 160 )
      {
          Onde_1 = 0; vOnde_1   = VCO_SAMPLES / 2 +  VCO_SAMPLES * (shape_1-128) / 64;
      }


      if ( shape_2 < 32 )
      {
          Onde_11 = Sinus; Onde_21 = SawUp[octa2]; vOnde_11 = 32 - shape_2;
      }
      else if ( shape_2 < 64 )
      {
          Onde_11 = SawUp[octa2]; Onde_21 = Triangle[octa2]; vOnde_11 = 64 - shape_2;
      }
      else if ( shape_2 < 96 )
      {
          Onde_11 = Triangle[octa2]; Onde_21 = SawDown[octa2]; vOnde_11 = 96 - shape_2;
      }
      else if ( shape_2 < 128 )
      {
          Onde_11 = SawDown[octa2]; Onde_21 = Square[octa2]; vOnde_11 = 128 - shape_2;
      }
      else if ( shape_2 < 160 )
      {
          Onde_11 = 0; vOnde_11 = VCO_SAMPLES / 2 +  VCO_SAMPLES * (shape_2-128) / 64;
      }


  
      int index_write = pt_limite_write;
      
      while  (index_write != pt_upper_limite_write)
      {    
        if (Onde_1)
             n1 =   interpolate2OndesWithVol(Onde_1, Onde_2, VCOs[0].index,  vOnde_1) * ampli_1 / 100 ;
        else
             n1 =  interpolateSquare ( vOnde_1 , VCOs[0].index ) * ampli_1 / 100;

        if (Onde_11)
            if ( VCOs[1].Sync == 2 )
                n11 =   interpolate2OndesWithVol( Onde_11, Onde_21, VCOs[0].index + ( VCOs[1].index / 100 ) * AmSync , vOnde_11) * ampli_2 / 100;
            else
                n11 =   interpolate2OndesWithVol( Onde_11, Onde_21, VCOs[1].index ,  vOnde_11) * ampli_2 / 100  ;
        else
            if ( VCOs[1].Sync == 2 )
                n11 =   interpolateSquare ( vOnde_11,  VCOs[0].index + ( VCOs[1].index / 100 ) * AmSync ) * ampli_2 / 100;                                                                                                                                                                                                                                                                                                                                                                           
            else
                n11 =   interpolateSquare ( vOnde_11,  VCOs[1].index ) * ampli_2 / 100;

        n = n1 + n11;
        if ( VCOs[0].fFiltre )  n=lFp.next(n);

       

        if ( Smooth > 0 )
        {  
            n =  last_n + SMOOTH_VALUE * (n - last_n) / 0xFF;
            Smooth--;
        }
        
        last_n = n;

        n += 0x8000;
        n >>=4; 
        
        audiBuf[index_write++] =  (uint32_t)n;
        if ( index_write >= AUDIBUF_SIZE ) index_write=0;

       
        VCOs[0].index += VCOs[0].inc;
        if (VCOs[0].index >= SAMPLES_LIM) {
            VCOs[0].index-=SAMPLES_LIM;
            if ( VCOs[1].Sync == 1) VCOs[1].index=VCOs[0].index;
        }


        VCOs[1].index += VCOs[1].inc;
        if (VCOs[1].index >= SAMPLES_LIM) VCOs[1].index-=SAMPLES_LIM;
		
      }
      pt_canwrite=false;
  }

}


void VCO::SetNote(int pos, int note)
{
    VCOs[pos].LastNote=note;
    float fNote = (float)note+(2.0*VCOs[pos].Fine/100)+(float)VCOs[pos].Freq;
    float fFrequency = ((pow(2.0,(fNote-69.0)/12.0)) * 440.0); 
    VCOs[pos].inc  = ( (float)VCO_SAMPLES * fFrequency / 41015.625 ) * VCO_FRACTIONAL;
    Smooth = SMOOTH_TIME;
}

void VCO::UpdateNote(int pos)
{
//    float fNote = (float)VCOs[pos].LastNote+(2.0*VCOs[pos].Phase/100)+(float)VCOs[pos].Freq+(float)LFoBi.LFos[0].Out/500;

    float fNote = (float)VCOs[pos].LastNote+(2.0*VCOs[pos].Fine/100)+(float)VCOs[pos].Freq;
    float fFrequency = ((pow(2.0,(fNote-69.0)/12.0)) * 440.0); 
    VCOs[pos].inc  = ( (float)VCO_SAMPLES * fFrequency / 41015.625 )  * VCO_FRACTIONAL;
    Smooth = SMOOTH_TIME_SHORT;
}







