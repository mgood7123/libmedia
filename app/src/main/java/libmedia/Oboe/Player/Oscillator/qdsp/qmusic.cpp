/**
 *  scales.cpp
 *
 *  Created by Marek Bereza on 05/02/2013.
 */

#include "qmusic.h"


qfloat qmtof(int f) {
	static qfloat a = QFLOAT(8.17579891564);
	static qfloat b = QFLOAT(0.0577622650);
	return qmul(a, qexp(qmul(b, QINT(f))));
}


//
//
//float mtof(float f)
//{
//	return (8.17579891564 * exp(.0577622650 * f));
//}
/*
float ftom(float f)
{
	return (17.3123405046 * log(.12231220585 * f));
}


int getScaled(int pos, int scale) {
	
	if(scale==CHROMATIC) return pos;
	else if(scale==PENTATONIC) {
		int octave = pos / 5;
		int note = 0;
		switch(pos % 5) {
			case 0: note = 0; break;
			case 1: note = 3; break;
			case 2: note = 5; break;
			case 3: note = 7; break;
			case 4: note = 10; break;
		}
		return octave * 12 + note;
	}
	
	else if(scale==MINOR) {
		int octave = pos / 7;
		int note = 0;
		switch(pos % 7) {
			case 0: note = 0;  break;
			case 1: note = 2;  break;
			case 2: note = 3;  break;
			case 3: note = 5;  break;
			case 4: note = 7;  break;
			case 5: note = 8;  break;
			case 6: note = 11; break;
		}
		return octave * 12 + note;
	}
	
	else if(scale==MAJOR) {
		int octave = pos / 7;
		int note = 0;
		switch(pos % 7) {
			case 0: note = 0;  break;
			case 1: note = 2;  break;
			case 2: note = 4;  break;
			case 3: note = 5;  break;
			case 4: note = 7;  break;
			case 5: note = 9;  break;
			case 6: note = 11; break;
		}
		return octave * 12 + note;
	}
	
	else if(scale==WHOLE) {
		int octave = pos / 6;
		int note = 0;
		switch(pos % 6) {
			case 0: note = 0;  break;
			case 1: note = 2;  break;
			case 2: note = 4;  break;
			case 3: note = 6;  break;
			case 4: note = 8;  break;
			case 5: note = 10;  break;
		}
		return octave * 12 + note;
	}
	
	
}

float midiNoteToSpeed(int midiNote, int originalNote) {
	return pow(2, (midiNote-originalNote)/12.f);
	
}*/