// -----------------------------------------------
// LL113
// -----------------------------------------------
// Historique:
// (Baloran) laurent.lecatelier@free.fr, version 0.0.1  10/02/12013
// -----------------------------------------------
// SDFile.h DÃƒÂ©finition Class gestion de la carte SD


#ifndef _SDFile_H_
#define _SDFile_H_

#include <SD.h>

#define MAX_FILES 128

extern char *DefSdName;

class SDFile 
{

public:
  // Constructor and Destructor
  SDFile();
  ~SDFile();
  
  boolean WriteFile(byte Source, char *FileName, byte *data, int lg);
  
  
  boolean ReadFile(byte Source, char *FileName, byte *data, int lg);

  boolean OpenFileForWrite(byte Source, char *FileName);
  boolean OpenFileForRead(byte Source, char *FileName);

  boolean OpenFileForWrite(char *format, char *FileName);
  boolean OpenFileForRead(char *format, char *FileName);

  boolean ReadStream(byte *data, int lg);

  boolean ReadStreamCh(byte *data);
  uint16_t ReadStreamUint();
  int16_t ReadStreamInt();
  char ReadStreamChar();
  byte ReadStreamByte();

  boolean WriteStream(byte *data, int lg);
  boolean CloseFile();

  void SetFileName(byte Source, char *FileName);
  void SetFileName(char *Format, char *FileName);

  int GetListe(byte Source, char *format);
  
  void Setup();
  
  char *ListeFiles;
  int NbFiles;
  boolean Init;
  

protected:
  char *GetDirName(byte Source);
  char tmpFileName[16];
  File fFile;
  boolean fFileIsOpen;
  
};

#endif // _SDFile_H_


