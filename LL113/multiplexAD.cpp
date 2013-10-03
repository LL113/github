           // -----------------------------------------------
// LL113
// -----------------------------------------------
// Historique:
// (Baloran) laurent.lecatelier@free.fr, version 0.0.1  10/02/12013
// -----------------------------------------------
// multiplex.cpp Code Class gestion du matÃƒÂ©riel et des multiplexeurs


#include "Arduino.h"
#include "multiplexAD.h"
#include "utilld.h"



multiplexAD::multiplexAD()
{
  inc_scan = 0;
  for (int ct = 0; ct < NB_AD; ct++)
  {
    mm[ct].Value=0;
    mm[ct].Old_1=0;
    mm[ct].Old_2=0;
    mm[ct].DeltaChange=DELTA_PAGE;
    mm[ct].Type = MAD_POTAR;      
  }

#ifdef _HARDWARE_1_
  mm[14].Type = MAD_RIEN;      
  mm[15].Type = MAD_RIEN;      
  mm[27].Type = MAD_RIEN;      
  mm[28].Type = MAD_RIEN;      
  mm[29].Type = MAD_RIEN;      
  mm[30].Type = MAD_RIEN;      
  mm[31].Type = MAD_RIEN;      
#endif

#ifdef _HARDWARE_2_
  mm[11].Type = MAD_RIEN;      
  mm[12].Type = MAD_RIEN;      
  mm[13].Type = MAD_RIEN;      
  mm[14].Type = MAD_RIEN;      
  mm[15].Type = MAD_RIEN;      
  mm[30].Type = MAD_RIEN;      
  mm[31].Type = MAD_RIEN;      
#endif
  
  
  mm[32].Type = MAD_ECHEL;      
  mm[33].Type = MAD_ECHEL;      
}


multiplexAD::~multiplexAD()
{
}

void multiplexAD::Setup()
{
  // Lit les controles et affecte les valeurs 
  for (inc_scan = 0; inc_scan < NB_AD; inc_scan++)
  {
    if ( mm[inc_scan].Type == MAD_POTAR && inc_scan < 32)
    {
      digitalWrite(ADDR_MIXAD_0, ( inc_scan & 0x01 ) );  
      digitalWrite(ADDR_MIXAD_1, ( inc_scan & 0x02 ) >> 1);  
      digitalWrite(ADDR_MIXAD_2, ( inc_scan & 0x04 ) >> 2);  
      digitalWrite(ADDR_MIXAD_3, ( inc_scan & 0x08 ) >> 3);
      digitalWrite(ADDR_MIXAD_4, ( inc_scan & 0x10 ) ?0:1);
      mm[inc_scan].Old_1 = mm[inc_scan].Old_2 = mm[inc_scan].Value = analogRead( A3 );
      mm[inc_scan].DeltaChange=DELTA_PAGE; // Comme une chgt de page...
      mm[inc_scan].incDeltaChange=0;
    }
    if ( inc_scan == 32 )
    {
      mm[inc_scan].Value = analogRead( A4 );
      refA4=0;
    }    
    if ( inc_scan == 33 )
    {
      mm[inc_scan].Value = analogRead( A5 );
      refA5=0;
    }    
  }
  inc_scan = 0;
}


void multiplexAD::IncAD()
{
  last_inc_scan = inc_scan;
  
  do { 
    inc_scan++;
    if (inc_scan>=NB_AD) inc_scan=0;
  }  while ( mm[inc_scan].Type == MAD_RIEN );

}

boolean  multiplexAD::ScanNextAD()
{
  boolean ret=false;
  int refvalue,  ref1;

  if ( inc_scan < 32 )
  { 
    digitalWrite(ADDR_MIXAD_0, ( inc_scan & 0x01 ) );  
    digitalWrite(ADDR_MIXAD_1, ( inc_scan & 0x02 ) >> 1);  
    digitalWrite(ADDR_MIXAD_2, ( inc_scan & 0x04 ) >> 2);  
    digitalWrite(ADDR_MIXAD_3, ( inc_scan & 0x08 ) >> 3);
    digitalWrite(ADDR_MIXAD_4, ( inc_scan & 0x10 )? 0 : 1 );
//    if ( inc_scan==0 )
//      refvalue =  ( analogRead( A3 ) + analogRead( A3 ) + analogRead( A3 ) ) / 3 ;
      refvalue =  ( analogRead( A3 ) + analogRead( A3 ) ) / 2 ;
//    else
//      refvalue =  analogRead( A3 );
  }

  if ( inc_scan == 32 )
  { 
//     refvalue =  ( analogRead( A4 ) + analogRead( A4 ) +  analogRead( A4 ) ) / 3;
     refvalue =  ( analogRead( A4 ) + analogRead( A4 )  ) / 2;
//     refvalue =  analogRead( A4 );
  }

  if ( inc_scan == 33 )
  { 
//     refvalue =  ( analogRead( A5 ) + analogRead( A5 ) +  analogRead( A5 ) ) / 3;
     refvalue =  ( analogRead( A5 ) + analogRead( A5 )  ) / 2;
//       refvalue =  analogRead( A5 );
  }

  if ( ( mm[inc_scan].Value - refvalue  > mm[inc_scan].DeltaChange ) || (  refvalue - mm[inc_scan].Value  > mm[inc_scan].DeltaChange ) )
  {

    if ( mm[inc_scan].Type == MAD_POTAR )
    {
      mm[inc_scan].Old_2 = mm[inc_scan].Old_1;
      mm[inc_scan].Old_1 = mm[inc_scan].Value;
      mm[inc_scan].Value= (mm[inc_scan].Old_2+mm[inc_scan].Old_1+refvalue*2) / 4 ;
      mm[inc_scan].DeltaChange=DELTA_ITEM;
      mm[inc_scan].incDeltaChange=0;
      ret=true;
    }
    else
    {
      // On dÃƒÂ©tecte un cght significatif 
        mm[inc_scan].Value= refvalue;
        mm[inc_scan].DeltaChange=DELTA_BTN;
        if ( inc_scan == 32)  refA4 = millis()+35;
        if ( inc_scan == 33)  refA5 = millis()+35;
    } 

    /*         
     if ( inc_scan == 0 )
     {
     Serial.print(mm[inc_scan].Old_3);
     Serial.print(" ");        
     Serial.print(mm[inc_scan].Old_2);
     Serial.print(" ");        
     Serial.print(mm[inc_scan].Old_1);
     Serial.print(" ");        
     Serial.print(mm[inc_scan].Value);
     Serial.print("\n");        
     }
     */
  }
  else
  {
    if (  mm[inc_scan].Type == MAD_POTAR )
    {
      mm[inc_scan].incDeltaChange++;
      if ( mm[inc_scan].incDeltaChange>=200) 
      {
        mm[inc_scan].incDeltaChange=0;  
        if ( mm[inc_scan].DeltaChange < DELTA_PAGE ) mm[inc_scan].DeltaChange++;
      }
    }
    else
    {
       if ( inc_scan==32 ) {
         if ( millis() > refA4 && refA4 > 0 )
         {
            mm[inc_scan].Value= refvalue;
            mm[inc_scan].DeltaChange=DELTA_BTN;
            mm[inc_scan].incDeltaChange=0;
            ret=true;
            refA4 = 0;
         }
       }
       if ( inc_scan==33 ) {
         if ( millis() > refA5 && refA5 > 0 )
         {
            mm[inc_scan].Value= refvalue;
            mm[inc_scan].DeltaChange=DELTA_BTN;
            mm[inc_scan].incDeltaChange=0;
            ret=true;
            refA5 = 0;
         }
       }
    }
  }
  IncAD();
  return ret;

}

void  multiplexAD::PageChanged()
{
  Setup();
}







