// -----------------------------------------------
// LL113
// -----------------------------------------------
// Historique:
// (Baloran) laurent.lecatelier@free.fr, version 0.0.1  10/02/12013
// -----------------------------------------------
// utilld.h DÃƒÂ©finition Class Menu, Leds et fonctions diverses


#ifndef _UTILLD_H_
#define _UTILLD_H_

// #define _DEBUG_MODE_ 1

#define _HARDWARE_1_ 1

// #define _HARDWARE_2_ 1



#define LENGTH_VALUE 7



#include <LiquidCrystal.h>
#include <MIDIMUL.h>
#include "Arduino.h"

#define MAX_FUNCT 5

#define MODE_GLOBAL     0
#define MODE_SEQEUCLID  1
#define MODE_SEQSTEP    2
#define MODE_SEQTR      3
#define MODE_ARPEGE     4
#define MODE_LFO        5
#define MODE_VCO        6
#define MODE_EDITEUR    6


#define MODE_PARAM      64


#define TEMPO_INIT 100
#define TEMPO_MIN 30
#define TEMPO_MAX 300

#define  CalcNb24(a) 24/a

extern byte ActiFunc;
extern byte LastActiFunc;

extern const char *listStyleMenu;

#define NB_STYLEMU 103

extern LiquidCrystal lcdM;
extern LiquidCrystal lcd1;
extern LiquidCrystal lcd2;
extern LiquidCrystal lcd3;
extern LiquidCrystal lcd4;

//Pin connected to ST_CP of 74HC595
#define CSH_latchPin 25
#define CSH_clockPin 27
#define CSH_dataPin  23

#define ENCODER_NONE  0
#define ENCODER_PUSH  1
#define ENCODER_PLUS  2
#define ENCODER_MOINS 3

#define CHGT_PAGE 255
#define CHGT_M1   254
#define CHGT_M2   253
#define CHGT_M3   252

#define ENCODER_A 42
#define ENCODER_B 43
#define ENCODER_P 44


#define TRIGIN_0 41
#define DYNCLK_IN 51
#define DYNSTARTSTOP_IN 53
#define DYNCLK_OUT 39
#define DYNSTARTSTOP_OUT 40
#define JACKCLK_OUT 29


#define CV_1 0
#define CV_2 1
#define CV_3 2
#define CV_4 3
#define CV_5 4
#define CV_6 5
#define CV_7 6
#define CV_8 7


#define TRIG_1 31
#define TRIG_2 32
#define TRIG_3 33
#define TRIG_4 34
#define TRIG_5 35
#define TRIG_6 36
#define TRIG_7 37
#define TRIG_8 38


// Pin CDS pour le Lecteur SD
#define SD_CSPin  52

#define NONE 0
#define DOWN2UP 1
#define UP2DOWN 2


struct strGlobalParam 
{        
   byte Active[MAX_FUNCT];
   int  Tempo;  
   int  Groove;  
   
   byte keyInput;
   byte canInput;
   byte Split;
   byte NoteSplit;
   byte UpperDest;
   int  UpperTrans;
   byte LowerDest;
   int  LowerTrans;
   byte LowerChan;
   byte CVTrig;
   byte ArpSrc;

   MIDIMul_Class *MidiInput;

   byte     ClockSrc;
   byte     ClockDiv;
   byte     TrigInDir;

   byte EuSrc;
   byte StSrc;
   byte TrSrc;

   byte LastNote; 
   byte LastUpperNote; 
   byte LastLowerNote;
  
   byte Bend;  
   
   int16_t FINE_CV[8];
   
   byte     bfill[4];
   uint32_t lfill[10];
};

extern strGlobalParam ParamGlo;


struct AccordNote {        
  byte Note[6];          
};


struct paramMenuList {        
  char *M1;          
  char *M2;          
  char *M3;          
  int     Val_1;      
  int     Min_1;      
  int     Max_1;
  const char*   List_1;          
  int     Val_2;      
  int     Min_2;      
  int     Max_2;
  char*   List_2;          
};


struct paramMenu {  
  char *S1;
  char *S2;
  char *S3;
  char *S4;
  char *M1;          
  char *M2;          
  char *M3;          
  int     Val_1;      
  int     Min_1;      
  int     Max_1;
  const char*   List_1;          
  int     Val_2;      
  int     Min_2;      
  int     Max_2;
  const char*   List_2;   
  int     Val_3;      
  int     Min_3;      
  int     Max_3;
  const char*   List_3;   
  paramMenuList *List;
};


class mMenu {

public:
  mMenu(paramMenu*, const char *);
  ~mMenu();

  void    SetM1(int);
  boolean ChangeM1(int, byte);
  boolean ChangeM2(int, byte);
  boolean ChangeM3(int, byte);
  boolean ChangeS1S5(int);
  void ShowM3(int);

  void    SetRealPath(char *path);
 void     ChangeTitre(char *p_Titre);

  char *MenuFName(char *Destin);
  char *GetFName(char *Destin);

  void DessinPage();
  int  curCmd;
  int  maxCmd;
  int  S1S5;
  int  M1;
  int  M2;
  int  M3;
  int  V1;
  int  V2;
  boolean FirstPage;
  boolean DBLCLK;
protected:
  paramMenu *dMenu;
  char *dTitre;
  char tmpCh[16]; 
  char tmpVal[16]; 
  uint32_t  dblClickMillis; 
  char *SDPath;
};


class mPatLed {

public:
  mPatLed();
  ~mPatLed();

  void Setup();
  void Traite(uint32_t reftime);
  void SetStep(int step);
  void SetPatt(uint16_t value);
  int Map(int value);
  void Show(int);

protected:
  uint32_t aPat;
  uint32_t old_aPat;
  boolean PatChanged;
};

void lcd_Note(char *dst, int index, byte lg=LENGTH_VALUE);
void lcd_Octa(char *dst, int index, byte lg=LENGTH_VALUE);
void lcd_SNum(char *, int, byte lg=LENGTH_VALUE);
void lcd_NoteAbs(char *dst, int index, byte lg=LENGTH_VALUE);
void lcd_HZ(char *, int, byte lg=LENGTH_VALUE);
void lcd_Text(char *, char *, byte lg=LENGTH_VALUE);
void lcd_Num(char *, int, byte lg=LENGTH_VALUE);
void lcd_NumOff(char *, int, byte lg=LENGTH_VALUE);
void lcd_Liste(char *dst, char *src, int index, byte lg=LENGTH_VALUE);
void lcd_ListeWS(char *dst, char *src, int index, byte lg);

int newmap(int value, int newminv, int newmaxv);
int btnnewmap(int value, int newminv, int newmaxv);
int encnewmap(int value, int newminv, int newmaxv, int curval, byte Encoder);

void Z_Text(int cpt, char *tmpCh);
void Z_Titre(int cpt, char *tmpCh);
void Z_Value(int cpt, char *tmpVal);
void Z_TextValue(int cpt, char *tmpCh, char *tmpVal);
void Z_Clear();
void EuclidGenerator(byte *Destin, int total, int hit);
uint16_t EuclidToPat(byte *Destin, int total);


void write_global(char *FileName);
void read_global(char *FileName);
void init_global();

void ChangeFunc(int);

extern byte CV_Type[];
extern byte TRIG_Type[];

uint32_t NoteToCv(byte note, byte mode);
byte TrigToTrig(byte note, byte mode);

#endif _UTILLD_H_




