/***** utilites.cpp *****/
/*This module contains a Attack Release envelope used in the woodwind model*/
#include <Bela.h>
#include <utilites.h>

//Initialize the variables for the envelope
utilites::utilites()
{
		sr_ = 44100;
		gain = 0;
		attack_ = 0.5;
		release_ = 0.1;
		i = 0;
}


//initialize gain and time counter on ever trigger
void utilites::trigger()
		
	{
			gain = 0;
			i = 0;
	}

//Attack function for the flute input envelope
//until gain is 1 keep increasing gain by the factor 1(samplerate/attacktime)
float utilites::attack()
{
	gain += 1/(sr_*attack_);
	if(gain>=1)
	{
		gain =1;
	}
	return gain;
}

//Release function for the flute input envelope
//until gain is 0 keep decreasing gain by the factor 1(samplerate/releasetime)
float utilites::release()
{
	gain -= 1/(sr_*release_);
	if(gain<=0)
	{
		gain =0;
	}
	return gain;
}