
// -----------------------------------------------
// LL113
// -----------------------------------------------
// Historique:
// (Baloran) laurent.lecatelier@free.fr, version 0.0.1  10/02/12013
// -----------------------------------------------
// Ajout des routes, du scan touches relachées, de LastNote
// Ajout des insertions, de l'historique, du flush


/*!
 *  @file		MIDIMul.h
 *  Project		MIDI Library option Multiples Serial & Routing
  *	Version		1.1
 *  @author		Laurent Lecatelier 
 *	@date		07/09/12
 *  License		Diesel

  Adaptation du remarquable travail de Francois Best, Midi Library
  
  http://arduino.cc/playground/Main/MIDILibrary
  
*/



#include <stdlib.h>
#include "Arduino.h"
#include "MIDIMul.h"

/*! \brief Main instance (the class comes pre-instantiated). */
// MIDIMul_Class MIDI1;

/*! \brief Default constructor for MIDIMul_Class. */
MIDIMul_Class::MIDIMul_Class()
{ 
	
#if USE_CALLBACKS
	
	// Initialise callbacks to NULL pointer
	mNoteOffCallback				= NULL;
	mNoteOnCallback					= NULL;
	mAfterTouchPolyCallback			= NULL;
	mControlChangeCallback			= NULL;
	mProgramChangeCallback			= NULL;
	mAfterTouchChannelCallback		= NULL;
	mPitchBendCallback				= NULL;
	mSystemExclusiveCallback		= NULL;
	mTimeCodeQuarterFrameCallback	= NULL;
	mSongPositionCallback			= NULL;
	mSongSelectCallback				= NULL;
	mTuneRequestCallback			= NULL;
	mClockCallback					= NULL;
	mStartCallback					= NULL;
	mContinueCallback				= NULL;
	mStopCallback					= NULL;
	mActiveSensingCallback			= NULL;
	mSystemResetCallback			= NULL;
	
#endif
	
	memset(&hNote,0,sizeof(histInNote));
	memset(&hOutNote,0,sizeof(histOutNote));

	AllIsOff=false;

//	LastNote=36;
	nMin=0;
	nMax=127;
	
	msSysEx=10;
}


/*! \brief Default destructor for MIDIMul_Class.
 
 This is not really useful for the Arduino, as it is never called...
 */
MIDIMul_Class::~MIDIMul_Class()
{

}


	
void MIDIMul_Class::setNoteMinMax(uint8_t pMin, uint8_t pMax)
{
	uint8_t nbRelach = 0;	

	nMin = pMin;
	nMax = pMax;

	for (int ct = 0; ct<HIST_NOTE_LENGTH;ct++)
	{
		if ( hNote.Note[ct] < nMin || hNote.Note[ct] > nMax ) 
		{
			hNote.Etat[ct] = 0 ;
		}
		if ( hNote.Etat[ct] == 0 ) nbRelach++;
	}
	if (nbRelach==HIST_NOTE_LENGTH) 
	{
		memset(&hNote,0,sizeof(histInNote));
	  AllIsOff=true;
  }
	
}



/*! \brief Call the begin method in the setup() function of the Arduino.
 
 All parameters are set to their default values:
 - Input channel set to 1 if no value is specified
 - Full thru mirroring
 */
void MIDIMul_Class::begin(USARTClass  *Hard)
{
	long unsigned Rate = MIDI_BAUDRATE;
	// Initialise the Serial port
	
	
	tempSendLast = millis();
	mustSendLast = false;
	
	ActivePort = Hard;
	
  ActivePort->begin(  Rate );
	
	
#if COMPILE_MIDI_OUT
	
#if USE_RUNNING_STATUS
	
	mRunningStatus_TX = InvalidType;
	
#endif // USE_RUNNING_STATUS
	
#endif // COMPILE_MIDI_OUT
	
	
#if COMPILE_MIDI_IN
	
	mRunningStatus_RX = InvalidType;
	mPendingMessageIndex = 0;
	mPendingMessageExpectedLenght = 0;
	
	mMessage.valid = false;
	mMessage.type = InvalidType;
	mMessage.channel = 0;
	mMessage.data1 = 0;
	mMessage.data2 = 0;
	
#endif // COMPILE_MIDI_IN
	
	
#if (COMPILE_MIDI_IN && COMPILE_MIDI_OUT && COMPILE_MIDI_THRU) // Thru
	
	mThruFilterMode = Full;
	mThruActivated = true;
	
#endif // Thru
	
}


#if COMPILE_MIDI_OUT

// Private method for generating a status uint8_t from channel and type
const uint8_t MIDIMul_Class::genstatus(const kMIDIType inType,
								 const uint8_t inChannel) const
{
	
	return ((uint8_t)inType | ((inChannel-1) & 0x0F));
	
}


/*! \brief Generate and send a MIDI message from the values given.
 \param type	The message type (see type defines for reference)
 \param data1	The first data uint8_t.
 \param data2	The second data uint8_t (if the message contains only 1 data uint8_t, set this one to 0).
 \param channel	The output channel on which the message will be sent (values from 1 to 16). Note: you cannot send to OMNI.
 
 This is an internal method, use it only if you need to send raw data from your code, at your own risks.
 */
void MIDIMul_Class::send(kMIDIType type,
					  uint8_t data1,
					  uint8_t data2,
					  uint8_t channel)
{
	

	if (type <= PitchBend) {
		// Channel messages
		
		// Protection: remove MSBs on data
		data1 &= 0x7F;
		data2 &= 0x7F;
		
		uint8_t statusuint8_t = genstatus(type,channel);
		
#if USE_RUNNING_STATUS
		// Check Running Status
		if (mRunningStatus_TX != statusuint8_t) {
			// New message, memorise and send header
			mRunningStatus_TX = statusuint8_t;
			ActivePort->write(mRunningStatus_TX);
		}
#else
		// Don't care about running status, send the Control uint8_t.
		ActivePort->write(statusuint8_t);
#endif
		
		// Then send data
		ActivePort->write(data1);
		if (type != ProgramChange && type != AfterTouchChannel) {
			ActivePort->write(data2);
		}
		return;
	}
	if (type >= TuneRequest && type <= SystemReset) {
		// System Real-time and 1 uint8_t.
		sendRealTime(type);
	}
	
}


/*! \brief Send a Note On message 
 \param NoteNumber	Pitch value in the MIDI format (0 to 127). Take a look at the values, names and frequencies of notes here: http://www.phys.unsw.edu.au/jw/notes.html\n
 \param Velocity	Note attack velocity (0 to 127). A NoteOn with 0 velocity is considered as a NoteOff.
 \param Channel		The channel on which the message will be sent (1 to 16). 
 */
void MIDIMul_Class::sendNoteOn(uint8_t NoteNumber,
							uint8_t Velocity,
							uint8_t Channel)
{ 
	
	send(NoteOn,NoteNumber,Velocity,Channel);

	for (int ct = 0; ct < HIST_NOTE_LENGTH; ct++ )
	{
		if ( hOutNote.Etat[ct] == 0 ) 
		{
			hOutNote.Etat[ct] = 1 ;
			hOutNote.Note[ct] = NoteNumber;
			hOutNote.Canal[ct] = Channel;
			break;
		}
	}

}

void MIDIMul_Class::Reset(byte rSet)
{
	    for(int ct=0;ct<HIST_NOTE_LENGTH;ct++)
	    { 
	      if ( hOutNote.Etat[ct] == 1  ) 
	      {
	        sendNoteOff(hOutNote.Note[ct], 0, hOutNote.Canal[ct] );
	      }
	    }
			memset(&hNote,0,sizeof(histInNote));
			
			if ( rSet > 0 )
			{	
				memset(&hOutNote,0,sizeof(histOutNote));
			  AllIsOff=true;
			}
}

void MIDIMul_Class::insertNoteOn(uint8_t NoteNumber,
							uint8_t Velocity,
							uint8_t Channel)
{ 
			mMessage.type = NoteOn;
			mMessage.channel = Channel;
			mMessage.data1 = NoteNumber ;
			mMessage.data2 = Velocity;
			mMessage.valid = true;

			ExecRoute();
			launchCallback();
			

}


void MIDIMul_Class::insertNoteOff(uint8_t NoteNumber,
							uint8_t Velocity,
							uint8_t Channel)
{ 
			mMessage.type = NoteOff;
			mMessage.channel = Channel;
			mMessage.data1 = NoteNumber ;
			mMessage.data2 = 0;
			mMessage.valid = true;

			ExecRoute();
			launchCallback();

}

/*! \brief Send a Note Off message (a real Note Off, not a Note On with null velocity)
 \param NoteNumber	Pitch value in the MIDI format (0 to 127). Take a look at the values, names and frequencies of notes here: http://www.phys.unsw.edu.au/jw/notes.html\n
 \param Velocity	Release velocity (0 to 127).
 \param Channel		The channel on which the message will be sent (1 to 16).
 */
void MIDIMul_Class::sendNoteOff(uint8_t NoteNumber,
							 uint8_t Velocity,
							 uint8_t Channel)
{
	
	send(NoteOff,NoteNumber,Velocity,Channel);

	for (int ct = 0; ct < HIST_NOTE_LENGTH; ct++ )
	{
		if ( hOutNote.Etat[ct] == 1 && hOutNote.Note[ct] == NoteNumber && hOutNote.Canal[ct] == Channel ) 
		{
			hOutNote.Etat[ct] = 0;
			break;
		}
	}

}


/*! \brief Send a Program Change message 
 \param ProgramNumber	The Program to select (0 to 127).
 \param Channel			The channel on which the message will be sent (1 to 16).
 */
void MIDIMul_Class::sendProgramChange(uint8_t ProgramNumber,
								   uint8_t Channel)
{
	
	send(ProgramChange,ProgramNumber,0,Channel);

}


/*! \brief Send a Control Change message 
 \param ControlNumber	The controller number (0 to 127). See the detailed description here: http://www.somascape.org/midi/tech/spec.html#ctrlnums
 \param ControlValue	The value for the specified controller (0 to 127).
 \param Channel			The channel on which the message will be sent (1 to 16). 
 */
void MIDIMul_Class::sendControlChange(uint8_t ControlNumber,
								   uint8_t ControlValue,
								   uint8_t Channel)
{
	
	send(ControlChange,ControlNumber,ControlValue,Channel);

}


/*! \brief ² a Polyphonic AfterTouch message (applies to only one specified note)
 \param NoteNumber		The note to apply AfterTouch to (0 to 127).
 \param Pressure		The amount of AfterTouch to apply (0 to 127).
 \param Channel			The channel on which the message will be sent (1 to 16). 
 */
void MIDIMul_Class::sendPolyPressure(uint8_t NoteNumber,
								  uint8_t Pressure,
								  uint8_t Channel)
{
	
	send(AfterTouchPoly,NoteNumber,Pressure,Channel);

}


/*! \brief Send a MonoPhonic AfterTouch message (applies to all notes)
 \param Pressure		The amount of AfterTouch to apply to all notes.
 \param Channel			The channel on which the message will be sent (1 to 16). 
 */
void MIDIMul_Class::sendAfterTouch(uint8_t Pressure,
								uint8_t Channel)
{
	
	send(AfterTouchChannel,Pressure,0,Channel);

}


/*! \brief Send a Pitch Bend message using a signed integer value.
 \param PitchValue	The amount of bend to send (in a signed integer format), between -8192 (maximum downwards bend) and 8191 (max upwards bend), center value is 0.
 \param Channel		The channel on which the message will be sent (1 to 16).
 */
void MIDIMul_Class::sendPitchBend(int PitchValue,
							   uint8_t Channel)
{
	
	unsigned int bend = PitchValue + 8192;
	sendPitchBend(bend,Channel);
	
}


/*! \brief Send a Pitch Bend message using an unsigned integer value.
 \param PitchValue	The amount of bend to send (in a signed integer format), between 0 (maximum downwards bend) and 16383 (max upwards bend), center value is 8192.
 \param Channel		The channel on which the message will be sent (1 to 16).
 */
void MIDIMul_Class::sendPitchBend(unsigned int PitchValue,
							   uint8_t Channel)
{
	
	send(PitchBend,(PitchValue & 0x7F),(PitchValue >> 7) & 0x7F,Channel);
	
}


/*! \brief Send a Pitch Bend message using a floating point value.
 \param PitchValue	The amount of bend to send (in a floating point format), between -1.0f (maximum downwards bend) and +1.0f (max upwards bend), center value is 0.0f.
 \param Channel		The channel on which the message will be sent (1 to 16).
 */
void MIDIMul_Class::sendPitchBend(double PitchValue,
							   uint8_t Channel)
{
	
	unsigned int pitchval = (PitchValue+1.f)*8192;
	if (pitchval > 16383) pitchval = 16383;		// overflow protection
	sendPitchBend(pitchval,Channel);
	
}


/*! \brief Generate and send a System Exclusive frame.
 \param length	The size of the array to send
 \param array	The uint8_t array containing the data to send
 \param ArrayContainsBoundaries  When set to 'true', 0xF0 & 0xF7 uint8_ts (start & stop SysEx) will NOT be sent (and therefore must be included in the array).
 default value is set to 'false' for compatibility with previous versions of the library.
 */
void MIDIMul_Class::sendSysEx(int length,
						   const uint8_t *const array,
						   bool ArrayContainsBoundaries)
{
	int i;
	unsigned long ref = millis();

	if ( ref - tempSendLast < msSysEx ) 
	{	
//		  Serial.write("Diffère\n");
			for (i=0;i<length;++i) 
			{
				mLastMessage[i]=array[i];
			}
			Lastlength=length;
			mustSendLast=true;
			return; 
	}
	
	if (ArrayContainsBoundaries == false) {
		
		ActivePort->write(0xF0);
		
		for (i=0;i<length;++i) {
			
			ActivePort->write(array[i]);
			
		}
		
		ActivePort->write(0xF7);

		mustSendLast = false;

//		Serial.write("Midi Ok\n");
		
	}
	else {
		
		tempSendLast = ref;

		for (i=0;i<length;++i) 
		{
			ActivePort->write(array[i]);
		}

		mustSendLast = false;

//		Serial.write("Midi Ok\n");
	
	}
	
#if USE_RUNNING_STATUS
	mRunningStatus_TX = InvalidType;
#endif
	
}

void MIDIMul_Class::SetSysExMs(uint8_t _msSysEx) 
{
	msSysEx = _msSysEx;
}


void MIDIMul_Class::FlushSysEx()
{
		if ( !mustSendLast || ( millis() - tempSendLast < msSysEx*2 ) ) return;
			
//		Serial.write("Ok Differé\n");
		
		for (int i=0;i<Lastlength;++i) {
			
			ActivePort->write(mLastMessage[i]);
			
		}
		mustSendLast = false;
		
}


/*! \brief Send a Tune Request message. 
 
 When a MIDI unit receives this message, it should tune its oscillators (if equipped with any) 
 */
void MIDIMul_Class::sendTuneRequest()
{
	
	sendRealTime(TuneRequest);

}


/*! \brief Send a MIDI Time Code Quarter Frame. 
 
 See MIDI Specification for more information.
 \param TypeNibble	MTC type
 \param ValuesNibble	MTC data
 */
void MIDIMul_Class::sendTimeCodeQuarterFrame(uint8_t TypeNibble, uint8_t ValuesNibble)
{
	
	uint8_t data = ( ((TypeNibble & 0x07) << 4) | (ValuesNibble & 0x0F) );
	sendTimeCodeQuarterFrame(data);
	
}


/*! \brief Send a MIDI Time Code Quarter Frame. 
 
 See MIDI Specification for more information.
 \param data	 if you want to encode directly the nibbles in your program, you can send the uint8_t here.
 */
void MIDIMul_Class::sendTimeCodeQuarterFrame(uint8_t data)
{
	
	ActivePort->write((uint8_t)TimeCodeQuarterFrame);
	ActivePort->write(data);

#if USE_RUNNING_STATUS
	mRunningStatus_TX = InvalidType;
#endif
	
}


/*! \brief Send a Song Position Pointer message.
 \param Beats	The number of beats since the start of the song.
 */
void MIDIMul_Class::sendSongPosition(unsigned int Beats)
{
	
	ActivePort->write((uint8_t)SongPosition);
	ActivePort->write(Beats & 0x7F);
	ActivePort->write((Beats >> 7) & 0x7F);

#if USE_RUNNING_STATUS
	mRunningStatus_TX = InvalidType;
#endif
	
}


/*! \brief Send a Song Select message */
void MIDIMul_Class::sendSongSelect(uint8_t SongNumber)
{
	
	ActivePort->write((uint8_t)SongSelect);
	ActivePort->write(SongNumber & 0x7F);

#if USE_RUNNING_STATUS
	mRunningStatus_TX = InvalidType;
#endif
	
}


/*! \brief Send a Real Time (one uint8_t) message. 
 
 \param Type The available Real Time types are: Start, Stop, Continue, Clock, ActiveSensing and SystemReset.
 You can also send a Tune Request with this method.
 @see kMIDIType
 */
void MIDIMul_Class::sendRealTime(kMIDIType Type)
{
	switch (Type) {
		case TuneRequest: // Not really real-time, but one uint8_t anyway.
		case Clock:
		case Start:
		case Stop:	
		case Continue:
		case ActiveSensing:
		case SystemReset:
			ActivePort->write((uint8_t)Type);
			break;
		default:
			// Invalid Real Time marker
			break;
	}
	
	// Do not cancel Running Status for real-time messages as they can be interleaved within any message.
	// Though, TuneRequest can be sent here, and as it is a System Common message, it must reset Running Status.
#if USE_RUNNING_STATUS
	if (Type == TuneRequest) mRunningStatus_TX = InvalidType;
#endif
	
}

#endif // COMPILE_MIDI_OUT



#if COMPILE_MIDI_IN

/*! Ajoute une route vers un autre port...

*/

void MIDIMul_Class::RouteAdd(MIDIMul_Class *Port, uint8_t Index, uint8_t Routing , unsigned int Channel, bool Activ )
{
	Route[RouteCount].MIDIClass = (void *)Port;
	Route[RouteCount].Index = Index;
	Route[RouteCount].Routing = Routing;
	Route[RouteCount].Canal = Channel;
	Route[RouteCount].Activ = Activ;
	RouteCount++;
}

void MIDIMul_Class::RouteChange(uint8_t Index, uint8_t Routing , unsigned int Channel, bool Activ )
{
	for(int ct=0;ct<RouteCount;ct++)
	{
		if ( Route[ct].Index == Index ) 
		{
			Route[ct].Routing = Routing;
			Route[ct].Canal = Channel;
			Route[ct].Activ = Activ;
			break;
		}
	}
}
		
void MIDIMul_Class::RouteActiv(uint8_t Index, bool Activ)
{
	for(int ct=0;ct<RouteCount;ct++)
	{
		if ( Route[ct].Index == Index ) 
		{
			Route[ct].Activ = Activ;
			break;
		}
	}
}

			
void MIDIMul_Class::ExecRoute()
{
	
	
	for(int ct=0;ct<RouteCount;ct++)
	{
		if ( Route[ct].Activ ) 
		{
	
			if (mMessage.type >= NoteOff && mMessage.type <= PitchBend &&  ( Route[ct].Routing & Route_Canal ) == Route_Canal ) // Type Channel
			{	
				if ( ( ( (unsigned int)1 << (mMessage.channel-1) ) &  Route[ct].Canal ) != 0 )
				{
					if ( ( Route[ct].Routing & Route_NoNote ) == Route_NoNote )
				 	{
						 if ( mMessage.type >= AfterTouchPoly ) ((MIDIMul_Class*)(Route[ct].MIDIClass))->send(mMessage.type,mMessage.data1,mMessage.data2,mMessage.channel);
					}
					else
					{
						((MIDIMul_Class*)(Route[ct].MIDIClass))->send(mMessage.type,mMessage.data1,mMessage.data2,mMessage.channel);
					}
				}
			}
			else {
				
				// Send the message to the output
				switch (mMessage.type) {
						// Real Time and 1 uint8_t
					case Clock:
					case Start:
					case Stop:
					case Continue:
					case ActiveSensing:
					case SystemReset:
					case TuneRequest:	
						if (( Route[ct].Routing & Route_Real ) == Route_Real ) ((MIDIMul_Class*)(Route[ct].MIDIClass))->sendRealTime(mMessage.type);
						break;
						
					case SystemExclusive:
						// Send SysEx (0xF0 and 0xF7 are included in the buffer)
						if (( Route[ct].Routing & Route_SysEx ) == Route_SysEx ) ((MIDIMul_Class*)(Route[ct].MIDIClass))->sendSysEx(mMessage.data1,mMessage.sysex_array,true); 
						break;
						
					case SongSelect:
						if (( Route[ct].Routing & Route_Real ) == Route_Real ) ((MIDIMul_Class*)(Route[ct].MIDIClass))->sendSongSelect(mMessage.data1);
						break;
						
					case SongPosition:
						if (( Route[ct].Routing & Route_Real ) == Route_Real ) ((MIDIMul_Class*)(Route[ct].MIDIClass))->sendSongPosition(mMessage.data1 | ((unsigned)mMessage.data2<<7));
						break;
						
					case TimeCodeQuarterFrame:
						if (( Route[ct].Routing & Route_Real ) == Route_Real ) ((MIDIMul_Class*)(Route[ct].MIDIClass))->sendTimeCodeQuarterFrame(mMessage.data1,mMessage.data2);
						break;
					default:
						break;
						
				}
				
			}
		}
	}
}
	
	
/*! \brief Reading/thru-ing method, the same as read() with a given input channel to read on. */
bool MIDIMul_Class::read()
{

	if ( parse() ) 
	{

			// Exécute les routes pour ce canal
			ExecRoute();
			
			launchCallback();
			
			return true;
	}
		
	
	return false;
	
}


bool MIDIMul_Class::parse_new()
{ 
	int peek = ActivePort->peek();
	if ( peek==-1 ) return false;

	const uint8_t extracted = (uint8_t)peek;
	
	
			
	switch (getTypeFromStatusuint8_t(extracted) )
	{
			
			// 1 uint8_t messages
		case Start:
		case Continue:
		case Stop:
		case Clock:
		case ActiveSensing:
		case SystemReset:
		case TuneRequest:
			// Handle the message type directly here.
			ActivePort->read();
			mMessage.type = getTypeFromStatusuint8_t(extracted);
			mMessage.channel = ( peek & 0x0F );
			mMessage.data1 = 0;
			mMessage.data2 = 0;
			mMessage.valid = true;
			return true;
			break;
			
			// 2 uint8_ts messages
		case ProgramChange:
		case AfterTouchChannel:
		case TimeCodeQuarterFrame:
		case SongSelect:
			if ( ActivePort->available()<2 ) return false;
			ActivePort->read();
			mMessage.type = getTypeFromStatusuint8_t(extracted);
			mMessage.channel = ( peek & 0x0F ) + 1;
			mMessage.data1 = ActivePort->read() ;
			mMessage.data2 = 0;
			mMessage.valid = true;
			return true;
			break;
			
			// 3 uint8_ts messages
		case NoteOn:
		case NoteOff:
		case ControlChange:
		case PitchBend:
		case AfterTouchPoly:
		case SongPosition:
			if ( ActivePort->available()<3 ) return false;
			ActivePort->read();
			mMessage.type = getTypeFromStatusuint8_t(extracted);
			mMessage.channel = ( peek & 0x0F ) + 1;
			mMessage.data1 = ActivePort->read() ;
			mMessage.data2 = ActivePort->read() ;
			mMessage.valid = true;
			return true;
			break;
			
		default:
			// This is obviously wrong. Let's get the hell out'a here.
			ActivePort->read();
			break;
	}
	
	return false;		
}  


// Private method: MIDI parser
bool MIDIMul_Class::parse()
{ 
	
	const int uint8_ts_available = ActivePort->available();
	
	if (uint8_ts_available <= 0) {
		// No data available.
		return false;
	}
	
	// If the buffer is full -> Don't Panic! Call the Vogons to destroy it.
	
	if ( uint8_ts_available == SERIAL_BUFFER_SIZE) {
		ActivePort->flush();
#ifdef _DEBUG_MODE_
  Serial.println(uint8_ts_available);
#endif
	}	
	else {
		
		/* Parsing algorithm:
		 Get a uint8_t from the serial buffer.
		 * If there is no pending message to be recomposed, start a new one.
		 - Find type and channel (if pertinent)
		 - Look for other uint8_ts in buffer, call parser recursively, until the message is assembled or the buffer is empty.
		 * Else, add the extracted uint8_t to the pending message, and check validity. When the message is done, store it.
		 */
		
		
		const uint8_t extracted = ActivePort->read();
		
		if (mPendingMessageIndex == 0) { // Start a new pending message
			mPendingMessage[0] = extracted;
			
						// Check for running status first
			switch (getTypeFromStatusuint8_t(mRunningStatus_RX)) {
					// Only these types allow Running Status:
				case NoteOff:
				case NoteOn:
				case AfterTouchPoly:
				case ControlChange:
				case ProgramChange:
				case AfterTouchChannel:
				case PitchBend:	
					
					// If the status uint8_t is not received, prepend it to the pending message
					if (extracted < 0x80) {
						mPendingMessage[0] = mRunningStatus_RX;
						mPendingMessage[1] = extracted;
						mPendingMessageIndex = 1;
					}
					// Else: well, we received another status uint8_t, so the running status does not apply here.
					// It will be updated upon completion of this message.
					
					break;
					
				default:
					// No running status
					break;
			}
			
			switch (getTypeFromStatusuint8_t(mPendingMessage[0])) {
					
					// 1 uint8_t messages
				case Start:
				case Continue:
				case Stop:
				case Clock:
				case ActiveSensing:
				case SystemReset:
				case TuneRequest:
					// Handle the message type directly here.
					mMessage.type = getTypeFromStatusuint8_t(mPendingMessage[0]);
					mMessage.channel = 0;
					mMessage.data1 = 0;
					mMessage.data2 = 0;
					mMessage.valid = true;
					
					// \fix Running Status broken when receiving Clock messages.
					// Do not reset all input attributes, Running Status must remain unchanged.
					//reset_input_attributes(); 
					
					// We still need to reset these
					mPendingMessageIndex = 0;
					mPendingMessageExpectedLenght = 0;
					
					return true;
					break;
					
					// 2 uint8_ts messages
				case ProgramChange:
				case AfterTouchChannel:
				case TimeCodeQuarterFrame:
				case SongSelect:
					mPendingMessageExpectedLenght = 2;
					break;
					
					// 3 uint8_ts messages
				case NoteOn:
				case NoteOff:
				case ControlChange:
				case PitchBend:
				case AfterTouchPoly:
				case SongPosition:
					mPendingMessageExpectedLenght = 3;
					break;
					
				case SystemExclusive:
					mPendingMessageExpectedLenght = MIDI_SYSEX_ARRAY_SIZE; // As the message can be any lenght between 3 and MIDI_SYSEX_ARRAY_SIZE uint8_ts
					mRunningStatus_RX = InvalidType;
					break;
					
				case InvalidType:
				default:
					// This is obviously wrong. Let's get the hell out'a here.
					reset_input_attributes();
					return false;
					break;
			}
			
			// Then update the index of the pending message.
			mPendingMessageIndex++;
			
#if USE_1uint8_t_PARSING
			// Message is not complete.
			return false;
#else
			// Call the parser recursively
			// to parse the rest of the message.
			return parse();
#endif
			
		}
		else { 
			
			// First, test if this is a status uint8_t
			if (extracted >= 0x80) {

				// Reception of status uint8_ts in the middle of an uncompleted message
				// are allowed only for interleaved Real Time message or EOX
				switch (getTypeFromStatusuint8_t(extracted)) {
					case Clock:
					case Start:
					case Continue:
					case Stop:
					case ActiveSensing:
					case SystemReset:
						
						/*
						 This is tricky. Here we will have to extract the one-uint8_t message,
						 pass it to the structure for being read outside the MIDI class,
						 and recompose the message it was interleaved into.
						 
						 Oh, and without killing the running status.. 
						 
						 This is done by leaving the pending message as is, it will be completed on next calls.
						 */
						
						mMessage.type = (kMIDIType)extracted;
						mMessage.data1 = 0;
						mMessage.data2 = 0;
						mMessage.channel = 0;
						mMessage.valid = true;
						return true;
						
						break;
						
						// End of Exclusive
					case 0xF7:
						if (getTypeFromStatusuint8_t(mPendingMessage[0]) == SystemExclusive) {
							
							// Store System Exclusive array in midimsg structure
							for (uint8_t i=0;i<MIDI_SYSEX_ARRAY_SIZE;i++) {
								mMessage.sysex_array[i] = mPendingMessage[i];
							}
							
							mMessage.type = SystemExclusive;

							// Get length
							mMessage.data1 = (mPendingMessageIndex+1) & 0xFF;	
							mMessage.data2 = (mPendingMessageIndex+1) >> 8;
							
							mMessage.channel = 0;
							mMessage.valid = true;
							
							reset_input_attributes();
							
							return true;
						}
						else {
							// Well well well.. error.
							reset_input_attributes();
							return false;
						}
						
						break;
						
					case ProgramChange:
					case AfterTouchChannel:
					case TimeCodeQuarterFrame:
					case SongSelect:
						mPendingMessage[0] = extracted;
						mPendingMessageExpectedLenght = 2;
						mPendingMessageIndex=1;
					  return parse();

						break;
					
						// 3 uint8_ts messages
					case NoteOn:
					case NoteOff:
					case ControlChange:
					case PitchBend:
					case AfterTouchPoly:
					case SongPosition:
						mPendingMessage[0] = extracted;
						mPendingMessageExpectedLenght = 3;
						mPendingMessageIndex=1;
					  return parse();

						break;
												
					default:
						break;
				}
				
				
				
			}
			
			
			// Add extracted data uint8_t to pending message
			mPendingMessage[mPendingMessageIndex] = extracted;
			
			
			// Now we are going to check if we have reached the end of the message
			if (mPendingMessageIndex >= (mPendingMessageExpectedLenght-1)) {
				
				// "FML" case: fall down here with an overflown SysEx..
				// This means we received the last possible data uint8_t that can fit the buffer.
				// If this happens, try increasing MIDI_SYSEX_ARRAY_SIZE.
				if (getTypeFromStatusuint8_t(mPendingMessage[0]) == SystemExclusive) {
					reset_input_attributes();
					return false;
				}
				
				
				mMessage.type = getTypeFromStatusuint8_t(mPendingMessage[0]);
				mMessage.channel = (mPendingMessage[0] & 0x0F)+1; // Don't check if it is a Channel Message
				
				mMessage.data1 = mPendingMessage[1];
				
				// Save data2 only if applicable
				if (mPendingMessageExpectedLenght == 3)	mMessage.data2 = mPendingMessage[2];
				else mMessage.data2 = 0;
				
				// Reset local variables
				mPendingMessageIndex = 0;
				mPendingMessageExpectedLenght = 0;
				
				mMessage.valid = true;
				
				// Activate running status (if enabled for the received type)
				switch (mMessage.type) {
					case NoteOff:
					case NoteOn:
					case AfterTouchPoly:
					case ControlChange:
					case ProgramChange:
					case AfterTouchChannel:
					case PitchBend:	
						// Running status enabled: store it from received message
						mRunningStatus_RX = mPendingMessage[0];
						break;
						
					default:
						// No running status
						mRunningStatus_RX = InvalidType;
						break;
				}
				return true;
			}
			else {
				// Then update the index of the pending message.
				mPendingMessageIndex++;
				
#if USE_1uint8_t_PARSING
				// Message is not complete.
				return false;
#else
				// Call the parser recursively
				// to parse the rest of the message.
				return parse();
#endif
				
			}
			
		}
		
	}
	
	// What are our chances to fall here?
	return false;
}



// Private method: reset input attributes
void MIDIMul_Class::reset_input_attributes()
{
	
	mPendingMessageIndex = 0;
	mPendingMessageExpectedLenght = 0;
	mRunningStatus_RX = InvalidType;
	
}


// Getters
/*! \brief Get the last received message's type
 
 Returns an enumerated type. @see kMIDIType
 */
kMIDIType MIDIMul_Class::getType() const
{
	
	return mMessage.type;

}


/*! \brief Get the channel of the message stored in the structure.
 
 Channel range is 1 to 16. For non-channel messages, this will return 0.
 */
uint8_t MIDIMul_Class::getChannel() const
{
	
	return mMessage.channel;

}


/*! \brief Get the first data uint8_t of the last received message. */
uint8_t MIDIMul_Class::getData1() const
{
	
	return mMessage.data1;

}


/*! \brief Get the second data uint8_t of the last received message. */
uint8_t MIDIMul_Class::getData2() const
{ 
	
	return mMessage.data2;

}


/*! \brief Get the System Exclusive uint8_t array. 
 
 @see getSysExArrayLength to get the array's length in uint8_ts.
 */
const uint8_t * MIDIMul_Class::getSysExArray() const
{ 
	
	return mMessage.sysex_array;

}

/*! \brief Get the lenght of the System Exclusive array.
 
 It is coded using data1 as LSB and data2 as MSB.
 \return The array's length, in uint8_ts.
 */
unsigned int MIDIMul_Class::getSysExArrayLength() const
{
	
	unsigned int coded_size = ((unsigned int)(mMessage.data2) << 8) | mMessage.data1;
	
	return (coded_size > MIDI_SYSEX_ARRAY_SIZE) ? MIDI_SYSEX_ARRAY_SIZE : coded_size;
	
}


/*! \brief Check if a valid message is stored in the structure. */
bool MIDIMul_Class::check() const
{ 
	
	return mMessage.valid;

}


#if USE_CALLBACKS

void MIDIMul_Class::setHandleNoteOff(void (*fptr)(uint8_t channel, uint8_t note, uint8_t velocity))			{ mNoteOffCallback = fptr; }
void MIDIMul_Class::setHandleNoteOn(void (*fptr)(uint8_t channel, uint8_t note, uint8_t velocity))			{ mNoteOnCallback = fptr; }
void MIDIMul_Class::setHandleAfterTouchPoly(void (*fptr)(uint8_t channel, uint8_t note, uint8_t pressure))	{ mAfterTouchPolyCallback = fptr; }
void MIDIMul_Class::setHandleControlChange(void (*fptr)(uint8_t channel, uint8_t number, uint8_t value))	{ mControlChangeCallback = fptr; }
void MIDIMul_Class::setHandleProgramChange(void (*fptr)(uint8_t channel, uint8_t number))				{ mProgramChangeCallback = fptr; }
void MIDIMul_Class::setHandleAfterTouchChannel(void (*fptr)(uint8_t channel, uint8_t pressure))			{ mAfterTouchChannelCallback = fptr; }
void MIDIMul_Class::setHandlePitchBend(void (*fptr)(uint8_t channel, int bend))						{ mPitchBendCallback = fptr; }
void MIDIMul_Class::setHandleSystemExclusive(void (*fptr)(uint8_t * array, uint8_t size))				{ mSystemExclusiveCallback = fptr; }
void MIDIMul_Class::setHandleTimeCodeQuarterFrame(void (*fptr)(uint8_t data))							{ mTimeCodeQuarterFrameCallback = fptr; }
void MIDIMul_Class::setHandleSongPosition(void (*fptr)(unsigned int beats))						{ mSongPositionCallback = fptr; }
void MIDIMul_Class::setHandleSongSelect(void (*fptr)(uint8_t songnumber))								{ mSongSelectCallback = fptr; }
void MIDIMul_Class::setHandleTuneRequest(void (*fptr)(void))										{ mTuneRequestCallback = fptr; }
void MIDIMul_Class::setHandleClock(void (*fptr)(void))												{ mClockCallback = fptr; }
void MIDIMul_Class::setHandleStart(void (*fptr)(void))												{ mStartCallback = fptr; }
void MIDIMul_Class::setHandleContinue(void (*fptr)(void))											{ mContinueCallback = fptr; }
void MIDIMul_Class::setHandleStop(void (*fptr)(void))												{ mStopCallback = fptr; }
void MIDIMul_Class::setHandleActiveSensing(void (*fptr)(void))										{ mActiveSensingCallback = fptr; }
void MIDIMul_Class::setHandleSystemReset(void (*fptr)(void))										{ mSystemResetCallback = fptr; }


/*! \brief Detach an external function from the given type.
 
 Use this method to cancel the effects of setHandle********.
 \param Type		The type of message to unbind. When a message of this type is received, no function will be called.
 */
void MIDIMul_Class::disconnectCallbackFromType(kMIDIType Type)
{
	
	switch (Type) {
		case NoteOff:               mNoteOffCallback = NULL;                break;
		case NoteOn:                mNoteOnCallback = NULL;                 break;
		case AfterTouchPoly:        mAfterTouchPolyCallback = NULL;         break;
		case ControlChange:         mControlChangeCallback = NULL;          break;
		case ProgramChange:         mProgramChangeCallback = NULL;          break;
		case AfterTouchChannel:     mAfterTouchChannelCallback = NULL;      break;
		case PitchBend:             mPitchBendCallback = NULL;              break;
		case SystemExclusive:       mSystemExclusiveCallback = NULL;        break;
		case TimeCodeQuarterFrame:  mTimeCodeQuarterFrameCallback = NULL;   break;
		case SongPosition:          mSongPositionCallback = NULL;           break;
		case SongSelect:            mSongSelectCallback = NULL;             break;
		case TuneRequest:           mTuneRequestCallback = NULL;            break;
		case Clock:                 mClockCallback = NULL;                  break;
		case Start:                 mStartCallback = NULL;                  break;
		case Continue:              mContinueCallback = NULL;               break;
		case Stop:                  mStopCallback = NULL;                   break;
		case ActiveSensing:         mActiveSensingCallback = NULL;          break;
		case SystemReset:           mSystemResetCallback = NULL;            break;
		default:
			break;
	}
	
}


// Private - launch callback function based on received type.
void MIDIMul_Class::launchCallback()
{
	uint8_t nbRelach = 0;	
	uint8_t	ct;
	// The order is mixed to allow frequent messages to trigger their callback faster.
	
	switch (mMessage.type) {
			// Notes
		case NoteOff:				
			if (mNoteOffCallback != NULL)				
					mNoteOffCallback(mMessage.channel,mMessage.data1,mMessage.data2);	
	
			if ( mMessage.data1 >= nMin && mMessage.data1 <= nMax )
			{
				hNote.Version++;
				for (ct = 0; ct<HIST_NOTE_LENGTH;ct++)
				{
					if ( hNote.Etat[ct] == 1 && hNote.Note[ct] == mMessage.data1 ) 
					{
						hNote.Etat[ct] = 0 ;
					}
					if ( hNote.Etat[ct] == 0 ) nbRelach++;
				}
				if (nbRelach==HIST_NOTE_LENGTH) 
				{
					memset(&hNote,0,sizeof(histInNote));
	 			  AllIsOff=true;
			  }
			}
			break;
					
					
		case NoteOn:				
				if (mNoteOnCallback != NULL)				
						mNoteOnCallback(mMessage.channel,mMessage.data1,mMessage.data2);	
						
						
				if ( mMessage.data1 >= nMin && mMessage.data1 <= nMax )
				{
//						LastNote=mMessage.data1 ;
				
						hNote.Version++;
		 			  AllIsOff=false;
						for (ct = 0; ct < HIST_NOTE_LENGTH; ct++ )
						{
							if ( hNote.Etat[ct] == 0 ) 
							{
								hNote.Etat[ct] = 1 ;
								hNote.Note[ct] = mMessage.data1;
								hNote.Velo[ct] = mMessage.data2 ;
								hNote.rTime[ct] = millis();
								break;
							}
						}
				}
				break;
			
			// Real-time messages
		case Clock:					if (mClockCallback != NULL)					mClockCallback();			break;			
		case Start:					if (mStartCallback != NULL)					mStartCallback();			break;
		case Continue:				if (mContinueCallback != NULL)				mContinueCallback();		break;
		case Stop:					if (mStopCallback != NULL)					mStopCallback();			break;
		case ActiveSensing:			if (mActiveSensingCallback != NULL)			mActiveSensingCallback();	break;
			
			// Continuous controllers
		case ControlChange:			if (mControlChangeCallback != NULL)			mControlChangeCallback(mMessage.channel,mMessage.data1,mMessage.data2);	break;
		case PitchBend:				if (mPitchBendCallback != NULL)				mPitchBendCallback(mMessage.channel,(int)((mMessage.data1 & 0x7F) | ((mMessage.data2 & 0x7F)<< 7)) - 8192);	break; // TODO: check this
		case AfterTouchPoly:		if (mAfterTouchPolyCallback != NULL)		mAfterTouchPolyCallback(mMessage.channel,mMessage.data1,mMessage.data2);	break;
		case AfterTouchChannel:		if (mAfterTouchChannelCallback != NULL)		mAfterTouchChannelCallback(mMessage.channel,mMessage.data1);	break;
			
		case ProgramChange:			if (mProgramChangeCallback != NULL)			mProgramChangeCallback(mMessage.channel,mMessage.data1);	break;
		case SystemExclusive:		if (mSystemExclusiveCallback != NULL)		mSystemExclusiveCallback(mMessage.sysex_array,mMessage.data1);	break;
			
			// Occasional messages
		case TimeCodeQuarterFrame:	if (mTimeCodeQuarterFrameCallback != NULL)	mTimeCodeQuarterFrameCallback(mMessage.data1);	break;
		case SongPosition:			if (mSongPositionCallback != NULL)			mSongPositionCallback((mMessage.data1 & 0x7F) | ((mMessage.data2 & 0x7F)<< 7));	break;
		case SongSelect:			if (mSongSelectCallback != NULL)			mSongSelectCallback(mMessage.data1);	break;
		case TuneRequest:			if (mTuneRequestCallback != NULL)			mTuneRequestCallback();	break;
			
		case SystemReset:			if (mSystemResetCallback != NULL)			mSystemResetCallback();	break;
		case InvalidType:
		default:
			break;
	}
	
}


#endif // USE_CALLBACKS


#endif // COMPILE_MIDI_IN






