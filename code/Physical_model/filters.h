/***** filters.h *****/
#pragma once

#include <vector>
#include <cmath>

class filters
{
	public:
		filters();
		//! Set the b[0] coefficient value
		void setB0( float b0 ) { b[0] = b0; };
		//! Set the b[1] coefficient value
		void setB1( float b1 ) { b[1] = b1; };
		 //! Set the b[2] coefficient value
		void setB2( float b2 ) { b[2] = b2; };
		//! Set the a[1] coefficient value
		void setA1( float a1 ) { a[1] = a1; };
		//! Set the a[2] coefficient value
		void setA2( float a2 ) { a[2] = a2; };
		float setfreq();
		void setfr(float i){fr_=i;};
		void setl(float i){l = i;};
		void setsr(float i){sr_= i;};
		void calculateCoefficients(float fq);
		float filterSignal(float in);
		float calculatePhaseDelay();
		float twoPointAverage(float in);
		float DampenString(float in);
		float TuneString(float in, float fd);
		float DynamicLevel(float in, float l);
		float calculateDampingPhaseDelay();
		void pickDirectionFilter();
		void updateLiveParams_ (float pickDir, float pickPos, float Dampen, float T60, float stiff)
		{
			pd = pickDir;
			pp = pickPos;
			d = Dampen;
			t =  T60;
			s = stiff;
		};
		float lagrangeInterPolation(float in, float frac);
		~filters(){}
	
		std::vector<float> a;
		std::vector<float> b;
		std::vector<float> input;
		std::vector<float> output;
		unsigned int writePointer;
		unsigned int readPointer;
		unsigned int counter;
		float w;
		float m,n,p,q;
		float timeDelay;
		float dampDelay;
		float coeff;
		int i;
		float temp;
		float delayLineSize;
		float gLastOutput;
		
		protected:
		float dfreq;
		float sr_;
		float fr_;
		float d;
		float pp;
		float pd;
		float t;
		float s;
		float l;
}; 