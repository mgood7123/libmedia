//
//  qdsp.hpp
//  Qsynth
//
//  Created by Marek on 3/16/15.
//
//

#pragma once

#include "qfloat.h"


/*
 TODO
 
 drum envelope
 different waveforms 
	- noise
	- bandlimited square saw and tri
	- non-bandlimited square saw and tri
	- karplus-strong
 sequencer
 delay
 
 */


class QOnePole {
public:
	
	qfloat a0;
	qfloat b1;
	
	qfloat out;
	QOnePole() {
		out = 0;
	}
	
	void calcCoeffs(float freq) {
		// Coefficient calculation:
		// x = exp(-2.0*pi*freq/samplerate);
		// a0 = 1.0-x;
		// b1 = -x;
		
		qfloat x = qexp(QFLOAT(-2.0*PI*freq/44100.f));
		
		a0 = qfloat_1 - x;
		b1 = -x;
	}
	qfloat lpf(qfloat in) {
		out = qmul(a0, in) - qmul(b1, out);
		return out;
	}
	qfloat hpf(qfloat in) {
		return in - lpf(in);
	}
};




class QOsc {
public:
	qfloat phaseInc;
	qfloat phase;
	QOsc() {
		setFrequency(QFLOAT(200));
	}
	
	void setFrequency(qfloat f) {
		if(f<0) f = -f;
		phaseInc = qmul(f, QTWO_PI_OVER_SR);
	}
	
	qfloat getSample() {
		phase += phaseInc;
		if(phase>Q2PI) {
			phase -= Q2PI;
		}
		return qfloat_sin_interp(phase);
	}
};


class QSawOsc {
public:
	qfloat phaseInc;
	qfloat phase;
	QSawOsc() {
		setFrequency(QFLOAT(200));
	}
	
	void setFrequency(qfloat f) {
		phaseInc = qmul(f, QTWO_PI_OVER_SR);
	}
	
	qfloat getSample() {
		phase += phaseInc;
		if(phase>Q2PI) {
			phase -= Q2PI;
			
		}
		return qfloat_saw_interp(phase);
	}
};

// maps [-1, 1]  to  [0, 1]
qfloat qlfo(qfloat val);



// single shot drum envelope (just attack and release)
class QAREnvelope {
public:
	
	enum {
		NONE,
		ATTACKING,
		RELEASING
	};
	qfloat Q1_44100;
	QAREnvelope() {
		Q1_44100 = QFLOAT(1.f/44100.f);
		pos = -1;
		
		currVal = 0;
		STATE = NONE;
		
		setEnvelope(QFLOAT(0.001), QFLOAT(0.1));
	}
	
	void setEnvelope(qfloat attackTime, qfloat releaseTime) {
		setAttack(attackTime);
		setRelease(releaseTime);
	}
	
	void setAttack(qfloat attack) {
		attackDelta  = qmul(qdiv(qfloat_1, attack), Q1_44100);
	}
	
	void setRelease(qfloat release) {
		releaseDelta = qmul(qdiv(qfloat_1, release), Q1_44100);
	}
	
	qfloat currVal;
	int STATE;
	
	
	qfloat getSample() {
		
		if(STATE==ATTACKING) {
			currVal += attackDelta;
			if(currVal>qfloat_1) {
				STATE = RELEASING;
				currVal = qfloat_1;
			}
			// give it a nice curve on the way out
			return qmul(currVal, currVal);
			
		} else if(STATE==RELEASING) {
			currVal -= releaseDelta;
			if(currVal<0) {
				currVal = 0;
				STATE = NONE;
			}
			// give it a nice curve on the way out
			return qmul(currVal, currVal);
			
		} else {
			return 0;
		}
	}
	
	void trigger() {
		STATE = ATTACKING;
	}
private:
	int pos;
	qfloat attackDelta;
	qfloat releaseDelta;
	
};




class QLPF {
public:
	qfloat filt;
	qfloat amt, mAmt;
	QLPF() {
		filt = 0;
		amt = QFLOAT(0.5);
		mAmt = qfloat_1 - amt;
	}
	
	void setAmount(qfloat amt) {
		this->amt = amt;
		mAmt = qfloat_1 - amt;
	}
	qfloat filter(qfloat inp) {
		filt = qmul(filt, amt) + qmul(inp, mAmt);
		return filt;
	}
};




static inline qfloat qclip(qfloat inp) {
	if(inp>qfloat_1) {
		return qfloat_1;
	} else if(inp<-qfloat_1) {
		return -qfloat_1;
	} else {
		return inp;
	}
}


class QKarplusStrong {
public:
#define KARPLUS_STRONG_MAX_DELAY 1024
	qfloat buffer[KARPLUS_STRONG_MAX_DELAY];
	int inputPos;
	int outputPos;
	int DELAY_SIZE;
	int pos;
	qfloat decay;
	QKarplusStrong() {
		decay = QFLOAT(0.99);
		inputPos = 0;
		outputPos = 1;
		DELAY_SIZE = 512;
		pos = 0;
		setCoeffs(QFLOAT(0.5));
	}
	
	qfloat getDelayedValue() {
		outputPos++;
		if(outputPos>=DELAY_SIZE) {
			outputPos = 0;
		}
		return buffer[outputPos];
	}
	
	QLPF filter;
	void setCoeffs(qfloat amt) {
		filter.setAmount(amt);
	}
	void addDelay(qfloat sample) {
		inputPos = outputPos-1;
		if(inputPos<0) {
			inputPos = DELAY_SIZE-1;
		}
		buffer[inputPos] = sample;
	}
	
	void setLength(int length) {
		if(length>KARPLUS_STRONG_MAX_DELAY) length = KARPLUS_STRONG_MAX_DELAY;
		DELAY_SIZE = length;
	}
	void trigger() {
		pos = 0;
	}
	qfloat getSample() {
		if(pos<50) pos++;
		qfloat noise = QFLOAT(qranduf());
		if(pos>=50) {
			noise = 0;
		}
		qfloat out = noise + filter.filter(getDelayedValue());
		addDelay(qmul(out, decay));
		return out;
	}
};






#if 0
#include <math.h> // get rid of this!!!!!
// THSI DOESNT WORK YET
class QResoLPF {
public:
	qfloat z;
	qfloat x;
	qfloat y;
	qfloat r;
	qfloat c;
	qfloat cutoff;
	qfloat resonance;
	
	void setCoeffs(qfloat cut, qfloat res) {
		cutoff = qmul(cut, QFLOAT(0.5));
		
		if(cutoff<QFLOAT(10)) cutoff = QFLOAT(10);
		else if(cutoff>QFLOAT(11025)) cutoff = QFLOAT(11025);
		
		resonance = qfloat_1 + qmul(res, qfloat(9));
		
		if(resonance<qfloat_1) resonance = qfloat_1;
		
		z=QFLOAT(cos(3.14159*2.f*Q_FLOAT(cutoff)/44100.f));

		
		c = QFLOAT(2) - qmul(QFLOAT(2),z);
		
		qfloat Q_SQRT2 = QFLOAT((float)sqrt(2));
		
		
		qfloat zm1 = z - qfloat_1;
		
		qfloat powzm1 = qmul(qmul(zm1, zm1), zm1);
		
		r = qdiv((qmul(Q_SQRT2,QFLOAT(sqrt(-Q_FLOAT(powzm1)))) + qmul(resonance,zm1))
		
		
		, qmul(resonance,(zm1)));

	}
	
	qfloat filter(qfloat inp) {
		x += qmul((inp - y), c);
		y += x;
		x = qmul(x, r);
		return y;
	}
};
#endif




class QFloatParam {
public:
	
	QFloatParam(qfloat val = 0) {
		set(val);
		setSpeed(QFLOAT(0.995));
	}
	void set(qfloat val) {
		this->val = val;
		this->target = val;
	}
	
	void setSpeed(qfloat lerp) {
		this->lerp = lerp;
		this->mlerp = qfloat_1 - this->lerp;
	}
	qfloat get() {
		val = qmul(val, lerp) + qmul(target, mlerp);
		return val;
	}
	
	void update(qfloat target) {
		this->target = target;
	}
private:
	qfloat val;
	qfloat target;
	qfloat lerp;
	qfloat mlerp;
};





class QDelay {
public:
	int pos;
	qfloat feedback;
	qfloat mix;
	qfloat *buffer;
	int maxDelay;
	int delayTime;
	QLPF filter;
	QDelay() {
		filter.setAmount(0);
		pos = 0;
		delayTime = 1000;
		feedback = QFLOAT(0.5);
		mix = QFLOAT(0.5);
		maxDelay = 10000;
		buffer = new qfloat[maxDelay];
		for(int i = 0; i < maxDelay; i++) {
			buffer[i] = 0;
		}
	}
	
	qfloat process(qfloat in) {
		pos++;
		pos %= delayTime;
		qfloat out = buffer[pos];
		buffer[pos] = filter.filter(qmul(feedback,out) + in);
		return in + qmul((out - in), mix);
	}
};



static inline qfloat QReadFrac(qfloat *buff, qfloat index, int length);
static inline qfloat LReadFrac(qfloat *buff, lfloat index, int length);



class QInterpolatingDelay {
public:
	
	qfloat *buffer;
	
	qfloat	actualDelayTime;
	int		writeHead;
	qfloat	readHead;
	int		delayTime;
	qfloat	feedback;
	qfloat mix;
	qfloat lerpAmt;
	qfloat lerpAmtM;
	int MAX_DELAY;
	
	QInterpolatingDelay(int MAX_DELAY) {
		this->MAX_DELAY = MAX_DELAY;
		actualDelayTime = 0;
		writeHead		= 0;
		readHead		= 0;
		delayTime		= 100;
		feedback		= QFLOAT(0.5);
		mix = QFLOAT(0.5);
		buffer = new qfloat [MAX_DELAY];
		
		for(int i =0; i < MAX_DELAY; i++) {
			buffer[i] = 0;
		}
		
		lerpAmt = QFLOAT(0.99975);
		lerpAmtM = qfloat_1 - lerpAmt;
		
	}
	
	
	
	qfloat process(qfloat inp) {
		
		writeHead++;
		writeHead %= MAX_DELAY;
		
		
		actualDelayTime = qmul(actualDelayTime, lerpAmt) + qmul(QINT(delayTime), lerpAmtM);
		
		readHead = (writeHead * qfloat_1) - actualDelayTime;
		if(readHead<0) readHead += MAX_DELAY * qfloat_1;
		
		float out = QReadFrac(buffer, readHead, MAX_DELAY);
		
		buffer[writeHead] = qmul(out, feedback) + inp;
		
		return inp + qmul((out - inp), mix);
		
	}
};

