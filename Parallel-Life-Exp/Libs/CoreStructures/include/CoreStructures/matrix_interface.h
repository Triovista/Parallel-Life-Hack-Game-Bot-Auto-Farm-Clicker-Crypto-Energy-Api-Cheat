
#pragma once

#include "GUMemory.h"
#include "gu_math.h"
#include "GUObject.h"
#include "GUMatrix4.h"
#include <vector>
#include <functional>
#include <iostream>
#include <cstdarg>
#include <iomanip>
#include <sstream>
#include <complex>


/*

matrix models an (n x m) matrix (real:{C numeric scalar type} or complex:{complex<float>, complex<double>}) with elements stored in column-major format.  Value semantics apply.  The actual matrix model stores the matrix order (n, m) and a pointer (unique_ptr) to the actual matrix data in M.  A NULL matrix has the condition M.get()==nullptr && n = m = 0.  C++ 11 rvalue references and move semantics are applied.  THIS IS A NON-OPTIMISED IMPLEMENTATION TO TEST TEMPLATING AND LINEAR ALGEBRA CONCEPTS.  A FINAL ASYNCHRONOUS DIRECTCOMPUTE / OPENCL BACKED IMPLEMENTATION WILL BE BUILT FROM THIS PROTOTYPE

note: complex matrices {complex<float>, complex<double>} do not currently support specialisations of the following methods.  These require interface refactoring to make compatible with complex<> types.  This can be done in ver2 development

eigen_system (and supporting private methods)

... this means these functions cannot be called on matrix<complex<float> > and matrix<complex<double> >

*/


namespace CoreStructures {


	//
	// matrix / linear system states
	//

	typedef enum {gu_no_solution, gu_unique_solution, gu_inf_solution} gu_lsys_state; // linear system states
	typedef enum {gu_lu_okay, gu_lu_fail_singular, gu_lu_fail_error, gu_lu_no_solution} gu_lu_decomp_state; // LU decomposition states
	typedef enum {gu_cholesky_okay, gu_cholesky_not_pd, gu_cholesky_error} gu_cholesky_state; // Cholesky decomposition states for sqaure, real PD matrices
	typedef enum {gu_hessenberg_okay, gu_hessenberg_error} gu_hessenberg_state; // Hessenberg decomposition states
	typedef enum {gu_norm_frobenius = 1, gu_norm_max, gu_norm_op1, gu_norm_op_inf} gu_norm_type; // vector / matrix norm types


	//
	// forward declarations
	//

	template <typename T>
	struct matrix;


	// model auxiliary data for a given linear system AX = B where X and B are assumed to represent column vectors
	template<typename T>
	struct gu_lsystem_aux {

		unsigned int				rankA, rankM; // rank of coefficient matrix (A) and linear system M = A | B (where | represents the matrix concatenation operator declared below)
		unsigned int				num_unknowns; // number of unknowns in A
		matrix<T>					E, C; // echelon and row canonical form of the linear system M = A | B
		std::vector<unsigned int>	P;	// pivot indices for the linear system M = A | B.  This actually represents the pivot indices for A where M = A | B represents a consistent system of equations (at least 1 solution exists).  If M represents an inconsistent system then gu_no_solution applies
		std::vector<unsigned int>	F; // free variable indices for coefficient matrix A (F.size represents the number of free variables)
	};


	template <typename T>
	std::ostream& operator<<(std::ostream& os, const matrix<T>& A);

	template <typename T>
	matrix<T> gaussianElimination(const matrix<T>& A, const matrix<T>& B, gu_lsys_state *solutionStatus, gu_lsystem_aux<T> *L = nullptr);

	template <typename T>
	matrix<T> lup_solve(const matrix<T>& D, const std::vector<unsigned int>& P, const matrix<T>& B);


	//
	// matrix<> interface declaration
	//
	template <typename T>
	struct matrix {

	typedef std::unique_ptr<T, decltype(&::free)> matrix_ptr;

	public:

		static const double		precision; // fp precision for matrix operations eg. gaussian elimination operation (log base 10 number - see gu_math.h)

	private:

		unsigned int			n, m;
		matrix_ptr				M;


		//
		// private methods
		//
		
		inline bool valid_row_index(unsigned int i) const;

		inline bool valid_column_index(unsigned int j) const;

		inline T& element(unsigned int i, unsigned int j); // dereference matrix element a(ij).  matrix indices are not zero-indexed to conform to mathematical convention
		inline T element(unsigned int i, unsigned int j) const; // dereference matrix element a(ij).  matrix indices are not zero-indexed to conform to mathematical convention

		inline T& a(unsigned int i, unsigned int j); // dereference matrix element a(ij).  matrix indices are not zero-indexed to conform to mathematical convention
		inline T a(unsigned int i, unsigned int j) const; // dereference matrix element a(ij).  matrix indices are not zero-indexed to conform to mathematical convention

		inline T& a_(unsigned int i, unsigned int j); // zero-indexed version of a()

		inline T a_(unsigned int i, unsigned int j) const; // zero-indexed version of a() const;
	
		void extract_va_arg(va_list *arg_list, T *value);

		void defmatrix(T& e11, va_list *matrix_values, T *buffer); // populate matrix data buffer of order (n x m) with the values in the variadic parameter list {e11, matrix_values}.  If (n x m) values are not specified in {e11, matrix_values} then the result is undefined.  defmatrix is called from constructor methods that take variadic parameter lists where n, m and the matrix buffer (but not necessarily M) are already initialised.  The data in {e11, matrix_values} is specified in row-major format to make it easier to read matrix content in the actual parameter declaration

		void make_null(); // set the matrix to a NULL matrix

	public:

		//
		// static interface
		//

		static matrix<T> nullmatrix(); // create and return a null matrix

		static matrix<T> zeromatrix(unsigned int n, unsigned int m); // create and return a zero matrix of order (n x m)

		static matrix<T> I(unsigned int n); // create and return identity matrix of order (n x n)

		static matrix<T> identity(unsigned int n); // create and return identity matrix of order (n x n)

		static matrix<T> columnVector(unsigned int n, ...); // create and return an (n x 1) column vector as a matrix.  If (n x 1) values are not specified in the parameter list then the behaviour is undefined.  A NULL matrix is returned if the required matrix cannot be created

		static matrix<T> rowVector(unsigned int m, ...); // create and return a (1 x m) row vector as a matrix.  If (1 x m) values are not specified in the parameter list then the behaviour is undefined.  A NULL matrix is returned if the required matrix cannot be created

		static matrix<T> diag(unsigned int n, ...); // create and return an (n x n) square matrix with the diagonal elements populated with the values in the parameter list.  If n values are not specified in the parameter list then the result is undefined

		static matrix<T> permutation_matrix(const std::vector<unsigned int>& v); // create (row) permutation matrix from permutation vector v.  If the matrix cannot be created or v is empty a NULL matrix is returned.  It is assumed the indices in v lie in the interval [1, v.size()]


		// constructors

		matrix(); // null matrix

		matrix(unsigned int n_); // identity matrix of order (n_ x n_).  A NULL matrix is returned if the required matrix cannot be defined or n_ = 0

		matrix(unsigned int n_, unsigned int m_); // define zero matrix of order (n_ x m_).  A NULL matrix is returned if the required matrix cannot be defined, n_ = 0 or m_ = 0

		matrix(unsigned int n_, unsigned int m_, T e11, ...); // define matrix of order (n_ x m_).  A NULL matrix is returned if the required matrix cannot be defined, n_ = 0 or m_ = 0.  If (n_ x m_) values are not specified in the parameter list {e11, ...} then the result is undefined.  The data is specified in row-major format to make it easier to read matrix content in the actual parameter declaration
		
		matrix(unsigned int n_, unsigned int m_, va_list *matrix_values); // define matrix of order (n_ x m_).  A NULL matrix is returned if the required matrix cannot be defined, n_ = 0 or m_ = 0.  If (n_ x m_) values are not specified in the parameter list matrix_values then the result is undefined.  The data in matrix_values is specified in row-major format.  The calling function has responsibility of managing matrix_values with appropriate calls to va_start and va_end

		matrix(unsigned int n_, unsigned int m_, std::nullptr_t nil_); // define zero matrix of order (n_ x m_).  A NULL matrix is returned if the required matrix cannot be defined, n_ = 0 or m_ = 0
		
		matrix(unsigned int n_, unsigned int m_, const T* data); // define matrix of order (n_ x m_).  A NULL matrix is returned if the required matrix cannot be defined, n_ = 0 or m_ = 0.  If data==nullptr then a zero matrix is returned, otherwise the values in data are copied into the matrix.  data is specified in column-major format. No ownership or dependency on data exists once the constructor exits.  If less than n_ * m_ values are specified in data then the result is undefined.  The caller should pass an explicit T* type to avoid ambiguity with the ... constructor overload

		matrix(unsigned int n_, unsigned int m_, matrix_ptr&& data); // define matrix of order (n_ x m_).  A NULL matrix is returned if the rvalue references a nullptr, n_ = 0 or m_ = 0.  If (n_ x m_) values are not referenced in data then the result is undefined

		matrix(unsigned int n_, unsigned int m_, std::function<T (int, int)> fn); // define a matrix of order (n_ x m_) and call fn(i, j) for each element a(ij).  A NULL matrix is returned if the required matrix cannot be defined, n_ = 0 or m_ = 0.  Elements are calculated for each column in turn, proceeding from the left column (j=1).  Within each column, each element / row a(ij) is processed from the top to the bottom, proceeding from i=1

		matrix(const GUMatrix4& A); // create matrix from linear operator represented in mat4 A.  A NULL matrix is returned if the matrix cannot be created

		matrix(const matrix& A); // copy constructor.  A NULL matrix is returned if the required matrix cannot be copied or A is a NULL matrix

		matrix(matrix&& A); // move constructor
		

		// accessor methods

		bool is_real() const; // return true if the matrix is real (float, double), otherwise return false (complex specialisation overrides to return true)

		bool is_complex() const; // return true if the matrix is complex (complex<float>, complex<double>), otherwise return false

		bool is_null() const;  // return true if the matrix is NULL

		bool is_square() const; // return true if n==m and M!=nullptr, otherwise return false

		bool is_hermitian() const; // return true if the given (complex) matrix A is Hermitian where A^H = A, otherwise return false.  If A is a real matrix, this reduces to is_symmetric()

		bool is_skew_hermitian() const; // return true if the given (complex) matrix A is skew-Hermitian where A^H = -A, otherwise return false.  If A is a real matrix this reduces to is_skew_symmetric().  For a matrix to be skew-Hermitian, the leading diagonal elements must be pure imaginary values (of the form 0 + ix).  Skew-Hermitian matrices are symmetrical on the imaginary components and skew-symmetric on the real-components (check this!)

		bool is_symmetric() const; // return true if the given matrix A is symmetric where A^T = A, otherwise return false

		bool is_skew_symmetric() const; // return true if the given (real) matrix A is skew-symmetric where A^T = -A, otherwise return false

		bool is_unitary() const; // return true if the given (complex) matrix A is a unitary matrix where A^H A = A A^H = I and therefore A^H = A^-1, otherwise return false.  If A is a real matrix this reduces to is_orthogonal()

		bool is_orthogonal() const; // return true if the given (real) matrix A is orthogonal where A^T = A^-1, otherwise return false

		bool is_normal() const; // return true of the given matrix A is normal where A^H A = A A^H for complex matrices and A^T A = A A^T for real matrices, otherwise return false

		bool is_posdef() const; // return true if the given real matrix A is positive definite where x^T A x > 0 (for x != 0 - see Kreyszig  p.983), otherwise return false.  Sylvester's criterion is applied, where the determinant of each submatrix (1x1, 2x2, ... , nxn) located at a(1, 1) > 0 (ie. all the principle minors are positive), to determine if A is positive definite (CHECK WE DO NOT SUM PRINCIPLE MINORS FOR EACH SUBMATRIX ORDER).  If A is a NULL matrix or not a square matrix then false is returned.  This function is not as efficient as cholesky_crout() where cholesky_crout relies on real, square, symmetric positive definite matrices 

		bool is_singular() const; // return true if the given matrix (A) is singular (A^-1 does NOT exist), otherwise return false

		bool is_zero() const; // return true if the given matrix(A) is a zero matrix, as determined by tequal<T>(aij, T(0), matrix<T>::precision), false otherwise

		unsigned int rank() const; // return the rank of the given matrix (A) where rank(A) = dim(Im F) and F is the linear transform corresponding to A.  Return 0 if A is a NULL matrix or the rank evaluation cannot be completed successfully

		unsigned int nullity() const; // return the nullity of the given matrix (A) where nullity(A) = dim(Ker F) and F is the linear transform corresponding to A (see Lipschutz & Lipson theorems 3.12 and 5.6).  Return 0 if A is a NULL matrix or the nullity evaluation cannot be completed successfully

		T trace() const; // return the trace of the given (square) matrix (A).  Return T(0) if A is a NULL matrix or A is not square.  This is equal to the sum of principle minors of order (1)

		unsigned int rows() const; // return the number of rows

		unsigned int columns() const; // return the number of columns

		T& operator()(unsigned int i, unsigned int j); // return a (mutable) reference to matrix element aij.  The matrix is assumed to be valid and ij are valid element indices
		
		T operator()(unsigned int i, unsigned int j) const; // return matrix element aij.  The matrix is assumed to be valid and ij are valid element indices



		matrix<T> row(unsigned int i) const; // return row i of the given matrix as a (1 x m) row vector.  A NULL matrix is returned if the given matrix is null, i is not a valid row index or the row vector cannot be created

		void setRow(unsigned int i, const T *data); // set the values in row i from the given array.  If the matrix is NULL, i is not a valid row index or data==nullptr then the function does nothing.  If m values are not specified in the array pointed to by data then the result is undefined

		void setRow(unsigned int i, const matrix<T>& B); // set row i in the given matrix to the row vector B.  No change is made if i is not a valid row index, the given matrix is a NULL matrix or B is a NULL matrix, B is not a row vector or B.m != m

		matrix<T> column(unsigned int j) const; // return column j of the given matrix as an (n x 1) column vector.  A NULL matrix is returned if the given matrix is null, j is not a valid column index or the column vector cannot be created

		void setColumn(unsigned int j, const T *data); // set the values in column j from the given array.  If the matrix is NULL, j is not a valid column index or data==nullptr then the function does nothing.  If n values are not specified in the array pointed to by data then the result is undefined

		void setColumn(unsigned int j, const matrix<T>& B); // set column j in the given matrix to the column vector B.  No change is made if j is not a valid column index, the given matrix is a NULL matrix or B is a NULL matrix, B is not a column vector or B.n != n

		void set_submatrix(unsigned int i, unsigned int j, const matrix<T>& B); // insert matrix B into the given matrix (A).  If A or B are NULL matrices then the function does nothing.  It is assumed (i, j) are valid indices and B's extent lies within A, otherwise the result is undefined

		matrix<T> submatrix(unsigned int i, unsigned int j, unsigned int num_rows, unsigned int num_cols) const; // return the submatrix starting at element a(ij) and having the order (num_rows x num_cols).  A NULL matrix is returned if the required submatrix cannot be created.  It is assumed (i, j) are valid indices and (num_rows, num_cols) are valid extents.  If the submatrix extent does not fit the host matrix then the result is undefined.  This function is a more generalised form of matrixRow() and matrixColumn() and a complementary function to removeRowAndColumnFromMatrix(), where subMatrix() takes a contiguous block of elements, removeRowAndColumnFromMatrix() can allow non-contiguous rows and columns to form the resulting submatrix

		matrix<T> remove_row_col(unsigned int i, unsigned int j) const; // return the submatrix of the given matrix (A) with row i and column j removed.  If i or j are outside their respective ranges [1, n] and [1, m], then the respective row or column is not deleted.  For example, setting i=0 and j=[1, m] creates a submatrix with only column j removed and setting j=0 and i=[1, n] creates a submatrix with only row i removed.  If A is a NULL matrix, no columns or rows exist in the resulting submatrix (A is a column or row vector) or the required submatrix cannot be created then a NULL matrix is returned


		// unary operators

		matrix<T> operator-() const; // additive inverse (negate).  A NULL matrix is returned if the given matrix is NULL or the negated matrix cannot be created

		matrix<T> transpose() const; // return the transpose of the given matrix or a NULL matrix if the given matrix is NULL or the transpose matrix cannot be created

		matrix<T> operator^(const int i) const;  // return the given -square- matrix (A) raised to the integer power i.  If i<-1 or A is not a square matrix then a NULL matrix is returned.  If i=-1 then the inverse of A is returned if A is non-singular, otherwise a NULL matrix is returned.  This calls inv() and is defined for syntactic convinience only, allowing statements such as Ainv = A^-1 as well as Ainv = A.inv().  If i=0 then an (n x n) identity matrix is returned, if i=1 then A is returned and if i>1 then A^i is returned.  Care must be taken when considering operator precedence: ^ is by default the bitwise OR operator and has a lower precedence than *, so expressions like A * A^-1 = (A * A)^-1.  Parenthesis should be used around ^ to force precedence ie. A * (A^-1)
		
		matrix<T> inv() const;  // return the inverse matrix if non-singular, otherwise return a NULL matrix

		T det() const; // return the determinant of the given matrix (A) using Doolittle LUP decomposition.  If the given matrix is NULL or not a square matrix then T(0) is returned

		T norm(gu_norm_type t = gu_norm_frobenius) const; // return the matrix norm (default to the Frobenius norm), otherwise return T(0) if the given matrix is a NULL matrix

		matrix<T> conj() const; // return the complex conjugate of the given (complex) matrix A.  If A is a real matrix, then A is returned

		matrix<T> hconj() const; // return the conjugate transpose (Hermitian conjugate) A^H of the given (complex) matrix A.  If A is a real matrix then the transpose A^T is returned


		// binary operators

		matrix<T> &operator=(const matrix<T>& A); // copy assign - set the given matrix to equal A.  If the matrix copy cannot be created a NULL matrix is returned

		matrix<T> &operator=(matrix<T>&& A); // move assign

		bool operator==(const matrix<T>& B); // return true if the given matrix equals B (as determined by tequal<T>, otherwise return false.  If both matrices is NULL then true is returned

		matrix<T> operator+(const matrix<T>& B) const; // return the given matrix added to B.  A NULL matrix is returned if the resulting matrix cannot be created.  Additive rules - let 0 denote a NULL matrix, x denote matrices of order (x1, x2) and y denote matrices of order (y1, y2) where (x1, x2) != (y1, y2): (x + 0 = x); (0 + x = x); (0 + 0 = 0); (x + y = 0)
		matrix<T>& operator+=(const matrix<T>& B);

		matrix<T> operator-(const matrix<T>& B) const;
		matrix<T>& operator-=(const matrix<T>& B);

		matrix<T> operator*(T k) const; // scalar multiplication
		matrix<T>& operator*=(T k);

		matrix<T> operator*(const matrix<T>& B) const; // post-multiply the given matrix with B.  If the new matrix cannot be created then a NULL matrix is returned.  Multiplication rules given matrices x, y and NULL (0): x * 0 = 0; 0 * x = 0; 0 * 0 = 0; x * y {x.m!=y.n} = 0; x * y {x.m==y.n} = x * y;
		matrix<T>& operator*=(const matrix<T>& B);

		matrix<T> operator|(const matrix<T>& B) const; // concatenate matrix B as additional columns.  Return a NULL matrix if n ≠ B.n, M == B.M == nullptr or the resulting matrix cannot be created.  If only one matrix is NULL the resulting matrix equals the non-null matrix

		matrix<T> operator||(const matrix<T>& B) const; // concatenate matrix B as additional rows.  Return a NULL matrix if m ≠ B.m, M == B.M == nullptr or the resulting matrix cannot be created.  Use | and || to build block matrices.  For example, to build a block matrix by row R = (A | B | C) || (D | E | F) while by column R = (A || D) | (B || E) | (C || F)


		//
		// linear systems modelling (real-valued matrices)
		//

		// Elementary row operations - perform in-place transformations on the given matrix

		void rowInterchange(unsigned int i, unsigned int j); // E1. Ri <-> Rj (preconditions: matrix is not NULL; i!=j; (1 <= i <= n); (1 <= j <= n))

		void rowScale(unsigned int i, const T& k); // E2. kRi -> Ri (preconditions: matrix is not NULL; (1 <= i <= n); k≠0)
		
		void rowAddition(unsigned int i, const T& k, unsigned int j); // E3. kRi + Rj -> Rj (preconditions: matrix is not NULL; i!=j; (1 <= i <= n); (1 <= j <= n))

		void rowScaleAdd(unsigned int i, const T& k, unsigned int j, const T& k_); // E. kRi + k_Rj -> Rj (preconditions: matrix is not NULL; i!=j; (1 <= i <= n); (1 <= j <= n); k_≠0)


		// Elementary column operations - perform in-place transformations on the given matrix

		void colInterchange(unsigned int i, unsigned int j); // E1. Ci <-> Cj (preconditions: matrix is not NULL; i!=j; (1 <= i <= m); (1 <= j <= m))

		void colScale(unsigned int i, const T& k); // E2. kCi -> Ci (preconditions: matrix is not NULL; (1 <= i <= m); k≠0)
		
		void colAddition(unsigned int i, const T& k, unsigned int j); // E3. kCi + Cj -> Cj (preconditions: matrix is not NULL; i!=j; (1 <= i <= m); (1 <= j <= m))

		void colScaleAdd(unsigned int i, const T& k, unsigned int j, const T& k_); // E. kCi + k_Cj -> Cj (preconditions: matrix is not NULL; i!=j; (1 <= i <= m); (1 <= j <= m); k_≠0)


	private:

		T* createRowNormalisationCoeffVector() const; // create row normalisation coefficient vector.  matrix is assumed to be a valid

		bool convertToEchelonForm(unsigned int *pivotIndex); // in-place conversion of matrix to echelon form.  Return true if the conversion completed successfully, false otherwise

		bool convertToRowCanonicalForm(unsigned int *pivotIndex); // convert matrix into row canonical form.  It is assumed the matrix is already in echelon form and pivotIndex contains the per-row column indices of the pivot variables.  Return true if the conversion was successful, false otherwise

	public:

		matrix<T> echelonForm() const; // return the echelon form of the matrix.  A NULL matrix is returned if the echelon form cannot be created

		matrix<T> row_canonical_form() const; // return the row canoniocal form of the given matrix (M).  A NULL matrix is returned if the row canonical form cannot be created 

		T cofactor(unsigned int i, unsigned int j) const; // return the cofactor of element a(ij) in the given matrix (A).  A is assumed to be a valid matrix and (i, j) are assumed to be valid row and column indices.  T(0) is returned if the cofactor could not be calculated

		matrix<T> cofactors() const; // return the cofactor matrix for the given matrix (A).  A NULL matrix is returned if A is a NULL matrix or the cofactor matrix cannot be created

		matrix<T> adj() const; // return the -classical- adjoint (adjugate) of the given matrix (A), adj(A) = C^T where C is the matrix of cofactors - see Lipschutz & Lipson sec 8.9.  A NULL matrix returned if A is a NULL matrix or the classical adjoint cannot be created

		matrix<T> balance(std::vector<T> *scale=nullptr) const; // return the balanced matrix (B) of the given square matrix (A).  Return a NULL matrix if A is a NULL matrix, A is not a square matrix or the balanced matrix cannot be created.  The resulting matrix is similar to A but balance() does not record the similarity transforms applied to A.  Since B is similar to A det(B) = det(A) and both A and B have the same eigenvalues.  Derived from Press et al. Numerical Recipes 3rd ed.

		matrix<T> solveCanonicalFreeVarSystem(const std::vector<unsigned int>& F, const std::vector<T> &V, const std::vector<unsigned int>& P, unsigned int rankA, unsigned int dimX); // for the given matrix (C) already in row canonical form, create a solution vector (X) for the given free variables in F with corresponding values in V by solving the pivot variables with respect to the given free variables.  P stores the pivot (column) indices for the given row canonical matrix and rankA represents the number of pivot variables (rank of the coefficient submatrix (A) contained in C).  It is assumed C represents a consistent linear system.  dimX represents the number of elements in the solution vector X

		friend matrix<T> gaussianElimination<>(const matrix<T>& A, const matrix<T>& B, gu_lsys_state *solutionStatus, gu_lsystem_aux<T> *L); // solve the system of linear equations [A, B] using Gaussian Elimination where A is the coefficient matrix and B is the constant matrix.  The function returns the solution vector while the status of the system is returned in *solution.  solution cannot be NULL.  If L is a not equal to nullptr then auxiliary information about the matrix is returned in L (see gu_lsystem_aux<> structure above).  If A or B are NULL matrices, A.n ≠ B.n or the Gaussian Elimination cannot be created then a NULL matrix is returned, gu_no_solution is returned in *solution and L is returned in an indeterminate state.  If gu_inf_solution is returned then a matrix is returned where each column represents the solution for each free variable fi = 1 and fj (j!=i) = 0.  For a homogeneous system where B=[0] the resulting vectors represent the basis for the solution space, which is the basis of the kernel space of A.  For a non-homogeneous system where B != [0] the vectors represent just one set of solutions.  In this case, if the caller requires additional solutions L must be returned and a solution must be recalculated for each free variable the caller wants to evaluate
		

		// LU decomposition

	private:
		
		gu_lu_decomp_state lup_decomposition_doolittle(std::vector<unsigned int> *permutationVector, T *parity);  // for the given square matrix (A), derive the LUP decomposition of A so that PA = LU using Doolittle's method (see Lengyel p.426).  Return gu_lu_okay if A is invertible and the decomposition was successful, otherwise return gu_lu_fail_singular.  If gu_lu_fail_singular is returned, A (and permutationVector if given) are left in an indeterminate state since the LUP decomposition is performed in-place.  permuationVector is assumed to be a vector of order (n) into which the row exchange permutations will be stored.  permuationVector is also assumed to be initialised to P[i-1] = i : 1<=i<=n.  The row exchange parity is be stored in *parity.  permutationVector and parity are optional since they are not needed for all applications of the LUP decomposition so it is left to the calling function to determine the need for these structures and perform the relevant initialisation of permutationVector.  For example, the determinant calculation does not use the permutationVector but LU-based linear solvers do

	public:

		gu_lu_decomp_state lup_decomp(matrix<T> *D, std::vector<unsigned int> *P) const; // return the LUP decomposition of the given matrix (A) where [P]A = LU.  Return gu_lu_okay if A is a square invertible matrix and the decomposition was successful, gu_lu_fail_error if the decomposition cannot be created, otherwise return gu_lu_fail_singular.  If successful, D contains the combined LU representation (see Lengyel p.426) and P contains the permutation vector (which needs to be subsequently transformed into a permutation matrix)

		gu_lu_decomp_state lup_decomp(matrix<T> *L, matrix<T> *U, matrix<T> *P) const; // return the LUP decomposition of the given matrix (A) where PA = LU.  Return gu_lu_okay if A is a square invertible matrix and the decomposition was successful, gu_lu_fail_error if the decomposition cannot be created, otherwise return gu_lu_fail_singular

		friend matrix<T> lup_solve<>(const matrix<T>& D, const std::vector<unsigned int>& P, const matrix<T>& B); // lup_solve performs forward and backward substitution on the given LUP decomposition (D), where D represents the combined LU matrices (Lengyel p.425) and P represents the row permutation vector, to solve ([P]A)x = (LU)x = [P]B.  It is assumed the order of P = (D.n x 1).  B represents the known constant vector and is assumed to be of the order (D.n x 1).  The function returns the column vector x of the order (D.n x 1) which represents the solution to the system.  Given D represents the LU decomposition of a given matrix (A), it is assumed the solution vector x exists and is unique given the pre-conditions for LUP factorisation (see above)


		// Cholesky decomposition

		gu_cholesky_state cholesky_decomp(matrix<T> *C) const; // calculate and return the Cholesky (Cholesky-Crout) decomposition of the given real square symmetric positive-definite matrix (A).  gu_cholesky_error is returned and a NULL matrix is returned in C if A is a NULL matrix, not real, square, symmetric or the decomposition cannot be created. gu_cholesky_not_pd is returned and a NULL matrix is returned in C if A is a real square symmetric matrix but not positive-definite.  Otherwise gu_cholesky_okay is returned and the cholesky decomposition is returned in *C


		// Hessenberg form

		matrix<T> hessenberg_form(std::vector<unsigned int> *perm, gu_hessenberg_state *state, bool zeroL = true) const; // reduce the given real non-symmetric matrix (A) to Hessenberg form (H).  This is done via a series of similarity transforms performing matrix elimination.  By default the lower triangle (below the first lower sub-diagonal) is set to zero.  As such H is similar to A and has the same determinant and eigen values.  If zeroL is false then the lower sub-triangle is left unchanged and retains the multipliers used in the transforms.  Derived from Press et al. Numerical Recipes 3rd ed.


		// Eigen-system extraction (for real-valued matrices)

	private:

		matrix<T> eltran(const std::vector<unsigned int>& P) const; // accumulate the (stabalised) elementary similarity transforms used in the reduction of the given matrix (A) into Hessenberg form.  The lower-triangle, below the sub-diagonal of A, contain the multipliers used in the reduction while the permutation of the transformations are stored in P.  Derived from Press et al. Numerical Recipes 3rd ed.

		std::vector<std::complex<T>> hqr(); // perform in-place extraction of the eigen values for the given Hessenberg matrix (H).  The state of H is undefined once the function completes (with success or failiure) since the QR decomposition is performed in-place in H.  A complex<T> vector (E) is returned containing the eigen-values for H.  If the eigen values cannot be created an empty vector is returned.  Derived from Press et al. Numerical Recipes 3rd ed.

		std::vector<std::complex<T>> hqr2(matrix<T> *Z); // perform in-place extraction of the eigenvalues and eigenvectors for the given Hessenberg matrix (H).  Return the unsorted eigenvalues in a std::vector and the corresponding eigenvectors in *Z.  If the eigenvalues or eigenvectors cannot be created an empty vector is returned.  Derived from Press et al. Numerical Recipes 3rd ed.
		
		void tred2(std::vector<T>* d, std::vector<T>* e); // perform in-place deconstruction of the given real, symmetric matrix (A) into tri-diagonal form.  A is replaced by the orthogonal matrix Q that repesents the transform to tri-diagonal form.  d[0..n-1] returns the diagonal elements of the tri-diagonal matrix and e[1..n-1] returns the off-diagonal elements.  Derived from Press et al. Numerical Recipes 3rd ed.

		bool tqli(std::vector<T>* d, std::vector<T>* e); // QL decomposition to calculate the eigenvalues and eigenvectors for a given real, square, symmetric, tri-diagonal matrix.  <d, e> represent the tri-diagonal matrix, where d[0..n-1] represents the elements of the leading diagonal and e[1..n-1] (e[0] is not used) represents the off-diagonal elements.  The given matrix (Z) represents either an (n x n) identity matrix if <d, e> represent a tri-diagonal matrix that has not been pre-processed, otherwise Z represents the transform matix (Q) output by tred2 if <d, e> represent a matrix reduced to tri-diagonal form by tred2().  The n eigenvalues are output in d and the corresponding normalised eigenvectors are output in the columns of the given matrix.  e is returned in an indeterminate state.  If the function succeeds then true is returned, otherwise the function returns false.  Derived from Press et al. Numerical Recipes 3rd ed.

		void balbak(const std::vector<T>& scale);  // forms the eigenvectors of a real non-symmetric matrix by backtransforming those of the corresponding balanced matrix determined by balance().  Derived from Press et al. Numerical Recipes 3rd ed.

		void sort_vecs(std::vector<std::complex<T>> *wri); // in-place sort of eigenvectors stored as columns in the given matrix (Z) and corresponding eigenvalues in *wri.  Derived from Press et al. Numerical Recipes 3rd ed.

	public:

		bool eigen_system(std::vector<std::complex<T>>* d, matrix<T>* Z) const; // extract the eigenvalues and eigenvectors for the given square, real matrix A.  The eigenvalues are returned in *d and the corresponding eigenvectors are returned in the column vectors of *Z.  If the eigensystem cannot be created, false is returned, otherwise the function returns true



		// Utility methods

		// permutation functions		
		friend std::vector<unsigned int> identity_permutation_vec(unsigned int n); // create an identity permutation vector P of order (n) for the given matrix where P[i-1] = i : 1<=i<=n

		// stream IO functions
		friend std::ostream& operator<< <>(std::ostream& os, const matrix<T>& A);
	};


	//
	// precision constant for matrix<> types
	//

	template <typename T>
	const double matrix<T>::precision = 1e-5;


	//
	// utility functions
	//

	std::ostream& operator<<(std::ostream& os, const gu_lsys_state &state);
	std::ostream& operator<<(std::ostream& os, const gu_lu_decomp_state &l);
	std::ostream& operator<<(std::ostream& os, const gu_cholesky_state &c);
	std::ostream& operator<<(std::ostream& os, const gu_hessenberg_state &h);

}



/*

further development...

- apply lambda to all elements of the matrix

- add overloaded method for gaussian elimination where M = A | B already defined

(minimise bridge between mat4, vecx and matrix<> types...)

- construct matrix<> from vecx and mat4 types (static factory method to create column or row matrix from vecx type)

- extract submatrix (or entire matrix) as vecx and mat4 type (more general form of above 'convert to mat4')

- Skew symmetric matrix creation (from GUVector4 - GUVector4 models a homogeneous vector in R4) -> NO -> Allow check for skew symmetric status, but creation here is quite specific (see Hecker) - do not make part of general matrix class.

- create and return elementary matrices

- return characteristic polynomial (vector of coefficients) ... should allow us to verify a given matrix is a root of the characteristic polynomial!!!

- lup factorisation of unique solutions in an over-determined system A (n>m) but rank(A) = m ??? STRIP LINEARLY DEPENDENT VECTORS AND DERIVE SYSTEM SOLUTION FROM THIS!!!

- complex matrix LU and linear system modelling (where relevant)

- block indexing (currently not available) - so build matrix from blocks - store block index as proxy to actual data.  Allow rvalue move semantics to apply on block matrix construction? (NOT IN CURRENT PROTOTYPE!!!)


further testing...

- further test cholesky decomposition
- further test eigensystem

*/



