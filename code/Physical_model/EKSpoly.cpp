/***** EKSpoly.cpp *****/
/*
*Polyphony manager with maximum 7 note polyphony due to CPU usage, and two selectable sound programs
*/
#include "EKSpoly.h"
#include "utilites.h"

//Initialization constructor
EKSpoly::EKSpoly() 
{
	paramread = 0;
	maxVoices =7;
	voiceNumber = 0;
	amountOn = 0;
	program = 2;
	eks.reserve(maxVoices);
	flute.reserve(maxVoices);
	for(int i = 0; i < maxVoices; i++) 
	  {
         eks.push_back(new kspluck);
         flute.push_back(new woodwind);
         lastPressed[i] = 0;
      }
}

//Update vector size corresponding to the input midi note
/*
* on note input calculate buffer size using samplerate/frequency,  
* convert it to an unsigned integer value
* and set the buffer size in the selected model to change the pitch
*/
void EKSpoly::BuffResize()
{
	double size = (sr_/fr_);	
	if(program == 1)
	{
	unsigned int buffsize = static_cast<unsigned int>(size - (static_cast<int>(eks[voiceNumber] ->delta)));
	eks[voiceNumber]-> setBufferSize(buffsize);
	}
	if(program == 2)
	{
	unsigned int buffsize = static_cast<unsigned int>(size);
	flute[voiceNumber]-> setBufferSize(buffsize);
	}
}

//Set sample rate
void EKSpoly::setsr(float k)
{
		sr_ = k;
		for(int i=0; i<maxVoices;i++)
		{
		eks[i] ->setsr(k);
		flute[i] ->setsr(k);
		}
}


//Note On message
/*
*Keep Count of how many notes have been pressed. 
*Set frequency , velocity and fill the noise buffer in the selected program
*/
void EKSpoly::noteon(float note, float velocity) 
{
	amountOn++;
	float fr = pow(2, (note - 69)/12)*440;
	float l = velocity/127;
	fr_= fr;
	l_ = l;
	lastPressed[voiceNumber]=note;
	if(program == 1)
	{
	eks[voiceNumber]-> isNoteOn = true;
	eks[voiceNumber]-> setfr(fr);
	eks[voiceNumber]-> setl(l);
	eks[voiceNumber]-> kstrigger();
	}
	if(program == 2)
	{
	flute[voiceNumber]-> isNoteOn = true;
	flute[voiceNumber]-> setfr(fr);
	flute[voiceNumber]-> setl(l);
	flute[voiceNumber]-> blow();
	}
}

//Noteoff message
/*
*If Note is off replace the position in lastpressed array with zero to inform the polyphony manager 
*that there is a free voicing avaiable and set isNoteOn boolean to false
*
*/
void EKSpoly::noteoff(float note)
{
	amountOn --;
	for(int i= 0; i<7; i++)
	{
		if(lastPressed[i]==note)
		{
			lastPressed[i] = 0;
			if(program == 1)
			{
				eks[i] -> isNoteOn = false;
			}
			if(program == 2)
			{
				flute[i] -> isNoteOn = false;
			}
		}
	}
}


//Generate samples function that generates sound inside the render function
float EKSpoly::playNote()
{
	float out = 0;
	for(int i= 0; i<maxVoices; i++)
	{
		if(program == 1)
		{
		out += eks[i]->pluck();
		}
		if(program == 2)
		{
		out += flute[i]->playflute();
		}
	}
	return out;	
}

//Change program
/*
*0 for Extended Karplus Strong Model
*1 for jet-woodwind model
*/
void EKSpoly::changeProgram(int in)
{
	program = in;
}

//Update live parameters to control various aspects of the model
/*
*on note input look for empty spaces in the lastpressed array which would denote that there is an available voice. 
*if all voices are full, simply warp around the index 0 and replace the first voicing
*update required audio parameters to respective programs.
*/
void EKSpoly::updateLiveParams(float pickDir, float pickPos, float Dampen, float T60, float stiff, float ref1, float ref2, float vibratoDepth, float attack, float release, float ovrblw)
{
			for(int i = 0; i<maxVoices; i++)
			{
			if(lastPressed[i] == 0)
			{ 
			  voiceNumber = i;
			  break;
			}
			if(amountOn >= maxVoices)
			{
				voiceNumber = (voiceNumber + 1)%maxVoices;
			}
			}
			if(program == 1)
			{
			    eks[voiceNumber]-> updateLiveParams_(pickDir,pickPos,Dampen,T60,stiff);
			}
			if(program == 2)
			{
				flute[voiceNumber] -> updateLiveParams_(ref1, ref2, vibratoDepth, Dampen, T60, attack, release, ovrblw);
			}
}