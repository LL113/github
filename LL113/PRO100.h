// -----------------------------------------------
// LL113
// -----------------------------------------------
// Historique:
// (Baloran) laurent.lecatelier@free.fr, version 0.0.1  10/02/12013
// -----------------------------------------------
// PRO100.H DÃƒÂ©finitions instrument du SOLTON PROJECT 100


#ifndef _PRO100_H_
#define _PRO100_H_

#include "edSynth.h"

extern const char *listOffOn;
extern const char *listChanel;
extern const char *listPC;

const char *pro100_ListRange="16'    8'     4'     ";
const char *pro100_ListOmni ="Off    On     Mono   Poly   ";

const paramSynth PRO100[] = {

   {"DCO1 Sq" ,1,1,0,1,0,listOffOn,64,TYPE_CC},
   {"Sawtooh" ,1,2,0,1,0,listOffOn,65,TYPE_CC},
   {"V Pulse" ,1,3,0,1,0,listOffOn,66,TYPE_CC},
   {"C Pulse" ,1,4,0,1,0,listOffOn,67,TYPE_CC},
   {"Range"   ,1,5,0,2,0,pro100_ListRange,0,TYPE_CC},
   {"DCO2 Sq" ,1,6,0,1,0,listOffOn,68,TYPE_CC},
   {"Sawtooh" ,1,7,0,1,0,listOffOn,69,TYPE_CC},
   {"V Pulse" ,1,8,0,1,0,listOffOn,70,TYPE_CC},
   {"C Pulse" ,1,9,0,1,0,listOffOn,71,TYPE_CC},
   {"Range"   ,1,10,0,2,0,pro100_ListRange,1,TYPE_CC},
  
   {"DCO1 Pw" ,1,11,0,85,0,0,15,TYPE_CC},
   {"DCO2 Pw" ,1,12,0,85,0,0,16,TYPE_CC},
   {"Noise"   ,1,13,0,85,0,0,10,TYPE_CC},
   {"Detune"  ,1,14,0,85,0,0,11,TYPE_CC},
   {"LFO*DCO" ,1,15,0,1,0,listOffOn,74,TYPE_CC},

   {"Chorus"  ,1,16,0,1,0,listPC,102,TYPE_PC},
   {"MemChor" ,1,17,0,1,0,listOffOn,78,TYPE_CC},
   {"Portame" ,1,18,0,1,0,listPC,114,TYPE_PC},

   {"VCF Frq" ,2,1,0,85,0,0,17,TYPE_CC},
   {"Resonan" ,2,2,0,85,0,0,18,TYPE_CC},
   {"VCF Env" ,2,3,0,85,0,0,19,TYPE_CC},
   {"VCF Key" ,2,4,0,1,0,listOffOn,77,TYPE_CC},
   {"LFO*VCF" ,2 ,5,0,1,0,listOffOn,75,TYPE_CC},

   {"LFO Spd" ,2,6,0,85,0,0,12,TYPE_CC},
   {"Delay"   ,2,7,0,85,0,0,13,TYPE_CC},
   {"Level"   ,2,8,0,85,0,0,14,TYPE_CC},
   {"LFO*VCA" ,2,10,0,1,0,listOffOn,76,TYPE_CC},


   {"VCF Env" ,2,11,0,85,0,0,19,TYPE_CC},
   {"Attack"  ,2,12,0,85,0,0,2,TYPE_CC},
   {"Decay"   ,2,13,0,85,0,0,3,TYPE_CC},
   {"Sustain" ,2,14,0,85,0,0,4,TYPE_CC},
   {"Release" ,2,15,0,85,0,0,5,TYPE_CC},

   {"VCA Env" ,2,16,0,85,0,0,20,TYPE_CC},
   {"Attack"  ,2,17,0,85,0,0,6,TYPE_CC},
   {"Decay"   ,2,18,0,85,0,0,7,TYPE_CC},
   {"Sustain" ,2,19,0,85,0,0,8,TYPE_CC},
   {"Release" ,2,20,0,85,0,0,9,TYPE_CC},

   {"Edit"    ,3,1,0,1,0,listPC,100,TYPE_PC},
   {"All Off" ,3,2,0,1,0,listPC,123,TYPE_PC},
   {"Omni"    ,3,3,0,1,0,listOffOn,124,TYPE_CC_LIST},
   {"Loc.Off" ,3,4,0,1,0,listOffOn,122,TYPE_CC},
  	   
   {"\x01SOLTON PROJECT 100\x02",0,0,10,2,0x0002,"SOLP100",0,0 },
};    


paramSysEx exPRO100[] = {
   {"RIEN",0,0,0,B_14BITS,0,B_14BITS,""  },
};    
  
  
#endif // _PRO100_H_



