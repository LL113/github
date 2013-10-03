#include "audioint.h"

uint16_t volatile audiBuf[AUDIBUF_SIZE];

int volatile pt_audiBuf = 0;
int volatile pt_upper_limite_write = AUDIBUF_AVANCE; 
int volatile pt_limite_write = AUDIBUF_AVANCE;
boolean volatile pt_canwrite = false;
int volatile pt_sizeBufAudio = AUDIBUF_AVANCE;

void adi_setup()
{
  /* turn on the timer clock in the power management controller */

  pt_sizeBufAudio = AUDIBUF_AVANCE;

  pmc_set_writeprotect(false);
  pmc_enable_periph_clk(ID_TC4);

  /* we want wavesel 01 with RC */
  TC_Configure(/* clock */TC1,/* channel */1, TC_CMR_WAVE | TC_CMR_WAVSEL_UP_RC | TC_CMR_TCCLKS_TIMER_CLOCK2);
  TC_SetRC(TC1,1,256); // sets <> 44.1 Khz interrupt rate
  TC_Start(TC1, 1);
  
  // enable timer interrupts on the timer 
  TC1->TC_CHANNEL[1].TC_IER=TC_IER_CPCS;
  TC1->TC_CHANNEL[1].TC_IDR=~TC_IER_CPCS;
  
  /* Enable the interrupt in the nested vector interrupt controller */
  /* TC4_IRQn where 4 is the timer number * timer channels (3) + the channel number (=(1*3)+1) for timer1 channel1 */
  NVIC_EnableIRQ(TC4_IRQn);

  // this is a cheat - enable the DAC
  analogWrite(DAC0,0);

}

extern uint32_t prob_cpt;

 
void TC4_Handler()
{
  // We need to get the status to clear it and allow the interrupt to fire again
  TC_GetStatus(TC1, 1);
  
  uint32_t ulOutput = audiBuf[pt_audiBuf++];
  if ( pt_audiBuf == AUDIBUF_SIZE ) pt_audiBuf=0;
  
  if ( pt_audiBuf == pt_limite_write ) 
  {
                if ( !pt_canwrite )
                {
        		pt_limite_write = pt_upper_limite_write;
        		pt_upper_limite_write = pt_limite_write + pt_sizeBufAudio;
        		if ( pt_upper_limite_write >= AUDIBUF_SIZE ) pt_upper_limite_write-=AUDIBUF_SIZE;
        		pt_canwrite = true;
                }
/*                
                else
                {
         		pt_canwrite = true;
                }
*/                
  }
  
 dacc_write_conversion_data(DACC_INTERFACE, ulOutput);

//digitalWrite( 40, (prob_cpt++ & 1) );

}


LowPassFilter::LowPassFilter()
{
	;
}




