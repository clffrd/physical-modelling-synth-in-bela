/***** EKSpoly.h *****/
#pragma once
#include <Bela.h>
#include "filters.h"
#include "kspluck.h"
#include "woodwind.h"
#include <cmath>
#include <vector>

class EKSpoly
{

	public:
	EKSpoly();
	void noteon(float note, float velocity);
	void setfr(float in);
	void setl(float in);
	void PDcalc();
	void BuffResize();
	void noteoff(float note);
	void setsr(float in);
	void updateLiveParams(float pickDir, float pickPos, float Dampen, float T60, float stiff, float ref1, float ref2, float vibratoDepth, float attack, float release, float ovrBlw);
	float playNote();
	void changeProgram(int in);
	~EKSpoly()
		{
			for(int i=0; i<maxVoices; i++)
		{
			delete eks[i];
			delete flute[i];	
		}
			
		}
	
	protected:
	unsigned int paramread;
	unsigned int excess;
	float sr_;
	float fr_;
	float l_;
	int voiceNumber;
	int maxVoices;
	int	amountOn;
	float lastPressed[14];
	std::vector<kspluck*> eks;
	std::vector<woodwind*> flute;
	int program;
	int state;
};