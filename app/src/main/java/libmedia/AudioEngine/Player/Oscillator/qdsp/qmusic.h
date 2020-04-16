/**     ___           ___           ___                         ___           ___     
 *     /__/\         /  /\         /  /\         _____         /  /\         /__/|    
 *    |  |::\       /  /::\       /  /::|       /  /::\       /  /::\       |  |:|    
 *    |  |:|:\     /  /:/\:\     /  /:/:|      /  /:/\:\     /  /:/\:\      |  |:|    
 *  __|__|:|\:\   /  /:/~/::\   /  /:/|:|__   /  /:/~/::\   /  /:/  \:\   __|__|:|    
 * /__/::::| \:\ /__/:/ /:/\:\ /__/:/ |:| /\ /__/:/ /:/\:| /__/:/ \__\:\ /__/::::\____
 * \  \:\~~\__\/ \  \:\/:/__\/ \__\/  |:|/:/ \  \:\/:/~/:/ \  \:\ /  /:/    ~\~~\::::/
 *  \  \:\        \  \::/          |  |:/:/   \  \::/ /:/   \  \:\  /:/      |~~|:|~~ 
 *   \  \:\        \  \:\          |  |::/     \  \:\/:/     \  \:\/:/       |  |:|   
 *    \  \:\        \  \:\         |  |:/       \  \::/       \  \::/        |  |:|   
 *     \__\/         \__\/         |__|/         \__\/         \__\/         |__|/   
 *
 *  Description: Musical helpers
 *				 
 *  scales.h, created by Marek Bereza on 05/02/2013.
 */


#pragma once

#include "qfloat.h"



#define PENTATONIC 2
#define CHROMATIC 1
#define WHOLE 3
#define MAJOR 4
#define MINOR 0


qfloat qmtof(int m);
//float mtof(int m);
int ftom(qfloat f);

int getScaled(int pos, int scale);


// this gives you a speed at which to play back a sample
// given the midi note you want to pitch it at, and its
// original pitch as a midi note.
qfloat midiNoteToSpeed(int note, int originalNote);