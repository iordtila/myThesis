#include "myResidual.H"

void myResidual
(
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
)
{
#pragma HLS INTERFACE m_axi depth=3756 port=lPtr offset=slave
#pragma HLS INTERFACE m_axi depth=3756 port=uPtr offset=slave
#pragma HLS INTERFACE m_axi depth=3756 port=upperPtr offset=slave
#pragma HLS INTERFACE m_axi depth=3756 port=lowerPtr offset=slave
#pragma HLS INTERFACE m_axi depth=525 port=psiPtr offset=slave
#pragma HLS INTERFACE m_axi depth=525 port=diagPtr offset=slave
#pragma HLS INTERFACE m_axi depth=525 port=sourcePtr offset=slave
#pragma HLS INTERFACE m_axi depth=525 port=rAPtr offset=slave
#pragma HLS INTERFACE s_axilite port=lPtr bundle=control
#pragma HLS INTERFACE s_axilite port=uPtr bundle=control
#pragma HLS INTERFACE s_axilite port=upperPtr bundle=control
#pragma HLS INTERFACE s_axilite port=lowerPtr bundle=control
#pragma HLS INTERFACE s_axilite port=psiPtr bundle=control
#pragma HLS INTERFACE s_axilite port=diagPtr bundle=control
#pragma HLS INTERFACE s_axilite port=sourcePtr bundle=control
#pragma HLS INTERFACE s_axilite port=rAPtr bundle=control
#pragma HLS INTERFACE s_axilite port=nCells bundle=control
#pragma HLS INTERFACE s_axilite port=nFaces bundle=control
#pragma HLS INTERFACE s_axilite port=return bundle=control

	for (int cell=0; cell<nCells; cell++)
 	{
		rAPtr[cell] = sourcePtr[cell] - diagPtr[cell]*psiPtr[cell];
    	}

    	for (int face=0; face<nFaces; face++)
    	{
		rAPtr[uPtr[face]] -= lowerPtr[face]*psiPtr[lPtr[face]];
     		rAPtr[lPtr[face]] -= upperPtr[face]*psiPtr[uPtr[face]];
    	}


}
