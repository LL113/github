// -----------------------------------------------
// LL113
// -----------------------------------------------
// Historique:
// (Baloran) laurent.lecatelier@free.fr, version 0.0.1  10/02/12013
// -----------------------------------------------
// Edsynth.cpp Code Class gestion des Instruments

#include <LiquidCrystal.h>
#include <MIDIMul.h> 
#include "Arduino.h"
#include "edSN.h"
#include "utilld.h"
#include "multiplexAD.h"
#include "SDFile.h"

extern SDFile MySD;
extern mPatLed PatLed;
extern LiquidCrystal lcdM;
extern LiquidCrystal lcd1;
extern LiquidCrystal lcd2;
extern LiquidCrystal lcd3;
extern LiquidCrystal lcd4;

extern SuperOut SPOut;

extern MIDIMul_Class MIDI1;
extern MIDIMul_Class MIDI2;
extern MIDIMul_Class MIDI3;

extern multiplexAD mxAD;

byte MidiInterface[3]={ 
  NOUT_MIDI1,  NOUT_MIDI2, NOUT_MIDI3  };

const char *listPort     ="Out-1 Out-2 Out-3 ";
const char *listOffOn    ="Off    On     ";
const char *listChanel   ="1      2      3      4      5      6      7      8      9      10     11     12     13     14     15     16     ";
const char *listValRes   ="Value Reset Random";


paramMenuList MenuListEd[] = {
  { 
    "Load", "Name", "", 0, MODE_EDITEUR, -1, 0 , 0, 0, 0, 0                   }
  ,
  { 
    "Save", "Name", "Indice", 0, 0, NB_STYLEMU, listStyleMenu , 1, 1, 64, 0                     }
  , 
  { 
    "Send", "", "", 0, 0, 2, listValRes , 0, 0, 0, 0                 }
  , 
  { 
    "Synth", "Name", "", 0, MODE_EDITEUR, -1, 0, 0, 0, 0, 0                 }
  , 
  { 
    "", "", "", 0, 0, 0, 0 , 0, 0, 0, 0                 }
};


paramMenu menuEd = {
  "",
  "",
  "",
  "-In",
  "Midi",     
  "Chanel",          
  "Prog",          
  0,0,2,listPort,     
  1,1,16,0,     
  0,0,127,0,     
  MenuListEd 
};

edSN::edSN(byte pNumFunc)
{ 
  NumFunc = pNumFunc;

  /* 
   maxPos=0;
   while ( curSynth[maxPos].Page > 0 )
   {
   if (curSynth[maxPos].Page>MaxPage) MaxPage = curSynth[maxPos].Page;
   maxPos++;
   }
   
   
   Values = (int *)malloc( maxPos * sizeof(int) );
   
   int pos = 0;
   while ( pos < maxPos )
   {
   Values[pos] = curSynth[pos].rValue;
   pos++;
   }
   */

  Menu = new mMenu(&menuEd,"Editeur");
  ActiveMidi = &MIDI1;

  outInterface = MidiInterface[0];

  MidiChan=menuEd.Val_2;

  curSynth = 0;
  curStr = 0;  
  curSysEx = 0;  
  strIndex = 0;
  sysExIndex = 0;
  Values = 0;
  nbBtn = 0;
  nbPush = 0;
  nbStr = 0;
  nbSysEx = 0;
  maxPos = 0;
  MaxPage = 0;
  /* 
   delta_event = (uint32_t)curSynth[maxPos].Mini * 1000;
   identity = curSynth[maxPos].rValue ;
   SDPath = curSynth[maxPos].List ;
   Menu->SetRealPath((char *)SDPath);
   */
  is_echo = true;
}

edSN::~edSN()
{
}

void edSN::Setup()
{
  CurPage = 1;
  if ( curSynth ) free(curSynth);
  if ( curStr ) free(curStr);	
  if ( curSysEx ) free(curSysEx);
  if ( strIndex ) free(strIndex);
  if ( sysExIndex ) free(sysExIndex);
  if ( Values ) free(Values);
  curSynth = 0;
  curStr = 0;
  curSysEx = 0;
  strIndex = 0;
  sysExIndex = 0;
  Values = 0;

  maxPos = 0;
  MaxPage = 0;

  nbBtn = 0;
  nbPush = 0;
  nbStr = 0;
  nbSysEx = 0;

  sendPos=-1;
  lastChangeValue = CHGT_PAGE;
  delaiSysEx = 10;
  delta_event = (uint32_t)delaiSysEx * 1000;
  next_event=micros();

  i_Name = -1;
  i_Version = -1;
  i_PathSD = -1;

  //	if ( curSysEx ) free(curSysEx);
}

void edSN::LoadFile(char *FileName)
{
#ifdef _DEBUG_MODE_ALL
  Serial.print("edSN::LoadFile=");   
  Serial.print(FileName);   
#endif

  if ( MySD.OpenFileForRead(MODE_EDITEUR,FileName) )
  {
    MySD.ReadStream((byte *)tmpVal, 5);

#ifdef _DEBUG_MODE_ALL
    Serial.print(tmpVal[0]);   
    Serial.print(tmpVal[1]);   
    Serial.print(tmpVal[2]);   
    Serial.print(tmpVal[3]);   
    Serial.print(tmpVal[4]);   
    Serial.println(" OK");   
#endif  

    if  ( memcmp(tmpVal,"LL113",5) != 0 ) return;

    /*		
     w.Write(Name);
     w.Write(PathSD);
     w.Write(Version);
     w.Write((Int16)NbPage);
     w.Write((Int16)HorsPatch);
     w.Write((Int16)Btns.Count);
     w.Write((Int16)PushBtns.Count);
     w.Write((Int16)Dicos.Dicos.Count);
     w.Write((Int16)Delaims);
     */

    Setup();
    i_Name = MySD.ReadStreamInt();


    i_PathSD = MySD.ReadStreamInt();
    i_Version = MySD.ReadStreamInt();
    MaxPage = MySD.ReadStreamInt();
    nbBtn = MySD.ReadStreamInt();
    nbPush = MySD.ReadStreamInt();
    delaiSysEx = MySD.ReadStreamInt() ;
    delta_event = (uint32_t)delaiSysEx * 1000;
    ActiveMidi->SetSysExMs(delaiSysEx);

    maxPos = nbBtn + nbPush;

    //    maxPos = nbBtn;

#ifdef _DEBUG_MODE_ALL
    Serial.print(MaxPage);   
    Serial.print(",");   
    Serial.print(nbBtn);   
    Serial.print(",");   
    Serial.print(nbPush);   
    Serial.print(",");   
    Serial.print(delaiSysEx);   
    Serial.println("");   
#endif  


    curSynth = (paramSynth *)malloc( maxPos * sizeof(paramSynth));


    Values = (int *)malloc( maxPos * sizeof(int) );

    for (int ct=0; ct < maxPos ; ct++)
    {

      if ( MySD.ReadStreamByte()!= 1) break;


      curSynth[ct].i_Name = MySD.ReadStreamInt();      
      curSynth[ct].Page = MySD.ReadStreamInt();      
      curSynth[ct].Index = MySD.ReadStreamInt();      
      curSynth[ct].Mini = MySD.ReadStreamInt();      
      curSynth[ct].Maxi = MySD.ReadStreamInt();
      curSynth[ct].Step = MySD.ReadStreamInt();
      curSynth[ct].Value = MySD.ReadStreamInt();
      curSynth[ct].fAff = MySD.ReadStreamByte();  
      curSynth[ct].fVA = MySD.ReadStreamByte();   
      curSynth[ct].fC1 = MySD.ReadStreamByte();   
      curSynth[ct].fC2 = MySD.ReadStreamByte();   
      curSynth[ct].TypeBtn = MySD.ReadStreamByte();   
      curSynth[ct].EvCond = MySD.ReadStreamByte();   
      curSynth[ct].C1 = MySD.ReadStreamInt();      
      curSynth[ct].C2 = MySD.ReadStreamInt();      
      curSynth[ct].i_List = MySD.ReadStreamInt(); 
      curSynth[ct].i_Cmd = MySD.ReadStreamInt(); 
      curSynth[ct].i_Cond = MySD.ReadStreamInt(); 

      Values[ct] = curSynth[ct].Value;

#ifdef _DEBUG_MODE_ALL
      Serial.print(curSynth[ct].i_Name );   
      Serial.print(",");   
      Serial.print(curSynth[ct].Page );   
      Serial.print(",");   
      Serial.print(curSynth[ct].Index );   
      Serial.print(",");   
      Serial.print(curSynth[ct].Mini );   
      Serial.print(",");   
      Serial.print(curSynth[ct].Maxi );   
      Serial.print(",");   
      Serial.print(curSynth[ct].EvCond);   
      Serial.print(",");   
      Serial.print(curSynth[ct].i_Cmd);   
      Serial.print(",");   
      Serial.print(curSynth[ct].i_Cond );   
      Serial.println("");   
#endif  


    }

    // Lecture index et table des chaines
    if ( MySD.ReadStreamByte() != 2) 
    {
      MySD.CloseFile();
      Setup();
      return;
    }


    nbStr = MySD.ReadStreamInt();
    strIndex = (int16_t *)malloc( nbStr * sizeof(int16_t));
    MySD.ReadStream( (byte *)strIndex, nbStr * sizeof(int16_t));
    int16_t nbOct = MySD.ReadStreamInt();
    curStr = (byte *)malloc( nbOct );
    MySD.ReadStream(curStr, nbOct);


#ifdef _DEBUG_MODE_ALL
    for ( int ct =0;ct <nbStr;ct++)
      Serial.println( (char *)(curStr + strIndex[ct] ));   
    Serial.print("nbstr=");   
    Serial.print(nbStr );   
    Serial.print(",nbOct=");   
    Serial.print(nbOct );   
    Serial.print(",");   
    Serial.print( (char *)(curStr) );   
    Serial.println("");   
#endif  

    // Lecture index et table des SysEx
    if (  MySD.ReadStreamByte() != 3) 
    {
      MySD.CloseFile();
      Setup();
      return;
    }
    nbSysEx = MySD.ReadStreamInt();
    sysExIndex = (int16_t *)malloc( nbSysEx * sizeof(int16_t));
    MySD.ReadStream( (byte *)sysExIndex, nbSysEx * sizeof(int16_t) );
    nbOct = MySD.ReadStreamInt();
    curSysEx = (byte *)malloc( nbOct );
    MySD.ReadStream(curSysEx, nbOct);		

#ifdef _DEBUG_MODE_ALL
    Serial.print("nbSysEx=");   
    Serial.print(nbSysEx );   
    Serial.print(",nbOct=");   
    Serial.println(nbOct );   
    for (int ct=0;ct<nbOct;ct++) 
    {
      sprintf(tmpCh,"%X ",curSysEx[ct]);
      Serial.print(tmpCh);
    }

#endif  

    MySD.CloseFile();
  }

  ActiveMidi->SetSysExMs(delaiSysEx);

  if ( i_Name != -1 )
  {
    int ct=1;
    TitreStr[0]=1;
    TitreStr[19]=2;
    TitreStr[20]=0;
    byte *pt= (byte *)curStr + strIndex[ i_Name ];
    while (*pt)
    {
      TitreStr[ct++]=*(pt++);
      if (ct>18) break;
    }
    while (ct<19) TitreStr[ct++]=32;
    Menu->ChangeTitre(TitreStr);

    SDPath = (const char*)curStr + strIndex[ i_PathSD ];
    if ( !SD.exists( (char *)SDPath )) SD.mkdir((char *)SDPath);
  
    Menu->SetRealPath( (char *)SDPath);

  }
  OldC1=-1;
  OldC2=-1;

}


void edSN::UpdateCtrl(int pos)
{
  int cpt = curSynth[pos].Index;

  switch ( ( curSynth[pos].fAff & 0x0F ) )
  {
  case 0 :
    lcd_Num(tmpVal,Values[pos]);
    break;
  case 1 :
    lcd_SNum(tmpVal,Values[pos]);
    break;
  case 2 :
    lcd_Liste(tmpVal, (char *)(curStr + strIndex[ curSynth[pos].i_List ]), Values[pos] - curSynth[pos].Mini);
    break;
  case 3 :
    lcd_Num(tmpVal,Values[pos] + 1 );
    break;
  }

  Z_Value(cpt,tmpVal);

  // Envoie la valeur....
  midiSend(pos);
}

void edSN::DessinPage()
{
  Menu->DessinPage();
  
  if (is_echo) {
    lcdM.setCursor(13,3);
    lcdM.print("-In");
  }
  else
  {
    lcdM.setCursor(13,3);
    lcdM.print("+In");
  }  
  
  Z_Clear();
  for (int pos = 0; pos < maxPos; pos++)
  {
    if (curSynth[pos].Page == CurPage ) 
    {
      int cur =  curSynth[pos].Index;
      int val =  Values[pos];

      lcd_Text(tmpCh, (char *)(curStr + strIndex[ curSynth[pos].i_Name ]));
      if ( curSynth[pos].Mini == curSynth[pos].Maxi )
      {
        tmpVal[0]=0;
      }
      else
      {
        switch ( ( curSynth[pos].fAff & 0x0F )  )
        {
        case 0 :
          lcd_Num(tmpVal,Values[pos]);
          break;
        case 1 :
          lcd_SNum(tmpVal,Values[pos]);
          break;
        case 2 :
          lcd_Liste(tmpVal,(char *)(curStr + strIndex[ curSynth[pos].i_List ]),Values[pos] - curSynth[pos].Mini);
          break;
        case 3 :
          lcd_Num(tmpVal,Values[pos]+1);
          break;
       }
      }

      Z_TextValue(cur,tmpCh,tmpVal);

    }
  }

}



boolean edSN::ChangeValue(int cpt, int value, byte Encoder)
{  
  boolean ret = false;
  long newValue;

  // cpt compte de 1 Ã¯Â¿Â½  ...
  if (cpt >=16) cpt-=6;
  cpt++;

  for (int pos = 0; pos < maxPos; pos++)
  {
    if (curSynth[pos].Page == CurPage &&  curSynth[pos].Index==cpt && curSynth[pos].Mini != curSynth[pos].Maxi) 
    {
      newValue = encnewmap(value, curSynth[pos].Mini, curSynth[pos].Maxi,  Values[pos], Encoder );
      if ( newValue != Values[pos])
      {
        ret = true;
        Values[pos] = newValue;

#ifdef _DEBUG_MODE_ALL
        Serial.print("Values[pos]=");   
        Serial.print(Values[pos]);   
        Serial.print(",pos=");   
        Serial.println(pos);   
#endif  
        UpdateCtrl(pos);

      }
      break;
    }
  }  
  return ret;
}



boolean edSN::ChangePage(int value, byte Encoder)
{  
  boolean ret = false;
  int newValue = encnewmap(value, 1, MaxPage, CurPage, Encoder );
  if ( newValue != CurPage )
  {
    ret = true;
    CurPage = newValue;
    DessinPage();
  }
  return ret;
}


void edSN::FmtNb(int16_t value, int16_t mini, byte format, byte *dest)
{
  switch(format)
  {
  case 0 :
    *dest = value - mini;
    break;
  case 1 :
    *dest = ( (uint8_t)(value) & 0xF0) >> 4;
    *(dest+1) = ( (uint8_t)(value) & 0x0F);
    break;
  case 2 :
    *(dest) = ( (uint8_t)(value) & 0x0F);
    *(dest+1) = ( (uint8_t)(value) & 0xF0) >> 4;
    break;
  case 3 :
    *dest = ( (uint16_t)(value) & 0x3F80) >> 7;
    *(dest+1) = ( (uint16_t)(value) & 0x007F);
    break;
  case 4 :
    *dest = ( (uint16_t)(value) & 0x007F);
    *(dest+1) = ( (uint16_t)(value) & 0x3F80) >> 7;
    break;  
  }
}

void edSN::midiSend(int pos)
{
  char temp;
  paramSysEx *Cmd;
  int sysExPt=0;
  
  if ( curSynth[pos].i_Cond >= 0 ) 
  {
    if (curSynth[pos].EvCond == 1 || (curSynth[pos].C1 != OldC1 && curSynth[pos].EvCond == 3) || (curSynth[pos].C2 != OldC2 && curSynth[pos].EvCond == 5))
    {
      Cmd = (paramSysEx *)( curSysEx + sysExIndex[curSynth[pos].i_Cond] );
  
      if ( curSynth[pos].fC1 == 6  )
      {
          Cmd->pBuf = 6;
          SysTmp[sysExPt]=176  | ( MidiChan-1 );
          SysTmp[sysExPt+1]=99;
          SysTmp[sysExPt+2]=( (uint16_t)(curSynth[pos].C1) & 0x3F80) >> 7;
          SysTmp[sysExPt+3]=176  | ( MidiChan-1 );
          SysTmp[sysExPt+4]=98;
          SysTmp[sysExPt+5]=( (uint16_t)(curSynth[pos].C1) & 0x007F);
      }
      else
      {
        for (int ct=0;ct<Cmd->pBuf;ct++) 
          SysTmp[sysExPt+ct]=Cmd->Byte[ct];
    
        if ( Cmd->i_CH != 255 ) SysTmp[sysExPt+Cmd->i_CH ] |= ( MidiChan-1 );
        if ( Cmd->i_ID != 255 ) SysTmp[sysExPt+Cmd->i_ID ] |= identity;
        if ( Cmd->i_C1 != 255 ) FmtNb( curSynth[pos].C1, 0, curSynth[pos].fC1 , &SysTmp[sysExPt+Cmd->i_C1 ] ) ;
        if ( Cmd->i_C2 != 255 ) FmtNb( curSynth[pos].C2, 0, curSynth[pos].fC2 , &SysTmp[sysExPt+Cmd->i_C2 ] ) ;
        if ( Cmd->i_Value != 255 ) FmtNb( Values[pos], curSynth[pos].Mini, curSynth[pos].fVA , &SysTmp[sysExPt+Cmd->i_Value ] ) ;
      }
 
      sysExPt+=Cmd->pBuf;
  
    }
  }
 
  if ( curSynth[pos].i_Cmd >= 0 ) 
  {
    Cmd = (paramSysEx *)( curSysEx + sysExIndex[curSynth[pos].i_Cmd] );
   
    if ( curSynth[pos].fVA == 6  )
    {
        Cmd->pBuf = 6;
        SysTmp[sysExPt]=176  | ( MidiChan-1 );
        SysTmp[sysExPt+1]=6;
        SysTmp[sysExPt+2]=( (uint16_t)(Values[pos]) & 0x3F80) >> 7;
        SysTmp[sysExPt+3]=176  | ( MidiChan-1 );
        SysTmp[sysExPt+4]=38;
        SysTmp[sysExPt+5]=( (uint16_t)(Values[pos]) & 0x007F);
    }
    else
    {

      for (int ct=0;ct<Cmd->pBuf;ct++) 
        SysTmp[sysExPt+ct]=Cmd->Byte[ct];
    
      if ( Cmd->i_CH != 255 ) SysTmp[sysExPt+Cmd->i_CH ] |= ( MidiChan-1 );
      if ( Cmd->i_ID != 255 ) SysTmp[sysExPt+Cmd->i_ID ] |= identity;
      if ( Cmd->i_C1 != 255 ) FmtNb( curSynth[pos].C1, 0, curSynth[pos].fC1 , &SysTmp[sysExPt+Cmd->i_C1 ] ) ;
      if ( Cmd->i_C2 != 255 ) FmtNb( curSynth[pos].C2, 0, curSynth[pos].fC2 , &SysTmp[sysExPt+Cmd->i_C2 ] ) ;
      if ( Cmd->i_Value != 255 ) FmtNb( Values[pos], curSynth[pos].Mini, curSynth[pos].fVA , &SysTmp[sysExPt+Cmd->i_Value ] ) ;
    }
  
    sysExPt+=Cmd->pBuf;
  }
  
  if ( curSynth[pos].i_Cond >= 0 ) 
  {
    if (curSynth[pos].EvCond == 2 || (curSynth[pos].C1 != OldC1 && curSynth[pos].EvCond == 4) || (curSynth[pos].C2 != OldC2 && curSynth[pos].EvCond == 6))
    {
      Cmd = (paramSysEx *)( curSysEx + sysExIndex[curSynth[pos].i_Cond] );
 
      if ( curSynth[pos].fC1 == 6  )
      {
          Cmd->pBuf = 6;
          SysTmp[sysExPt]=176  | ( MidiChan-1 );
          SysTmp[sysExPt+1]=99;
          SysTmp[sysExPt+2]=( (uint16_t)(curSynth[pos].C1) & 0x3F80) >> 7;
          SysTmp[sysExPt+3]=176  | ( MidiChan-1 );
          SysTmp[sysExPt+4]=98;
          SysTmp[sysExPt+5]=( (uint16_t)(curSynth[pos].C1) & 0x007F);
      }
      else
      {
        for (int ct=0;ct<Cmd->pBuf;ct++) 
          SysTmp[sysExPt+ct]=Cmd->Byte[ct];
    
        if ( Cmd->i_CH != 255 ) SysTmp[sysExPt+Cmd->i_CH ] |= ( MidiChan-1 );
        if ( Cmd->i_ID != 255 ) SysTmp[sysExPt+Cmd->i_ID ] |= identity;
        if ( Cmd->i_C1 != 255 ) FmtNb( curSynth[pos].C1, 0, curSynth[pos].fC1 , &SysTmp[sysExPt+Cmd->i_C1 ] ) ;
        if ( Cmd->i_C2 != 255 ) FmtNb( curSynth[pos].C2, 0, curSynth[pos].fC2 , &SysTmp[sysExPt+Cmd->i_C2 ] ) ;
        if ( Cmd->i_Value != 255 ) FmtNb( Values[pos], curSynth[pos].Mini, curSynth[pos].fVA , &SysTmp[sysExPt+Cmd->i_Value ] ) ;
      }
      
      sysExPt+=Cmd->pBuf;
    }
  }

  #ifdef _DEBUG_MODE_ALL
      Serial.print(curSynth[pos].i_Cond);   
      Serial.print(",");   
      Serial.print(curSynth[pos].i_Cmd);   
      Serial.print(",");   
      Serial.print(Values[pos]);   
      Serial.print(",");   
      Serial.print(curSynth[pos].C1);   
      Serial.print(",");   
      Serial.print(curSynth[pos].C2);   
      Serial.print("=");   
  #endif  

  if ( sysExPt > 0 )
  {
      ActiveMidi->sendSysEx(sysExPt, SysTmp, true);
      
  #ifdef _DEBUG_MODE_ALL
      for (int ct=0;ct<sysExPt;ct++) 
      {
        sprintf(tmpCh,"%X ",SysTmp[ct]);
        Serial.print(tmpCh);
      }
  #endif  

      OldC1 = curSynth[pos].C1;
      OldC2 = curSynth[pos].C2;

  }
  #ifdef _DEBUG_MODE_ALL
      Serial.println("");   
  #endif  
  


} 

void edSN::Send()
{
  sendPos = 0;

}


boolean edSN::ChangeM1(int value, byte Encoder)
{
  boolean ret = false;

  /// Traitement spÃƒÂ©ciale pour le chgt de repertoire...

  if ( !Menu->FirstPage )
  {  
    int newValue = encnewmap(value, 0, Menu->maxCmd-1 , Menu->curCmd,  Encoder );
    if ( newValue != Menu->curCmd )
    {
      switch(newValue)
      {
      case 0:
      case 1:
        Menu->SetRealPath( (char *)SDPath);
        break;
      case 3:
        Menu->SetRealPath( "EDIT" );
        break;
      }
    }
  }
  
  
  if ( Menu->ChangeM1(value, Encoder) )
  {
    if ( Menu->FirstPage )
    {  
      switch(Menu->M1)
      {
      case 0:
        ActiveMidi = &MIDI1;
        outInterface = MidiInterface[0];
        break;
      case 1:
        ActiveMidi = &MIDI2;
        outInterface = MidiInterface[1];
        break;
      case 2:
        ActiveMidi = &MIDI3;
        outInterface = MidiInterface[2];
        break;
      }
      ActiveMidi->SetSysExMs(delaiSysEx);
    }
  }
  return ret;
}

boolean edSN::ChangeM2(int value, byte Encoder)
{
  boolean ret = false;
  if ( Menu->ChangeM2(value, Encoder) )
  {
    if ( Menu->FirstPage )
    {  
      MidiChan = Menu->M2;
    }
    else
    {
      if ( Menu->curCmd==3 )
      {
        ActiveMidi->sendControlChange(0 , Menu->V1, MidiChan);
      }
    }
  }
  return ret;
}

boolean edSN::ChangeM3(int value, byte Encoder)
{
  boolean ret = false;
  if ( Menu->ChangeM3(value, Encoder) )
  {
    if ( Menu->FirstPage )
    {  
        ActiveMidi->sendProgramChange(Menu->M3, MidiChan);
    }
    else
    {
      if ( Menu->curCmd==3 )
      {
        ActiveMidi->sendProgramChange(Menu->V2, MidiChan);
      }
    }
  }
  return ret;
}

boolean edSN::ChangeS1S15(int value)
{

  boolean ret = false;
  if ( Menu->ChangeS1S5(value) )
  {
    if ( Menu->FirstPage )
    {  
      switch(Menu->S1S5)
      {

      case 1 : 
        break;        

      case 2 : 
        break;        
        
      case 3 : 
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
        case 4:  /* Load */
          LoadFile(Menu->GetFName(FileName));
          Menu->SetRealPath( "EDIT" );
          DessinPage();
          break;

        case 0:  /* Load */
          if ( MySD.OpenFileForRead((char *)SDPath, Menu->GetFName(FileName) ) )
          {
            int pos=0;
            int rid; 
            MySD.ReadStream((byte *)&rid, sizeof(int) ) ;
            if (rid==nbBtn)
            {
              for (int pos = 0; pos < maxPos; pos++)
              {
                  MySD.ReadStream((byte *)&rid, sizeof(int) ) ;
                  Values[pos]=rid;
              }
              MySD.CloseFile();
              typeSend=0;
              Send();
            }
            else
            {
              MySD.CloseFile();
            }
          }
          break;

        case 1:  /* Save */
          if ( MySD.OpenFileForWrite((char *)SDPath,  Menu->MenuFName(FileName) ) )
          {
            int pos=0;
            MySD.WriteStream( (byte *)&nbBtn, sizeof(int) ) ;
            for (int pos = 0; pos < maxPos; pos++)
            {
                MySD.WriteStream((byte *)&Values[pos], sizeof(int) ) ;
            }
            MySD.CloseFile();
            PatLed.Show(0);
          }
          break;

        case 2:  /* Send */
          typeSend = Menu->V1;
          Send();
          break;
        }         
      }
    }

  }

  return ret;
}

void edSN::Encoder(byte Value)
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

void edSN::Interface(int Num, int Value)
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
    ChangeS1S15(Value);
    break;
  }

  // Il faut trouver un moment pour le midi flush Sysex       MIDI1.FlushSysEx();
}

void edSN::Traite(long reftime)
{
  if ( reftime < next_event ) 
  {
    return;    
  } 

  if (  ActiFunc==NumFunc )
  {
    if ( sendPos > - 1)
    {
      if ( curSynth[sendPos].Page>0 && ( curSynth[sendPos].fAff & 0x80 ) == 0 ) 
      {
          if ( typeSend==1) Values[sendPos] =( curSynth[sendPos].Mini > 0)? curSynth[sendPos].Mini : 0;
          if ( typeSend==2) Values[sendPos] = random( ( curSynth[sendPos].Mini > 0)? curSynth[sendPos].Mini : 0 , curSynth[sendPos].Maxi+1 );
          
          midiSend(sendPos);
      }
      sendPos++;
      if ( sendPos >= maxPos )
      {
        OldC1=-1;
        OldC2=-1;
        typeSend=0;
        sendPos=-1;
        DessinPage();
        PatLed.Show(0);
      }
      else
      {
        PatLed.SetStep(sendPos%16);
      }
    }
    else
    {
      ActiveMidi->FlushSysEx();
    }
  }

  next_event = next_event + delta_event;


}

void edSN::HandleNoteOn(byte Interface, byte channel, byte pitch, byte velocity)
{
  if ( is_echo &&  ActiFunc==NumFunc )
  {
    SPOut.NoteOn(outInterface,pitch,velocity,MidiChan);
  }
}


void edSN::HandleNoteOff(byte Interface, byte channel, byte pitch, byte velocity)
{
  if ( is_echo &&   ActiFunc==NumFunc )
  {
    SPOut.NoteOff(outInterface, pitch, velocity,MidiChan);
  }
}


void edSN::HandlePitchBend(byte Interface, byte channel, int bend)
{
  if ( is_echo &&   ActiFunc==NumFunc )
  {
    SPOut.PitchBend(outInterface, bend, MidiChan);
  }
}

void edSN::HandleControlChange(byte Interface, byte channel, byte number, byte value)
{
  if (  is_echo && ActiFunc==NumFunc )
  {
    SPOut.ControlChange(outInterface, number, value, MidiChan);
  }

}

void edSN::HandleProgramChange(byte Interface, byte channel, byte number)
{
  if (  is_echo && ActiFunc==NumFunc )
  {
    SPOut.ProgramChange(outInterface, number, MidiChan);
  }

}

void edSN::HandleAfterTouch(byte Interface, byte channel, byte value)
{
  if (  is_echo &&  ActiFunc==NumFunc )
  {
    SPOut.AfterTouch(outInterface, value, MidiChan);
  }
}









