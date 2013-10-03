
// -----------------------------------------------
// LL113
// -----------------------------------------------
// Historique:
// (Baloran) laurent.lecatelier@free.fr, version 0.0.1  10/02/12013
// -----------------------------------------------
// Ajout des routes, du scan touches relachées, de LastNote


/*!
 *  @file		MIDIMul.h
 *  Project		MIDI Library option Multiples Serial & Routing
  *	Version		1.1
 *  @author		Laurent Lecatelier 
 *	@date		07/09/12
 *  License		Diesel

  Simple adaptation du remarquable travail de Francois Best, Midi Library
  
  http://arduino.cc/playground/Main/MIDILibrary
  
*/


#ifndef LIB_MIDIMUL_H_
#define LIB_MIDIMUL_H_

#include <inttypes.h> 
#include "variant.h"

/*  
    ###############################################################
    #                                                             #
    #    CONFIGURATION AREA                                       #
    #                                                             #
    #    Here are a few settings you can change to customize      #
    #    the library for your own project. You can for example    #
    #    choose to compile only parts of it so you gain flash     #
    #    space and optimise the speed of your sketch.             #
    #                                                             #
    ###############################################################
 */


#define COMPILE_MIDI_IN         1           // Set this setting to 1 to use the MIDI input.
#define COMPILE_MIDI_OUT        1           // Set this setting to 1 to use the MIDI output. 

#define USE_RUNNING_STATUS	   	1		        // Running status enables short messages when sending multiple values
                                            // of the same type and channel.
                                            // Set to 0 if you have troubles with controlling you hardware.


#define USE_CALLBACKS           1           // Set this to 1 if you want to use callback handlers (to bind your functions to the library).
                                            // To use the callbacks, you need to have COMPILE_MIDI_IN set to 1

#define USE_1uint8_t_PARSING    1          // Each call to MIDI.read will only parse one uint8_t (might be faster).


// END OF CONFIGURATION AREA 
// (do not modify anything under this line unless you know what you are doing)


#define MIDI_BAUDRATE			31250L

#define MIDI_SYSEX_ARRAY_SIZE	255			// Maximum size is 65535 uint8_ts.


/*! Enumeration of MIDI types */
enum kMIDIType {
	NoteOff	              = 0x80,	///< Note Off
	NoteOn                = 0x90,	///< Note On
	AfterTouchPoly        = 0xA0,	///< Polyphonic AfterTouch
	ControlChange         = 0xB0,	///< Control Change / Channel Mode
	ProgramChange         = 0xC0,	///< Program Change
	AfterTouchChannel     = 0xD0,	///< Channel (monophonic) AfterTouch
	PitchBend             = 0xE0,	///< Pitch Bend
	SystemExclusive       = 0xF0,	///< System Exclusive
	TimeCodeQuarterFrame  = 0xF1,	///< System Common - MIDI Time Code Quarter Frame
	SongPosition          = 0xF2,	///< System Common - Song Position Pointer
	SongSelect            = 0xF3,	///< System Common - Song Select
	TuneRequest           = 0xF6,	///< System Common - Tune Request
	Clock                 = 0xF8,	///< System Real Time - Timing Clock
	Start                 = 0xFA,	///< System Real Time - Start
	Continue              = 0xFB,	///< System Real Time - Continue
	Stop                  = 0xFC,	///< System Real Time - Stop
	ActiveSensing         = 0xFE,	///< System Real Time - Active Sensing
	SystemReset           = 0xFF,	///< System Real Time - System Reset
	InvalidType           = 0x00    ///< For notifying errors
};


/*! Enumeration Route  */
enum kRoute {
	Route_None            = 0x00,  
	Route_SysEx           = 0x01,  
	Route_Real            = 0x02,
	Route_Canal           = 0x04,  
	Route_All             = 0x07,   
	Route_NoNote          = 0x08   
};

/*! Enumeration Channel  */
enum kCanal {
	Can_All               = 0xFFFF,  
	Can_None              = 0x0000,  
	Can_1            	  = 0x0001,
	Can_2            	  = 0x0002,
	Can_3            	  = 0x0004,
	Can_4            	  = 0x0008,
	Can_5            	  = 0x0010,
	Can_6            	  = 0x0020,
	Can_7            	  = 0x0040,
	Can_8            	  = 0x0080,
	Can_9            	  = 0x0100,
	Can_10            	  = 0x0200,
	Can_11            	  = 0x0400,
	Can_12           	  = 0x0800,
	Can_13           	  = 0x1000,
	Can_14           	  = 0x2000,
	Can_15           	  = 0x4000,
	Can_16           	  = 0x8000
};

/*! The midimsg structure contains decoded data of a MIDI message read from the serial port with read() or thru(). \n */
struct midimsg {
	/*! The MIDI channel on which the message was recieved. \n Value goes from 1 to 16. */
	uint8_t channel; 
	/*! The type of the message (see the define section for types reference) */
	kMIDIType type;
	/*! The first data uint8_t.\n Value goes from 0 to 127.\n */
	uint8_t data1;
	/*! The second data uint8_t. If the message is only 2 uint8_ts long, this one is null.\n Value goes from 0 to 127. */
	uint8_t data2;
	/*! System Exclusive dedicated uint8_t array. \n Array length is stocked on 16 bits, in data1 (LSB) and data2 (MSB) */
	uint8_t sysex_array[MIDI_SYSEX_ARRAY_SIZE];
	/*! This boolean indicates if the message is valid or not. There is no channel consideration here, validity means the message respects the MIDI norm. */
	bool valid;
};




/*! \brief The main class for MIDI handling.\n
	See member descriptions to know how to use it,
	or check out the examples supplied with the library.
 */
 

 
struct strRoute {
	void *MIDIClass; 
	uint8_t Index;
	uint8_t Routing;
	uint16_t Canal;
	bool Activ;
};

#define HIST_NOTE_LENGTH 16

struct histInNote {
	uint32_t	 Version; 
	uint8_t 	 Etat[HIST_NOTE_LENGTH];
	uint8_t    Note[HIST_NOTE_LENGTH];
	uint8_t  	 Velo[HIST_NOTE_LENGTH];
	uint8_t  	 Canal[HIST_NOTE_LENGTH];
  uint32_t	 rTime[HIST_NOTE_LENGTH];
};


struct histOutNote {
	uint8_t 	 Etat[HIST_NOTE_LENGTH];
	uint8_t    Note[HIST_NOTE_LENGTH];
	uint8_t  	 Canal[HIST_NOTE_LENGTH];
};


class MIDIMul_Class {
	
	
public:
	// Constructor and Destructor
	MIDIMul_Class();
	~MIDIMul_Class();
	
	
	void begin(USARTClass *Hard);
	
	
	
	
/* ####### OUTPUT COMPILATION BLOCK ####### */	
#if COMPILE_MIDI_OUT

public:	
	
	void sendNoteOn(uint8_t NoteNumber,uint8_t Velocity,uint8_t Channel);
	void sendNoteOff(uint8_t NoteNumber,uint8_t Velocity,uint8_t Channel);
	void sendProgramChange(uint8_t ProgramNumber,uint8_t Channel);
	void sendControlChange(uint8_t ControlNumber, uint8_t ControlValue,uint8_t Channel);
	void sendPitchBend(int PitchValue,uint8_t Channel);
	void sendPitchBend(unsigned int PitchValue,uint8_t Channel);
	void sendPitchBend(double PitchValue,uint8_t Channel);
	void sendPolyPressure(uint8_t NoteNumber,uint8_t Pressure,uint8_t Channel);
	void sendAfterTouch(uint8_t Pressure,uint8_t Channel);
	void sendSysEx(int length, const uint8_t *const array,bool ArrayContainsBoundaries = false);	
	void sendTimeCodeQuarterFrame(uint8_t TypeNibble, uint8_t ValuesNibble);
	void sendTimeCodeQuarterFrame(uint8_t data);
	void sendSongPosition(unsigned int Beats);
	void sendSongSelect(uint8_t SongNumber);
	void sendTuneRequest();
	void sendRealTime(kMIDIType Type);
	
	void send(kMIDIType type, uint8_t param1, uint8_t param2, uint8_t channel);
	
	histInNote hNote;
	histOutNote hOutNote;
	
	bool AllIsOff;


//	uint8_t LastNote;

	uint8_t nMin;
	uint8_t nMax;

	void setNoteMinMax(uint8_t nMin, uint8_t nMax) ;
	void insertNoteOn(uint8_t NoteNumber,		uint8_t Velocity, uint8_t Channel);
	void insertNoteOff(uint8_t NoteNumber,		uint8_t Velocity, uint8_t Channel);
	void Reset(byte);
	void SetSysExMs(uint8_t) ;
	
	
private:
	
	const uint8_t genstatus(const kMIDIType inType,const uint8_t inChannel) const;
	
	
	// Attributes
#if USE_RUNNING_STATUS
	uint8_t			mRunningStatus_TX;
#endif // USE_RUNNING_STATUS

#endif	// COMPILE_MIDI_OUT
	

	
/* ####### INPUT COMPILATION BLOCK ####### */
#if COMPILE_MIDI_IN	
	
public:
	
	bool read();
	
	// Getters
	kMIDIType getType() const;
	uint8_t getChannel() const;
	uint8_t getData1() const;
	uint8_t getData2() const;
	const uint8_t * getSysExArray() const;
	unsigned int getSysExArrayLength() const;
	bool check() const;
	
	/*! \brief Extract an enumerated MIDI type from a status uint8_t.
	 
	 This is a utility static method, used internally, made public so you can handle kMIDITypes more easily.
	 */
	static inline const kMIDIType getTypeFromStatusuint8_t(const uint8_t inStatus) 
    {
		if ((inStatus < 0x80) 
			|| (inStatus == 0xF4) 
			|| (inStatus == 0xF5) 
			|| (inStatus == 0xF9) 
			|| (inStatus == 0xFD)) return InvalidType; // data uint8_ts and undefined.
		if (inStatus < 0xF0) return (kMIDIType)(inStatus & 0xF0);	// Channel message, remove channel nibble.
		else return (kMIDIType)inStatus;
	}
	

	void RouteAdd(MIDIMul_Class *Port, uint8_t Index, uint8_t Routing, unsigned int Channel, bool Activ );
	void RouteChange(uint8_t Index, uint8_t Routing, unsigned int Channel, bool Activ  );
	void RouteActiv(uint8_t Index, bool Activ );
	void ExecRoute();
	void FlushSysEx();
	
#if USE_CALLBACKS
	
	void setHandleNoteOff(void (*fptr)(uint8_t channel, uint8_t note, uint8_t velocity));
	void setHandleNoteOn(void (*fptr)(uint8_t channel, uint8_t note, uint8_t velocity));
	void setHandleAfterTouchPoly(void (*fptr)(uint8_t channel, uint8_t note, uint8_t pressure));
	void setHandleControlChange(void (*fptr)(uint8_t channel, uint8_t number, uint8_t value));
	void setHandleProgramChange(void (*fptr)(uint8_t channel, uint8_t number));
	void setHandleAfterTouchChannel(void (*fptr)(uint8_t channel, uint8_t pressure));
	void setHandlePitchBend(void (*fptr)(uint8_t channel, int bend));
	void setHandleSystemExclusive(void (*fptr)(uint8_t * array, uint8_t size));
	void setHandleTimeCodeQuarterFrame(void (*fptr)(uint8_t data));
	void setHandleSongPosition(void (*fptr)(unsigned int beats));
	void setHandleSongSelect(void (*fptr)(uint8_t songnumber));
	void setHandleTuneRequest(void (*fptr)(void));
	void setHandleClock(void (*fptr)(void));
	void setHandleStart(void (*fptr)(void));
	void setHandleContinue(void (*fptr)(void));
	void setHandleStop(void (*fptr)(void));
	void setHandleActiveSensing(void (*fptr)(void));
	void setHandleSystemReset(void (*fptr)(void));
	
	void disconnectCallbackFromType(kMIDIType Type);
	
#endif // USE_CALLBACKS
	
	
private:
	
	bool parse_new();

	bool parse();

	void reset_input_attributes();
	
	// Attributes
	uint8_t			mRunningStatus_RX;
	uint8_t			mInputChannel;
	
	USARTClass  *ActivePort;
	uint8_t 			RouteCount;
	strRoute 		Route[4];
	
	
	uint8_t			mLastMessage[MIDI_SYSEX_ARRAY_SIZE];
	int 				Lastlength;
	bool        mustSendLast;

	unsigned long     tempSendLast;
	uint8_t						msSysEx;
	
	uint8_t			mPendingMessage[MIDI_SYSEX_ARRAY_SIZE];
	
	unsigned int	mPendingMessageExpectedLenght;
	unsigned int	mPendingMessageIndex;					// Extended to unsigned int for larger sysex payloads.
	
	midimsg			mMessage;
	
	
#if USE_CALLBACKS
	
	void launchCallback();
	
	void (*mNoteOffCallback)(uint8_t channel, uint8_t note, uint8_t velocity);
	void (*mNoteOnCallback)(uint8_t channel, uint8_t note, uint8_t velocity);
	void (*mAfterTouchPolyCallback)(uint8_t channel, uint8_t note, uint8_t velocity);
	void (*mControlChangeCallback)(uint8_t channel, uint8_t, uint8_t);
	void (*mProgramChangeCallback)(uint8_t channel, uint8_t);
	void (*mAfterTouchChannelCallback)(uint8_t channel, uint8_t);
	void (*mPitchBendCallback)(uint8_t channel, int);
	void (*mSystemExclusiveCallback)(uint8_t * array, uint8_t size);
	void (*mTimeCodeQuarterFrameCallback)(uint8_t data);
	void (*mSongPositionCallback)(unsigned int beats);
	void (*mSongSelectCallback)(uint8_t songnumber);
	void (*mTuneRequestCallback)(void);
	void (*mClockCallback)(void);
	void (*mStartCallback)(void);
	void (*mContinueCallback)(void);
	void (*mStopCallback)(void);
	void (*mActiveSensingCallback)(void);
	void (*mSystemResetCallback)(void);
	
#endif // USE_CALLBACKS
	
	
#endif // COMPILE_MIDI_IN
	


};

extern MIDIMul_Class MIDI;

#endif // LIB_MIDI_H_
