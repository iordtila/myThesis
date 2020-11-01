#ifndef MYRESIDUAL_H
#define MYRESIDUAL_H

#include <stdio.h>
#include <stdlib.h>

#define NFACES_VAL 3756
#define NCELLS_VAL 525

void myResidual(
	int nCells,
	int nFaces,
	volatile double* rAPtr,
	volatile const double* sourcePtr,
	volatile const double* diagPtr,
	volatile const double* psiPtr,
	volatile const double* lowerPtr,
	volatile const double* upperPtr,
	volatile const int* uPtr,
	volatile const int* lPtr
);

#endif
