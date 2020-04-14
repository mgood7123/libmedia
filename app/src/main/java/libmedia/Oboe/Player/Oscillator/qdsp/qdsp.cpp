//
//  qdsp.cpp
//  Qsynth
//
//  Created by Marek on 3/16/15.
//
//

#include "qdsp.h"


qfloat qlfo(qfloat val) {
	return qmul(qfloat_1 + val, Q05);
}


static inline qfloat QReadFrac(qfloat *buff, qfloat index, int length) {
	
	int i = Q_INT(index);
	int j = i + 1;
	if(j >= length) j -= length;
	
	qfloat frac = index - QINT(i);
	
	return qmul(buff[i], (qfloat_1 - frac)) + qmul(buff[j], frac);
	
}


static inline qfloat LReadFrac(qfloat *buff, lfloat index, int length) {
	int i = L_INT(index);
	int j = i + 1;
	if(j >= length) j -= length;
	
	qfloat frac = L_Q(index - LINT(i)); // Q257 better not cast to a float or this will be slow
	
	return qmul(buff[i], (qfloat_1 - frac)) + qmul(buff[j], frac);
}