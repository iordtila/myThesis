/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     | Website:  https://openfoam.org
    \\  /    A nd           | Copyright (C) 2011-2018 OpenFOAM Foundation
     \\/     M anipulation  |
-------------------------------------------------------------------------------
License
    This file is part of OpenFOAM.

    OpenFOAM is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM.  If not, see <http://www.gnu.org/licenses/>.

\*---------------------------------------------------------------------------*/

#include "GaussSeidelSmoother.H"
#include <algorithm>


// * * * * * * * * * * * * * * My function start * * * * * * * * * * * * * * //
void mySmooth
(
	 int nCells,
	 const int* ownStartPtr,
	 double* bPrimePtr,
	 const double* upperPtr,
	 const double* lowerPtr,
	 double* psiPtr,
	 const int* uPtr,
	 const double* diagPtr
)
{
	double psii;
        int fStart;
        int fEnd = ownStartPtr[0];

        for (int celli=0; celli<nCells; celli++)
        {

		fStart = fEnd;
		fEnd = ownStartPtr[celli + 1];


		psii = bPrimePtr[celli];


		for (int facei=fStart; facei<fEnd; facei++)
		{
		psii -= upperPtr[facei]*psiPtr[uPtr[facei]];
		}

		psii /= diagPtr[celli];


		for (int facei=fStart; facei<fEnd; facei++)
		{
		bPrimePtr[uPtr[facei]] -= lowerPtr[facei]*psii;
		}

		psiPtr[celli] = psii;   
        }
}


// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
    defineTypeNameAndDebug(GaussSeidelSmoother, 0);

    lduMatrix::smoother::addsymMatrixConstructorToTable<GaussSeidelSmoother>
        addGaussSeidelSmootherSymMatrixConstructorToTable_;

    lduMatrix::smoother::addasymMatrixConstructorToTable<GaussSeidelSmoother>
        addGaussSeidelSmootherAsymMatrixConstructorToTable_;
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::GaussSeidelSmoother::GaussSeidelSmoother
(
    const word& fieldName,
    const lduMatrix& matrix,
    const FieldField<Field, scalar>& interfaceBouCoeffs,
    const FieldField<Field, scalar>& interfaceIntCoeffs,
    const lduInterfaceFieldPtrsList& interfaces
)
:
    lduMatrix::smoother
    (
        fieldName,
        matrix,
        interfaceBouCoeffs,
        interfaceIntCoeffs,
        interfaces
    )
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

void Foam::GaussSeidelSmoother::smooth
(
    const word& fieldName_,
    scalarField& psi,
    const lduMatrix& matrix_,
    const scalarField& source,
    const FieldField<Field, scalar>& interfaceBouCoeffs_,
    const lduInterfaceFieldPtrsList& interfaces_,
    const direction cmpt,
    const label nSweeps
)
{
    scalar* __restrict__ psiPtr = psi.begin();

    const label nCells = psi.size();

    scalarField bPrime(nCells);
    scalar* __restrict__ bPrimePtr = bPrime.begin();

    const scalar* const __restrict__ diagPtr = matrix_.diag().begin();
    const scalar* const __restrict__ upperPtr = matrix_.upper().begin();
    const scalar* const __restrict__ lowerPtr = matrix_.lower().begin();

    const label* const __restrict__ uPtr = matrix_.lduAddr().upperAddr().begin();

    const label* const __restrict__ ownStartPtr = matrix_.lduAddr().ownerStartAddr().begin();

    // Parallel boundary initialisation.  The parallel boundary is treated
    // as an effective jacobi interface in the boundary.
    // Note: there is a change of sign in the coupled
    // interface update.  The reason for this is that the
    // internal coefficients are all located at the l.h.s. of
    // the matrix whereas the "implicit" coefficients on the
    // coupled boundaries are all created as if the
    // coefficient contribution is of a source-kind (i.e. they
    // have a sign as if they are on the r.h.s. of the matrix.
    // To compensate for this, it is necessary to turn the
    // sign of the contribution.

    FieldField<Field, scalar>& mBouCoeffs =
        const_cast<FieldField<Field, scalar>&>
        (
            interfaceBouCoeffs_
        );

    forAll(mBouCoeffs, patchi)
    {
        if (interfaces_.set(patchi))
        {
            mBouCoeffs[patchi].negate();
        }
    }

    for (label sweep=0; sweep<nSweeps; sweep++)
    {
        bPrime = source;

        matrix_.initMatrixInterfaces
        (
            mBouCoeffs,
            interfaces_,
            psi,
            bPrime,
            cmpt
        );

        matrix_.updateMatrixInterfaces
        (
            mBouCoeffs,
            interfaces_,
            psi,
            bPrime,
            cmpt
        );




// My function call
	mySmooth
	(
		nCells,
		ownStartPtr,
		bPrimePtr,
		upperPtr,
		lowerPtr,
		psiPtr,
		uPtr,
		diagPtr
	);
		



    // Restore interfaceBouCoeffs_
    forAll(mBouCoeffs, patchi)
    {
        if (interfaces_.set(patchi))
        {
            mBouCoeffs[patchi].negate();
        }
    }
}


void Foam::GaussSeidelSmoother::smooth
(
    scalarField& psi,
    const scalarField& source,
    const direction cmpt,
    const label nSweeps
) const
{
    smooth
    (
        fieldName_,
        psi,
        matrix_,
        source,
        interfaceBouCoeffs_,
        interfaces_,
        cmpt,
        nSweeps
    );
}


// ************************************************************************* //
