// -----------------------------------------------
// LL113
// -----------------------------------------------
// Historique:
// (Baloran) laurent.lecatelier@free.fr, version 0.0.1  10/02/12013
// -----------------------------------------------
// SDFile.cpp Code Class gestion de la carte SD


#include "Arduino.h"
#include "sdfile.h"
#include "utilld.h"

char *DefSdName="@DEF01";

SDFile::SDFile()
{

  ListeFiles = (char *)malloc(MAX_FILES * 8);
  Init=false;
  fFileIsOpen=false;
}




SDFile::~SDFile()
{
  free(ListeFiles);
}

void SDFile::Setup()
{
  pinMode(SD_CSPin, OUTPUT);
  Init=SD.begin(SD_CSPin);
  if (Init)
  {
    if ( !SD.exists( "PARAM" )) SD.mkdir("PARAM");
    if ( !SD.exists( "SEQEU" )) SD.mkdir("SEQEU");
    if ( !SD.exists( "SEQST" )) SD.mkdir("SEQST");
    if ( !SD.exists( "SEQTR" )) SD.mkdir("SEQTR");
    if ( !SD.exists( "ARP" )) SD.mkdir("ARP");
    if ( !SD.exists( "LFO" )) SD.mkdir("LFO");
    if ( !SD.exists( "VCO" )) SD.mkdir("VCO");
    if ( !SD.exists( "EDIT" )) SD.mkdir("EDIT");
    if ( !SD.exists( "GLOBAL" )) SD.mkdir("GLOBAL");
  }
}

boolean SDFile::WriteFile(byte Source, char *FileName, byte *data, int lg)
{
  if (fFileIsOpen) return false;
  if ( OpenFileForWrite(Source, FileName ) )
  {
    WriteStream(data, lg) ;
    CloseFile();
  }
}

boolean SDFile::ReadFile(byte Source, char *FileName, byte *data, int lg)
{
  if (fFileIsOpen) return false;
  if ( OpenFileForRead(Source, FileName ) )
  {
    ReadStream(data, lg) ;
    CloseFile();
  }
}

boolean SDFile::OpenFileForWrite(byte Source, char *FileName)
{
  if (fFileIsOpen) return false;
  if (!Init) Setup();
  SetFileName(Source,FileName);
  if (  SD.exists(tmpFileName) ) 
    SD.remove(tmpFileName);
  fFile = SD.open(tmpFileName, FILE_WRITE);
  fFileIsOpen = (fFile);
  return (fFileIsOpen);
}


boolean SDFile::OpenFileForWrite(char *format, char *FileName)
{
  if (fFileIsOpen) return false;
  if (!Init) Setup();
  SetFileName(format,FileName);
  if (  SD.exists(tmpFileName) ) 
    SD.remove(tmpFileName);
  fFile = SD.open(tmpFileName, FILE_WRITE);
  fFileIsOpen = (fFile);
  return (fFileIsOpen);
}


boolean SDFile::OpenFileForRead(byte Source, char *FileName)
{
  if (fFileIsOpen) return false;
  if (!Init) Setup();
  SetFileName(Source,FileName);
  if (  !SD.exists(tmpFileName) ) return false;
  fFile = SD.open(tmpFileName);
  fFileIsOpen = (fFile);
  return (fFileIsOpen);
}


boolean SDFile::OpenFileForRead(char *format, char *FileName)
{
  if (fFileIsOpen) return false;
  if (!Init) Setup();
  SetFileName(format,FileName);
  if (  !SD.exists(tmpFileName) ) return false;
  fFile = SD.open(tmpFileName);
  fFileIsOpen = (fFile);
  return (fFileIsOpen);
}

boolean SDFile::ReadStream(byte *data, int lg)
{
  int wr=0;
  if (!fFileIsOpen) return false;
  while (fFile.available() && wr < lg) {
    data[wr++] = fFile.read();
  }
  return ( wr == lg ) ;
}


uint16_t SDFile::ReadStreamUint()
{
  uint16_t ret;
  fFile.read( &ret, sizeof(uint16_t) );
  return ret;
}

int16_t SDFile::ReadStreamInt()
{
  int16_t ret;
  fFile.read( &ret, sizeof(int16_t) );
  return ret;
}

char SDFile::ReadStreamChar()
{
  char ret;
  fFile.read( &ret, sizeof(char) );
  return ret;

}

byte SDFile::ReadStreamByte()
{
  byte ret;
  fFile.read( &ret, sizeof(byte) );
  return ret;
}  

boolean SDFile::ReadStreamCh(byte *data)
{
  int lg;
  if (!fFileIsOpen) return false;
  lg = fFile.read();
  fFile.read( (byte *)data, lg);
  return (true) ;
}



boolean SDFile::WriteStream(byte *data, int lg)
{
  if (!fFileIsOpen) return false;
  return ( fFile.write(data, lg) == lg ) ;
}


boolean SDFile::CloseFile()
{    
  if (!fFileIsOpen) return false;
  fFile.close();
  fFileIsOpen=false;
}


void SDFile::SetFileName(byte Source, char *FileName)
{

  switch(Source)
  {
  case MODE_PARAM:
    sprintf(tmpFileName,"PARAM/%s",FileName);
    break;
  case MODE_GLOBAL:
    sprintf(tmpFileName,"GLOBAL/%s",FileName);
    break;
  case MODE_SEQEUCLID:
    sprintf(tmpFileName,"SEQEU/%s",FileName);
    break;
  case MODE_SEQSTEP:
    sprintf(tmpFileName,"SEQST/%s",FileName);
    break;
  case MODE_SEQTR:
    sprintf(tmpFileName,"SEQTR/%s",FileName);
    break;
  case MODE_ARPEGE:
    sprintf(tmpFileName,"ARP/%s",FileName);
    break;
  case MODE_LFO:
    sprintf(tmpFileName,"LFO/%s",FileName);
    break;
    //  case MODE_VCO:
    //    sprintf(tmpFileName,"VCO/%s",FileName);
    //    break;
  case MODE_EDITEUR:
  case MODE_EDITEUR+1:
  case MODE_EDITEUR+2:
  case MODE_EDITEUR+3:
  case MODE_EDITEUR+4:
  case MODE_EDITEUR+5:
  case MODE_EDITEUR+6:
  case MODE_EDITEUR+7:
  case MODE_EDITEUR+8:
    sprintf(tmpFileName,"EDIT/%s",FileName);
    break;
  }
#ifdef _DEBUG_MODE_
  Serial.println(tmpFileName);
#endif
}

void SDFile::SetFileName(char *format, char *FileName)
{
  char tpformat[12];
  strcpy(tpformat,format);
  strcat(tpformat,"/%s");
  sprintf(tmpFileName,tpformat,FileName);
#ifdef _DEBUG_MODE_
  Serial.println(tmpFileName);
#endif
}


int SDFile::GetListe(byte Source, char *format)
{
  NbFiles = 0;
  memset(ListeFiles,32,MAX_FILES * 8);

  File root;  

  if ( format == 0 )
  {
    switch(Source)
    {
    case MODE_PARAM:
      root = SD.open("/PARAM");
      break;
    case MODE_GLOBAL:
      root = SD.open("/GLOBAL");
      break;
    case MODE_SEQEUCLID:
      root = SD.open("/SEQEU");
      break;
    case MODE_SEQSTEP:
      root = SD.open("/SEQST");
      break;
    case MODE_SEQTR:
      root = SD.open("/SEQTR");
      break;
    case MODE_ARPEGE:
      root = SD.open("/ARP");
      break;
    case MODE_LFO:
      root = SD.open("/LFO");
      break;
      //   case MODE_VCO:
      //     root = SD.open("/VCO");
      //     break;
    case MODE_EDITEUR:
    case MODE_EDITEUR+1:
    case MODE_EDITEUR+2:
    case MODE_EDITEUR+3:
    case MODE_EDITEUR+4:
    case MODE_EDITEUR+5:
    case MODE_EDITEUR+6:
    case MODE_EDITEUR+7:
    case MODE_EDITEUR+8:
      root = SD.open("/EDIT");
      break;
    }  
  }  
  else
  {
    strcpy(tmpFileName,"/");
    strcat(tmpFileName,format);
    root = SD.open(tmpFileName);

  }

  while(true) {
    File entry =  root.openNextFile();
    if (! entry) {
      break;
    }
#ifdef _DEBUG_MODE_
    Serial.println(entry.name());
#endif
    char *buf=entry.name();      
    int cpt=0; 
    while ( buf[cpt] > 0 && cpt < 8) {
      ListeFiles[NbFiles*8+cpt]=buf[cpt] ;
      cpt++;
    }
    NbFiles++;
  }

  return NbFiles;
}




