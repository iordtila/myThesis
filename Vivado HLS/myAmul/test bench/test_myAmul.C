#include "myAmul.H"
#include <cstdlib>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>

using namespace std;
double RandomDouble(double a, double b);

int main()
{

	int nCells = NCELLS_VAL;
	int nFaces = NFACES_VAL;
	double* upperPtr;
	double* lowerPtr;
	double* psiPtr;
	int* uPtr;				//Must have values <= NCELLS_VAL
	int* lPtr;
	double* diagPtr;
	double* ApsiPtr;

	int i;


	upperPtr = new double[NFACES_VAL];
	lowerPtr = new double[NFACES_VAL];
	ApsiPtr = new double[NCELLS_VAL];
	psiPtr = new double[NCELLS_VAL];
	diagPtr = new double[NCELLS_VAL];
	uPtr = new int[NFACES_VAL];
	lPtr = new int[NFACES_VAL];



	for( i = 0; i < NFACES_VAL;i++)
	{
		upperPtr[i] = RandomDouble(-6, 50);
		lowerPtr[i] = RandomDouble(-6, 50);
		lPtr[i] = rand() % NCELLS_VAL;
		uPtr[i] = rand() % NCELLS_VAL;
	}


	for(i = 0 ; i < NCELLS_VAL; i++)
	{

		ApsiPtr[i] = RandomDouble(-10,700);
		psiPtr[i] = RandomDouble(-700, 40000);
		diagPtr[i] = RandomDouble(-150, 40);
	}



	myAmul
	(
		nCells,
		nFaces,
		ApsiPtr,
		diagPtr,
		psiPtr,
		lowerPtr,
		upperPtr,
		uPtr,
		lPtr
	);

	ofstream file1;
	file1.open("test_output.dat");

	for(i=0; i<NCELLS_VAL; i++)
	{
		file1<<ApsiPtr[i]<<"\n";
	}


	file1.close();

	delete[] upperPtr;
	delete[] lowerPtr;
	delete[] ApsiPtr;
	delete[] psiPtr;
	delete[] diagPtr;
	delete[] uPtr;
	delete[] lPtr;

	if (system("diff -w test_output.dat test_output.gold.dat")) {

		fprintf(stdout, "\n\n\n*******************************************\n");
		fprintf(stdout, "FAIL: Output DOES NOT match the golden output\n");
		fprintf(stdout, "*******************************************\n\n\n");
		 return 0;
	} else {
		fprintf(stdout, "\n\n\n*******************************************\n");
		fprintf(stdout, "PASS: The output matches the golden output!\n");
		fprintf(stdout, "*******************************************\n\n\n");
		 return 0;
	}




}






















double RandomDouble(double a, double b) {
    double random = ((double) rand()) / (double) RAND_MAX;
    double diff = b - a;
    double r = random * diff;
    return a + r;
}
