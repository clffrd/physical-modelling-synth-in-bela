/***** filters.cpp *****/
/*This module contains all the filters used in the implementation of the physical modelling synth*/
#include"filters.h"
#include <Bela.h>

//Initialize filter varables
filters :: filters() 

{
	  b.resize( 5, 0.0 );
	  a.resize( 5, 0.0 );
	  b[0] = 1.0;
	  a[0] = 1.0;
	  input.resize( 5, 0.0 );
	  output.resize( 5, 0.0 );
	  i = 0;
	  m = 0;
	  n = 0;
	  p = 0;
	  q = 0;
	  w = 0.5f;
	  timeDelay = 0;
	  coeff = 0;
	  temp = 0;
	  gLastOutput = 0;
	  writePointer = 0;
	  readPointer=0;
	  counter = 0;
}


//Calculate coefficients for the string stiffness filter
//freq defines the design frequency for which the all pass stiffness filter will be assigned to. 
//calculate coefficients once per note and update the values into vecotrs a and b to be used in the transfer function.
void filters::calculateCoefficients(float freq)

{
	temp = 0.5f + (s * 0.5f);
	if ( temp > 0.99999 ) temp = 0.99999;
	coeff = temp*temp;
	a[2]=coeff;
	b[0]=coeff;
	b[2]=1.0f;
	coeff = - 2.0 * temp *cos(2*M_PI*(freq/sr_));
	a[1] = coeff;
	b[1] = coeff;
}

//String Stiffness biquad All Pass filter
/*
* Equal gain response for all frequencies. 
* takes one sample as input argument and returns the filtered sample
* transfer function h(z) = (b(0) + b(1)*z^-1 + b(2)*z^-2) / (a[2]*z^-2 + a[1]*z^-1)
*implementation based on STK's stifkarp
*/
float filters::filterSignal(float in)

{
	input[0] =  in;
	float out = (b[0] * input[0] + b[1] * input[1] + b[2] * input[2]);
	out -= (a[2] * output[2] + a[1] * output[1]);
	input[2] = input[1];
	input[1] = input[0];
	output[2] = output[1];
	output[1] = out;
	return out;
}

//Calculate the phase delay caused by the biquad filter
// based on https://dsp.stackexchange.com/questions/67504
//w stands for omega
float filters::calculatePhaseDelay()

{
	w = 2*M_PI*fr_/sr_;
	m = (b[0]+b[1]*cos(w)+b[2]*cos(2*w)) ;
	n =	(b[1]*sin((-1)*w)+b[2]*sin((-2)*w));
	p =	a[0]+a[1]*cos(w)+a[2]*cos(2*w);
	q=	a[1]*sin((-1)*w)+a[2]*sin((-2)*w);
	timeDelay = (-1*atan2(n,m) + atan2(q,p))/w;

	return timeDelay;
}

//Two point averaging filter
//(input+previousInput)/2
float filters::twoPointAverage(float in)
{

	input[0] = in;
	float ksBuff = (input[0] + input[1])*0.5f;
	input[1] = input[0];
	return ksBuff;
}
 
//One Zero String Damping Filter
//Hd(z) = rho(1 − S) + Sz−1)
//rho= (0.001)^PT/t60
//rho is the loss factor
float filters::DampenString(float in )

{
	input[0] = in;
	float rho = pow(0.001,1.0/(fr_*t));
	float DampOut = rho*((1 - (d*0.5))*input[0] + (d*0.5)*input[1]);
	input[1] = input[0];
	return DampOut;
}

//All pass tuning method
// Not used due to rapid gain fluctiations
float filters::TuneString(float in, float fd)
{
	input[0] = in;
	float eta = (fd - 1) / (fd + 1);
	float tunedOut =  eta*output[0] + eta*input[0] - input[1];
	input[1] = input[0];
	output[0] = tunedOut;
	return tunedOut;

}

//Dynamic Level-low pass filter to make the models velocity sensitive

float filters::DynamicLevel(float in, float l)
{
	input[0]=in;
	float Lw = M_PI*(fr_/sr_);
	float Lgain = Lw/(1+Lw);
	float Lpole2 = (1-Lw)/(1+Lw);
	float Lo = pow(l,1/3);
	float DynamicLevelInput = Lgain*input[0]+Lgain*input[1]+Lpole2*output[0];
    output[0] = DynamicLevelInput;
    input[1] = input[0];
    float DynamicLevelOutput = l*Lo*input[0]+(1-l)*DynamicLevelInput;
    float out =  DynamicLevelOutput;
    return out;
}

//Lagrange Tuning method
// creates a latency of 2 samples to accomodate for future samples that are required to interpolate between past samples. 
float filters::lagrangeInterPolation(float in, float frac)
{
	float out=0;
	float fd = 0;
	input[readPointer]=in;
	counter++;
	readPointer= (readPointer+1)%5;
	if (counter==4)
	{
		 counter = 3; //once latency is reached , then stabilize
		 if(frac<1)
		 {
		 	fd = frac + 1;
		 }
		 if(frac>1)
		 {
		 	fd = frac;
		 }
		 float fdm1 = fd-1;
		 float fdm2 = fd-2;
		 float fdm3 = fd-3;
		 float fdm4 = fd-4;
		 float h0 = fdm1*fdm2*fdm3*fdm4/24;
		 float h1 = (0-fd*fdm2*fdm3*fdm4)/6;
		 float h2 = fd*fdm1*fdm3*fdm4/4;
		 float h3 = (0-fd*fdm1*fdm2*fdm4)/6;
		 float h4 = fd*fdm1*fdm2*fdm3/24;
		 out = input[readPointer]*h0 +input[(readPointer+1)%5]*h1+input[(readPointer+2)%5]*h2+input[(readPointer+3)%5]*h3+input[(readPointer+4)%5]*h4;
	}
	return out;
}

//Calculate the phase delay caused by the damping filter
//controlled by two parameters 1) brightness of the damping filter d and 2) decay time of the damping filter t
float filters::calculateDampingPhaseDelay ()

{ 		
	float w = 2*M_PI*fr_/sr_;
	float arg = (-1*d*sin(w*t))/((1-d)+d*cos(w*t));
	float taninv = atan(arg);
	dampDelay =  (-1*(1/(w*t)))*taninv;
	return dampDelay;
}
