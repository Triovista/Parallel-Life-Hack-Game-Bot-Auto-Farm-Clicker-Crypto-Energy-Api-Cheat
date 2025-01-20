
#pragma once

#include "matrix_interface.h"


namespace CoreStructures {

	//
	// complex matrix template specialisations
	//

	template <> void matrix<float>::extract_va_arg(va_list *arg_list, float *value);

	template <> bool matrix<std::complex<float> >::is_real() const;
	template <> bool matrix<std::complex<double> >::is_real() const;

	template <> matrix<std::complex<float> > matrix<std::complex<float> >::conj() const;
	template <> matrix<std::complex<double> > matrix<std::complex<double> >::conj() const;

	template <> matrix<std::complex<float> > matrix<std::complex<float> >::hconj() const;
	template <> matrix<std::complex<double> > matrix<std::complex<double> >::hconj() const;

	template <> bool matrix<std::complex<float> >::is_hermitian() const;
	template <> bool matrix<std::complex<double> >::is_hermitian() const;

	template <> bool matrix<std::complex<float> >::is_skew_hermitian() const;
	template <> bool matrix<std::complex<double> >::is_skew_hermitian() const;

	template <> bool matrix<std::complex<float> >::is_unitary() const;
	template <> bool matrix<std::complex<double> >::is_unitary() const;

	template <> bool matrix<std::complex<float> >::is_normal() const;
	template <> bool matrix<std::complex<double> >::is_normal() const;

	template <> bool matrix<std::complex<float> >::is_posdef() const;
	template <> bool matrix<std::complex<float> >::is_singular() const;
	template <> unsigned int matrix<std::complex<float> >::rank() const;
	template <> unsigned int matrix<std::complex<float> >::nullity() const;


	// operators

	template <> matrix<std::complex<float> > matrix<std::complex<float> >::operator^(const int i) const;
	template <> matrix<std::complex<float> > matrix<std::complex<float> >::inv() const;
	template <> std::complex<float> matrix<std::complex<float> >::det() const;
	template <> std::complex<float> matrix<std::complex<float> >::norm(gu_norm_type t) const;
	
	
	// Linear systems modelling

	template <> matrix<std::complex<float> > matrix<std::complex<float> >::echelonForm() const;
	template <> matrix<std::complex<float> > matrix<std::complex<float> >::row_canonical_form() const;
	template <> std::complex<float> matrix<std::complex<float> >::cofactor(unsigned int i, unsigned int j) const;
	template <> matrix<std::complex<float> > matrix<std::complex<float> >::cofactors() const;
	template <> matrix<std::complex<float> > matrix<std::complex<float> >::adj() const;
	template <> matrix<std::complex<float> > matrix<std::complex<float> >::balance(std::vector<std::complex<float> > *scale) const;
	template <> matrix<std::complex<float> > matrix<std::complex<float> >::solveCanonicalFreeVarSystem(const std::vector<unsigned int>& F, const std::vector<std::complex<float> > &V, const std::vector<unsigned int>& P, unsigned int rankA, unsigned int dimX);
	template <> matrix<std::complex<float> > gaussianElimination<>(const matrix<std::complex<float> >& A, const matrix<std::complex<float> >& B, gu_lsys_state *solutionStatus, gu_lsystem_aux<std::complex<float> > *L);


	// LU decomposition

	template <> gu_lu_decomp_state matrix<std::complex<float> >::lup_decomp(matrix<std::complex<float> > *D, std::vector<unsigned int> *P) const;

	template <> gu_lu_decomp_state matrix<std::complex<float> >::lup_decomp(matrix<std::complex<float> > *L, matrix<std::complex<float> > *U, matrix<std::complex<float> > *P) const;

	template <> matrix<std::complex<float> > lup_solve<>(const matrix<std::complex<float> >& D, const std::vector<unsigned int>& P, const matrix<std::complex<float> >& B);


	// Cholesky decomposition

	template <> gu_cholesky_state matrix<std::complex<float> >::cholesky_decomp(matrix<std::complex<float> > *C) const; 

	
	// Hessenberg form

	template <> matrix<std::complex<float> > matrix<std::complex<float> >::hessenberg_form(std::vector<unsigned int> *perm, gu_hessenberg_state *state, bool zeroL) const; 

}