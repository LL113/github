// -----------------------------------------------
// LL113
// -----------------------------------------------
// Historique:
// (Baloran) laurent.lecatelier@free.fr, version 0.0.1  10/02/12013
// -----------------------------------------------
// utilld.cpp Code Class Menu, Leds et fonctions diverses

#include "utilld.h"
#include "SDFile.h"


char *libMenu="Mnu";
char *libRet="Ret";
char *libFunct="";
char *libNot="   ";
char *libExec="Exec";

extern SDFile MySD;

extern strGlobalParam ParamGlo;


// Dans utilld.h NB_STYLEMU 104
const char *listStyleMenu="@DEF  ACID  AMBI  BRASS BALAD BEAT  BEBOP BIG   BAND  BLUES BOSSA CELTIKCHICAGCHILI CLASSICULTE DANCE DARK  DEATH DIRTY DISCO DREAM DUB   EAST  ELECTREURO  FANFARFOLK  FUNK  GABBERGARAGEGAVOTTGLAM  GOSPEL"
"GRUNGEHAIR  HARD  HEAVY HIPHOPHOUSE INDIE INDUS INSTRUITALO JAVA  JAZZ  JUNGLEKOMPA K-POP LATIN LO-FI LOGOBILOUNGEMADIS MARCHEMENUETMETAL MILON MURGA NEO   NEW   NOISE NU    OLD   OPERA PARTY POP   POWER "
"PROGREPUNK  RAI   RAP   RAVE  REGGAERETRO ROCK  RONDE RUMBA SALSA SLOW  SKA   SLAM  SONATESOUL  SPEED SWING SYMPHOSYNTH TANGO TECH  TEEN  TECKTOTRANCETRIBE TWIST TRIP  VALSE UNBLAKVOCAL X-OVERYELA  ZOUK  ZUMBA ";

char *nameNote[]={ 
  "C%d","C#%d","D%d","D#%d","E%d","F%d","F#%d","G%d","G#%d","A%d","A#%d","B%d" };

mMenu::mMenu(paramMenu *p_Menu, const char *p_Titre)
{
  dMenu = p_Menu;
  dTitre = (char *)p_Titre;
  FirstPage=true;
  curCmd=0;
  maxCmd=0;
  while ( *dMenu->List[maxCmd].M1 ) maxCmd++;
  S1S5=0;
  M1=0;
  M2=0;
  M3=0;
  V1=dMenu->List[curCmd].Val_1;
  V2=dMenu->List[curCmd].Val_2;
  dblClickMillis=millis();
  SDPath=0;
  DBLCLK = false;
}

mMenu::~mMenu()
{
}


void mMenu::ChangeTitre(char *p_Titre)
{
  dTitre = p_Titre;
  DessinPage();
}

void mMenu::SetRealPath(char *path)
{
  SDPath = path;
//  MySD.GetListe(0,SDPath);
}

void mMenu::DessinPage()
{
  lcdM.clear();
  lcdM.print(dTitre);
  if (FirstPage)
  {
    lcdM.setCursor(0,1);
    lcdM.print(dMenu->M1);
    lcdM.setCursor(7,1);
    lcdM.print(dMenu->M2);
    lcdM.setCursor(14,1);
    lcdM.print(dMenu->M3);
    lcdM.setCursor(0,3);
    lcdM.print(dMenu->S1);
    lcdM.setCursor(4,3);
    lcdM.print(dMenu->S2);
    lcdM.setCursor(8,3);
    lcdM.print(dMenu->S3);
    lcdM.setCursor(13,3);
    lcdM.print(dMenu->S4);
    lcdM.setCursor(17,3);
    lcdM.print(libMenu);

    if ( dMenu->Max_1 - dMenu->Min_1 > 0 )
    {        
      if ( dMenu->List_1 )
        lcd_Liste(tmpVal,(char *)dMenu->List_1,dMenu->Val_1,6);
      else
        lcd_Num(tmpVal,dMenu->Val_1,6);
      lcdM.setCursor(0,2);
      lcdM.print(tmpVal);             
    }

    if ( dMenu->Max_2 - dMenu->Min_2 > 0 )
    {        
      if ( dMenu->List_2 )
        lcd_Liste(tmpVal,(char *)dMenu->List_2,dMenu->Val_2,6);
      else
        lcd_Num(tmpVal,dMenu->Val_2,6);
      lcdM.setCursor(7,2);
      lcdM.print(tmpVal);             
    }

    if ( dMenu->Max_3 - dMenu->Min_3 > 0 )
    {        
      if ( dMenu->List_3 )
        lcd_Liste(tmpVal,(char *)dMenu->List_3,dMenu->Val_3,6);
      else
        lcd_Num(tmpVal,dMenu->Val_3,5);
      lcdM.setCursor(14,2);
      lcdM.print(tmpVal);             
    }

  }
  else
  {
    lcdM.setCursor(0,1);
    lcdM.print(libFunct);
    lcdM.setCursor(7,1);
    lcdM.print(dMenu->List[curCmd].M2);
    lcdM.setCursor(14,1);
    lcdM.print(dMenu->List[curCmd].M3);
    lcdM.setCursor(0,2);
    lcdM.print(dMenu->List[curCmd].M1);

    lcdM.setCursor(0,3);
    lcdM.print(libNot);
    lcdM.setCursor(4,3);
    lcdM.print(libNot);
    lcdM.setCursor(8,3);
    lcdM.print(libExec);
    lcdM.setCursor(13,3);
    lcdM.print(libNot);
    lcdM.setCursor(17,3);
    lcdM.print(libRet);


    if ( dMenu->List[curCmd].Max_1 - dMenu->List[curCmd].Min_1 > 0 )
    {        
      if ( dMenu->List[curCmd].List_1 )
        lcd_Liste(tmpVal,(char *)dMenu->List[curCmd].List_1,dMenu->List[curCmd].Val_1,6);
      else
        lcd_Num(tmpVal,dMenu->List[curCmd].Val_1,6);
      lcdM.setCursor(7,2);
      lcdM.print(tmpVal);      
    }
    else
    {
      if ( dMenu->List[curCmd].Max_1 == - 1) // Fichier....
      {
        if ( MySD.GetListe(dMenu->List[curCmd].Min_1, SDPath) > 0 )
        {
          lcd_Liste(tmpVal,MySD.ListeFiles,dMenu->List[curCmd].Val_1,8);
          lcdM.setCursor(7,2);
          lcdM.print(tmpVal);      
        }
      }
    }

    if ( dMenu->List[curCmd].Max_2 - dMenu->List[curCmd].Min_2 > 0 )
    {        
      if ( dMenu->List[curCmd].List_2 )
        lcd_Liste(tmpVal,(char *)dMenu->List[curCmd].List_1,dMenu->List[curCmd].Val_2,6);
      else
        lcd_Num(tmpVal,dMenu->List[curCmd].Val_2,6);
      lcdM.setCursor(14,2);
      lcdM.print(tmpVal);             
    }
  }
}

char *mMenu::GetFName(char *Destin)
{
  byte ct=0;
  lcd_Liste(tmpVal,MySD.ListeFiles,dMenu->List[curCmd].Val_1,8);
  while(tmpVal[ct]>32 && ct<8) ct++;
  tmpVal[ct]=0;
  strcpy(Destin,tmpVal);
  return Destin;
}

char *mMenu::MenuFName(char *Destin)
{
  byte ct=0;
  lcd_Liste(tmpVal,(char *)listStyleMenu,V1,6);
  while(tmpVal[ct]>32 && ct<6) ct++;
  tmpVal[ct]=0;
  strcat(tmpVal,"%02d");
  sprintf(Destin,tmpVal,V2);
#ifdef _DEBUG_MODE_
  Serial.println(tmpVal);   
#endif
  return Destin;
}

void mMenu::SetM1(int newValue)
{
  if (FirstPage)
  {
    if ( newValue != dMenu->Val_1 )
    {
      M1=dMenu->Val_1 = newValue;
      if ( dMenu->List_1 )
        lcd_Liste(tmpVal,(char *)dMenu->List_1,dMenu->Val_1,6);
      else
        lcd_Num(tmpVal,dMenu->Val_1,6);
      lcdM.setCursor(0,2);
      lcdM.print(tmpVal);    
    }
  }
}



boolean mMenu::ChangeM1(int value, byte Encoder)
{
  boolean ret = false;

  if (FirstPage)
  {
    int newValue = encnewmap(value, dMenu->Min_1, dMenu->Max_1, dMenu->Val_1,  Encoder ); 
    if ( newValue != dMenu->Val_1 )
    {
      M1=dMenu->Val_1 = newValue;
      if ( dMenu->List_1 )
        lcd_Liste(tmpVal,(char *)dMenu->List_1,dMenu->Val_1,6);
      else
        lcd_Num(tmpVal,dMenu->Val_1,6);
      lcdM.setCursor(0,2);
      lcdM.print(tmpVal);    
      ret = true;
    }
  }
  else
  {
    int newValue = encnewmap(value, 0, maxCmd-1 , curCmd,  Encoder );
    if ( newValue != curCmd )
    {
      curCmd = newValue;
      DessinPage();        
      V1=dMenu->List[curCmd].Val_1;
      V2=dMenu->List[curCmd].Val_2;

      ret = true;
    }
  }
  return ret;
}


boolean mMenu::ChangeM2(int value, byte Encoder)
{
  boolean ret = false;
  if (FirstPage)
  {
    int newValue = encnewmap(value, dMenu->Min_2, dMenu->Max_2, dMenu->Val_2,  Encoder );
    if ( newValue != dMenu->Val_2 )
    {
      M2=dMenu->Val_2 = newValue;
      if ( dMenu->List_2 )
        lcd_Liste(tmpVal,(char *)dMenu->List_2,dMenu->Val_2,6);
      else
        lcd_Num(tmpVal,dMenu->Val_2,6);
      lcdM.setCursor(7,2);
      lcdM.print(tmpVal);             
      ret = true;
    }
  }
  else
  {
    if ( dMenu->List[curCmd].Max_1 == - 1 && MySD.NbFiles > 0 )
    {
      int newValue = encnewmap(value, 0,  MySD.NbFiles-1,  dMenu->List[curCmd].Val_1,  Encoder );
      if ( newValue != dMenu->List[curCmd].Val_1 )
      {
        V1=dMenu->List[curCmd].Val_1 = newValue;
        lcd_Liste(tmpVal,MySD.ListeFiles,dMenu->List[curCmd].Val_1,8);
        lcdM.setCursor(7,2);
        lcdM.print(tmpVal);             
        ret = true;
      }
    }
    else
    {
      int newValue = encnewmap(value, dMenu->List[curCmd].Min_1, dMenu->List[curCmd].Max_1 ,  dMenu->List[curCmd].Val_1,  Encoder );
      if ( newValue != dMenu->List[curCmd].Val_1 )
      {
        V1=dMenu->List[curCmd].Val_1 = newValue;
        if ( dMenu->List[curCmd].List_1 )
          lcd_Liste(tmpVal,(char *)dMenu->List[curCmd].List_1,dMenu->List[curCmd].Val_1,6);
        else
          lcd_Num(tmpVal,dMenu->List[curCmd].Val_1,6);
        lcdM.setCursor(7,2);
        lcdM.print(tmpVal);             
        ret = true;
      }
    }
  }
  return ret;
}

boolean mMenu::ChangeM3(int value, byte Encoder)
{
  boolean ret = false;
  if (FirstPage)
  {
    int newValue = encnewmap(value, dMenu->Min_3, dMenu->Max_3, dMenu->Val_3,  Encoder );
    if ( newValue != dMenu->Val_3 )
    {
      M3 = dMenu->Val_3 = newValue;
      if ( dMenu->List_3 )
        lcd_Liste(tmpVal,(char *)dMenu->List_3,dMenu->Val_3,6);
      else
        lcd_Num(tmpVal,dMenu->Val_3,6);
      lcdM.setCursor(14,2);
      lcdM.print(tmpVal);             
      ret = true;
    }

    /*    
     #ifdef _DEBUG_MODE_ 
     Serial.print("mMenu::ChangeM3=");
     Serial.print(value);
     Serial.print(",");
     Serial.println(newValue);
     #endif
     */
  }
  else
  {
    int newValue = encnewmap(value, dMenu->List[curCmd].Min_2, dMenu->List[curCmd].Max_2,  dMenu->List[curCmd].Val_2,  Encoder );
    if ( newValue != dMenu->List[curCmd].Val_2 )
    {
      V2=dMenu->List[curCmd].Val_2 = newValue;
      if ( dMenu->List[curCmd].List_2 )
        lcd_Liste(tmpVal,(char *)dMenu->List[curCmd].List_2,dMenu->List[curCmd].Val_2,6);
      else
        lcd_Num(tmpVal,dMenu->List[curCmd].Val_2,6);
      lcdM.setCursor(14,2);
      lcdM.print(tmpVal);             
      ret = true;
    }
  }
  return ret;
}

void mMenu::ShowM3(int value)
{
  M3 = dMenu->Val_3 = value;
  if ( dMenu->List_3 )
    lcd_Liste(tmpVal,(char *)dMenu->List_3,dMenu->Val_3,6);
  else
    lcd_Num(tmpVal,dMenu->Val_3,5);
  lcdM.setCursor(14,2);
  lcdM.print(tmpVal);             
}

boolean mMenu::ChangeS1S5(int value)
{
  boolean ret = false;
  int newValue = btnnewmap(value, 0, 5 );

  if ( newValue != S1S5 )
  {
    S1S5 = newValue;
    ret = true;
    switch (S1S5 )
    {
    case 5:
      if (millis() > dblClickMillis )
      { 
        FirstPage = !FirstPage;
        DessinPage();               
        dblClickMillis=millis()+400;
      }   
      else
      {
        if ( ActiFunc != MODE_GLOBAL )
        {        
          FirstPage = true;
          LastActiFunc = ActiFunc;
          ChangeFunc(MODE_GLOBAL);
        }
        else
        {
          FirstPage = true;
          ChangeFunc(LastActiFunc);
        }
      }
      ret = false;
      break;
   case 4:
   case 3:
   case 2:
   case 1:
      if (millis() > dblClickMillis )
      {
          dblClickMillis=millis()+400;
          DBLCLK = false;
      }
      else
      {
          DBLCLK = true;
      }
      break;      
    }
  }
  return ret;
}


void lcd_Text(char *dst, char *src, byte lg)
{
  byte cpt=0;
  while ( src[cpt] && cpt < lg ) {
    dst[cpt]=src[cpt];
    cpt++;
  }
  while ( cpt < lg ) dst[cpt++]=32;
  dst[cpt++]=0;
}

void lcd_Num(char *dst, int value, byte lg)
{
  char tmp[10];
  sprintf(tmp,"%d",value);
  lcd_Text(dst, tmp, lg);
}

void lcd_NumOff(char *dst, int value, byte lg)
{
  char tmp[10];
  if ( value == 0 )
    strcpy(tmp,(const char*)"Off");
  else
    sprintf(tmp,"%d",value);
  lcd_Text(dst, tmp, lg);
}

extern float *cv_INT_HZ;

void lcd_HZ(char *dst, int value, byte lg)
{
  char tmp[20];
  if ( cv_INT_HZ[value] >= 10.0 )
    sprintf(tmp,"%.2fHz",cv_INT_HZ[value]);
  else
    sprintf(tmp,"%.3fHz",cv_INT_HZ[value]);
  lcd_Text(dst, tmp, lg);

}

void lcd_Note(char *dst, int value, byte lg)
{
  char tmp[10];
  if (value<0)
    sprintf(tmp,"Key%d",value);
  else if (value==0)
    sprintf(tmp,"Key pr",value);
  else if (value<24)
    sprintf(tmp,"Key+%d",value);
  else
    sprintf(tmp,nameNote[value%12],value/12);
  lcd_Text(dst, tmp, lg);
}

void lcd_NoteAbs(char *dst, int value, byte lg)
{
  char tmp[10];
  sprintf(tmp,nameNote[value%12],value/12);
  lcd_Text(dst, tmp, lg);
}

void lcd_Octa(char *dst, int value, byte lg)
{
  char tmp[10];
  if (value>0) 
    sprintf(tmp,"+%d oct",value);
  else if (value<0) 
    sprintf(tmp,"%d oct",value);
  else 
    sprintf(tmp," %d oct",value);
  lcd_Text(dst, tmp, lg);
}

void lcd_SNum(char *dst, int value, byte lg)
{
  char tmp[10];
  if (value>0) 
    sprintf(tmp,"+%d",value);
  else 
    sprintf(tmp,"%d",value);
  lcd_Text(dst, tmp, lg);
}



void lcd_Liste(char *dst, char *src, int index, byte lg)
{
  byte cpt=0;
  int  offset=index*lg;
  while ( cpt < lg ) {
    dst[cpt]=src[cpt+offset];
    cpt++;
  }
  dst[cpt++]=0;
}

void lcd_ListeWS(char *dst, char *src, int index, byte lg)
{
  byte cpt=0;
  int  offset=index*lg;
  while ( cpt < lg ) {
    dst[cpt]=src[cpt+offset];
    cpt++;
  }
  dst[cpt--]=0;
  while(dst[cpt]==32 && cpt>0) dst[cpt--]=0;
}





#ifdef _HARDWARE_1_

  #define MINI_CURS  6
  #define MAXI_CURS  1015

  int newmap(int value, int newminv, int newmaxv)
  {
    if ( value > MAXI_CURS ) value = MAXI_CURS;
    if ( value < MINI_CURS ) value =  MINI_CURS;
    int ndelta = newmaxv + 1 - newminv;
    int resul = (int)( ( (uint32_t)( value - MINI_CURS )  * ndelta ) / ( MAXI_CURS - MINI_CURS ) )  + newminv;
    if ( resul > newmaxv ) return newmaxv;
    if ( resul < newminv ) return newminv;
    return resul; 
  }

  int btnnewmap(int value, int newminv, int newmaxv )
  {
    value+=MAXI_CURS/(2*(newmaxv-newminv));
    if ( value > MAXI_CURS ) value = MAXI_CURS;
    if ( value < MINI_CURS ) value =  MINI_CURS;
    int ndelta = newmaxv  - newminv;
    int resul = (int)( ( (uint32_t)( value - MINI_CURS )  * ndelta ) / ( MAXI_CURS - MINI_CURS ) )  + newminv;
    if ( resul > newmaxv ) return newmaxv;
    if ( resul < newminv ) return newminv;
    return resul; 
  }

  int encnewmap(int value, int newminv, int newmaxv, int curval, byte Encoder)
  {
    int resul, ndelta;
    if ( Encoder == ENCODER_NONE )
    {
      if ( value > MAXI_CURS ) value = MAXI_CURS;
      if ( value < MINI_CURS ) value =  MINI_CURS;
      ndelta = newmaxv  + 1 - newminv;
      resul = (int)( ( (uint32_t)( value - MINI_CURS )  * ndelta ) / ( MAXI_CURS - MINI_CURS ) )  + newminv;
    }
    else
    {
      switch(Encoder)
      {
      case ENCODER_PUSH:
        if ( curval==max(newminv,0) ) return newmaxv;
        else if ( curval==newmaxv ) return newminv;
        else return max(newminv,0);
        break;
      case ENCODER_PLUS:
        resul = curval+1;
        break;
      case ENCODER_MOINS:
        resul = curval-1;
        break;
      }
    }
    if ( resul > newmaxv ) return newmaxv;
    if ( resul < newminv ) return newminv;
    return resul; 
  }
  

#endif


#ifdef _HARDWARE_2_

  #define MINI_CURS  1
  #define MAXI_CURS  1005
  
  int newmap(int value, int newminv, int newmaxv)
  {
    if ( value > MAXI_CURS ) value = MAXI_CURS;
    if ( value < MINI_CURS ) value =  MINI_CURS;
    int ndelta = newmaxv + 1 - newminv;
    int resul = (int)( ( (uint32_t)( value - MINI_CURS )  * ndelta ) / ( MAXI_CURS - MINI_CURS ) )  + newminv;
    if ( resul > newmaxv ) return newmaxv;
    if ( resul < newminv ) return newminv;
    return resul; 
  }
  
  int btnnewmap(int value, int newminv, int newmaxv )
  {
    int pas = MAXI_CURS / ( newmaxv - newminv + 1 );
    if ( value < pas / 2) return newminv;
    int resul =  (newmaxv-newminv+1 )-( value+pas/2 )/pas + newminv; 
    if ( resul > newmaxv ) return newmaxv;
    if ( resul < newminv ) return newminv;
    return resul; 
  }
  
  int encnewmap(int value, int newminv, int newmaxv, int curval, byte Encoder)
  {
    int resul, ndelta;
    if ( Encoder == ENCODER_NONE )
    {
      if ( value > MAXI_CURS ) value = MAXI_CURS;
      if ( value < MINI_CURS ) value =  MINI_CURS;
      ndelta = newmaxv  + 1 - newminv;
      resul = (int)( ( (uint32_t)( value - MINI_CURS )  * ndelta ) / ( MAXI_CURS - MINI_CURS ) )  + newminv;
    }
    else
    {
      switch(Encoder)
      {
      case ENCODER_PUSH:
        if ( curval==max(newminv,0) ) return newmaxv;
        else if ( curval==newmaxv ) return newminv;
        else return max(newminv,0);
        break;
      case ENCODER_PLUS:
        resul = curval-1;
        break;
      case ENCODER_MOINS:
        resul = curval+1;
        break;
      }
    }
    if ( resul > newmaxv ) return newmaxv;
    if ( resul < newminv ) return newminv;
    return resul; 
  }

  
#endif



void Z_TextValue(int cpt, char *tmpCh, char *tmpVal)
{
  switch( cpt  )
  {
  case 1: 
  case 2: 
  case 3: 
  case 4: 
  case 5:
    lcd1.setCursor((cpt-1) * 8+1,0);
    lcd1.print(tmpCh);             
    lcd1.setCursor((cpt-1) * 8+1,1);
    lcd1.print(tmpVal);             
    break; 

  case 6: 
  case 7: 
  case 8: 
  case 9: 
  case 10:
    lcd2.setCursor((cpt-6) * 8+1,0);
    lcd2.print(tmpCh);             
    lcd2.setCursor((cpt-6) * 8+1,1);
    lcd2.print(tmpVal);             
    break; 

  case 11: 
  case 12: 
  case 13: 
  case 14: 
  case 15:
    lcd3.setCursor((cpt-11) * 8+1,0);
    lcd3.print(tmpCh);             
    lcd3.setCursor((cpt-11) * 8+1,1);
    lcd3.print(tmpVal);             
    break; 

  case 16: 
  case 17: 
  case 18: 
  case 19: 
  case 20:
    lcd4.setCursor((cpt-16) * 8+1,0);
    lcd4.print(tmpCh);             
    lcd4.setCursor((cpt-16) * 8+1,1);
    lcd4.print(tmpVal);             
    break; 
  }
}

void Z_Titre(int cpt, char *tmpCh)
{
  switch( cpt  )
  {
  case 1:
    lcd1.setCursor(0,0);
    lcd1.print(tmpCh);             
    break; 
  case 11:
    lcd3.setCursor(0,0);
    lcd3.print(tmpCh);             
    break; 
  }
}

void Z_Text(int cpt, char *tmpCh)
{
  switch( cpt  )
  {
  case 1: 
  case 2: 
  case 3: 
  case 4: 
  case 5:
    lcd1.setCursor((cpt-1) * 8+1,0);
    lcd1.print(tmpCh);             
    break; 

  case 6: 
  case 7: 
  case 8: 
  case 9: 
  case 10:
    lcd2.setCursor((cpt-6) * 8+1,0);
    lcd2.print(tmpCh);             
    break; 

  case 11: 
  case 12: 
  case 13: 
  case 14: 
  case 15:
    lcd3.setCursor((cpt-11) * 8+1,0);
    lcd3.print(tmpCh);             
    break; 

  case 16: 
  case 17: 
  case 18: 
  case 19: 
  case 20:
    lcd4.setCursor((cpt-16) * 8+1,0);
    lcd4.print(tmpCh);             
    break; 
  }
}


void Z_Value(int cpt, char *tmpVal)
{
  switch( cpt  )
  {
  case 1: 
  case 2: 
  case 3: 
  case 4: 
  case 5:
    lcd1.setCursor((cpt-1) * 8+1,1);
    lcd1.print(tmpVal);             
    break; 

  case 6: 
  case 7: 
  case 8: 
  case 9: 
  case 10:
    lcd2.setCursor((cpt-6) * 8+1,1);
    lcd2.print(tmpVal);             
    break; 

  case 11: 
  case 12: 
  case 13: 
  case 14: 
  case 15:
    lcd3.setCursor((cpt-11) * 8+1,1);
    lcd3.print(tmpVal);             
    break; 

  case 16: 
  case 17: 
  case 18: 
  case 19: 
  case 20:
    lcd4.setCursor((cpt-16) * 8+1,1);
    lcd4.print(tmpVal);             
    break; 
  }
}


void Z_Clear()
{
  lcd1.clear();
  lcd2.clear();
  lcd3.clear();
  lcd4.clear();
  for (int ct=1;ct<5;ct++)
  {
    int x=ct*8;
    lcd1.Carac(x,0,6);
    lcd1.Carac(x,1,7);
    lcd2.Carac(x,0,6);
    lcd2.Carac(x,1,7);
    lcd3.Carac(x,0,6);
    lcd3.Carac(x,1,7);
    lcd4.Carac(x,0,6);
    lcd4.Carac(x,1,7);
  }
}


uint16_t EuclidToPat(byte *Destin, int total)
{
  uint16_t ret=0;
  uint16_t masq=1;
  if (total>16) total=16;
  for (int ct=0;ct<total;ct++)    
  {
    if ( Destin[ct] ) ret|=masq;
    masq<<=1;
  }
  return ret;
}

void EuclidGenerator(byte *Destin, int total, int hits)
{
  int cpt=0;

  if (hits >= total || total == 1 || hits == 0) 
  { 
    if (hits >= total) 
    {
      for (uint i = 0; i < total; i++) 
      { 
        Destin[cpt++] = 127;
      }
    } 
    else if (total == 1) 
    {
      if (hits == 1) 
      {
        Destin[cpt++] = 127;
      } 
      else 
      {
        Destin[cpt++] = 0;
      }
    } 
    else 
    {
      for (uint i = 0; i < total; i++) 
      {
        Destin[cpt++] = 0;
      }
    }
  } 
  else 
  { 
    //sane input
    int pauses = total - hits;
    if (pauses >= hits) 
    { 
      //first case more pauses than p
      int per_pulse = floor( (double)pauses / hits );
      int remainder = pauses % hits;
      for (uint i = 0; i < hits; i++) 
      {
        Destin[cpt++] = 127;
        for (uint j = 0; j < per_pulse; j++) 
        {
          Destin[cpt++] = 0;
        }
        if (i < remainder) 
        {
          Destin[cpt++] = 0;
        }
      }
    } 
    else 
    { 
      //second case more p than pauses
      int per_pause = floor( (double)(hits - pauses) / pauses );
      int remainder = (hits - pauses) % pauses;
      for (uint i = 0; i < pauses; i++) 
      {
        Destin[cpt++] = 127;
        Destin[cpt++] = 0;
        for (uint j = 0; j < per_pause; j++) 
        {
          Destin[cpt++] = 127;
        }
        if (i < remainder)
        {
          Destin[cpt++] = 127;
        }
      }
    }
  }
}



mPatLed::mPatLed()
{

}

mPatLed::~mPatLed()
{
}

void mPatLed::Setup()
{
  pinMode(CSH_latchPin,OUTPUT);  
  pinMode(CSH_clockPin,OUTPUT);    
  pinMode(CSH_dataPin,OUTPUT);  
  PatChanged=true;
  aPat=0;
  old_aPat=0xFFFFF;
  Traite(0);
}

void mPatLed::Traite(uint32_t reftime)
{
  if (PatChanged)
  {
    if (old_aPat != aPat)
    {
      digitalWrite(CSH_latchPin, LOW);

#ifdef _HARDWARE_1_
      shiftOut(CSH_dataPin, CSH_clockPin, MSBFIRST, aPat);  
      shiftOut(CSH_dataPin, CSH_clockPin, MSBFIRST, (aPat >> 8));  
#endif

      
#ifdef _HARDWARE_2_
      shiftOut(CSH_dataPin, CSH_clockPin, MSBFIRST, (aPat >> 8));  
      shiftOut(CSH_dataPin, CSH_clockPin, MSBFIRST, aPat);  
#endif

      digitalWrite(CSH_latchPin, HIGH);
      old_aPat = aPat;
    }
    PatChanged=false;
    aPat=0;
  }
  aPat=0;
}

void mPatLed::SetStep(int step)
{
  uint32_t value = 1 << (step-1) ;
  if ((aPat & value )==value)
    aPat  &= ~value;
  else
    aPat  |= value;
  PatChanged=true;
}

void mPatLed::SetPatt(uint16_t value)
{
  aPat  |= value;
  PatChanged=true;
}


int mPatLed::Map(int value)
{
  char tmpVal[20];
  int newvalue = btnnewmap( value , 0, 16 );

#ifdef _DEBUG_MODE_

  Serial.print("mPatLed::Map=");
  Serial.print(value);
  Serial.print(",");
  Serial.println(newvalue);

#endif

  //  if ( newvalue > 0 ) newvalue = 17 - newvalue;

  return newvalue;
}


void mPatLed::Show(int mode)
{
  switch (mode)
  {
  case 0:
    {  
      uint8_t v1=0x01;
      uint8_t v2=0x80;
      for (int ct=0;ct<8;ct++)
      {
        digitalWrite(CSH_latchPin, LOW);
        shiftOut(CSH_dataPin, CSH_clockPin, MSBFIRST, v1);  
        shiftOut(CSH_dataPin, CSH_clockPin, MSBFIRST, v2);  

#ifdef _HARDWARE_1_
        shiftOut(CSH_dataPin, CSH_clockPin, MSBFIRST, v1);  
        shiftOut(CSH_dataPin, CSH_clockPin, MSBFIRST, v2);  
#endif

      
#ifdef _HARDWARE_2_
        shiftOut(CSH_dataPin, CSH_clockPin, MSBFIRST, v2);  
        shiftOut(CSH_dataPin, CSH_clockPin, MSBFIRST, v1);  
#endif

        digitalWrite(CSH_latchPin, HIGH);
        delay(20);
        v1<<=1;
        v2>>=1;
      }
    }
    break;
  case 1:
    {
       uint16_t v1;
       for (int ct=0;ct<16;ct++)
      {
        digitalWrite(CSH_latchPin, LOW);
#ifdef _HARDWARE_1_
        shiftOut(CSH_dataPin, CSH_clockPin, MSBFIRST, v1 );  
        shiftOut(CSH_dataPin, CSH_clockPin, MSBFIRST, (v1 >> 8));  
#endif

      
#ifdef _HARDWARE_2_
        shiftOut(CSH_dataPin, CSH_clockPin, MSBFIRST, (v1 >> 8));  
        shiftOut(CSH_dataPin, CSH_clockPin, MSBFIRST, v1 );  
#endif
        digitalWrite(CSH_latchPin, HIGH);
        delay(20);
        v1<<=1;
      }
    }
    break;
  }
  digitalWrite(CSH_latchPin, LOW);
  shiftOut(CSH_dataPin, CSH_clockPin, MSBFIRST, 0);  
  shiftOut(CSH_dataPin, CSH_clockPin, MSBFIRST, 0);  
  digitalWrite(CSH_latchPin, HIGH);

}

extern const int32_t HzVolt[];

uint32_t NoteToCv(byte note, byte mode)
{
  uint32_t cv;
  if ( CV_Type[mode-CV_1] == 0 )
    cv =  (int32_t)400 * (note-24) / 12 + ParamGlo.FINE_CV[mode-CV_1];
  else
    cv =  HzVolt[note] + ParamGlo.FINE_CV[mode-CV_1];
  if (cv > 4095) return 0;
  return cv;
}

byte TrigToTrig(byte note, byte mode)
{
  byte trig;
  if ( TRIG_Type[mode-TRIG_1] == 0 )
    trig =  note;
  else
    trig = (note==LOW)?HIGH:LOW;
  return trig;
}




