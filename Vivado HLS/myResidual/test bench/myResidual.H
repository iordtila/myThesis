#ifndef MYRESIDUAL_H
#define MYRESIDUAL_H

#include <stdio.h>
#include <stdlib.h>

#define NFACES_VAL 3756
#define NCELLS_VAL 525

void myResidual(
	int nCells,
	int nFaces,
	double* rAPtr,
	const double* sourcePtr,
	const double* diagPtr,
	const double* psiPtr,
	const double* lowerPtr,
	const double* upperPtr,
	const int* uPtr,
	const int* lPtr
);

#endif
