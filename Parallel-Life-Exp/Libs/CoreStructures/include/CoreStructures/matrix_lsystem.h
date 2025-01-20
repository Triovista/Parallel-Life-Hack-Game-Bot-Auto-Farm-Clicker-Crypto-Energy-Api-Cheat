#pragma once

#include "matrix_core.h"

// #define __GU_DEBUG_MATRIX__ 1

namespace CoreStructures {


	// Elementary row operations

	// E1. Ri <-> Rj
	// preconditions: matrix is not NULL; i!=j; (1 <= i <= n); (1 <= j <= R->n)
	template <typename T>
	void matrix<T>::rowInterchange(unsigned int i, unsigned int j) {

		auto iptr = M.get() + (i-1);
		auto jptr = M.get() + (j-1);
	
		for (unsigned int p=0;p<m;p++, iptr+=n, jptr+=n) {

			T t = T(*iptr);
			*iptr = *jptr;
			*jptr = t;
		}
	}

	
	// E2. kRi -> Ri (k≠0)
	// preconditions: matrix is not NULL; (1 <= i <= n); k≠0
	template <typename T>
	void matrix<T>::rowScale(unsigned int i, const T& k) {
	
		auto iptr = M.get() + (i-1);
	
		for (unsigned int p=0;p<m;p++, iptr+=n)
			*iptr *= k;
	}


	// E3. kRi + Rj -> Rj
	// preconditions: matrix is not NULL; i!=j; (1 <= i <= n); (1 <= j <= n)
	template <typename T>
	void matrix<T>::rowAddition(unsigned int i, const T& k, unsigned int j) {
	
		auto iptr = M.get() + (i-1);
		auto jptr = M.get() + (j-1);
	
		for (unsigned int p=0;p<m;p++, iptr+=n, jptr+=n)
			*jptr = *iptr * k + *jptr;
	}


	// E. kRi + k_Rj -> Rj (k_ ≠ 0)
	// preconditions: matrix is not NULL; i!=j; (1 <= i <= n); (1 <= j <= n); k_≠0
	template <typename T>
	void matrix<T>::rowScaleAdd(unsigned int i, const T& k, unsigned int j, const T& k_) {
	
		auto iptr = M.get() + (i-1);
		auto jptr = M.get() + (j-1);
	
		for (unsigned int p=0;p<m;p++, iptr+=n, jptr+=n)
			*jptr = (*iptr * k) + (*jptr * k_);
	}


	// elimentary column operations - perform in-place transformations on the given matrix

	// E1. Ci <-> Cj (preconditions: matrix is not NULL; i!=j; (1 <= i <= m); (1 <= j <= m))
	template <typename T>
	void matrix<T>::colInterchange(unsigned int i, unsigned int j) {

		auto iptr = M.get() + (i-1)*n;
		auto jptr = M.get() + (j-1)*n;
	
		auto temp = (T*)malloc(n * sizeof(T));

		if (temp) {

			memcpy_s(temp, n * sizeof(T), iptr, n * sizeof(T)); // copy column i into temp
			memcpy_s(iptr, n * sizeof(T), jptr, n * sizeof(T)); // copy column j into column i
			memcpy_s(jptr, n * sizeof(T), temp, n * sizeof(T)); // copy temp into column j

			free(temp);
		}
	}

	// E2. kCi -> Ci (preconditions: matrix is not NULL; (1 <= i <= m); k≠0)
	template <typename T>
	void matrix<T>::colScale(unsigned int i, const T& k) {

		auto iptr = M.get() + (i-1)*n;
	
		for (unsigned int p=0; p<n; p++, iptr++)
			*iptr *= k;
	}
	
	// E3. kCi + Cj -> Cj (preconditions: matrix is not NULL; i!=j; (1 <= i <= m); (1 <= j <= m))
	template <typename T>
	void matrix<T>::colAddition(unsigned int i, const T& k, unsigned int j) {

		auto iptr = M.get() + (i-1)*n;
		auto jptr = M.get() + (j-1)*n;
	
		for (unsigned int p=0; p<n; p++, iptr++, jptr++)
			*jptr = *iptr * k + *jptr;
	}

	// E. kCi + k_Cj -> Cj (preconditions: matrix is not NULL; i!=j; (1 <= i <= m); (1 <= j <= m); k_≠0)
	template <typename T>
	void matrix<T>::colScaleAdd(unsigned int i, const T& k, unsigned int j, const T& k_) {

		auto iptr = M.get() + (i-1)*n;
		auto jptr = M.get() + (j-1)*n;
	
		for (unsigned int p=0; p<n; p++, iptr++, jptr++)
			*jptr = (*iptr * k) + (*jptr * k_);
	}


	// linear systems modelling

	// create row normalisation coefficient vector.  matrix is assumed to be a valid
	template <typename T>
	T* matrix<T>::createRowNormalisationCoeffVector() const {

		auto N = (T*)malloc(n * sizeof(T));
		
		if (N) {

			// find largest absolute value in each row and store in normalisation array N
			for (unsigned int i=0;i<n;i++) {
		
				auto Aptr = M.get() + i;
				T maxValue = T(0);
		
				for (unsigned int j=0;j<m;j++, Aptr+=n) {
			
					T a = abs(*Aptr);
			
					if (a > maxValue)
						maxValue = a;
				}

#ifdef __GU_DEBUG_MATRIX__
				cout << __FUNCTION__ << ": r" << i+1 << " maxValue = " << maxValue << endl;
#endif
				
				if (!tequal<T>(maxValue, T(0), precision/*1e-5*/))
					N[i] = T(1) / maxValue;
				else
					N[i] = T(0); // zero row indicates a singular matrix (if matrix is square) but this is not checked here
			}
		}
	
		return N;
	}


	// convert matrix into echelon form (via forward elimination).  This puts zeros below every pivot variable and zero-rows are placed at the bottom of the matrix.  Each row's first non-zero entry a(ij) is to the right of the first non-zero entry a(i'j') in the previous row (where i'=i-1 and j'<j).  The first non-zero entry in a row is the pivot variable.  Implicit pivoting is used to improve numerical stability.  Degenerate rows where 0x1 + 0x2 + ... + 0xm = b (b≠0) are filtered to the bottom of the matrix.  If is left to a seperate function to determine whether the system has any solution.  The matrix and pivotIndex are assumed to be valid (not NULL).  pivotIndex values are assumed to be initialised to 0.  Any zero row will have its pivot index unchanged and a pivot index of 0 corresponds with a zero row in echelon form
	template <typename T>
	bool matrix<T>::convertToEchelonForm(unsigned int *pivotIndex) {

		unsigned int		i, j, j1, r1, *pivotIndexPtr = pivotIndex;
		bool				eop, found;

		// create row normalisation coefficients
		T *N = createRowNormalisationCoeffVector();
		
		if (!N)
			return false;

		// submatrix has order (n' x m') where n' = n-(r1-1), m' = m-(j1-1)
		// define submatrix base element a(r1, j1)
		j1 = r1 = 1;
	
		eop = false;
	
		while (!eop) {
		
#ifdef __GU_DEBUG_MATRIX__
			cout << "r1 = " << r1 << endl;
#endif
		
			// Find first non-zero entry in current submatrix
		
			j = j1;
			found = false;
		
			while (j<=m && !found) {
			
				i = r1;
			
				while (i<=n && !found) { // search current column j
				
					if (!tequal(T(0), element(i, j), precision/*1e-5*/)) {
					
						found = true;
					
						// once found we can look for largest pivot in remainer of column and apply implicit pivoting.  i points to the first pivot in column j so at least one pivot value will exist in column j
					
						auto Aptr = M.get() + ((j-1) * n) + (i-1);
					
						// initialise pivot value to pivot value at i
						T maxPivotValue = abs(*Aptr) * N[(i-1)];
						Aptr++;
					
						// check rest of column to see if any larger (normalised) pivot exists
						for (unsigned int i_ = i+1;i_<=n;i_++, Aptr++) {
						
							T a = abs(*Aptr) * N[(i_-1)]; // normalise value in column for comparison
						
							if (a > maxPivotValue) {
							
								maxPivotValue = a;
								i = i_; // pivot row index i is set to new largest pivot value in column
							}
						}
					
					}
					else
						i++;
				}
			
				if (!found) // remainder of column was zero so move to next column
					j++;
			}
		
		
#ifdef __GU_DEBUG_MATRIX__
			cout << "pivot at : i=" << i << ", j=" << j << endl;
			cout << (*this) << endl;
#endif
		
			// a(ij) references first non-zero element (if !found then end of process reached)
			if (found) {
		
				if (r1<n) { // not on last row of matrix
		
#ifdef __GU_DEBUG_MATRIX__
					cout << "row scaling..." << endl;
#endif
				
					if (i!=r1) { // i >= r1

						// First non-zero element not in first row of submatrix so swap row i with r1
						rowInterchange(r1, i);
					
						// swap row normalisation factors
						swap(N[i-1], N[r1-1]);
					}
		
					
					// Now replace each row ri (i>r1) so all entries below a(r1,j) evaluate to 0.0
					// Note:  We do not recalculate N[] used for implicit pivoting above (see Lengyel p.422-p.424).
					
					T c_denom = T(1) / element(r1, j);
					
					for (i=r1+1;i<=n;i++) {
					
#if 1
						// c = -a(ij)/pivot approach, where pivot = a(r1, j)
					
						// Perform matrixRowScaleAdd but only on relevant submatrix.  Set elements below pivot to 0.0 so avoid floating point errors that might accumulate.
						T c = (element(i, j)) * c_denom;
					
						// Note: if 0s exist below the pivot then m = 0.  The result is that the following row operation *iptr = (*r1ptr * m) + *iptr reduces to *iptr = 0 + *iptr = *iptr which leaves the row unaffected.  The effect of this is that applying an echelon matrix to this function leaves it unchanged.
						auto iptr = M.get() + ((j-1) * n) + (i-1);
						*iptr = 0.0f;
					
						iptr += n; // iptr now on remainder of row to change
					
						auto r1ptr = M.get() + (j * n) + (r1-1);
					
						for (unsigned int p=j+1;p<=m;p++, iptr+=n, r1ptr+=n)
							*iptr -= *r1ptr * c;
					
#else
						// c = -aijR1 + pivot Ri approach - numbers get very big for larger matrices introducing numerical precision problems.  Normalise each row to improve this result.
						matrixRowScale(A, i, 1.0f/_m(A, i, j));
						matrixRowScaleAdd(A, r1, -(_m(A, i, j)), i, _m(A, r1, j));
#endif
					
					}
				}

				if (pivotIndexPtr) {
				
					*pivotIndexPtr = j;
					pivotIndexPtr++;
				}
		
				r1++;
				j1 = j+1;
		
			} else {
			
				eop = true;
			}

#ifdef __GU_DEBUG_MATRIX__
			cout << "after row scaling..." << endl;
			cout << (*this) << endl;
#endif
		
		}
	
		// dispose local resources
		free(N);

		return true;
	}


	template <typename T>
	bool matrix<T>::convertToRowCanonicalForm(unsigned int *pivotIndex) {

		unsigned int		j, j_, r;
		T					c, *jptr;

#ifdef __GU_DEBUG_MATRIX__
		cout << __FUNCTION__ << ": pivotIndex..." << endl; 
		for (unsigned int *iptr=pivotIndex, i=0;i<n;i++, iptr++)
			cout << "row " << i+1 << ": " << *iptr << endl;
#endif
		
		r = n;
	
		while (r>=1) {
	
			if (pivotIndex[r-1] > 0) { // note: use [r-1] since pivotIndex is zero indexed
						
				j = pivotIndex[r-1];
			
#ifdef __GU_DEBUG_MATRIX__
				cout << "pivot: row=" << r << "  col=" << j << endl;
#endif

				c = T(1)/element(r, j);
			
				// Given a(rj) as the pivot, scale Rr by 1.0/a(rj) so the pivot becomes 1.0
				
				jptr = M.get() + ((j-1) * n) + (r-1);
				*jptr = T(1);
			
				jptr += n;
				j_ = j+1;
			
				while (j_<=m) {
				
					*jptr *= c;
				
					jptr += n;
					j_++;
				}
			
			
				// Zero out elements above a(rj) - since Rr has non-zero pivot all rows Ri (i<r) are non-zero and have a valid pivot
				if (r>1) {
				
					//matrixRowAddition(A, r, -(_m(A, i, j)), i, 0); // row scale coefficient m=a(ij)
				
					for (int rd=1, i=r-1;i>=1;i--, rd++) {
					
						c = -(element(i, j));
					
						j_ = pivotIndex[i-1];
					
						jptr = M.get() + ((j_-1) * n) + (i-1);
					
						T *rptr = jptr + rd;
					
						while(j_<=m) {
						
							if (j_==j)
								*jptr = T(0);
							else
								*jptr = *rptr * c + *jptr;
						
							j_++;
						
							jptr += n;
							rptr += n;
						}
					}
				}
			}

			// Move to previous row to search for next pivot
			r--;
		}

		return true;
	}


	template <typename T>
	matrix<T> matrix<T>::echelonForm() const {

		matrix<T> M(*this);

		if (!M.is_null())
			M.convertToEchelonForm(nullptr);

		return M;
	}


	template <typename T>
	matrix<T> matrix<T>::row_canonical_form() const {
		
		matrix<T> M(*this);

		if (M.is_null())
			return matrix<T>::nullmatrix();

		// create pivot index vector and initialise to 0
		auto P = std::unique_ptr<unsigned int, decltype(&::free)>((unsigned int*)calloc(M.n, sizeof(unsigned int)), ::free);
		auto pivotIndex = P.get();

		if (pivotIndex==nullptr)
			return matrix<T>::nullmatrix();
		
		// perform in-place convertion of M to echelon then row-canonical form
		bool status = M.convertToEchelonForm(pivotIndex);

		if (!status)
			return matrix<T>::nullmatrix();

		status = M.convertToRowCanonicalForm(pivotIndex);

		if (!status)
			return matrix<T>::nullmatrix();
		
		return M;
	}


	template <typename T>
	T matrix<T>::cofactor(unsigned int i, unsigned int j) const {

		static const T sign[2] = {T(1), T(-1)}; // use i+j&0x01 as index to obtain correct cofactor sign for element a(ij)

		return sign[i+j&0x01] * remove_row_col(i, j).det();
	}


	template <typename T>
	matrix<T> matrix<T>::cofactors() const {

		if (is_null() || !is_square())
			return matrix<T>::nullmatrix();

		matrix<T> C = matrix<T>(n, m);

		if (C.is_null())
			return matrix<T>::nullmatrix();

		auto cptr = C.M.get();

		for (unsigned int j=1; j<=m; j++) {

			for (unsigned int i=1; i<=n; i++, cptr++) {

				*cptr = cofactor(i, j);
			}
		}

		return C;
	}


	template <typename T>
	matrix<T> matrix<T>::adj() const {

		return cofactors().transpose();
	}


	template <typename T>
	matrix<T> matrix<T>::balance(std::vector<T> *scale) const {

		if (is_null() || !is_square())
			return matrix<T>::nullmatrix();

		matrix<T> A = matrix<T>(*this);

		if (!A.is_null()) {

			const T radix = T(2);
			T sqrdx = radix * radix;

			unsigned int last = 0;

			while (last==0) {

				last = 1;

				for (unsigned int i=1; i<=A.n; i++) {

					T r = T(0);
					T c = T(0);

					for (unsigned int j=1; j<=A.n; j++) {

						if (j != i) {

							c += abs(A(j, i));
							r += abs(A(i, j));
						}
					}

					if (c && r) {

						T g = r / radix;
						T f = T(1);
						T s = c + r;

						while (c < g) {

							f *= radix;
							c *= sqrdx;
						}

						g = r * radix;

						while (c > g) {

							f /= radix;
							c /= sqrdx;
						}

						if ( (c + r)/f < T(0.95)*s) {

							last = 0;

							g = T(1) / f;

							if (scale)
								(*scale)[(i)-1] *= f;

							for (unsigned int j=1; j<=A.n; j++)
								A(i, j) *= g;

							for (unsigned int j=1; j<=A.n; j++)
								A(j, i) *= f;
						}
					}
				}
			}
		}

		return A;
	}


	template <typename T>
	matrix<T> matrix<T>::solveCanonicalFreeVarSystem(const std::vector<unsigned int>& F, const std::vector<T> &V, const std::vector<unsigned int>& P, unsigned int rankA, unsigned int dimX) {

		unsigned int			i, j, l;

		matrix<T> R(dimX, 1); // result is a (dimX x 1) column vector

		// initialise R with known free variables
		for (i=0; i<F.size(); i++)
			R(F[i], 1) = V[i];

		l = 0;

		// solve for each pivot
		for (i=0; i<rankA; i++) {

			// skip free variables that do not apply to current (and subsequent rows)
			while (l < F.size() && F[l] < P[i])
				l++;

			// initialise pivot value with relevant value from the given solution vector in the given (row canonical) matrix
			R(P[i], 1) = a(i+1, m);

			for (j=l; j<F.size(); j++)
				R(P[i], 1) += -a(i+1, F[j]) * V[j]; // process elements corresponding to free variables only - since given matrix in row canonical form, elements at non-free(pivot) indices are 0
		}

		return R;
	}


	template <typename T>
	matrix<T> gaussianElimination(const matrix<T>& A, const matrix<T>& B, gu_lsys_state *solution, gu_lsystem_aux<T> *L) {
		
		if (A.is_null() || B.is_null() || (A.n != B.n)) { // A.n = B.n so we can concatenate, but A.m != B.n can be true (A not necessarily square)

			*solution = gu_no_solution;
			return matrix<T>::nullmatrix();
		}

		matrix<T> M = A | B;

		if (M.is_null()) {

			*solution = gu_no_solution;
			return matrix<T>::nullmatrix();
		}


		if (L)
			L->num_unknowns = A.m;


		// create pivot index vector and initialise to 0
		auto P = std::unique_ptr<unsigned int, decltype(&::free)>((unsigned int*)calloc(M.n, sizeof(unsigned int)), ::free);

		auto pivotIndex = P.get();

		if (pivotIndex==nullptr) {

			*solution = gu_no_solution;
			return matrix<T>::nullmatrix();
		}


		// convert M to echelon form and return pivot indices in pivotIndex
		M.convertToEchelonForm(pivotIndex);


		// setup std::vector based on returned pivotIndex pointer (** update this later so always use std::vector)
		auto P_vec = std::vector<unsigned int>(M.n);
		for (unsigned int i=0; i<M.n; i++)
			P_vec[i] = pivotIndex[i];


		// export echelon form of system along with generated pivot indices
		if (L) {
			
			L->P = P_vec;
			L->E = M;
		}


#ifdef __GU_DEBUG_MATRIX__
		cout << "echelon form of M..." << endl;
		cout << M << endl;
#endif


		// Make sure there are no degenerate rows where 0x + 0y + ... 0z = b where b ≠ 0.  If so return gu_no_solution.  Otherwise determine if there is a unique or ∞ number of solutions
	
		// From Lipschutz & Lipson p.79 theorem 3.8...
		// A solution exists iff Rank(A) = Rank(M)
		// A unique solution exists iff Rank(A) = Rank(M) = A.n
	
		// Rank(M) equals the number of pivots in the echelon form of M.  Given M' as the echelon form of M = [A, B] automatically gives the echelon form of A = A' we do not need to calculate A' seperately.  Given M and M' are of the order (A->n, A->m + B->m) we use the submatrix of M' [M'->n, M'->m-B->m] as A'.  So although a rank function exists we use M' directly here for efficiency.

		unsigned int		rankA, rankM;
	
		rankA = rankM = 0;
	
		for (unsigned int i=0;i<M.n;i++) { // process each row - use pivotIndex to find rank
	
			if (pivotIndex[i]>0) {
			
				rankM++;
			
				if (pivotIndex[i]<=A.m)
					rankA++;
			}
		}

#ifdef __GU_DEBUG_MATRIX__
		cout << "Rank(M) = " << rankM << endl << "Rank(A) = " << rankA << endl << "#unknowns = " << A.m << endl;
#endif

		// export rank values
		if (L) {

			L->rankA = rankA;
			L->rankM = rankM;
		}


		// calculate number of free variables in the system
		unsigned int s = A.m - rankA;
		
		// build free variable vector F for coefficient matrix A
		auto F = std::vector<unsigned int>(s);

		if (s>0) {
			
			for (unsigned int i=0, k=0, f=1; f<=A.m; f++) {

				if (f==pivotIndex[i])
					i++;
				else
					F[k++] = f;
			}
		}


		// export free variable vector (number of free variables stored in F.size())
		if (L)
			L->F = F;


		if (rankA == rankM) {

			// consistent

			M.convertToRowCanonicalForm(pivotIndex);

#ifdef __GU_DEBUG_MATRIX__
			cout << "row canonical form of M...\n" << M << endl;
#endif

			// export row canonical form of system
			if (L)
				L->C = M;


			if (rankM == A.m) {

				// rank = number of unknowns (columns in A) therefore unique solution exists
				// (see Kreyszig 7th ed. p 351 case b)
		
				*solution = gu_unique_solution;

				// extract the solution submatrix into S (handle multiple solution columns - see Besset, OO Numerical Methods, p.245)
				return M.submatrix(1, A.m+1, A.m, B.m); // A.m rows = A.n rows since A is square in this case

			} else {

#ifdef __GU_DEBUG_MATRIX__
				cout << "num free vars = " << s << endl;
#endif

				// unique or infinite number of solutions exist (consistent)
				*solution = gu_inf_solution;

				// create a matrix where each column represents the solution for each free variable fi = 1 and fj (j!=i) = 0.  For a homogeneous system where B=[0] the resulting vectors represent the basis for the solution space, which is the basis of the kernel space of A.  For a non-homogeneous system where B != [0] the vectors represent just one set of solutions.  In this case, if the caller requires additional solutions L must be returned and a solution must be recalculated for each free variable the caller wants to evaluate

				matrix<T> S(A.m, s);

				// initialise vector containing values for identified free variables in F
				auto V = std::vector<T>(F.size());

				// initialise V so all elements are 0
				for (unsigned int i=0; i<V.size(); i++)
					V[i] = T(0);

				// calculate solution vector for free variable fi where V[i] = 1 and V[j] (j!=i) = 0
				for (unsigned int i=0; i<s; i++) {

#ifdef __GU_DEBUG_MATRIX__
					cout << "\nfree var : " << i << "..." << endl;
#endif

					// set relevant free variable value to 1
					if (i>0)
						V[i-1] = T(0); // unset free variable
					
					V[i] = T(1);

#ifdef __GU_DEBUG_MATRIX__

					cout << "F = (  ";
					for (unsigned int k=0; k<F.size(); k++)
						cout << F[k] << "  ";
					cout << ")" << endl;

					cout << "V = (  ";
					for (unsigned int k=0; k<V.size(); k++)
						cout << V[k] << "  ";
					cout << ")" << endl;

#endif

					matrix<T> X = M.solveCanonicalFreeVarSystem(F, V, P_vec, rankA, A.m);

#ifdef __GU_DEBUG_MATRIX__
					cout << "X = " << X << endl;
#endif

					S.set_submatrix(1, i+1, X);
				}

				return S;
			}

		} else {

			// inconsistent - no solution

			*solution = gu_no_solution;

			return matrix<T>::nullmatrix();
		}

	}



	
	// LUP factorisation

	template <typename T>
	gu_lu_decomp_state matrix<T>::lup_decomposition_doolittle(std::vector<unsigned int> *permutationVector, T *parity) {
	
		unsigned int		i, j, k;
		int					pivotRowIndex;
		T					sum, maxValue;
	
		// create row normalisation array
		T *N = createRowNormalisationCoeffVector();
	
		T rowParity = T(1);
		gu_lu_decomp_state luState = gu_lu_okay;
		
		for (j=1; j<=n && luState==gu_lu_okay; j++) {
	
			// First apply eq.14.15.  Since U(1j) = A(1j) we do not need to change the top row of A since U(1j) = A(1j).  For elements on the leading diagonal, we apply eq. 14.17 later.  Therefore we only apply eq.14.15 to elements a(ij) where 1<i<j
		
			for (i=2; i<j; i++) {
			
				sum = element(i, j);
				
				for (k=1; k<i; k++)
					sum -= element(i, k) * element(k, j);
			
				element(i, j) = sum;
			}


			// debug
			// if (j==3)
			// 	cout << "debug 1 : j = " << j << " :: lup=\n" << (*this) << endl;


			// Apply eq.14.16 to elements {a(ij):j<=i<=n} (ie. elements on and below the leading diagonal for the current column.)  Given the 1.0/U(jj) scale factor in eq.14.16, we need to pivot in the column so the largest positive value lies at U(jj).  To do this apply eq.14.17 in-place to calculate {P(ij):j<=i<=n}, pivot on the largest calculated value then scale the calculated {P(ij):j<=i<=n} elements
		
			pivotRowIndex = -1;
			maxValue = T(0);
		
			for (i=j; i<=n; i++) {
			
				// apply eq.14.17 to calculate P(ij) (eq.14.16 without the 1/U(jj) scale)
				T p_ij = element(i, j);
			
				for (k=1; k<j; k++)
					p_ij -= element(i, k) * element(k, j);
			
				element(i, j) = p_ij;

				// check if current P(ij) is the largest absolute (scaled) value (implicit pivoting)
				p_ij = abs(p_ij) * N[i-1];
			
				if (tgreater<T>(p_ij, maxValue, precision)) {
				//if (p_ij > maxValue) {
				
					maxValue = p_ij;
					pivotRowIndex = i;
				}
			}
		
		
			// debug
			//if (j==3)
			//	cout << "debug 2 : j = " << j << " :: lup=\n" << (*this) << endl;


			// Now pivot if necessary
			if (pivotRowIndex != j) {
			
				if (pivotRowIndex!=-1) {
				
					// valid pivot found - exchange rows
					for (k=1; k<=n; k++)
						swap(element(j, k), element(pivotRowIndex, k));
				
					// swap row indices in permutation vector if defined
					if (permutationVector)
						swap((*permutationVector)[j-1], (*permutationVector)[pivotRowIndex-1]);
				
					// invert row parity (sign) after row swap
					rowParity = -rowParity;
				
					// swap normalisation coefficients
					swap(N[j-1], N[pivotRowIndex-1]);
				
				} else {
				
					luState = gu_lu_fail_singular; // LU decomposition is to terminate if A is singular.  If pivotIndex = -1 then all elements below and including U(jj) are zero.  This results in Rank(A) < Rank(M) given M as the augmented linear system [A, B] - see below.  In this case A is singular
				}
			}


			// debug
			//if (j==3)
			//	cout << "debug 3 : j = " << j << " :: lup=\n" << (*this) << endl;


			// Finally divide by pivot element ie. apply 1/(Ujj) to {P(ij):j<i<=n} note: we do not scale U(jj) itself.
			if (luState==gu_lu_okay && j<n) {
			
				T d = T(1) / element(j, j);
			
				for (i=j+1; i<=n; i++)
					element(i, j) *= d;
			}

			// debug
			//if (j==3)
			//	cout << "debug 4 : j = " << j << " :: lup=\n" << (*this) << endl;
		}
	
		// store rowParity in *parity if defined
		if (parity)
			*parity = rowParity;
	
		// Dispose of local resources
		free(N);
		
		return luState;
	}

	
	template <typename T>
	gu_lu_decomp_state matrix<T>::lup_decomp(matrix<T> *D, std::vector<unsigned int> *P) const {

		matrix<T> D_ = matrix<T>(*this);

		if (D_.is_null() || !D_.is_square())
			return gu_lu_fail_singular;

		std::vector<unsigned int> v = identity_permutation_vec(D_.n);

		gu_lu_decomp_state lu_state = D_.lup_decomposition_doolittle(&v, NULL);

		if (lu_state != gu_lu_okay)
			return lu_state;

		*P = std::move(v);
		*D = std::move(D_);

		return gu_lu_okay;
	}


	template <typename T>
	gu_lu_decomp_state matrix<T>::lup_decomp(matrix<T> *L, matrix<T> *U, matrix<T> *P) const {

		matrix<T> D = matrix<T>(*this);

		if (D.is_null() || !D.is_square())
			return gu_lu_fail_singular;

		std::vector<unsigned int> v = identity_permutation_vec(D.n);

		gu_lu_decomp_state lu_state = D.lup_decomposition_doolittle(&v, NULL); 

		if (lu_state != gu_lu_okay)
			return lu_state;

		// extract LU
		auto L_ = matrix<T>::zeromatrix(D.n, D.n);
		auto U_ = matrix<T>::zeromatrix(D.n, D.n);

		// extract P - convert permutation vector into permutation matrix
		auto P_ = matrix<T>::permutation_matrix(v);

		if (L_.is_null() || U_.is_null() || P_.is_null())
			return gu_lu_fail_error;

		auto Lptr = L_.M.get();
		auto Uptr = U_.M.get();
		
		for (unsigned int j=1; j<=D.n;j++) { // process column j

			for (unsigned int i=1; i<=D.n; i++, Lptr++, Uptr++) { // process row i
				
				// process element a(ij)...

				if (i==j) { // from Doolittle, leading diagonal of L contains 1.0 only (unit lower triangular matrix)
				
					*Lptr = 1.0f;
					*Uptr = D(i, j);
				
				} else if (i<j) { // in U triangle region

					*Uptr = D(i, j);
				
				} else { // i>j - in L triangle region
				
					*Lptr = D(i, j);
				}
			}
		}
		
		if (L) *L = L_;
		if (U) *U = U_;
		if (P) *P = P_;

		return gu_lu_okay;
	}

	
	template <typename T>
	matrix<T> lup_solve(const matrix<T>& D, const std::vector<unsigned int>& P, const matrix<T>& B) {
	
		matrix<T> x(D.n, 1);

		if (x.is_null())
			return matrix<T>::nullmatrix();
		
		auto xptr = x.M.get();
		auto bptr = B.M.get();
		
		// initialise x to the constants in r but mapped via permutation vector P
		for (unsigned int i=0; i<D.n; i++)
			xptr[i] = bptr[P[i]-1]; // index B with P[i]-1 here since permutation indices start at 1, not 0
	
		// perform forward substitution step Ly = r : eq.14.4 (where L(ii) = 1.0 so no division necessary)
		for (unsigned int i=1; i<=D.n; i++) {
			
			T s = xptr[i-1];
			
			for (unsigned int k=1; k<i; k++)
				s -= D(i, k) * xptr[k-1];
			
			xptr[i-1] = s;
		}
		
			
		// perform backward substitution step Ux = y : eq.14.7
		for (unsigned int i=D.n; i>=1; i--) {

			auto s = xptr[i-1];
			
			for (unsigned int k=i+1; k<=D.n; k++)
				s -= D(i, k) * xptr[k-1];
			
			xptr[i-1] = s / D(i, i);
		}

		return x;
	}


	template <typename T>
	gu_cholesky_state matrix<T>::cholesky_decomp(matrix<T> *C) const {

		if (is_null() || !is_real() || !is_square() || !is_symmetric()) {

			*C = matrix<T>::nullmatrix();
			return gu_cholesky_error;
		}

		matrix<T> A = matrix<T>(*this);

		if (A.is_null()) {

			*C = matrix<T>::nullmatrix();
			return gu_cholesky_error;
		}

		unsigned int			i, j, k;
		gu_cholesky_state		cState = gu_cholesky_okay;
	
		// calculate each column in turn
		for (j=1; j<=A.n && cState==gu_cholesky_okay; j++) {
		
			// calc L(jj)
			auto Ljj = A(j, j);
		
			for (k=1; k<j; k++) {

				auto a = A(j, k);
				Ljj -= a*a;
			}
			
			if (Ljj > 0.0f) { // check if A is positive definite
		
				Ljj = sqrt(Ljj);
				A(j, j) = Ljj;
			
				// initialise elements a(1, j) to a(j-1, j) since the Cholesky decomposition is in lower triangle form
				for (i=1; i<j; i++)
					A(i, j) = 0.0f;
			
				// process elements a(j+1, j) to a(n, j)
				for (i=j+1; i<=A.n; i++) {
				
					auto sum = A(i, j);
				
					for (k=1; k<j; k++)
						sum -= A(i, k) * A(j, k);
				
					A(i, j) = sum / Ljj;
				}
		
			} else {
			
				cState = gu_cholesky_not_pd; // A is not positive definite.  This function can be used to determine this property for square symmetric real matrices or we can apply Sylvester's criteria where the determinant of each submatrix (1x1, 2x2, ... , nxn) located at a(1, 1) > 0 (ie. all the principle minors are positive)
		
			}
		}

		if (cState==gu_cholesky_okay)
			*C = A;
		else
			*C = matrix<T>::nullmatrix();

		return cState;
	}


	// Hessenberg form

	template <typename T>
	matrix<T> matrix<T>::hessenberg_form(std::vector<unsigned int> *perm, gu_hessenberg_state *state, bool zeroL) const {

		if (is_null() || !is_real()) {

			if (state)
				*state = gu_hessenberg_error;

			return matrix<T>::nullmatrix();
		}

		matrix<T> A = matrix<T>(*this);

		if (A.is_null()) {

			if (state)
				*state = gu_hessenberg_error;

			return matrix<T>::nullmatrix();
		}

		(*perm) = std::vector<unsigned int>(n);

		for (unsigned int m=2; m<A.n; m++) {

			T x = T(0);

			unsigned int i = m;

			for (unsigned int j=m; j<=A.n; j++) {

				if (abs(A(j, m-1)) > abs(x)) {

					x = A(j, m-1);
					i = j;
				}
			}

			// store permutation (permutations are indexed [0, n-1] not [1, n] as are matrix rows and columns
			(*perm)[(m)-1] = i;

			if (i != m) { // interchange rows and columns

				for (unsigned int j=m-1; j<=A.n; j++)
					swap(A(i, j), A(m, j));

				for (unsigned int j=1; j<=A.n; j++)
					swap(A(j, i), A(j, m));
			}

			if (x) { // perform elimination step

				for (unsigned int i=m+1; i<=A.n; i++) {

					T y = A(i, m-1);

					if (y) {

						y /= x;
						A(i, m-1) = y;

						for (unsigned int j=m; j<=A.n; j++)
							A(i, j) -= y * A(m, j);

						for (unsigned int j=1; j<=A.n; j++)
							A(j, m) += y * A(j, i);
					}
				}
			}
		}

		if (zeroL) { // zero-out values below first sub-diagonal

			for (unsigned int j=1; j<=A.n-2; j++) {
			
				for (unsigned int i=j+2; i<=A.n; i++)
					A(i, j) = T(0);
			}
		}

		if (state)
			*state = gu_hessenberg_okay;

		return A;
	}



	
	//
	// Eigen-system extraction
	//

	template <typename T>
	matrix<T> matrix<T>::eltran(const std::vector<unsigned int>& P) const {

		matrix<T> Z = matrix<T>::I(n);

		for (unsigned int mp=n-1; mp>1; mp--) {

			for (unsigned int k=mp+1; k<=n; k++)
				Z(k, mp) = a(k, mp-1);

			unsigned int i = P[(mp)-1];

			if (i != mp) {

				for (unsigned int j=mp; j<=n; j++) {

					Z(mp, j) = Z(i, j);
					Z(i, j) = T(0);
				}

				Z(i, mp) = T(1);
			}
		}

		return Z;
	}
	

	template <typename T>
	std::vector<std::complex<T>> matrix<T>::hqr() {

		int				i, j, k, l, m_, nn, its, mmin;
		T				u, v, w, x, y, z, t, s, q, r, p, anorm;

		std::vector<std::complex<T>> E_;
		auto E = std::vector<std::complex<T>>(n);

		// compute matrix norm
		anorm = abs(a(1, 1));
		for (i=2; i<=(int)n; i++) {

			for (j=(i-1); j<=(int)n; j++)
				anorm += abs(a(i, j));
		}

		nn = n;
		t = T(0);

		while (nn >= 1) {

			its = 0;

			do {

				// begin iteration - look for single small subdiagonal element
				for (l=nn; l>=2; l--) {

					s = abs(a(l-1, l-1)) + abs(a(l, l));
					
					if (s == T(0))
					//if (tequal<T>(s, T(0), precision))
						s = anorm;
					if (abs(a(l, l-1)) + s == s)
					//if (tequal<T>(abs(a(l, l-1)) + s, s, precision))
						break;
				}

				x = a(nn, nn);
				
				if (l == nn) { // one root found

					E[nn-1] = std::complex<T>(x+t, T(0));
					nn--;
				
				} else {

					y = a(nn-1, nn-1);
					w = a(nn, nn-1) * a(nn-1, nn);
					
					if (l == (nn-1)) { // two roots found

						p = T(0.5) * (y - x);
						q = p * p + w;
						z = sqrt(abs(q));
						x += t;
						
						if (q >= T(0)) { // real pair

							z = p + ( (p > T(0)) ? abs(z) : -abs(z) );
							//z = p + ((tgreater<T>(p, T(0), precision)) ? abs(z) : -abs(z));

							if (z)
								E[nn-1] = std::complex<T>(x-w/z, T(0));
							else
								E[nn-1] = std::complex<T>(x+z, T(0));
							
							E[(nn-1)-1] = std::complex<T>(x+z, T(0));

						} else { // complex pair

							E[nn-1] = std::complex<T>(x+p, z);
							E[(nn-1)-1] = std::complex<T>(x+p, -z);
						}

						nn -= 2;

					} else { // no roots found - continue iteration

						if (its==30) {
							std::cout << "error - too many iterations in hqr\n";
							goto hqr_fail;
						}

						if (its==10 || its==20) { // form exceptional shift

							t += x;

							for (i=1; i<=nn; i++)
								a(i, i) -= x;

							s = abs(a(nn, nn-1)) + abs(a(nn-1, nn-2));
							y = x = T(0.75) * s;
							w = T(-0.4375) * s * s;
						}

						++its;

						// form shift and look for 2 consecutive sub-diagonal elements
						for (m_=(nn-2); m_>=l; m_--) {

							z = a(m_, m_);
							r = x - z;
							s = y - z;

							p = (r * s - w) / a(m_+1, m_) + a(m_, m_+1);
							q = a(m_+1, m_+1) - z - r - s;
							r = a(m_+2, m_+1);
							s = abs(p) + abs(q) + abs(r);

							p /= s;
							q /= s;
							r /= s;

							if (m_ == l)
								break;

							u = abs(a(m_, m_-1)) * (abs(q) + abs(r));
							v = abs(p) * (abs(a(m_-1, m_-1)) + abs(z) + abs(a(m_+1, m_+1)));

							if (u+v == v)
							// if (tequal<T>(u + v, v, precision))
								break;
						}

						for (i=m_+2; i<=nn; i++) {

							a(i, i-2) = T(0);
							
							if (i != (m_+2))
								a(i, i-3) = T(0);
						}

						// double QR step on rows l to nn and columns m_ to nn
						for (k=m_; k<=nn-1; k++) {

							if (k != m_) { // begin setup of Householder vector

								p = a(k, k-1);
								q = a(k+1, k-1);
								r = T(0);

								if (k != (nn-1))
									r = a(k+2, k-1);

								if (x=abs(p)+abs(q)+abs(r)) {

									p /= x;
									q /= x;
									r /= x;
								}
							}

							T pqr_ = sqrt(p*p + q*q + r*r);

							s = (p > T(0)) ? abs(pqr_) : -abs(pqr_);
							// s = (tgreater<T>(p, T(0), precision)) ? abs(pqr_) : -abs(pqr_);

							if (s) {

								if (k == m_) {

									if (l != m_)
										a(k, k-1) = -a(k, k-1);
								
								} else {

									a(k, k-1) = -s * x;
								}

								p += s;
								
								x = p / s;
								y = q / s;
								z = r / s;
								
								q /= p;
								r /= p;

								for (j=k; j<=nn; j++) {

									p = a(k, j) + q * a(k+1, j);

									if (k != (nn-1)) {

										p += r * a(k+2, j);
										a(k+2, j) -= p * z;
									}

									a(k+1, j) -= p * y;
									a(k, j) -= p * x;
								}

								mmin = (nn < k+3) ? nn : k+3;

								for (i=l; i<=mmin; i++) {

									p = x * a(i, k) + y * a(i, k+1);

									if (k != (nn-1)) {

										p += z * a(i, k+2);
										a(i, k+2) -= p * r;
									}

									a(i, k+1) -= p * q;
									a(i, k) -= p;
								}
							}
						}
					}
				}

			} while (l < nn-1);
		}

		return E;

hqr_fail:

		return E_;
	}


	template <typename T>
	std::vector<std::complex<T>> matrix<T>::hqr2(matrix<T> *Z) {

		int		i, j, k, l, m, na, nn, its, mmin;
		T		p, q, r, s, t, u, v, w, x, y, z, anorm=T(0), ra, sa, vr, vi;

		auto wri = std::vector<std::complex<T>>(n);

		// Compute matrix norm for possible use in locating single small subdiagonal element.
		for (i=0; i<(int)n; i++) {

			for (j=std::max(i-1, 0); j<(int)n; j++)
				anorm += abs(a_(i, j));
		}

		nn = n - 1;
		t = T(0);

		while (nn >= 0) {

			its = 0;

			do {

				for (l=nn; l>0; l--) {

					s = abs(a_(l-1, l-1)) + abs(a_(l, l));

					if (s==T(0))
						s = anorm;
					
					if (abs(a_(l, l-1)) <= T(gu_epsilon_d) * s) {

						a_(l, l-1) = T(0);
						break;
					}
				}

				x = a_(nn, nn);

				if (l==nn) { // one root found

					wri[nn] = a_(nn, nn) = x + t;
					nn--;

				} else {

					y = a_(nn-1, nn-1);
					w = a_(nn, nn-1) * a_(nn-1, nn);

					if (l == nn-1) { // two roots found

						p = T(0.5) * (y - x);
						q = p * p + w;

						z = sqrt(abs(q));
						x += t;
						a_(nn, nn) = x;
						a_(nn-1, nn-1) = y + t;

						if (q >= T(0)) { // a real pair
							
							z = p + ( (p > T(0)) ? abs(z) : -abs(z) ); // z = p + sgn(z, p);
							
							wri[nn-1] = wri[nn] = x + z;
						
							if (z != T(0))
								wri[nn] = x - w / z;
						
							x = a_(nn, nn-1);
							s = abs(x) + abs(z);
							p = x / s;
							q = z / s;
							r = sqrt(p * p + q * q);
							p /= r;
							q /= r;

							for (j=nn-1; j<(int)n; j++) { // Row modification
						
								z = a_(nn-1, j);
								a_(nn-1, j) = q * z + p * a_(nn,j);
								a_(nn, j) = q * a_(nn, j) - p * z;
							}

							for (i=0; i<=nn; i++) { // Column modification
						
								z = a_(i, nn-1);
								a_(i, nn-1) = q * z + p * a_(i, nn);
								a_(i, nn) = q * a_(i, nn) - p * z;
							}
						
							for (i=0; i<(int)n; i++) { // Accumulate transformations
						
								z = Z->a_(i, nn-1);
								
								Z->a_(i, nn-1) = q * z + p * Z->a_(i, nn);
								Z->a_(i, nn) = q * Z->a_(i, nn) - p * z;
						
							}
						
						} else { // ...a complex pair

							wri[nn] = std::complex<T>(x+p, -z);
							wri[nn-1] = std::conj(wri[nn]);
						}

						nn -= 2;
					
					} else { // no roots found, continue iteration

						if (its == 30) {
							
							std::cout << "Too many iterations in hqr";
							goto fail_hqr2;
						}

						if (its == 10 || its == 20) { // Form exceptional shift

							t += x;
							
							for (i=0; i<nn+1; i++)
								a_(i, i) -= x;

							s=abs(a_(nn, nn-1)) + abs(a_(nn-1, nn-2));
							y = x = T(0.75) * s;
							w = T(-0.4375) * s * s;
						}

						++its;
						
						for (m=nn-2; m>=l; m--) { // Form shift and then look for 2 consecutive small subdiagonal elements
							
							z = a_(m, m);
							r = x - z;
							s = y - z;
							p = (r * s-w) / a_(m+1, m) + a_(m, m+1);
							q = a_(m+1, m+1) - z - r - s;
							r = a_(m+2, m+1);
							s = abs(p) + abs(q) + abs(r); // Scale to prevent overflow or underflow
							p /= s;
							q /= s;
							r /= s;

							if (m == l)
								break;
						
							u = abs(a_(m, m-1)) * (abs(q) + abs(r));
							v = abs(p) * (abs(a_(m-1, m-1)) + abs(z) + abs(a_(m+1, m+1)));
						
							if (u <= T(gu_epsilon_d) * v)
								break;
						}

						for (i=m; i<nn-1; i++) {
							
							a_(i+2, i) = T(0);
							if (i != m)
								a_(i+2, i-1) = T(0);
						}

						// Double QR step on rows l to nn and columns m to nn.
						for (k=m; k<nn; k++) {

							if (k != m) {
								
								p = a_(k, k-1); // Begin setup of Householder vector
								q = a_(k+1, k-1);
								r = T(0);
								
								if (k+1 != nn)
									r = a_(k+2, k-1);

								if ((x = abs(p) + abs(q) + abs(r)) != T(0)) {
							
									p /= x; // Scale to prevent overflow or underflow
									q /= x;
									r /= x;
								}
							}

							if ((s=((p>T(0)) ? fabs(sqrt(p*p+q*q+r*r)) : -fabs(sqrt(p*p+q*q+r*r)))) != T(0)) {
								
								if (k == m) {
								
									if (l != m)
										a_(k, k-1) = -a_(k, k-1);

								} else
									a_(k, k-1) = -s * x;
								
								p += s;
								
								x = p/s;
								y = q/s;
								z = r/s;
								
								q /= p;
								r /= p;
								
								for (j=k; j<(int)n; j++) { // Row modification
								
									p = a_(k, j) + q * a_(k+1, j);
									
									if (k+1 != nn) {
										
										p += r * a_(k+2, j);
										a_(k+2, j) -= p * z;
									}
								
									a_(k+1, j) -= p * y;
									a_(k, j) -= p * x;
								}
								
								mmin = (nn < k+3) ? nn : k+3;
								
								for (i=0; i<mmin+1; i++) { // Column modification

									p = x * a_(i, k) + y * a_(i, k+1);
								
									if (k+1 != nn) {
										
										p += z * a_(i, k+2);
										a_(i, k+2) -= p * r;
									}

									a_(i, k+1) -= p * q;
									a_(i, k) -= p;
								}

								for (i=0; i<(int)n; i++) { // Accumulate transformations
								
									p = x * Z->a_(i, k) + y * Z->a_(i, k+1);
									
									if (k+1 != nn) {
										
										p += z * Z->a_(i, k+2);
										Z->a_(i, k+2) -= p * r;
									}

									Z->a_(i, k+1) -= p * q;
									Z->a_(i, k) -= p;
								}
							}

						}
					}
				}
			
			} while (l+1 < nn);
		}

		// All roots found.  Backsubstitute to find vectors of upper triangular form
		if (anorm != T(0)) {

			for (nn=(int)n-1; nn>=0; nn--) {

				p = real(wri[nn]);
				q = imag(wri[nn]);

				na = nn - 1;

				if (q == T(0)) { // Real vector

					m = nn;
					a_(nn, nn) = T(1);

					for (i=nn-1; i>=0; i--) {

						w = a_(i, i) - p;
						r = T(0);

						for (j=m; j<=nn; j++)
							r += a_(i, j) * a_(j, nn);

						if (imag(wri[i]) < T(0)) {

							z = w;
							s = r;
						
						} else {
							
							m = i;

							if (imag(wri[i]) == T(0)) {

								t = w;
								
								if (t == T(0))
									t = T(gu_epsilon_d) * anorm;

								a_(i, nn) = -r / t;

							} else { // Solve real equations

								x = a_(i, i+1);
								y = a_(i+1, i);
								q = sqr(real(wri[i]) - p) + sqr(imag(wri[i]));
								t = (x * s - z * r) / q;
								a_(i, nn) = t;
								
								if (abs(x) > abs(z))
									a_(i+1, nn) = (-r - w * t) / x;
								else
									a_(i+1, nn) = (-s - y * t) / z;
							}

							t = abs(a_(i, nn)); // Overflow control
							
							if (T(gu_epsilon_d) * t * t > T(1)) {

								for (j=i; j<=nn; j++)
									a_(j, nn) /= t;
							}
						}
					}

				} else if (q < T(0)) { // Complex vector - only do one case - Last vector component chosen imaginary so that eigenvector matrix is triangular.

					m=na;

					if (abs(a_(nn, na)) > abs(a_(na, nn))) {

						a_(na, na) = q / a_(nn, na);
						a_(na, nn) = -(a_(nn, nn) - p) / a_(nn, na);

					} else {

						auto temp = std::complex<T>(T(0), -a_(na, nn)) / std::complex<T>(a_(na, na) - p, q);

						a_(na, na) = real(temp);
						a_(na, nn) = imag(temp);
					}

					a_(nn, na) = T(0);
					a_(nn, nn) = T(1);

					for (i=nn-2; i>=0; i--) {
						
						w = a_(i, i) - p;
						ra = sa = T(0);
						
						for (j=m; j<=nn; j++) {
							
							ra += a_(i, j) * a_(j, na);
							sa += a_(i, j) * a_(j, nn);
						}
						
						if (imag(wri[i]) < T(0)) {
							
							z = w;
							r = ra;
							s = sa;
						
						} else {
							
							m=i;
							
							if (imag(wri[i]) == T(0)) {
							
								auto temp = std::complex<T>(-ra, -sa) / std::complex<T>(w, q);
								a_(i, na) = real(temp);
								a_(i, nn) = imag(temp);

							} else { // Solve complex equations

								x = a_(i, i+1);
								y = a_(i+1, i);
								vr = sqr(real(wri[i]) - p) + sqr(imag(wri[i])) - q * q;
								vi = T(2) * q * (real(wri[i]) - p);
								
								if (vr == T(0) && vi == T(0))
									vr = T(gu_epsilon_d) * anorm * (abs(w) + abs(q) + abs(x) + abs(y) + abs(z));

								auto temp = std::complex<T>(x * r - z * ra + q * sa, x * s - z * sa - q * ra) / std::complex<T>(vr, vi);

								a_(i, na) = real(temp);
								a_(i, nn) = imag(temp);

								if (abs(x) > abs(z)+abs(q)) {

									a_(i+1, na) = (-ra - w * a_(i, na) + q * a_(i, nn)) / x;
									a_(i+1, nn) = (-sa - w * a_(i, nn) - q * a_(i, na)) / x;

								} else {

									auto temp = std::complex<T>(-r-y*a_(i, na), -s-y*a_(i, nn)) / std::complex<T>(z, q);

									a_(i+1, na) = real(temp);
									a_(i+1, nn) = imag(temp);
								}
							}
						}

						t = std::max(abs(a_(i, na)), abs(a_(i, nn))); //Overflow control
						
						if (T(gu_epsilon_d) * t * t > T(1)) {

							for (j=i; j<=nn; j++) {
								
								a_(j, na) /= t;
								a_(j, nn) /= t;
							}
						}
					
					}
				}
			}

			// Multiply by transformation matrix to give vectors of original full matrix
			for (j=(int)n-1; j>=0; j--) {

				for (i=0; i<(int)n; i++) {
					
					z = T(0);
					
					for (k=0; k<=j; k++)
						z += Z->a_(i, k) * a_(k, j);

					Z->a_(i, j) = z;
				}
			}

		}

		return wri;

fail_hqr2:

		// return empty vector
		
		auto wri_empty = std::vector<std::complex<T>>(0);

		return wri_empty;
	}


	template <typename T>
	void matrix<T>::tred2(std::vector<T>* d, std::vector<T>* e) {

		int		i, j, k, l;
		T		f, g, h, hh, scale;
		
		for (i = (int)n; i>=2; i--) {

			l = i-1;
			h = scale = T(0);

			if (l > 1) {
				
				for (k=1; k<=l; k++)
					scale += abs(a(i, k));

				if (scale == T(0)) {

					(*e)[(i)-1] = a(i, l);
				
				} else {

					for (k=1; k<=l; k++) {

						a(i, k) /= scale;
						h += a(i, k) * a(i, k);
					}

					f = a(i, l);
					g = (f>T(0)) ? -sqrt(h) : sqrt(h);
					(*e)[(i)-1] = scale * g;
					h -= f * g;
					a(i, l) = f - g;
					f = T(0);
					
					for (j=1; j<=l; j++) {

						a(j, i) = a(i, j) / h;
						g = T(0);
						
						for (k=1; k<=j; k++)
							g += a(j, k) * a(i, k);
						for (k=j+1; k<=l; k++)
							g += a(k, j) * a(i, k);

						(*e)[(j)-1] = g / h;
						f += (*e)[(j)-1] * a(i, j);
					}

					hh = f / (h + h);

					for (j=1; j<=l; j++) {

						f = a(i, j);
						(*e)[(j)-1] = g = (*e)[(j)-1] - hh * f;
						
						for (k=1; k<=j; k++)
							a(j, k) -= (f * (*e)[(k)-1] + g * a(i, k));
					}
				}
				
			} else {

				(*e)[(i)-1] = a(i, l);
			}

			(*d)[(i)-1] = h;
		}
		
		(*d)[/*1*/0] = T(0);
		(*e)[/*1*/0] = T(0);

		for (i=1; i<=(int)n; i++) {

			l = i - 1;

			if ((*d)[(i)-1]) {

				for (j=1; j<=l; j++) {

					g = T(0);

					for (k=1; k<=l; k++)
						g += a(i, k) * a(k, j);
					for (k=1; k<=l; k++)
						a(k, j) -= g * a(k, i);
				}
			}

			(*d)[(i)-1] = a(i, i);
			a(i, i) = T(1);

			for (j=1; j<=l; j++)
				a(j, i) = a(i, j) = T(0);
		}
	}


	template <typename T>
	bool matrix<T>::tqli(std::vector<T>* d, std::vector<T>* e) {

		int				i, k, l, m, iter;
		T				c, b, f, g, p, r, s, dd;

		for (i=2; i<=(int)n; i++)
			(*e)[(i-1)-1] = (*e)[(i)-1];

		(*e)[(n)-1] = T(0);

		for (l=1; l<=(int)n; l++) {

			iter = 0;

			do {

				for (m=l; m<=(int)n-1; m++) {

					dd = abs( (*d)[(m)-1] ) + abs( (*d)[/*(m+1)-1*/m] );

					if (abs( (*e)[(m)-1] ) + dd == dd)
						break;
				}

				if (m != l) {

					if (iter++ == 30) {
						
						std::cout << "Too many iterations in TQLI" << endl;
						return false;
					}

					g = ((*d)[/*(l+1)-1*/l] - (*d)[(l)-1]) / (T(2) * (*e)[(l)-1]);
					r = sqrt((g * g) + T(1));
					g = (*d)[(m)-1] - (*d)[(l)-1] + (*e)[(l)-1] / (g + ( (g<T(0)) ? -abs(r) : abs(r) ));
					s = c = T(1);
					p = T(0);
					
					for (i=m-1; i>=l; i--) {

						f = s * (*e)[(i)-1];
						b = c * (*e)[(i)-1];

						if (abs(f) >= abs(g)) {

							c = g / f;
							r = sqrt((c * c) + T(1));
							(*e)[/*(i+1)-1*/i] = f * r;
							c *= (s=T(1)/r);
						
						} else {

							s = f / g;
							r = sqrt((s * s) + T(1));
							(*e)[/*(i+1)-1*/i] = g * r;
							s *= (c=T(1)/r);
						}

						g = (*d)[/*(i+1)-1*/i] - p;
						r = ((*d)[(i)-1] - g) * s + T(2) * c * b;
						p = s * r;
						(*d)[/*(i+1)-1*/i] = g + p;
						g = c * r - b;

						for (k=1; k<=(int)n; k++) {

							f = a(k, i+1);
							a(k, i+1) = s * a(k, i) + c * f;
							a(k, i) = c * a(k, i) - s * f;
						}
					}

					(*d)[(l)-1] = (*d)[(l)-1] - p;
					(*e)[(l)-1] = g;
					(*e)[(m)-1] = T(0);
				}

			} while (m != l);
		}

		return true;
	}


	template <typename T>
	void matrix<T>::balbak(const std::vector<T>& scale) {

		for (int i=0; i<(int)n; i++) {

			for (int j=0; j<(int)n; j++)
				a_(i, j) *= scale[i];
		}
	}

	
	template <typename T>
	void matrix<T>::sort_vecs(std::vector<std::complex<T>> *wri) {

		int		i;
		auto	temp = std::vector<T>(n);

		for (int j=1; j<(int)n; j++) {

			std::complex<T> x = (*wri)[j];

			for (int k=0; k<(int)n; k++)
				temp[k] = a_(k, j);

			for (i=j-1; i>=0; i--) {

				if (real((*wri)[i]) >= real(x))
					break;

				(*wri)[i+1] = (*wri)[i];

				for (int k=0; k<(int)n; k++)
					a_(k, i+1) = a_(k, i);
			}

			(*wri)[i+1] = x;

			for (int k=0; k<(int)n; k++)
				a_(k, i+1) = temp[k];
		}
	}


	template <typename T>
	bool matrix<T>::eigen_system(std::vector<std::complex<T>>* d, matrix<T>* Z) const {
	
		bool eigensystem_status = false;

		if (!is_null() && is_real() && is_square() && d && Z) {

			if (is_symmetric()) {

				// process symmetric real matrix

				*Z = matrix<T>(*this);

				if (!Z->is_null()) {

					std::vector<T> d_(Z->n);
					std::vector<T> e(Z->n);

					Z->tred2(&d_, &e);
					eigensystem_status = Z->tqli(&d_, &e);

					if (eigensystem_status) {

						*d = std::vector<std::complex<T>>(d_.size());

						auto i = d_.begin();
						for (auto j = d->begin(); j!=d->end(); j++, i++)
							*j = *i;
					}
				}
			
			} else {

				// process asymmetric real matrix

				std::vector<unsigned int> perm;
				gu_hessenberg_state h_state;

				auto scale = std::vector<T>(n);

				// initialise scale elements to 1
				for (auto i = scale.begin(); i!=scale.end(); i++)
					*i = T(1);

				matrix<T> B = balance(&scale);
				matrix<T> H = B.hessenberg_form(&perm, &h_state, false);

#ifdef __GU_DEBUG_MATRIX__
				std::cout << "H = \n" << H << std::endl;
#endif

				if (!H.is_null()) {

					matrix<T> Z_ = H.eltran(perm);
					
					(*d) = H.hqr2(&Z_);

					Z_.balbak(scale);
					
					Z_.sort_vecs(d);

					*Z = Z_;

					eigensystem_status = true;
				}

			}
		}

		return eigensystem_status;
	}

}

