//
//  qfloat.hpp
//  Qsynth
//
//  Created by Marek on 3/15/15.
//
//

#pragma once

#include <stdint.h>

#ifndef PI
#	define PI 3.141592653589793
#endif

#ifndef TWO_PI
#	define TWO_PI 6.28318530717959
#endif

// this is the main type - an int shifted by 16 bits
// you can change Q_SHIFT to make it more or less accurate at the expense of range.
typedef int32_t qfloat;


static const qfloat qfloat_1 = 0x00010000; /*!< fix16_t value of 1 */
#define Q_SHIFT 16





// this is a secondary type that allows less accuracy but bigger numbers
// max is 128
typedef int32_t lfloat;

static const lfloat lfloat_1 = 0x0080; /*!< fix16_t value of 1 */
#define L_SHIFT 7



// a constant representing -1.0
static const qfloat qfloat_m1 = -qfloat_1;
#define QFLOAT_SINLUT_SIZE 256


// convert from lfloat (q257) to qfloat
qfloat L_Q(lfloat);



// creates a qfloat from a float
qfloat QFLOAT(float inp);


// qfloat to lfloat (q257)
lfloat Q_L(lfloat f);

// turn a float into a qfloat
#define QINT(inp) ((inp) << Q_SHIFT)

lfloat LFLOAT(float inp);


// convert from int to Q257
#define LINT(inp) ((inp) << L_SHIFT)






// qfloat to float
float Q_FLOAT(qfloat fl);

// lfloat to float
float L_FLOAT(lfloat fl);



// qfloat to int (truncating)
#define Q_INT(inp) ((inp) >> Q_SHIFT)

// lfloat to int
#define L_INT(inp) ((inp) >> L_SHIFT)





// multiply 2 floats together a*b
qfloat qmul(qfloat a, qfloat b);

// divide a/b
qfloat qdiv(qfloat a, qfloat b);

// returns a value between -1 and 1 (as a qfloat)
qfloat qranduf();

int randi(int min, int max);
int randi(int max);
static inline qfloat qfloat_from_int(int a)     { return a * qfloat_1; }

// trig. if you want to use them, call qinit before to generate the lookup table
void qinit();
qfloat qfloat_sin(qfloat phase);
qfloat qfloat_sin_interp(qfloat phase);
qfloat qfloat_saw_interp(qfloat phase);




// some constants
extern qfloat QPI;
extern qfloat Q2PI;
extern qfloat QTWO_PI_OVER_SR;
extern qfloat Q05; // (value of 0.5)

qfloat qexp(qfloat inp);

// for debugging, it turns the qfloat into a string for printing out
const char *qfloat_to_binary_string(qfloat inp);
qfloat qfloat_to_uint16(qfloat out);


