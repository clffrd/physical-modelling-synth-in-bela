/***** kspluck.h *****/
/***** utilities.h *****/
#pragma once
#include<vector>
#include<cmath>
#include<filters.h>
#include<utilites.h>

class kspluck
{
	public:
		kspluck();
			void setsr(float in);
			void setfr(float in);
			void setl(float in);
			void updateLiveParams_ (float pickDir, float pickPos, float Dampen, float T60, float stiff)
			{
				pd = pickDir;
				pp = pickPos;
				d = Dampen;
				t =  T60;
				s = stiff;
				for(int i = 0 ;i<4; i++)
				{
					biquad[i].updateLiveParams_(pickDir,pickPos,Dampen,T60,stiff);
				}
					damp.updateLiveParams_(pickDir,pickPos,Dampen,T60,stiff);
					allpassTune.updateLiveParams_(pickDir,pickPos,Dampen,T60,stiff);
					ks.updateLiveParams_(pickDir,pickPos,Dampen,T60,stiff);
					lagTune.updateLiveParams_(pickDir,pickPos,Dampen,T60,stiff);
			};
			bool isNoteOn;
			void kstrigger();
			float pluck();
			float tuningFilter();
			void PhaseDelays();
			float PDcalc();
			void setBufferSize(unsigned int in){ BufferSize = in;}
		~kspluck(){};
	

		std::vector<float> DelayLines;
		float gLastOutput;
		float gLastTuningOutput;
		float gLastDynamicLevelOut;
		unsigned int writePointer;
		float previousOut;
		float biquadDelay;
		float dampDelay;
		float tunedOut;
		float stiffAllPassOut;
		float delta;
		float frac;
		filters ks;
		filters biquad[4];
		filters damp;
		filters allpassTune;
		filters lagTune;
		filters dynamic_out;

		protected:
		unsigned int BufferSize;
		float dfreq;
		float freq;
		float sr;
		float fr;
		float d;
		float pp;
		float pd;
		float t;
		float s;
		float l;
};