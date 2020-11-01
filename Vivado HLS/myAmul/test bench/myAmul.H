#ifndef MYAMUL_H
#define MYAMUL_H

#include <stdio.h>
#include <stdlib.h>

#define NFACES_VAL 3756
#define NCELLS_VAL 525


void myAmul(
		int nCells,
		int nFaces,
		double* ApsiPtr,
		const double* diagPtr,
		const double* psiPtr,
		const double* lowerPtr,
		const double* upperPtr,
		const int* uPtr,
		const int* lPtr
);

#endif
