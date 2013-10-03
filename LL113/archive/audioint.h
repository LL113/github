#ifndef _Audioint_H_
#define _Audioint_H_


#include "Arduino.h"


#define AUDIBUF_AVANCE  256
#define AUDIBUF_TRAVAIL AUDIBUF_AVANCE*4
#define AUDIBUF_SIZE    AUDIBUF_TRAVAIL*2 

extern uint16_t volatile audiBuf[];
extern int volatile pt_audiBuf;
extern int volatile pt_sizeBufAudio;

extern int volatile pt_writebuf;
extern int volatile pt_upper_limite_write;
extern int volatile pt_limite_write;
extern boolean volatile pt_canwrite;
void adi_setup();


#define FX_SHIFT 12 
#define SHIFTED_1 4096

/** A resonant low pass filter for audio signals.
*/
class LowPassFilter
{

public:


	/** Constructor.
	*/
	LowPassFilter();


	/** Set the cut off frequency,
	@param cutoff use the range 0-255 to represent 0-8192 Hz (AUDIO_RATE/2).
	Be careful of distortion at the lower end, especially with high resonance.
	*/
	void setCutoffFreq(int32_t cutoff)
	{
		f = cutoff * 16;
		setFeedback();
	}


	/** Set the resonance.  If you hear unwanted distortion, back off the resonance.
	@param resonance in the range 0-255.
	*/
	void setResonance(int32_t resonance)
	{
		q = resonance * 16;
		setFeedback();
	}

	/** Calculate the next sample, given an input signal.
	@param in the signal input.
	@return the signal output.
	*/
	//16us
	inline int next(int in)
	{
		buf0+=fxmul(f,  ((in - buf0) + fxmul(fb, buf0-buf1)));
		buf1+=fxmul(f, buf0-buf1);
		return buf1;
	}


private:
	int32_t f;
	int32_t fb;
	int32_t q;
	int buf0,buf1;

	inline void setFeedback()
	{
		fb = q+fxmul(q, (int)SHIFTED_1 - f);
	}

	// convert an int into to its fixed representation
	inline int32_t fx(int32_t i)
	{
		return (i<<FX_SHIFT);
	}


	// // multiply two fixed point numbers (returns fixed point)
	// inline
	// long fxmul(long a, long b)
	// {
	// 	return (a*b)>>FX_SHIFT;
	// }

	// multiply two fixed point numbers (returns fixed point)
	inline int32_t fxmul(int32_t a, int32_t b)
	{
		return ((a*b)>>FX_SHIFT);
	}


};


#endif



