/***** utilities.h *****/
#pragma once
#include<vector>
#include<cmath>

class utilites
{
	public:
	utilites();
		void env(int in);
		void trigger();
		float attack();
		float release();
		void setAttackTime(float in){attack_ = in;}
		void setReleaseTime(float in){release_ = in;}
		void setsr(float in){sr_ = in;}
	~utilites(){};

	float gain;
	private:
		float sr_;
		float attack_;
		float release_;
		int i = 0;
};