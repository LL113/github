// -----------------------------------------------
// LL113
// -----------------------------------------------
// Historique:
// (Baloran) laurent.lecatelier@free.fr, version 0.0.1  10/02/12013
// -----------------------------------------------
// MSR2.H DÃƒÂ©finiation instrument du MARION MSR2 Oberheim


#ifndef _MSR2_H_
#define _MSR2_H_

#include "edSynth.h"


extern const char *listOffOn;
extern const char *listChanel;

const char *msr2_listWave     ="Off    Pulse  Wave   Both  ";
const char *msr2_extSigna     ="Noise  Extern ";
const char *msr2_listSync     ="Off    Soft   Medium Hard  ";
const char *msr2_listPoles    ="12db   24db   ";
const char *msr2_listFrqNot   ="Off    1/4    1/2    Full  ";
const char *msr2_listTrig     ="Note   Pedal 1Pedal 2Pedal 3LFO1   ";
const char *msr2_listModTrg   ="Single Multi  ";
const char *msr2_listLfoWav   ="Sinus  Triang UpSaw  DwnSaw Square Random Sample ";
const char *msr2_listLfoTrig  ="Note   Pedal 1Pedal 2Pedal 3VCA atkVCA hldVCA decVCA susVCA relVCF atkVCF hldVCF decVCF susVCF relGEN atkGEN hldGEN decGEN susGEN rel";
const char *msr2_listSample   ="Off    VCA EnvVCF EnvGEN EnvLFO1   LFO2   VibratoRamp 1 Ramp 2 C Note Voice  Random VelocitR VelocPress  Bend   Ctrl 1 Ctrl 2 Ctrl 3 Ctrl 4 Ctrl 5 Pedal 1Pedal 2Pedal 3Note   ";
const char *msr2_listPorMod   ="Fixed  Variab Expone ";
const char *msr2_listMonoNot  ="Low    High   ";
const char *msr2_listVoice    ="Dynami Reassi Rotate ReaRob RotBob Monoph Uniss 2Uniss 4";
const char *msr2_listSustain  ="Off    Pedal1 Pedal2 Pedal3 ";
const char *msr2_listModDst   ="HR1 frqHR1 pw HR1 shpHR2 frqHR2 pw HR2 shpHRO mixVCF fm VCF frqVCF resVCA volPan    VCA dlyVCA atkVCA decVCA relVCA ampVCF dlyVCF atkVCF decVCF relVCF ampGEN dlyGEN atkGEN decGEN relGEN ampLF1 spdLF1 ampLF2 spdLF2 ampPOR ratEXT amt";
const char *msr2_listMidiMode ="Multi  Omni   Poly   Mono   ";
const char *msr2_listPatchMode="Off    1      2      3      4      ";
const char *msr2_listVelo1    ="Off    Global User   Curv 1 Curv 2 Curv 3 Curv 4 ";
const char *msr2_listTun1     ="Equal  Global Tun1   Tun2   Tun3   Tun4   ";
const char *msr2_listVelo2    ="Off    User   Curv 1 Curv 2 Curv 3 Curv 4 ";
const char *msr2_listTun2     ="Equal  Tun 1  Tun 2  Tun 3  Tun 4  ";

const paramSynth MSR2[] = {

   {"HRO1 Fr",1,1,-99,99,0,0,0x0C,0 }, 
   {"Wave",1,2,0,3,2,msr2_listWave,0x0E,0 },  
   {"Pulse W",1,3,0,99,0,0,0x10,0 },  
   {"Sync",1,4,0,3,0,msr2_listSync,0x0D,0 },  
   {"LF1 Mod",1,6,-99,99,0,0,0x11,0 }, 
   {"Shape",1,7,-99,99,0,0,0x0F,0 },  
   {"LF2 PW",1,8,-99,99,0,0,0x12,0 },  
  
   {"HRO2 Fr",1,11,-99,99,0,0,0x13,0 },
   {"Wave",1,12,0,3,2,msr2_listWave,0x15,0 },  
   {"Pulse W",1,13,0,99,0,0,0x17,0 },  
   {"Detune",1,14,-99,99,10,0,0x14,0 }, 
   {"Ext Sig",1,15,0,1,0,msr2_extSigna,0x1B,0 }, 
   {"LF1 Mod",1,16,-99,99,0,0,0x18,0 },
   {"Shape",1,17,-99,99,0,0,0x16,0 }, 
   {"LF2 PW",1,18,-99,99,0,0,0x19,0 }, 
   {"HRO Mix",1,19,-99,99,0,0,0x1A,0 },
   {"Amount",1,20,0,99,0,0,0x1C,0 },
  
  
   {"VCF Frq",2,1,0,99,99,0,0x1D,0 },
   {"Poles",2,2,0,1,1,msr2_listPoles,0x1E,0 },  
   {"Frq*Pre",2,3,-99,99,0,0,0x21,0 },  
   {"Frq*LF2",2,4,-99,99,0,0,0x22,0 },  
   {"FM*Gene",2,5,-99,99,0,0,0x2F,0 }, 
   {"Resonan",2,6,0,99,0,0,0x1F,0 }, 
   {"Frq*Env",2,7,-99,99,0,0,0x20,0 },  
   {"Frq*Not",2,8,0,3,0,msr2_listFrqNot,0x23,0 },  
   {"FM Amou",2,9,0,99,0,0,0x2E,0 },  
   {"FM*Pres",2,10,-99,99,0,0,0x30,0 },  
   
   {"VCF Att",2,11,0,99,0,0,0x24,0 },
   {"Decay",2,12,0,99,0,0,0x25,0 },
   {"Sustain",2,13,0,99,0,0,0x26,0 },
   {"Release",2,14,0,99,0,0,0x27,0 },
   {"Amplitu",2,15,0,99,0,0,0x2A,0 },
   {"Delay",2,16,0,99,0,0,0x28,0 },
   {"Hold",2,17,0,99,0,0,0x29,0 },
   {"Trigger",2,18,0,4,0,msr2_listTrig,0x2C,0 },
   {"Trg mod",2,19,0,1,0,msr2_listModTrg,0x2D,0 },
   {"Amp*Vel",2,20,-99,99,0,0,0x2B,0 },
  
   {"VCA Att",3,1,0,99,0,0,0x35,0 },
   {"Decay",3,2,0,99,0,0,0x36,0 },
   {"Sustain",3,3,0,99,99,0,0x37,0 },
   {"Release",3,4,0,99,0,0,0x38,0 },
   {"Amplitu",3,5,0,99,99,0,0x3B,0 },
   {"Delay",3,6,0,99,0,0,0x39,0 },
   {"Hold",3,7,0,99,0,0,0x3A,0 },
   {"Trigger",3,8,0,4,0,msr2_listTrig,0x3D,0 },
   {"Trg mod",3,9,0,1,0,msr2_listModTrg,0x3E,0 },
   {"Amp*Vel",3,10,-99,99,0,0,0x3C,0 },
  
   {"GEN Att",3,11,0,99,0,0,0x3F,0 },
   {"Decay",3,12,0,99,0,0,0x40,0 },
   {"Sustain",3,13,0,99,0,0,0x41,0 },
   {"Release",3,14,0,99,0,0,0x42,0 },
   {"Amplitu",3,15,0,99,0,0,0x45,0 },
   {"Delay",3,16,0,99,0,0,0x43,0 },
   {"Hold",3,17,0,99,0,0,0x44,0 },
   {"Trigger",3,18,0,4,0,msr2_listTrig,0x47,0 },
   {"Trg mod",3,19,0,1,0,msr2_listModTrg,0x48,0 },
   {"Amp*Vel",3,20,-99,99,0,0,0x46,0 },
  
   {"LFO1 Wa",4,1,0,6,0,msr2_listLfoWav,0x49,0 },
   {"Amplitu",4,2,0,99,0,0,0x4B,0 },
   {"Rat*Pre",4,3,-99,99,0,0,0x4C,0 },
   {"Ph Retr",4,4,0,99,0,0,0x4F,0 },
   {"Delay",4,5,0,99,0,0,0x51,0 },
   {"Rate",4,6,0,99,0,0,0x4A,0 },
   {"Amp*Rp1",4,7,-99,99,0,0,0x4D,0 },
   {"Trigger",4,8,0,18,0,msr2_listLfoTrig,0x4E,0 },
   {"Trg mod",4,9,0,1,0,msr2_listModTrg,0x50,0 },
   {"Sample",4,10,0,24,0,msr2_listSample,0x52,0 },
  	   
   {"LFO2 Wa",4,11,0,6,0,msr2_listLfoWav,0x53,0 },
   {"Amplitu",4,12,0,99,0,0,0x55,0 },
   {"Rat*Pre",4,13,-99,99,0,0,0x56,0 },
   {"Ph Retr",4,14,0,99,0,0,0x59,0 },
   {"Delay",4,15,0,99,0,0,0x5B,0 },
   {"Rate",4,16,0,99,0,0,0x54,0 },
   {"Amp*Rp2",4,17,-99,99,0,0,0x57,0 },
   {"Trigger",4,18,0,18,0,msr2_listLfoTrig,0x58,0 },
   {"Trg mod",4,19,0,1,0,msr2_listModTrg,0x5A,0 },
   {"Sample",4,20,0,24,0,msr2_listSample,0x5C,0 },
  	   	   
   {"RAMP1 T",5,1,0,18,0,msr2_listLfoTrig,0x5D,0 },
   {"Mode",5,2,0,1,0,msr2_listModTrg,0x5E,0 },
   {"Rate",5,3,0,99,0,0,0x5F,0 },
   {"PORTA R",5,4,0,99,0,0,0x66,0 },
   {"Por*Vel",5,5,-99,99,0,0,0x68,0 },
   {"RAMP2 T",5,6,0,18,0,msr2_listLfoTrig,0x60,0 },
   {"Mode",5,7,0,1,0,msr2_listModTrg,0x61,0 },
   {"Rate",5,8,0,99,0,0,0x62,0 },
   {"PORTA M",5,9,0,2,0,msr2_listPorMod,0x67,0 },
   {"Por*Leg",5,10,0,1,0,listOffOn,0x69,0 },
  
   {"Voice",5,11,0,7,0,msr2_listVoice,0x63,0 },
   {"Priorit",5,12,0,1,0,msr2_listMonoNot,0x64,0 },
   {"Sustain",5,13,0,3,0,msr2_listSustain,0x65,0 },
   {"Midi Lo",5,14,0,127,0,0,0x6D,0 },
   {"Midi Hi",5,15,0,127,127,0,0x6E,0 },
   {"Bend Rg",5,16,0,12,2,0,0x6F,0 },
   {"Vibrato",5,17,0,1,0,listOffOn,0x70,0 },
   {"Velocit",5,18,-3,3,0,msr2_listVelo1,0x6A,0 },
   {"Tuning",5,19,-2,3,0,msr2_listTun1,0x6B,0 },
  
   {"Modul 1",6,1,0,0,0,0,0,0 },
   {"Src"   ,6,2,0,24,0,msr2_listSample,0x72,1 },
   {"Dest"  ,6,3,0,32,0,msr2_listModDst,0x73,1 },
   {"Amplitu",6,4,-99,99,0,0,0x74,1 },
   {"Modul 2",6,6,0,0,0,0,0,0 },
   {"Src"   ,6,7,0,24,0,msr2_listSample,0x72,2 },
   {"Dest"  ,6,8,0,32,0,msr2_listModDst,0x73,2 },
   {"Amplitu",6,9,-99,99,0,0,0x74,2 },
  	   
   {"Modul 3",6,11,0,0,0,0,0,0 },
   {"Src"   ,6,12,0,24,0,msr2_listSample,0x72,3 },
   {"Dest"  ,6,13,0,32,0,msr2_listModDst,0x73,3 },
   {"Amplitu",6,14,-99,99,0,0,0x74,3 },
   {"Modul 4",6,16,0,0,0,0,0,0 },
   {"Src"   ,6,17,0,24,0,msr2_listSample,0x72,4 },
   {"Dest"  ,6,18,0,32,0,msr2_listModDst,0x73,4 },
   {"Amplitu",6,19,-99,99,0,0,0x74,4 },	   
  
   {"Modul 5",7,1,0,0,0,0,0,0 },
   {"Src"   ,7,2,0,24,0,msr2_listSample,0x72,5 },
   {"Dest"  ,7,3,0,32,0,msr2_listModDst,0x73,5 },
   {"Amplitu",7,4,-99,99,0,0,0x74,5 },
   {"Modul 6",7,6,0,0,0,0,0,0 },
   {"Src"   ,7,7,0,24,0,msr2_listSample,0x72,6 },
   {"Dest"  ,7,8,0,32,0,msr2_listModDst,0x73,6 },
   {"Amplitu",7,9,-99,99,0,0,0x74,6 },
  	   
   {"Modul 7",7,11,0,0,0,0,0,0 },
   {"Src"   ,7,12,0,24,0,msr2_listSample,0x72,7 },
   {"Dest"  ,7,13,0,32,0,msr2_listModDst,0x73,7 },
   {"Amplitu",7,14,-99,99,0,0,0x74,7 },
   {"Modul 8",7,16,0,0,0,0,0,0 },
   {"Src"   ,7,17,0,24,0,msr2_listSample,0x72,8 },
   {"Dest"  ,7,18,0,32,0,msr2_listModDst,0x73,8 },
   {"Amplitu",7,19,-99,99,0,0,0x74,8 },	   
  	   
   {"Modul 9",8,1,0,0,0,0,0,0 },
   {"Src"   ,8,2,0,24,0,msr2_listSample,0x72,9 },
   {"Dest"  ,8,3,0,32,0,msr2_listModDst,0x73,9 },
   {"Amplitu",8,4,-99,99,0,0,0x74,9 },
   {"Modul10",8,6,0,0,0,0,0,0 },
   {"Src"   ,8,7,0,24,0,msr2_listSample,0x72,10},
   {"Dest"  ,8,8,0,32,0,msr2_listModDst,0x73,10},
   {"Amplitu",8,9,-99,99,0,0,0x74,10 },
  

   {"Tr Chan",8,11,0,15,0,listChanel,0x8F,0 },
   {"Transpo",8,12,0,1,0,listOffOn,0x90,0 },
   {"Range",8,13,-12,12,0,0,0x91,0 },
   {"Fine Tu",8,15,-99,99,0,0,0x92,0 },
   {"Vibr Rg",8,16,0,5,0,msr2_listLfoWav,0x93,0 },
   {"Rate"  ,8,17,0,99,0,0,0x94,0 },
   {"Amplitu",8,18,0,99,0,0,0x95,0 },
   {"Rat*Pre",8,19,-99,99,0,0,0x96,0 },
   {"Amp*Ct1",8,20,-99,99,0,0,0x97,0 },

   {"EQ 63Hz",9,1,-12,12,0,0,108,TYPE_CC},
   {"EQ125Hz",9,2,-12,12,0,0,109,TYPE_CC },
   {"EQ250Hz",9,3,-12,12,0,0,110,TYPE_CC },
   {"EQ500Hz",9,4,-12,12,0,0,111,TYPE_CC },
   {"EQ 1KHz",9,5,-12,12,0,0,112,TYPE_CC },
   {"EQ 2KHz",9,6,-12,12,0,0,113,TYPE_CC },
   {"EQ 4KHz",9,7,-12,12,0,0,114,TYPE_CC },
  
   {"MIX 1L",9,11,0,24,24,0,102,TYPE_CC },
   {"MIX 1R",9,12,0,24,24,0,103,TYPE_CC },
   {"MIX 2L",9,13,0,24,24,0,104,TYPE_CC },
   {"MIX 2R",9,14,0,24,24,0,105,TYPE_CC },
   {"MIX EL",9,15,0,24,24,0,106,TYPE_CC },
   {"MIX ER",9,16,0,24,24,0,107,TYPE_CC },
  	   
   {"Midi Ch",10,1,1,16,1,0,0x76,0 },
   {"Midi Md",10,2,0,3,0,msr2_listMidiMode,0x99,0 },
   {"Enab Ch",10,3,0,15,0,listChanel,0x9A,0 },
   {"Off/On" ,10,4,0,1,1,listOffOn,0x9B,0 },
   {"Glo Vel",10,6,-2,3,0,msr2_listVelo2,0x9E,0 },
   {"Glo Tun",10,7,-1,3,0,msr2_listTun2,0x9F,0 },
   {"Rec PGM",10,8,0,1,0,listOffOn,0x9D,0 },
   {"Pat MAP",10,9,-1,3,0,msr2_listPatchMode,0xA0,0 },

   {"Ctrl 1",10,11,1,31,0,0,0xAA,0 },
   {"Ctrl 2",10,12,1,31,0,0,0xAB,0 },
   {"Ctrl 3",10,13,1,31,0,0,0xAC,0 },
   {"Ctrl 4",10,14,1,31,0,0,0xAD,0 },
   {"Ctrl 5",10,15,1,31,0,0,0xAE,0 },
   {"Pedal 1",10,16,64,79,0,0,0xAF, 0},
   {"Pedal 2",10,17,64,79,0,0,0xB0,0 },
   {"Pedal 3",10,18,64,79,0,0,0xB1,0 },
  
   {"\x01  MARION MSR-2    \x02",0,0,20,8,0x0001,"MARION",0,0 },
   
// Titre de l'ÃƒÂ©diteur
// 20 = NB de millisecondes entre deux envois de SysEx pour Send ou Retr
// 8 = PLus grand nÃ‚Â° de page sauvegardÃƒÂ© dans un fichier "preset", les pages suivantes sont ignorÃƒÂ©es (global midi etc...)
// 0x0001 = identificateur unique du synthÃƒÂ©
// MARION = dossier de la SD pour les "preset"

};    


    
#define SX_2B 2
#define SX_1B 1

const paramSysEx exMSR2[] = {
   {"PARAM ASM",14,6,9,B_14BITS,11,B_14BITS,"\xF0\x00\x00\x59\x01\x01\x00\x03\x07\xAA\xAA\xDD\xDD\xF7"  },
   {"MODULAT 0",28,6,23,B_14BITS,25,B_14BITS,"\xF0\x00\x00\x59\x01\x01\x00\x03\x07\x00\x71\x00\x00\xF7\xF0\x00\x00\x59\x01\x01\x00\x03\x07\xAA\xAA\xDD\xDD\xF7"  },
   {"MODULAT 1",28,6,23,B_14BITS,25,B_14BITS,"\xF0\x00\x00\x59\x01\x01\x00\x03\x07\x00\x71\x00\x01\xF7\xF0\x00\x00\x59\x01\x01\x00\x03\x07\xAA\xAA\xDD\xDD\xF7"  },
   {"MODULAT 2",28,6,23,B_14BITS,25,B_14BITS,"\xF0\x00\x00\x59\x01\x01\x00\x03\x07\x00\x71\x00\x02\xF7\xF0\x00\x00\x59\x01\x01\x00\x03\x07\xAA\xAA\xDD\xDD\xF7"  },
   {"MODULAT 3",28,6,23,B_14BITS,25,B_14BITS,"\xF0\x00\x00\x59\x01\x01\x00\x03\x07\x00\x71\x00\x03\xF7\xF0\x00\x00\x59\x01\x01\x00\x03\x07\xAA\xAA\xDD\xDD\xF7"  },
   {"MODULAT 4",28,6,23,B_14BITS,25,B_14BITS,"\xF0\x00\x00\x59\x01\x01\x00\x03\x07\x00\x71\x00\x04\xF7\xF0\x00\x00\x59\x01\x01\x00\x03\x07\xAA\xAA\xDD\xDD\xF7"  },
   {"MODULAT 5",28,6,23,B_14BITS,25,B_14BITS,"\xF0\x00\x00\x59\x01\x01\x00\x03\x07\x00\x71\x00\x05\xF7\xF0\x00\x00\x59\x01\x01\x00\x03\x07\xAA\xAA\xDD\xDD\xF7"  },
   {"MODULAT 6",28,6,23,B_14BITS,25,B_14BITS,"\xF0\x00\x00\x59\x01\x01\x00\x03\x07\x00\x71\x00\x06\xF7\xF0\x00\x00\x59\x01\x01\x00\x03\x07\xAA\xAA\xDD\xDD\xF7"  },
   {"MODULAT 7",28,6,23,B_14BITS,25,B_14BITS,"\xF0\x00\x00\x59\x01\x01\x00\x03\x07\x00\x71\x00\x07\xF7\xF0\x00\x00\x59\x01\x01\x00\x03\x07\xAA\xAA\xDD\xDD\xF7"  },
   {"MODULAT 8",28,6,23,B_14BITS,25,B_14BITS,"\xF0\x00\x00\x59\x01\x01\x00\x03\x07\x00\x71\x00\x08\xF7\xF0\x00\x00\x59\x01\x01\x00\x03\x07\xAA\xAA\xDD\xDD\xF7"  },
   {"MODULAT 9",28,6,23,B_14BITS,25,B_14BITS,"\xF0\x00\x00\x59\x01\x01\x00\x03\x07\x00\x71\x00\x09\xF7\xF0\x00\x00\x59\x01\x01\x00\x03\x07\xAA\xAA\xDD\xDD\xF7"  },
   {"GLOBAL PAR",14,6,9,B_14BITS,11,B_14BITS,"\xF0\x00\x00\x59\x01\x00\x00\x03\x00\xAA\xAA\xDD\xDD\xF7"  }, //11
};    
  
#endif // _MSR2_H_



