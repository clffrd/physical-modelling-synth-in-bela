/***** kspluck.cpp *****/
/*This module contains the Extended karplus strong model implementation*/
#include <cmath>
#include <vector>
#include <algorithm>
#include <cstring>
#include <Bela.h>
#include<kspluck.h>
#include<filters.h>


//Initialize Extended karplus Strong model variables
kspluck::kspluck() 

{
	BufferSize = 200;
	DelayLines.resize(2000, 0.0);
	DelayLines.reserve(10000);
	gLastOutput = 0;
	gLastTuningOutput = 0;
	gLastDynamicLevelOut = 0;
	freq = 0;
	dfreq = 0;
	writePointer = 0;
	stiffAllPassOut = 0;
	biquadDelay = 0;
	dampDelay = 0;
	tunedOut = 0;
}


//Fill the input buffer with noise on note trigger
void kspluck::kstrigger()
{
	for(int i =0; i<DelayLines.size(); ++i)
	{
		DelayLines[i] = (((float)rand()) / ((float)RAND_MAX) * 2.0f - 1.0f);
	}
}

//Set frequency for all filters used in the Extended karplus strong algorithm
//Calculte filter coefficients for string stiffness allpass biquad for four different design frequency based on the input note.
//Calculate phase delay caused by the loop filters and the averaging filter to calculate the required fractional delay for interpolation.
void kspluck::setfr(float in)
{
	fr = in;
	freq = (2.0f*fr);
	dfreq = ((0.5*sr)-freq)*0.25f;
	biquadDelay = 0;
	ks.setfr(fr);
	damp.setfr(fr);
	allpassTune.setfr(fr);
	lagTune.setfr(fr);
	dynamic_out.setfr(fr);
	for(int i=0;i<4;i++)
	{
		biquad[i].setfr(fr);
	}
		for(int i = 0; i<4; i++)
	{
		biquad[i].calculateCoefficients(freq);
		freq += dfreq;
		biquadDelay += biquad[i].calculatePhaseDelay();
	}
		dampDelay = damp.calculateDampingPhaseDelay();
		rt_printf("%f\n",dampDelay);
		delta = dampDelay + biquadDelay + 0.5f;	
		frac = ((sr/fr) - floor(delta))  - floor(sr/fr - delta - 2);
}

//Set velocity to the dynamic level filter
void kspluck::setl(float in)
{
	l = in;	
	dynamic_out.setl(l);
}

//Set samplerate
void kspluck::setsr(float in)
{
	sr=in;
	damp.setsr(sr);
	lagTune.setsr(sr);
	allpassTune.setsr(sr);
	dynamic_out.setsr(sr);
	ks.setsr(sr);
	for(int j=0; j<4; j++)
		{
		biquad[j].setsr(sr);
		}
}

//The Digital waveguide that contains the main delay line and the loop filter. 
//noise impulse input -> pickposition filter -> pickdirection filter -----------------> two point averaging filter -------------------->dynamic level filter -> Audio output
//																		|															 |	
//																		|<-Tuning Filter <-String Stiffness Filter <-Damping filter<-|
float kspluck::pluck()
{
	//Main Delay Line
	float pickDirectionOut =  ((1-pd)*DelayLines[writePointer] + pd*gLastOutput);
	gLastOutput = pickDirectionOut;
	DelayLines[writePointer] = pickDirectionOut;
	int pickPosDelay = floor(pp*BufferSize);
	float pickpositionOut = DelayLines[writePointer] - 0.005*DelayLines[(writePointer-pickPosDelay)%BufferSize];
	DelayLines[writePointer] = pickpositionOut;
	float LoopBuff = ks.twoPointAverage(DelayLines[writePointer]);
	DelayLines[writePointer] =  LoopBuff ;
	float out = dynamic_out.DynamicLevel(DelayLines[writePointer], l);
	//Loop Filter
	LoopBuff = damp.DampenString( LoopBuff );
	for(int i=0; i<4; i++)
		{
		 LoopBuff  = biquad[i].filterSignal(LoopBuff);
		}
//	LoopBuff  = allpassTune.TuneString(LoopBuff, frac);
	LoopBuff  = lagTune.lagrangeInterPolation(LoopBuff, frac);
	DelayLines[writePointer] = LoopBuff;
	writePointer = writePointer + 1;
    if (writePointer>=BufferSize)
    {
    	writePointer = writePointer - BufferSize;
    }
	return out;
}



