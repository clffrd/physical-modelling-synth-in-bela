/***** woodwind.cpp *****/
/*This Module contains the implementation of the digital waveguide woodwind model*/
#include "woodwind.h"
#include <stdlib.h>
#include <filters.h>
#include <Bela.h>


//Initialize the woodwind model variables
woodwind::woodwind()
{
	ringBufferTwo.resize(1500,0);
	ringBufferOne.resize(1500,0);
	breath.resize(3000, 0);
	vib.resize(3000, 0);
	overBlow = 1;
	BufferSize = 110;
	dl1size = 55;
	dl2size = 22;
	dl1frac = 0;
	dl2frac = 0;
	ringBufferOneOut = 0;
	dl1ptr = dl1size;
	dl2ptr = dl2size;
	dl1outptr = 0;
	dl2outptr = 0;
	prevOut = 0;
	dc_filter_out = 0;
	dc_filter_in = 0;
	reedIn = 0;
	samples = 0;
	delta = 0;
	l = 1;
	vibDepth = 4.5;
	gain = 0;
}

//fill the input buffer with noise to drive the flute model.
void woodwind::blow()
{
	arEnv.trigger();
	for (int i = 0; i<breath.size(); i ++)
	{
		breath[i] = (((float)rand())/((float)RAND_MAX)*2.0f - 1.0f );
		vib[i] = sin(2*M_PI*4.5*i/sr);
		breath[i] = (0.0085*breath[i]+vibDepth*vib[i]);
	}
}

//set sample rate for the woodwind model
void woodwind::setsr(float in)
{
	sr = in;
	arEnv.setsr(sr);
	allPassTune[0].setsr(sr);
	allPassTune[1].setsr(sr);
	Level.setsr(sr);
}

//set input frequency based on the midi note input in all filters used in the woodwind model
// Calculate fractional delay for the two delay lines
void woodwind::setfr(float in)
{
	fr = in;
	allPassTune[0].setfr(fr);
	allPassTune[1].setfr(fr);
	Level.setfr(fr);
	dl1frac = (sr/fr)/2 - floor((sr/fr)/2);
	dl2frac = (sr/fr)/4 - floor((sr/fr)/4);

}

//Set input velocity to the dynamic level filter

void woodwind::setl(float in)
{
	l = in;	
	Level.setl(l);
}

//Set Buffer size to change size of the delay lines to alter the pitch of the note
void woodwind::setBufferSize(unsigned int in)
{
	BufferSize = in;
	dl1size = BufferSize/2 - 2;
	dl2size = BufferSize/4 - 2;
	BufferSize = dl1size + dl2size;

}


//This implements the bi directional digital waveguide used in the woodwind model
//dl1 is the jet reed where breath input happens
//dl2 is the acoustic tube of the instrument. 
//Two tuning filters, one for each delay line. 
//breath Input + flow envelope + vibrato ---+--> jet dl2-->x - x^3 --+--> reflection low pass filter ----> audio out
//											^					     ^							     |												
//											|						 |						         |											
//											|<------------------------acoustic tube dl1<-------------|
float woodwind::playflute()
{
	
	if(isNoteOn == true)
	{
		gain = arEnv.attack();
	}
	if(isNoteOn == false)
	{
		gain = arEnv.release();
	}
	if(samples>=(BufferSize*overBlow))
	{
		samples = 0;
	}
	//Jet Delay Line
	reedIn = breath[samples];
	float dynamic_level = Level.DynamicLevel(reedIn, l);
	samples++;
	ringBufferTwo[dl2ptr] = dynamic_level + r1*ringBufferOneOut;
	float tunedOut1 = allPassTune[0].lagrangeInterPolation(ringBufferTwo[dl2ptr],dl2frac+2);
	ringBufferTwo[dl2outptr] = tunedOut1;
	dl2ptr = dl2ptr + 1;
	if(dl2ptr > dl2size)
	{
		dl2ptr = 0;
	}
	dl2outptr = dl2outptr + 1;
	if(dl2outptr > dl2size)
	{
		dl2outptr = 0;
	}
	float reedOut = ringBufferTwo[dl2outptr];
	ringBufferTwo[dl2outptr] = 0;
	reedOut = reedOut - (pow(reedOut,3));
	reedOut = reedOut + r2*ringBufferOneOut;
	float fluteOut = prevOut + filterCoeff*(reedOut-prevOut);
	prevOut = fluteOut;
	float tunedOut2 =allPassTune[1].lagrangeInterPolation( fluteOut,dl1frac+2);
	//flute tube delay line
	ringBufferOne[dl1ptr] = tunedOut2;
	dl1outptr = dl1outptr + 1;
	if (dl1outptr > dl1size)
	{
		dl1outptr = 0;
	}
	dl1ptr = dl1ptr + 1;
	if (dl1ptr > dl1size)
	{
		dl1ptr = 0;
	}
	ringBufferOneOut = ringBufferOne[dl1outptr];
	//Dc Filter
	dc_filter_out = fluteOut - dc_filter_in + (DCfilterGain*dc_filter_out);
	dc_filter_in = fluteOut;
	float out = gain*dc_filter_out;
	return out;
}