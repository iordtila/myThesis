#include "myResidual.H"

void myResidual
(
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
)
{
#pragma HLS INTERFACE m_axi depth=3756 port=lPtr offset=slave bundle=gmem4
#pragma HLS INTERFACE m_axi depth=3756 port=uPtr offset=slave bundle=gmem4
#pragma HLS INTERFACE m_axi depth=3756 port=upperPtr offset=slave bundle=gmem3
#pragma HLS INTERFACE m_axi depth=3756 port=lowerPtr offset=slave bundle=gmem3
#pragma HLS INTERFACE m_axi depth=525 port=psiPtr offset=slave bundle=gmem2
#pragma HLS INTERFACE m_axi depth=525 port=diagPtr offset=slave bundle=gmem1
#pragma HLS INTERFACE m_axi depth=525 port=sourcePtr offset=slave bundle=gmem0
#pragma HLS INTERFACE m_axi depth=525 port=rAPtr offset=slave bundle=gmem0
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

	double rAPtr_temp[525];

	for (int cell=0; cell<nCells; cell++)
 	{
		#pragma HLS LOOP_TRIPCOUNT min=0 max=525
		#pragma HLS PIPELINE
		rAPtr_temp[cell] = sourcePtr[cell] - diagPtr[cell]*psiPtr[cell];
    	}

    	for (int face=0; face<nFaces; face++)
    	{
		#pragma HLS DEPENDENCE variable=rAPtr_temp intra false
		#pragma HLS PIPELINE
		#pragma HLS LOOP_TRIPCOUNT min=0 max=3756
		rAPtr_temp[uPtr[face]] -= lowerPtr[face]*psiPtr[lPtr[face]];
     		rAPtr_temp[lPtr[face]] -= upperPtr[face]*psiPtr[uPtr[face]];
    	}

	for(int i=0; i<nCells; i++)
	{
		#pragma HLS PIPELINE
		#pragma HLS LOOP_TRIPCOUNT min=0 max=525
		rAPtr[i] = rAPtr_temp[i];
    	}


}
