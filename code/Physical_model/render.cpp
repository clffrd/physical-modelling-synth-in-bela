/*____  _____ _        _
 | __ )| ____| |      / \
 |  _ \|  _| | |     / _ \
 | |_) | |___| |___ / ___ \
 |____/|_____|_____/_/   \_\
 http://bela.io
*/

#include <Bela.h>
#include "filters.h"
#include "kspluck.h"
#include "EKSpoly.h"
#include "utilites.h"
#include <libraries/Gui/Gui.h>
#include <libraries/Midi/Midi.h>
#include <cmath>
#include <vector>



//Controller numbers for different Parameters
enum {
	kMidiControllerPickPosition =21,
	kMidiControllerPickDirection =22,
	kMidiControllerBrightness =23,
	kMidiControllerDecayTime =24,
	kMidiControllerStringStiffness =25,
	kMidiControllerR1 = 26,
	kMidiControllerR2 = 27,
	kMidiControllerVib = 28,
	kMidiControllerProgram = 41,
	kMidiControllerAttack = 42,
	kMidiControllerRelease = 43,
	kMidiControllerOverBlow = 44,
};

Gui gui;
Midi gMidi;
const char* gMidiPort0 = "hw:1,0,0";
EKSpoly Synth;


//Parameter Initialization

float pitch = 220;
float DampeningFactor = 0.1f;
float L = 1;
unsigned int n = 0;
/*
Audio parameters in the params array as follows
*Pick Position
*Pick Direction
*Brightness 
*Decay Time
*String Stiffness
*R1
*R2
*vibrato
*Attack
*Release
*OverBlow
*/

float params[11] = {0,0,0.1,4,0.9,0.42,0.8,0.008,0.4,0.3,1};

//Midi cc messages mapped to their respective variables
void controlChange(int controller, int value)
{
	if (controller == kMidiControllerPickDirection)
	{
		params[0]= map(value, 0, 127, 0.1, 0.9);
	}
	
	if(controller == kMidiControllerPickPosition)
	{
		params[1]= map(value, 0, 127, 0.0, 1.0);
	}
	
	if (controller == kMidiControllerBrightness)
	{
		params[2]= map(value, 0, 127, 0, 1);
	}
	
		if (controller ==kMidiControllerDecayTime)
	{
		params[3]= map(value, 0, 127, 0.1, 10);
	}
	
	if(controller == kMidiControllerStringStiffness)
	{
		params[4]= map(value, 0, 127, 0.900000, 0.999999);
	}

	if(controller == kMidiControllerR1)
	{
		params[5] = map(value, 0, 127, 0.4, 0.8);
	}
	if(controller == kMidiControllerR2)
	{
		params[6] = map(value, 0, 127, 0.4, 0.8);
	}
	if(controller == kMidiControllerVib)
	{
		params[7] = map(value, 0, 127, 0.001, 0.05);
	}
	if(controller == kMidiControllerAttack)
	{
		params[8] = map(value, 0, 127, 0.1, 4);
	}
	if(controller == kMidiControllerRelease)
	{
		params[9] = map(value, 0, 127, 0.1, 4);
	}
	if(controller == kMidiControllerOverBlow)
	{
		params[10] = map(value, 0, 127, 1, 0.01);
	}
	if(controller == kMidiControllerProgram)
	{
		int programnumber = floor(map(value, 0,127, 1, 2.9));
		Synth.changeProgram(programnumber);
	}
}



bool setup(BelaContext *context, void *userData)
{
	// Initialise the MIDI device
	if(gMidi.readFrom(gMidiPort0) < 0) {
		rt_printf("Unable to read from MIDI port %s\n", gMidiPort0);
		return false;
	}
	gMidi.writeTo(gMidiPort0);
	gMidi.enableParser(true);
	Synth.setsr(context->audioSampleRate);
	return true;
}

void render(BelaContext *context, void *userData)

{
	float out = 0;
	while(gMidi.getParser()->numAvailableMessages() > 0) 
	{
		MidiChannelMessage message;
		message = gMidi.getParser()->getNextChannelMessage();
	// A MIDI "note on" message type might actually hold a real
	// note onset (e.g. key press), or it might hold a note off (key release).
	// The latter is signified by a velocity of 0.
		if(message.getType() == kmmNoteOn) 
		{
			int noteNumber = message.getDataByte(0);
			int velocity = message.getDataByte(1);
			// Velocity of 0 is really a note off
			Synth.updateLiveParams(params[0],params[1], params[2], params[3], params[4], params[5], params[6], params[7], params[8], params[9], params[10]);
			Synth.noteon(noteNumber, velocity);
			Synth.BuffResize();
		}

		else if(message.getType() == kmmNoteOff) 
		{
			// We can also encounter the "note off" message type which is the same
			// as "note on" with a velocity of 0.
			int noteNumber = message.getDataByte(0);
			Synth.noteoff(noteNumber);
		}
		
		else if(message.getType() == kmmControlChange)
		{
			//Control change messages 
			int controller = message.getDataByte(0);
			int value = message.getDataByte(1);
			controlChange(controller, value);
		}
	}
	

	for (unsigned int n = 0; n < context->audioFrames ; n++ )
	{
		//Write Generated samples to the output buffer
		out = Synth.playNote();
		audioWrite(context, n, 0, out);
		audioWrite(context, n, 1, out);
	}
}

void cleanup(BelaContext *context, void *userData)
{

}
