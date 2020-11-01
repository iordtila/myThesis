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

#pragma HLS INTERFACE m_axi depth=3756 port=uPtr offset=slave bundle=gmem3
#pragma HLS INTERFACE m_axi depth=3756 port=lPtr offset=slave bundle=gmem3
#pragma HLS INTERFACE m_axi depth=3756 port=upperPtr offset=slave bundle=gmem2
#pragma HLS INTERFACE m_axi depth=3756 port=lowerPtr offset=slave bundle=gmem2
#pragma HLS INTERFACE m_axi depth=525 port=psiPtr offset=slave bundle=gmem1
#pragma HLS INTERFACE m_axi depth=525 port=diagPtr offset=slave bundle=gmem0
#pragma HLS INTERFACE m_axi depth=525 port=ApsiPtr offset=slave bundle=gmem0

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



	double temp_ApsiPtr[525];

	for (int cell=0; cell<nCells; cell++)
    	{
		#pragma HLS LOOP_TRIPCOUNT min=0 max=525
		#pragma HLS PIPELINE
		
		temp_ApsiPtr[cell]= diagPtr[cell]*psiPtr[cell];
    	}

	for (int face=0; face<nFaces; face++)
    	{
		#pragma HLS DEPENDENCE variable=temp_ApsiPtr intra false
    		#pragma HLS LOOP_TRIPCOUNT min=0 max=3756
		#pragma HLS PIPELINE //Achieved II=5

    		temp_ApsiPtr[lPtr[face]] += upperPtr[face]*psiPtr[uPtr[face]];
    		temp_ApsiPtr[uPtr[face]] += lowerPtr[face]*psiPtr[lPtr[face]];
    	}

	for (int cell=0; cell<nCells; cell++)
	{
		#pragma HLS LOOP_TRIPCOUNT min=0 max=525
		#pragma HLS PIPELINE II=1
		ApsiPtr[cell] = temp_ApsiPtr[cell];
	}


}
