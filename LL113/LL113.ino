// -----------------------------------------------
// LL113
// -----------------------------------------------
// Historique:
// (Baloran) laurent.lecatelier@free.fr, version 0.0.1  10/02/12013
// -----------------------------------------------
// LL113.INO, boucle Arduino 

// Attention version Custo de LiquidCrystal
#include <LiquidCrystal.h>

// Attention version Custo de MIDIMul

// !!!! Dans RingBuffer.h j'ai augmentÃƒÂ© le SERIAL BUFFER 
/// #define SERIAL_BUFFER_SIZE 256

#include <MIDIMul.h>

#include <SPI.h>
#include <SD.h>

#include "multiplexAD.h"
#include "utilld.h"
#include "arpege.h"
#include "seqeuclid.h"
#include "seqstep.h"
#include "seqtr.h"
#include "SDFile.h"
#include "lfo.h"
#include "global.h"
#include "malloc.h"
#include "clock.h"
#include "mcp.h"
#include "EdSN.h"

// ------------------------
// Instances de classes
// ------------------------

// Ecrans LCD
LiquidCrystal lcdM(6, 9, 5, 4, 3, 2);
LiquidCrystal lcd1(6, 8, 5, 4, 3, 2);
LiquidCrystal lcd2(6, 11, 5, 4, 3, 2);
LiquidCrystal lcd3(6, 7, 5, 4, 3, 2);
LiquidCrystal lcd4(6, 10, 5, 4, 3, 2);



// Multiplex AD
multiplexAD mxAD;

// Translateur de n° de ligne analogique 
#ifdef _HARDWARE_1_
  byte hard_scan[NB_AD] = { 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33} ;
#endif

#ifdef _HARDWARE_2_
  byte hard_scan[NB_AD] = { 0,1,2,3,4,5,6,7,8,9,10,27,28,29,14,15,26,16,17,18,19,20,21,22,23,24,25,11,12,13,30,31,32,33} ;
#endif


int nbEditor=0;
edSN *Editeur[8];

/*
// MSR2
edSN Synth_1(MODE_EDITEUR_1);

// PRO100
edSN Synth_2(MODE_EDITEUR_2);
*/


// Arpege
Arpege Arp(MODE_ARPEGE);

// Sequenceur Euclidien
SeqEuclid SeqEu(MODE_SEQEUCLID);

// Sequenceur Step
SeqStep SeqSt(MODE_SEQSTEP);

// Sequenceur Step
SeqTrxx SeqTr(MODE_SEQTR);

// LFO
LFo LFoBi(MODE_LFO);

// CVCO
// VCO VCoBi(MODE_VCO);

// Carte SD
SDFile MySD;

// Mode GLobal
Global Glo;

// ParamÃƒÂ¨tres globaux
strGlobalParam ParamGlo;

MCP S4822;

// EntrÃƒÂ©es
InPut SPInp;

// Sortie
SuperOut SPOut;

// Clock
SysClock SClock;


// Interface Midi

MIDIMul_Class MIDI1;
MIDIMul_Class MIDI2;
MIDIMul_Class MIDI3;

uint32_t prob_cpt=0;
uint32_t resetSize=0;

// ------------------------
// Variables
// ------------------------

// PatLed
mPatLed PatLed;

// Table conversion int en HZ
float *cv_INT_HZ;

// Fonction active 
byte ActiFunc;
byte LastActiFunc;

// Redefinition caractÃƒÂ¨res LCD
byte barv[] = {
  B00110,
  B00110,
  B00110,
  B00110,
  B00110,
  B00110,
  B00110,
};

byte fleched[] = {
  B01000,
  B01100,
  B01110,
  B01111,
  B01110,
  B01100,
  B01000,
};

byte flecheg[] = {
  B00010,
  B00110,
  B01110,
  B11110,
  B01110,
  B00110,
  B00010,
};

byte descend[] = {
  B00000,
  B10000,
  B01000,
  B00100,
  B00010,
  B00001,
  B00000,
};

byte palier[] = {
  B11111,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
};
/*
byte b1[] = {
 B00000,
 B00000,
 B00000,
 B01111,
 B01000,
 B01000,
 B01000,
 };
 
 byte b2[] = {
 B01000,
 B01000,
 B01000,
 B01111,
 B00000,
 B00000,
 B00000,
 };
 */

byte b1[] = {
  B00110,
  B00110,
  B00110,
  B00110,
  B00110,
  B00110,
  B00110,
};

byte b2[] = {
  B00110,
  B00110,
  B00110,
  B00110,
  B00110,
  B00110,
  B00110,
};

char tmpGen[20];

// ------------------------
// Fonctions globales
// ------------------------

extern char _end;
extern "C" char *sbrk(int i);
char *ramstart=(char *)0x20070000;
char *ramend=(char *)0x20088000;

static void freeRAM(void) 
{
  char mes[100];
  int total=0;
  int count=0;
  
  int r=random(1,100)*8;
  char *memoryalloc=(char *)malloc(r);
  char *memoryleak =(char *)malloc(r);
  free(memoryalloc);
  total+=r;
  count+=1;
  sprintf(mes,"\nI have malloced %d\n",total);
  Serial.print(mes);
  
  sprintf(mes,"total+8*count = %d\n",total+8*count);
  Serial.print(mes);

  sprintf(mes,"\nmalloc_stats():\n");
  Serial.print(mes);
  malloc_stats();  
  
  struct mallinfo mi=mallinfo();

  sprintf(mes,"\nmallinfo():\n");  
  Serial.print(mes);
  sprintf(mes,"    arena=%d\n",mi.arena);
  Serial.print(mes);
  sprintf(mes,"  ordblks=%d\n",mi.ordblks);
  Serial.print(mes);
  sprintf(mes," uordblks=%d\n",mi.uordblks);
  Serial.print(mes);
  sprintf(mes," fordblks=%d\n",mi.fordblks);
  Serial.print(mes);
  sprintf(mes," keepcost=%d\n",mi.keepcost);
  Serial.print(mes);
  
  char *heapend=sbrk(0);
  register char * stack_ptr asm ("sp");
//char *stack_ptr=(char *)alloca(0); // also works

  sprintf(mes,"\nram start %lx\n", ramstart);
  Serial.print(mes);
  sprintf(mes,"data/bss end %lx\n", &_end);   
  Serial.print(mes);
  sprintf(mes,"heap end %lx\n", heapend); 
  Serial.print(mes);
  sprintf(mes,"stack ptr %lx\n",stack_ptr);  
  Serial.print(mes);
  sprintf(mes,"ram end %lx\n", ramend);
  Serial.print(mes);
  
  sprintf(mes,"\nDynamic ram used: %d\n",mi.uordblks);
  Serial.print(mes);
  sprintf(mes,"Program static ram used %d\n",&_end - ramstart); 
  Serial.print(mes);
  sprintf(mes,"Stack ram used %d\n\n",ramend - stack_ptr); 
  Serial.print(mes);
  sprintf(mes,"My guess at free mem: %d\n",stack_ptr - heapend + mi.fordblks); 
  Serial.print(mes);
}


void InitTables()
{
  cv_INT_HZ = (float*)malloc(200*sizeof(float));
  cv_INT_HZ[0] = 0.0625;
  for (int ct=1;ct<200;ct++)
  {
    cv_INT_HZ[ct]=cv_INT_HZ[ct-1]*1.035264924 ; /* =EXP(LN(2)/20) */
  }

}


void init_global()
{
    MySD.Setup();
    Glo.Init();
    SeqEu.Init(); 
    SeqSt.Init(); 
    SeqTr.Init(); 
    Arp.Init();
    LFoBi.Init();
//    VCoBi.Init();
    S4822.Setup();
    SPOut.Setup();
    SPInp.Setup();
}

void read_global(char * FileName)
{
  if ( MySD.OpenFileForRead((byte)MODE_GLOBAL, FileName))
  {
    Glo.Restore();
    SeqEu.Restore(); 
    SeqSt.Restore(); 
    SeqTr.Restore(); 
    Arp.Restore();
    LFoBi.Restore();
//    VCoBi.Restore();
    MySD.CloseFile();
  }   
}


void write_global(char * FileName)
{
  if ( MySD.OpenFileForWrite((byte)MODE_GLOBAL, FileName ))
  {
    Glo.Backup();
    SeqEu.Backup(); 
    SeqSt.Backup(); 
    SeqTr.Backup(); 
    Arp.Backup();
    LFoBi.Backup();
//    VCoBi.Backup();
    MySD.CloseFile();
  }   
}


void def_global()
{
  if ( MySD.OpenFileForRead(MODE_PARAM, DefSdName ) )
  {
    Glo.Restore();
    MySD.CloseFile();
  }
  if ( MySD.OpenFileForRead(MODE_ARPEGE, DefSdName ) )
  {
    Arp.Restore();
    MySD.CloseFile();
  }
  if ( MySD.OpenFileForRead(MODE_SEQEUCLID, DefSdName ) )
  {
    SeqEu.Restore();
    MySD.CloseFile();
  }
  if ( MySD.OpenFileForRead(MODE_SEQTR, DefSdName ) )
  {
    SeqTr.Restore();
    MySD.CloseFile();
  }
  if ( MySD.OpenFileForRead(MODE_SEQSTEP, DefSdName ) )
  {
    SeqSt.Restore();
    MySD.CloseFile();
  }
  if ( MySD.OpenFileForRead(MODE_LFO, DefSdName ) )
  {
    LFoBi.Restore();
    MySD.CloseFile();
  }
}

// ------------------
// Handle Midi 
// ------------------
void S1HandleNoteOn(byte channel, byte pitch, byte velocity) 
{ 
    if (velocity==0)
    {
      S1HandleNoteOff(channel, pitch, velocity) ;
      return;
    }
    SeqEu.HandleNoteOn(1,channel, pitch, velocity);
    SeqSt.HandleNoteOn(1,channel, pitch, velocity);
    SeqTr.HandleNoteOn(1,channel, pitch, velocity);
    Arp.HandleNoteOn(1,channel, pitch, velocity);
    Glo.HandleNoteOn(1,channel, pitch, velocity);
    LFoBi.HandleNoteOn(1,channel, pitch, velocity);
    for (int ed=0;ed<nbEditor;ed++) Editeur[ed]->HandleNoteOn(1,channel, pitch, velocity);
}


void S1HandleNoteOff(byte channel, byte pitch, byte velocity) 
{ 
    SeqEu.HandleNoteOff(1,channel, pitch, velocity);
    SeqSt.HandleNoteOff(1,channel, pitch, velocity);
    SeqTr.HandleNoteOff(1,channel, pitch, velocity);
    Arp.HandleNoteOff(1,channel, pitch, velocity);
    Glo.HandleNoteOff(1,channel, pitch, velocity);
    LFoBi.HandleNoteOff(1,channel, pitch, velocity);
    for (int ed=0;ed<nbEditor;ed++) Editeur[ed]->HandleNoteOff(1,channel, pitch, velocity);
}



void S1HandleControlChange(byte channel, byte number, byte value) 
{ 
    SeqEu.HandleControlChange(1,channel, number, value);
    SeqSt.HandleControlChange(1,channel, number, value);
    SeqTr.HandleControlChange(1,channel, number, value);
    Arp.HandleControlChange(1,channel, number, value);
    Glo.HandleControlChange(1,channel, number, value);
    for (int ed=0;ed<nbEditor;ed++) Editeur[ed]->HandleControlChange(1,channel, number, value);
}

void S1HandleProgramChange(byte channel, byte number) 
{ 
    SeqEu.HandleProgramChange(1,channel, number);
    SeqSt.HandleProgramChange(1,channel, number);
    SeqTr.HandleProgramChange(1,channel, number);
    Arp.HandleProgramChange(1,channel, number);
    Glo.HandleProgramChange(1,channel, number);
    for (int ed=0;ed<nbEditor;ed++) Editeur[ed]->HandleProgramChange(1,channel, number);
}

void S1HandlePitchBend(byte channel, int bend)
{
    SeqEu.HandlePitchBend(1,channel, bend);
    SeqSt.HandlePitchBend(1,channel, bend);
    SeqTr.HandlePitchBend(1,channel, bend);
    Arp.HandlePitchBend(1,channel, bend);
    Glo.HandlePitchBend(1,channel, bend);
    for (int ed=0;ed<nbEditor;ed++) Editeur[ed]->HandlePitchBend(1,channel, bend);
}

void S1HandleAfterTouch(byte channel, byte value)
{
    SeqEu.HandleAfterTouch(1,channel, value);
    SeqSt.HandleAfterTouch(1,channel, value);
    SeqTr.HandleAfterTouch(1,channel, value);
    Arp.HandleAfterTouch(1,channel, value);
    Glo.HandleAfterTouch(1,channel, value);
    for (int ed=0;ed<nbEditor;ed++) Editeur[ed]->HandleAfterTouch(1,channel, value);
}

void S1HandleClock() 
{ 
  SClock.HandleClock(1);
}

void S1HandleStart() 
{ 
  SClock.HandleStart(1);
}

void S1HandleContinue() 
{ 
  SClock.HandleContinue(1);
}

void S1HandleStop() 
{ 
  SClock.HandleStop(1);
}




void S2HandleNoteOn(byte channel, byte pitch, byte velocity) 
{ 
    if (velocity==0)
    {
      S2HandleNoteOff(channel, pitch, velocity) ;
      return;
    }
    SeqEu.HandleNoteOn(2,channel, pitch, velocity);
    SeqSt.HandleNoteOn(2,channel, pitch, velocity);
    SeqTr.HandleNoteOn(2,channel, pitch, velocity);
    Arp.HandleNoteOn(2,channel, pitch, velocity);
    Glo.HandleNoteOn(2,channel, pitch, velocity);
    LFoBi.HandleNoteOn(2,channel, pitch, velocity);
    for (int ed=0;ed<nbEditor;ed++) Editeur[ed]->HandleNoteOn(2,channel, pitch, velocity);
}


void S2HandleNoteOff(byte channel, byte pitch, byte velocity) 
{ 
    SeqEu.HandleNoteOff(2,channel, pitch, velocity);
    SeqSt.HandleNoteOff(2,channel, pitch, velocity);
    SeqTr.HandleNoteOff(2,channel, pitch, velocity);
    Arp.HandleNoteOff(2,channel, pitch, velocity);
    Glo.HandleNoteOff(2,channel, pitch, velocity);
    LFoBi.HandleNoteOff(2,channel, pitch, velocity);
    for (int ed=0;ed<nbEditor;ed++) Editeur[ed]->HandleNoteOff(2,channel, pitch, velocity);
}



void S2HandleControlChange(byte channel, byte number, byte value) 
{ 
    SeqEu.HandleControlChange(2,channel, number, value);
    SeqSt.HandleControlChange(2,channel, number, value);
    SeqTr.HandleControlChange(2,channel, number, value);
    Arp.HandleControlChange(2,channel, number, value);
    Glo.HandleControlChange(2,channel, number, value);
    for (int ed=0;ed<nbEditor;ed++) Editeur[ed]->HandleControlChange(2,channel, number, value);
}

void S2HandleProgramChange(byte channel, byte number) 
{ 
    SeqEu.HandleProgramChange(2,channel, number);
    SeqSt.HandleProgramChange(2,channel, number);
    SeqTr.HandleProgramChange(2,channel, number);
    Arp.HandleProgramChange(2,channel, number);
    Glo.HandleProgramChange(2,channel, number);
    for (int ed=0;ed<nbEditor;ed++) Editeur[ed]->HandleProgramChange(2,channel, number);
}

void S2HandlePitchBend(byte channel, int bend)
{
    SeqEu.HandlePitchBend(2,channel, bend);
    SeqSt.HandlePitchBend(2,channel, bend);
    SeqTr.HandlePitchBend(2,channel, bend);
    Arp.HandlePitchBend(2,channel, bend);
    Glo.HandlePitchBend(2,channel, bend);
    for (int ed=0;ed<nbEditor;ed++) Editeur[ed]->HandlePitchBend(2,channel, bend);
}

void S2HandleAfterTouch(byte channel, byte value)
{
    SeqEu.HandleAfterTouch(2,channel, value);
    SeqSt.HandleAfterTouch(2,channel, value);
    SeqTr.HandleAfterTouch(2,channel, value);
    Arp.HandleAfterTouch(2,channel, value);
    Glo.HandleAfterTouch(2,channel, value);
    for (int ed=0;ed<nbEditor;ed++) Editeur[ed]->HandleAfterTouch(2,channel, value);
}

void S2HandleClock() 
{ 
  SClock.HandleClock(2);
}

void S2HandleStart() 
{ 
  SClock.HandleStart(2);
}

void S2HandleContinue() 
{ 
  SClock.HandleContinue(2);
}

void S2HandleStop() 
{ 
  SClock.HandleStop(2);
}

void S3HandleNoteOn(byte channel, byte pitch, byte velocity) 
{ 
    if (velocity==0)
    {
      S3HandleNoteOff(channel, pitch, velocity) ;
      return;
    }
    SeqEu.HandleNoteOn(3,channel, pitch, velocity);
    SeqSt.HandleNoteOn(3,channel, pitch, velocity);
    SeqTr.HandleNoteOn(3,channel, pitch, velocity);
    Arp.HandleNoteOn(3,channel, pitch, velocity);
    Glo.HandleNoteOn(3,channel, pitch, velocity);
    LFoBi.HandleNoteOn(3,channel, pitch, velocity);
    for (int ed=0;ed<nbEditor;ed++) Editeur[ed]->HandleNoteOn(2,channel, pitch, velocity);
}


void S3HandleNoteOff(byte channel, byte pitch, byte velocity) 
{ 
    SeqEu.HandleNoteOff(3,channel, pitch, velocity);
    SeqSt.HandleNoteOff(3,channel, pitch, velocity);
    SeqTr.HandleNoteOff(3,channel, pitch, velocity);
    Arp.HandleNoteOff(3,channel, pitch, velocity);
    Glo.HandleNoteOff(3,channel, pitch, velocity);
    LFoBi.HandleNoteOff(3,channel, pitch, velocity);
    for (int ed=0;ed<nbEditor;ed++) Editeur[ed]->HandleNoteOff(3,channel, pitch, velocity);
}



void S3HandleControlChange(byte channel, byte number, byte value) 
{ 
    SeqEu.HandleControlChange(3,channel, number, value);
    SeqSt.HandleControlChange(3,channel, number, value);
    SeqTr.HandleControlChange(3,channel, number, value);
    Arp.HandleControlChange(3,channel, number, value);
    Glo.HandleControlChange(3,channel, number, value);
    for (int ed=0;ed<nbEditor;ed++) Editeur[ed]->HandleControlChange(3,channel, number, value);
}

void S3HandleProgramChange(byte channel, byte number) 
{ 
    SeqEu.HandleProgramChange(3,channel, number);
    SeqSt.HandleProgramChange(3,channel, number);
    SeqTr.HandleProgramChange(3,channel, number);
    Arp.HandleProgramChange(3,channel, number);
    Glo.HandleProgramChange(3,channel, number);
    for (int ed=0;ed<nbEditor;ed++) Editeur[ed]->HandleProgramChange(3,channel, number);
}
void S3HandlePitchBend(byte channel, int bend)
{
    SeqEu.HandlePitchBend(3,channel, bend);
    SeqSt.HandlePitchBend(3,channel, bend);
    SeqTr.HandlePitchBend(3,channel, bend);
    Arp.HandlePitchBend(3,channel, bend);
    Glo.HandlePitchBend(3,channel, bend);
    for (int ed=0;ed<nbEditor;ed++) Editeur[ed]->HandlePitchBend(3,channel, bend);
}

void S3HandleAfterTouch(byte channel, byte value)
{
    SeqEu.HandleAfterTouch(3,channel, value);
    SeqSt.HandleAfterTouch(3,channel, value);
    SeqTr.HandleAfterTouch(3,channel, value);
    Arp.HandleAfterTouch(3,channel, value);
    Glo.HandleAfterTouch(3,channel, value);
    for (int ed=0;ed<nbEditor;ed++) Editeur[ed]->HandleAfterTouch(3,channel, value);
}

void S3HandleClock() 
{ 
  SClock.HandleClock(3);
}

void S3HandleStart() 
{ 
  SClock.HandleStart(3);
}

void S3HandleContinue() 
{ 
  SClock.HandleContinue(3);
}

void S3HandleStop() 
{ 
  SClock.HandleStop(3);
}



// ------------------
// Setup Arduino
// ------------------

void setup()
{
  byte clr;
  
 
  // Adresses Encoder
  pinMode(ENCODER_A, INPUT);
  pinMode(ENCODER_B, INPUT);
  pinMode(ENCODER_P, INPUT);

  // Adresses 4067
  pinMode(22,OUTPUT);  
  pinMode(24,OUTPUT);  
  pinMode(26,OUTPUT);  
  pinMode(28,OUTPUT);  

  // InutilisÃƒÂ©es pour le moment
  pinMode(30,OUTPUT);  
  pinMode(32,OUTPUT);    


  // GATES
  pinMode(TRIG_1,OUTPUT);    
  pinMode(TRIG_2,OUTPUT);  
  pinMode(TRIG_3,OUTPUT);    
  pinMode(TRIG_4,OUTPUT);    
  pinMode(TRIG_5,OUTPUT);    
  pinMode(TRIG_6,OUTPUT);    
  pinMode(TRIG_7,OUTPUT);    
  pinMode(TRIG_8,OUTPUT);    

  // pinMode(PROB_TEST,OUTPUT);    

  pinMode(TRIGIN_0,INPUT);    
  pinMode(DYNCLK_IN,INPUT);    
  pinMode(DYNSTARTSTOP_IN,INPUT);    
  pinMode(DYNCLK_OUT,OUTPUT);    
  pinMode(DYNSTARTSTOP_OUT,OUTPUT);    
  pinMode(JACKCLK_OUT,OUTPUT);    


//  Le DUE ne gÃƒÂ¨re pas AnalogReference grrrr...
//  voir C:\arduino-1.5.1r2\hardware\arduino\sam\cores\arduino\wiring_analog.h l'enum de eAnalogReference
//  analogReference(EXTERNAL);


  // Mode 12 Bits
  analogWriteResolution(12);

  // Mode 10------------------------------------+ Bits
  analogReadResolution(10);

  //CrÃƒÂ©ation de tables de travail
  InitTables();


  /*
    MIDI1.begin(&Serial1);    
   MIDI2.begin(&Serial2);    
   MIDI3.begin(&Serial3);    
   */

  lcdM.begin(20, 4);


  lcd1.begin(40, 2);
  lcd2.begin(40, 2);
  lcd3.begin(40, 2);
  lcd4.begin(40, 2);

  lcd1.createChar(1, flecheg);
  lcd1.createChar(2, fleched);
  lcd1.createChar(3, barv);
  lcd1.createChar(4, descend);
  lcd1.createChar(5, palier);
  lcd1.createChar(6, b1);
  lcd1.createChar(7, b2);

  lcd2.createChar(1, flecheg);
  lcd2.createChar(2, fleched);
  lcd2.createChar(3, barv);
  lcd2.createChar(4, descend);
  lcd2.createChar(5, palier);
  lcd2.createChar(6, b1);
  lcd2.createChar(7, b2);

  lcd3.createChar(1, flecheg);
  lcd3.createChar(2, fleched);
  lcd3.createChar(3, barv);
  lcd3.createChar(4, descend);
  lcd3.createChar(5, palier);
  lcd3.createChar(6, b1);
  lcd3.createChar(7, b2);

  lcd4.createChar(1, flecheg);
  lcd4.createChar(2, fleched);
  lcd4.createChar(3, barv);
  lcd4.createChar(4, descend);
  lcd4.createChar(5, palier);
  lcd4.createChar(6, b1);
  lcd4.createChar(7, b2);

  lcdM.createChar(1, flecheg);
  lcdM.createChar(2, fleched);
  lcdM.createChar(3, barv);
  lcdM.createChar(4, descend);
  lcdM.createChar(5, palier);
  lcdM.createChar(6, b1);
  lcdM.createChar(7, b2);

// Initialisation Midi


  MIDI1.setHandleNoteOn(S1HandleNoteOn);  
  MIDI1.setHandleNoteOff(S1HandleNoteOff );
  MIDI1.setHandleControlChange(S1HandleControlChange);
  MIDI1.setHandleProgramChange(S1HandleProgramChange);
  MIDI1.setHandlePitchBend(S1HandlePitchBend);    
  MIDI1.setHandleAfterTouchChannel(S1HandleAfterTouch);    
  MIDI1.setHandleClock(S1HandleClock);    
  MIDI1.setHandleStart(S1HandleStart);    
  MIDI1.setHandleStop(S1HandleStop);    
  MIDI1.setHandleContinue(S1HandleContinue);   

  MIDI2.setHandleNoteOn(S2HandleNoteOn);  
  MIDI2.setHandleNoteOff(S2HandleNoteOff );
  MIDI2.setHandleControlChange(S2HandleControlChange);
  MIDI2.setHandleProgramChange(S2HandleProgramChange);
  MIDI2.setHandlePitchBend(S2HandlePitchBend);    
  MIDI2.setHandleAfterTouchChannel(S2HandleAfterTouch);    
  MIDI2.setHandleClock(S2HandleClock);    
  MIDI2.setHandleStart(S2HandleStart);    
  MIDI2.setHandleStop(S2HandleStop);    
  MIDI2.setHandleContinue(S2HandleContinue);   

  MIDI3.setHandleNoteOn(S3HandleNoteOn);  
  MIDI3.setHandleNoteOff(S3HandleNoteOff );
  MIDI3.setHandleControlChange(S3HandleControlChange);
  MIDI3.setHandleProgramChange(S3HandleProgramChange);
  MIDI3.setHandlePitchBend(S3HandlePitchBend);    
  MIDI3.setHandleAfterTouchChannel(S3HandleAfterTouch);    
  MIDI3.setHandleClock(S3HandleClock);    
  MIDI3.setHandleStart(S3HandleStart);    
  MIDI3.setHandleStop(S3HandleStop);    
  MIDI3.setHandleContinue(S3HandleContinue);   

  
  MIDI1.begin(&Serial3);    
  MIDI2.begin(&Serial2);    
  MIDI3.begin(&Serial1);    


  MIDI1.RouteAdd(&MIDI1, 1, Route_All, Can_All, false );
  MIDI1.RouteAdd(&MIDI2, 2, Route_All, Can_All, false );
  MIDI1.RouteAdd(&MIDI3, 3, Route_All, Can_All, false );
  MIDI2.RouteAdd(&MIDI1, 4, Route_All, Can_All, false );
  MIDI2.RouteAdd(&MIDI2, 5, Route_All, Can_All, false );
  MIDI2.RouteAdd(&MIDI3, 6, Route_All, Can_All, false );
  MIDI3.RouteAdd(&MIDI1, 7, Route_All, Can_All, false );
  MIDI3.RouteAdd(&MIDI2, 8, Route_All, Can_All, false );
  MIDI3.RouteAdd(&MIDI3, 9, Route_All, Can_All, false );

  
//  MIDI2.setHandleNoteOn(S2HandleNoteOn);  
//  MIDI2.setHandleNoteOff(S2HandleNoteOff );
//  MIDI2.begin(&Serial2);    


//  MIDI3.setHandleNoteOn(S3HandleNoteOn);  
//  MIDI3.setHandleNoteOff(S3HandleNoteOff );
//  MIDI3.begin(&Serial3);    

// Pour 'debug'
  
#ifdef _DEBUG_MODE_
  Serial.begin(115200);    
#endif

// Ecran par default
  ActiFunc = MODE_GLOBAL;
  LastActiFunc = ActiFunc;


  S4822.Setup();
  SPOut.Setup();
  SPInp.Setup();

  PatLed.Setup();

  mxAD.Setup();
  MySD.Setup();

  int nbEd= MySD.GetListe(MODE_EDITEUR, 0);
  if ( nbEd > 0 )
  {
      for (int ct=0 ; ct < nbEd ; ct++ )
      {
          lcd_ListeWS(tmpGen, MySD.ListeFiles, ct, 8);
          if ( tmpGen[0]=='@' )
          {
             Editeur[nbEditor]=new edSN(MODE_EDITEUR+nbEditor); 
             Editeur[nbEditor]->LoadFile(tmpGen);
             nbEditor++; 
          }
      }
  }


  LFoBi.ChangeFreq(0,80);
  LFoBi.ChangeFreq(1,80);

  SClock.ChangeTempo();

  // CHargement des @DEF
  if (MySD.Init) def_global();

  Redraw();

}

void Redraw()
{
  switch(ActiFunc)
  {
  case MODE_GLOBAL :
    Glo.DessinPage();
    break;
  case MODE_SEQEUCLID :
    SeqEu.DessinPage();
    break;
  case MODE_SEQSTEP :
    SeqSt.DessinPage();
    break;
  case MODE_SEQTR :
    SeqTr.DessinPage();
    break;
  case MODE_LFO :
    LFoBi.DessinPage();
    break;
//  case MODE_VCO :
//    VCoBi.DessinPage();
//    break;
  case MODE_ARPEGE :
    Arp.DessinPage();
    break;
    
  case MODE_EDITEUR:
  case MODE_EDITEUR+1:
  case MODE_EDITEUR+2:
  case MODE_EDITEUR+3:
  case MODE_EDITEUR+4:
  case MODE_EDITEUR+5:
  case MODE_EDITEUR+6:
  case MODE_EDITEUR+7:
    Editeur[ActiFunc-MODE_EDITEUR]->DessinPage();
    break;
  
  
  }  
  
#ifdef _DEBUG_MODE_
    Serial.println("   M1     M2     M3");
    Serial.println("+--------------------+");
    lcdM.dump();
    Serial.println("+--------------------+");
    Serial.println("  S1  S2  S3  S4  S5  ");
    Serial.println("       FUNCT");
    Serial.println("    <PAT 1..16>");    
    Serial.println("");
    Serial.println("    P1      P2      P3      P4      P5    ");
    Serial.println("+----------------------------------------+");
    lcd1.dump();
    lcd2.dump();
    Serial.println("+----------------------------------------+");
    Serial.println("    P6      P7      P8      P9      P10");
    Serial.println("PAGE                                     INC ");
    Serial.println("    P11     P12     P13     P14     P15");
    Serial.println("+----------------------------------------+");
    lcd3.dump();
    lcd4.dump();
    Serial.println("+----------------------------------------+");
    Serial.println("    P16     P17     P18     P19     P20");
    Serial.println("");
#endif   
}


void sChangeFunc(int value)
{
  int newValue = newmap(value, 0, MAX_FUNCT + nbEditor );
  if ( ActiFunc != newValue )
  {
    ChangeFunc(newValue);
  }
}


void ChangeFunc(int value)
{
  ActiFunc = value;
  Glo.UpdateRoute(0);
  mxAD.Setup();
#ifdef _DEBUG_MODE_
      Serial.println("LL113:ChangeFunc=");
      Serial.println(ActiFunc);    
#endif

  Redraw();
}



/* Boucle Princ
ipale Arduino */

void loop()
{
  
  // RecupÃƒÂ©ration du temps micro 
  uint32_t reftime = micros();

  SPInp.Traite(reftime);

  switch( SClock.Mode )
  {
    case CLOCK_TRIG :
      if ( SPInp.ActionTrigIn()==ParamGlo.TrigInDir )
      {
          SClock.ReceptTrig(reftime);
      }
      break;

   case CLOCK_DYN :
      if ( SPInp.ActionDynClk()==ParamGlo.TrigInDir )
      {
          SClock.ReceptDyn(reftime);
      }
      if ( SPInp.ActionDynStartStop() == UP2DOWN )
      {
            Glo.ExStart(false);
      }
      if ( SPInp.ActionDynStartStop() == DOWN2UP )
      {
            Glo.ExStop();
      }
      break;
      
  case CLOCK_MIDI_1 : 
  case CLOCK_MIDI_2 : 
  case CLOCK_MIDI_3 : 

      if ( SClock.Event == EVENT_START )
      {
            Glo.ExStart(false);
      }
      if ( SClock.Event == EVENT_STOP )
      {
            Glo.ExStop();
      }
      break;
  }
  
  SClock.Traite(reftime);

  // Chaque class recoit ce temps et traite
  Glo.Traite(reftime);
  
//  VCoBi.Traite(reftime);  
  LFoBi.Traite(reftime);  
  SPOut.Traite(reftime); 

  PatLed.Traite(reftime);  

  for (int ed=0;ed<nbEditor;ed++) Editeur[ed]->Traite(reftime); 


  if ( SPInp.Encoder != ENCODER_NONE )
  {
      switch(ActiFunc)
      {
      case MODE_SEQEUCLID :
        SeqEu.Encoder(  SPInp.Encoder );
        break;
      case MODE_SEQSTEP :
        SeqSt.Encoder(  SPInp.Encoder );
        break;
      case MODE_SEQTR :
        SeqTr.Encoder(  SPInp.Encoder );
         break;
      case MODE_LFO :
        LFoBi.Encoder(  SPInp.Encoder );
        break;
//      case MODE_VCO :
//        VCoBi.Encoder(  SPInp.Encoder );
//        break;
      case MODE_ARPEGE :
        Arp.Encoder(  SPInp.Encoder );
        break;
      case MODE_GLOBAL :
        Glo.Encoder(  SPInp.Encoder );
        break;
      case MODE_EDITEUR:
      case MODE_EDITEUR+1:
      case MODE_EDITEUR+2:
      case MODE_EDITEUR+3:
      case MODE_EDITEUR+4:
      case MODE_EDITEUR+5:
      case MODE_EDITEUR+6:
      case MODE_EDITEUR+7:
        Editeur[ActiFunc-MODE_EDITEUR]->Encoder(  SPInp.Encoder );
        break;
      }
  
  }
 
  
  
 
  if ( mxAD.ScanNextAD() )
  {

    int lastfunct = hard_scan[mxAD.last_inc_scan];

    if ( lastfunct == 26 ) // SEL 2
    {
      sChangeFunc ( mxAD.mm[mxAD.last_inc_scan].Value );
    }
    else
    {
      byte OldActiFunct = ActiFunc;
      switch(ActiFunc)
      {
      case MODE_SEQEUCLID :
        SeqEu.Interface( lastfunct ,  mxAD.mm[mxAD.last_inc_scan].Value );
        break;
      case MODE_SEQSTEP :
        SeqSt.Interface( lastfunct ,  mxAD.mm[mxAD.last_inc_scan].Value );
        break;
      case MODE_SEQTR :
        SeqTr.Interface( lastfunct ,  mxAD.mm[mxAD.last_inc_scan].Value );
        break;
      case MODE_LFO :
        LFoBi.Interface( lastfunct ,  mxAD.mm[mxAD.last_inc_scan].Value );
        break;
//      case MODE_VCO :
//        VCoBi.Interface( lastfunct ,  mxAD.mm[mxAD.last_inc_scan].Value );
        break;
      case MODE_ARPEGE :
        Arp.Interface( lastfunct , mxAD.mm[mxAD.last_inc_scan].Value );
        break;
      case MODE_GLOBAL :
        Glo.Interface( lastfunct , mxAD.mm[mxAD.last_inc_scan].Value );
        break;
      case MODE_EDITEUR:
      case MODE_EDITEUR+1:
      case MODE_EDITEUR+2:
      case MODE_EDITEUR+3:
      case MODE_EDITEUR+4:
      case MODE_EDITEUR+5:
      case MODE_EDITEUR+6:
      case MODE_EDITEUR+7:
        Editeur[ActiFunc-MODE_EDITEUR]->Interface( lastfunct ,  mxAD.mm[mxAD.last_inc_scan].Value );
        break;
      }
      if (ActiFunc != OldActiFunct)
      {
            mxAD.Setup();
            Redraw();
      }
    }	
  }
  else
  {
 
  }


  // Je lis les midi par modulo % 3 de la boucle  
  switch (mxAD.last_inc_scan % 3)
  {
  case 0:
    MIDI1.read();
    break;
  case 1:
    MIDI2.read();
    break;
  case 2:
    MIDI3.read();
    break;
  }


  // Je rafraichis les LCD par modulo % 5 de la boucle  
  // Chaque LCD ne rafraichissant qu'une ligne par boucle
  switch (mxAD.last_inc_scan % 5)
  {
  case 0:
    lcdM.Refresh();
    break;
  case 1:
    lcd1.Refresh();
    break;
  case 2:
    lcd2.Refresh();
    break;
  case 3:
    lcd3.Refresh();
    break;
  case 4:
    lcd4.Refresh();
    break;
  }
  
  digitalWrite( JACKCLK_OUT, (prob_cpt++ & 1) );
}




