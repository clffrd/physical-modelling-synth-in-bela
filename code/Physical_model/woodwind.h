/***** woodwind.h *****/
#pragma once
#include<vector>
#include<cmath>
#include<Bela.h>
#include<filters.h>
#include<utilites.h>

class woodwind
{
	public:
		woodwind();
			void init();
				void setsr(float in);
				void setfr(float in);
				void setl(float in);
				void setJetSize(int in);
				void stop();
				void calculateDelta ();
				void updateLiveParams_ (float reflectionCoeff1, float reflectionCoeff2, float vibrato, float Dampen, float T60, float attack, float release, float ovrblw)
				{
					r1 = reflectionCoeff1;
					r2 = reflectionCoeff2;
					d = Dampen;
					vibDepth = vibrato;
					t =  T60;
					overBlow = ovrblw;
					arEnv.setAttackTime(attack);
					arEnv.setReleaseTime(release);
					damp.updateLiveParams_(0,0,Dampen,T60,0);
				};
			void blow();
			float playflute();
			float tuningFilter();
			void PhaseDelays();
			void setState(int in);
			float PDcalc();
			void setBufferSize(unsigned int in);
		~woodwind(){};
		
		std::vector<float> ringBufferOne;
		std::vector<float> ringBufferTwo;
		std::vector<float> breath;
		std::vector<float> vib;

		float ringBufferOneOut;
		unsigned int dl1ptr;
		unsigned int dl2ptr;
		unsigned int dl1outptr;
		unsigned int dl2outptr;
		float prevOut;
		bool isNoteOn = false;
		float dc_filter_out;
		float dc_filter_in;
		float reedIn;
		int samples;
		float delta;
		float gain;
		filters damp;
		filters lagTune[2];
		filters Level;
		filters allPassTune[2];
		utilites arEnv;

			
	protected:
		float sr;
		float fr;
		unsigned int BufferSize;
		unsigned int dl1size;
		unsigned int dl2size;
		float dl1frac;
		float dl2frac;
		float r1 = 0.4;
		float r2 = 0.8;
		float vibDepth;
		float d;
		float t;
		float l;
		float overBlow;
		float filterCoeff = 0.6f;
		float DCfilterGain = 0.99f;
	
};