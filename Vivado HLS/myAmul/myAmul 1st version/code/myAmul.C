#include "myAmul.H"




void myAmul
(
	int nCells,
	int nFaces,
	double* ApsiPtr,
	const double* diagPtr,
	const double* psiPtr,
	const double* lowerPtr,
	const double* upperPtr,
	const int* uPtr,
	const int* lPtr
)
{

#pragma HLS INTERFACE m_axi depth=3756 port=uPtr offset=slave
#pragma HLS INTERFACE m_axi depth=3756 port=lPtr offset=slave
#pragma HLS INTERFACE m_axi depth=3756 port=upperPtr offset=slave
#pragma HLS INTERFACE m_axi depth=3756 port=lowerPtr offset=slave
#pragma HLS INTERFACE m_axi depth=525 port=psiPtr offset=slave
#pragma HLS INTERFACE m_axi depth=525 port=diagPtr offset=slave
#pragma HLS INTERFACE m_axi depth=525 port=ApsiPtr offset=slave

#pragma HLS INTERFACE s_axilite port=nCells bundle=control
#pragma HLS INTERFACE s_axilite port=nFaces bundle=control
#pragma HLS INTERFACE s_axilite port=ApsiPtr bundle=control
#pragma HLS INTERFACE s_axilite port=diagPtr bundle=control
#pragma HLS INTERFACE s_axilite port=psiPtr bundle=control
#pragma HLS INTERFACE s_axilite port=lowerPtr bundle=control
#pragma HLS INTERFACE s_axilite port=upperPtr bundle=control
#pragma HLS INTERFACE s_axilite port=uPtr bundle=control
#pragma HLS INTERFACE s_axilite port=lPtr bundle=control
#pragma HLS INTERFACE s_axilite port=return bundle=control



	for (int cell=0; cell<nCells; cell++)
    	{
		#pragma HLS LOOP_TRIPCOUNT min=0 max=525
		
		ApsiPtr[cell]= diagPtr[cell]*psiPtr[cell];
    	}

	for (int face=0; face<nFaces; face++)
    	{
    		#pragma HLS LOOP_TRIPCOUNT min=0 max=3756

    		ApsiPtr[lPtr[face]] += upperPtr[face]*psiPtr[uPtr[face]];
    		ApsiPtr[uPtr[face]] += lowerPtr[face]*psiPtr[lPtr[face]];
    	}


}
