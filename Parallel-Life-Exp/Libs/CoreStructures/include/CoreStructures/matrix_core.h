#pragma once

#include "matrix_interface.h"

// #define __GU_DEBUG_MATRIX__ 1

namespace CoreStructures {

	//
	// private methods
	//

	template <typename T>
	inline bool matrix<T>::valid_row_index(unsigned int i) const {
		return 1<=i && i<=n;
	}

	template <typename T>
	inline bool matrix<T>::valid_column_index(unsigned int j) const {
		return 1<=j && j<=m;
	}

	template <typename T>
	inline T& matrix<T>::element(unsigned int i, unsigned int j) {

		// cout << "non-const access 2\n";
		return *(M.get() + (i-1) + ((j-1) * n));
	}

	template <typename T>
	inline T matrix<T>::element(unsigned int i, unsigned int j) const {

		// cout << "CONST access 2\n";
		return *(M.get() + (i-1) + ((j-1) * n));
	}

	template <typename T>
	inline T& matrix<T>::a(unsigned int i, unsigned int j) {

		// cout << "non-const access 2\n";
		return *(M.get() + (i-1) + ((j-1) * n));
	}

	template <typename T>
	inline T matrix<T>::a(unsigned int i, unsigned int j) const {

		// cout << "CONST access 2\n";
		return *(M.get() + (i-1) + ((j-1) * n));
	}

	// zero-indexed version of a()
	template <typename T>
	inline T& matrix<T>::a_(unsigned int i, unsigned int j) {

		return *(M.get() + i + (j * n));
	}

	// zero-indexed version of a() const;
	template <typename T>
	inline T matrix<T>::a_(unsigned int i, unsigned int j) const {

		return *(M.get() + i + (j * n));
	}


	template <typename T>
	void matrix<T>::extract_va_arg(va_list *arg_list, T *value) {

		*value = T(va_arg(*arg_list, T));
	}
	

	template <typename T>
	void matrix<T>::defmatrix(T& e11, va_list *matrix_values, T *buffer) {

		// build first row
		*buffer = e11;

		auto fptr = buffer + n;
		for (unsigned int j=1;j<m;j++, fptr+=n)
			extract_va_arg(matrix_values, fptr);

		// build remaining rows
		for (unsigned int i=1;i<n;i++) {
				
			fptr = buffer + i;

			for (unsigned int j=0;j<m;j++, fptr+=n)
				extract_va_arg(matrix_values, fptr);
		}
	}


	template <typename T>
	void matrix<T>::make_null() {

		n = m = 0;
		M.reset(nullptr);
	}
	
	


	//
	// static interface
	//

	template <typename T>
	matrix<T> matrix<T>::nullmatrix() {

		return matrix<T>();
	}

	template <typename T>
	matrix<T> matrix<T>::zeromatrix(unsigned int n, unsigned int m) {

		return matrix<T>(n, m);
	}

	template <typename T>
	matrix<T> matrix<T>::I(unsigned int n) {

		return identity(n);
	}
	
	template <typename T>
	matrix<T> matrix<T>::identity(unsigned int n) {

		return matrix<T>(n);
	}

	template <typename T>
	matrix<T> matrix<T>::columnVector(unsigned int n, ...) {

		va_list			matrixValues;
		
		va_start(matrixValues, n);
			
		matrix<T> A = matrix<T>(n, 1, &matrixValues);

		va_end(matrixValues);

		return A;
	}

	template <typename T>
	matrix<T> matrix<T>::rowVector(unsigned int m, ...) {

		va_list			matrixValues;
			
		va_start(matrixValues, m);
			
		matrix<T> A = matrix<T>(1, m, &matrixValues);

		va_end(matrixValues);

		return A;
	}

	template <typename T>
	matrix<T> matrix<T>::diag(unsigned int n, ...) {

		matrix<T> A = matrix<T>(n, n);

		if (!A.is_null()) {

			va_list			matrix_values;
		
			va_start(matrix_values, n);
			
			auto Aptr = A.M.get();

			for (unsigned int i=1; i<=n; i++, Aptr+=(n+1))
				A.extract_va_arg(&matrix_values, Aptr);

			va_end(matrix_values);
		}

		return A;
	}

	template <typename T>
	matrix<T> matrix<T>::permutation_matrix(const std::vector<unsigned int>& v) {

		if (v.size()==0)
			return matrix<T>::nullmatrix();
		
		auto buffer = (T*)calloc(v.size() * v.size(), sizeof(T));

		if (!buffer)
			return matrix<T>::nullmatrix();

		for (unsigned int i=1;i<=v.size();i++)
			buffer[ (v[i-1]-1)*v.size() + i - 1 ] = T(1);
	
		return matrix<T>(v.size(), v.size(), matrix_ptr(buffer, ::free));
	}


	//
	// constructors
	//

	template <typename T>
	matrix<T>::matrix() : n(0), m(0), M(matrix_ptr(nullptr, ::free)) {}

	
	template <typename T>
	matrix<T>::matrix(unsigned int n_) : n(0), m(0), M(matrix_ptr(nullptr, ::free)) {

		if (n_ > 0) {

			auto buffer = (T*)calloc(n_ * n_, sizeof(T));

			if (buffer) {

				n = n_;
				m = n_;

				auto ptr = buffer;
				for (unsigned int i=0;i<n;i++, ptr+=(n+1))
					*ptr=T(1.0);

				M.reset(buffer);
			}
		}
	}
	

	template <typename T>
	matrix<T>::matrix(unsigned int n_, unsigned int m_) : n(0), m(0), M(matrix_ptr(nullptr, ::free)) {

		if (n_ > 0 && m_ > 0) {

			auto buffer = (T*)calloc(n_ * m_, sizeof(T));

			if (buffer) {

				n = n_;
				m = m_;
				M.reset(buffer);
			}
		}
	}


	template <typename T>
	matrix<T>::matrix(unsigned int n_, unsigned int m_, T e11, ...) : n(0), m(0), M(matrix_ptr(nullptr, ::free)) {
	
		if (n_ > 0 && m_ > 0) {

			auto buffer = (T*)malloc(n_ * m_ * sizeof(T));

			if (buffer) {

				n = n_;
				m = m_;

				va_list			matrix_values;
			
				va_start(matrix_values, e11);
				defmatrix(e11, &matrix_values, buffer);
				va_end(matrix_values);

				M.reset(buffer);
			}
		}
	}


	template <typename T>
	matrix<T>::matrix(unsigned int n_, unsigned int m_, va_list *matrix_values) : n(0), m(0), M(matrix_ptr(nullptr, ::free)) {

		if (n_ > 0 && m_ > 0) {

			auto buffer = (T*)malloc(n_ * m_ * sizeof(T));

			if (buffer) {

				n = n_;
				m = m_;

				T e11;
				extract_va_arg(matrix_values, &e11);

				defmatrix(e11, matrix_values, buffer);

				M.reset(buffer);
			}
		}
	}


	template <typename T>
	matrix<T>::matrix(unsigned int n_, unsigned int m_, std::nullptr_t nil_) : n(0), m(0), M(matrix_ptr(nullptr, ::free)) {

		if (n_ > 0 && m_ > 0) {
			
			auto buffer = (T*)calloc(n_ * m_, sizeof(T));

			if (buffer) {

				n = n_;
				m = m_;

				M.reset(buffer);
			}
		}
	}

	template <typename T>
	matrix<T>::matrix(unsigned int n_, unsigned int m_, const T* data) : n(0), m(0), M(matrix_ptr(nullptr, ::free)) {

		if (n_ > 0 && m_ > 0) {

			T* buffer = nullptr;

			if (data) {
				
				buffer = (T*)malloc(n_ * m_ * sizeof(T));

				if (buffer)
					memcpy_s(buffer, n_ * m_ * sizeof(T), data, n_ * m_ * sizeof(T));
			
			} else {

				buffer = (T*)calloc(n_ * m_, sizeof(T));
			}

			if (buffer) {

				n = n_;
				m = m_;
				M.reset(buffer);
			}
		}
	}


	template <typename T>
	matrix<T>::matrix(unsigned int n_, unsigned int m_, matrix_ptr&& data) : n(0), m(0), M(nullptr, ::free) {

		if (n_ > 0 && m_ > 0 && data.get()!=nullptr) {

			n = n_;
			m = m_;
			M = std::move(data);
		}
	}


	template <typename T>
	matrix<T>::matrix(unsigned int n_, unsigned int m_, std::function<T (int, int)> fn) : n(0), m(0), M(matrix_ptr(nullptr, ::free)) {

		if (n_ > 0 && m_ > 0) {

			auto buffer = (T*)malloc(n_ * m_ * sizeof(T));

			if (buffer) {
				
				n = n_;
				m = m_;

				auto ptr = buffer;

				for (unsigned int j=0;j<m;j++) {
				
					for (unsigned int i=0;i<n;i++, ptr++)
						*ptr = fn(i, j);
				}

				M.reset(buffer);
			}
		}
	}


	template <typename T>
	matrix<T>::matrix(const GUMatrix4& A) : n(0), m(0), M(matrix_ptr(nullptr, ::free)) {

		auto buffer = (T*)malloc(4 * 4 * sizeof(T));

		if (buffer) {

			n = m = 4;

			for (unsigned int i=0; i<16; i++)
				buffer[i] = T(A.M[i]);

			M.reset(buffer);
		}
	}


	// copy constructor
	template <typename T>
	matrix<T>::matrix(const matrix<T>& A) : n(0), m(0), M(matrix_ptr(nullptr, ::free)) {

		if (!A.is_null()) {

			auto buffer = (T*)malloc(A.n * A.m * sizeof(T));

			if (buffer) {

				n = A.n;
				m = A.m;

				memcpy_s(buffer, n * m * sizeof(T), A.M.get(), n * m * sizeof(T));

				M.reset(buffer);
			}
		}
	}


	// move constructor
	template <typename T>
	matrix<T>::matrix(matrix&& A) : n(0), m(0), M(nullptr, ::free) {

		if (!A.is_null()) {

			n = A.n;
			m = A.m;
			M = std::move(A.M);
		}
	}


	// accessor methods

	template <typename T>
	bool matrix<T>::is_real() const {

		// base template implementation assumes real-valued matrices - complex valued matrix implementation in template specialisation of is_real
		return true;
	}

	template <typename T>
	bool matrix<T>::is_complex() const {

		return !is_real();
	}

	template <typename T>
	bool matrix<T>::is_null() const {

		return M.get()==nullptr;
	}

	template <typename T>
	bool matrix<T>::is_square() const {

		return !is_null() && n==m;
	}

	template <typename T>
	bool matrix<T>::is_hermitian() const {

		// base template implementation assumes real-valued matrices - complex valued matrix implementation in template specialisation of is_hermitian
		return is_symmetric();
	}

	template <typename T>
	bool matrix<T>::is_skew_hermitian() const {

		// base template implementation assumes real-valued matrices - complex valued matrix implementation in template specialisation of is_skew_hermitian
		return false; // ** check skew-hermitian status of real-valued matrices (same as skew-symmetric??)
	}

	template <typename T>
	bool matrix<T>::is_symmetric() const {

		if (is_null() || /*!is_real() ||*/ !is_square())
			return false;

		if (n==1)
			return true; // matrix of order (1 x 1)

		bool is_symmetric = true;
		auto lptr = M.get() + 1;

		for (unsigned int j=1; j<n && is_symmetric; j++, lptr+=j) {

			auto uptr = lptr + n - 1;

			for (unsigned int i=j+1; i<=n && is_symmetric; i++, lptr++, uptr+=n)
				is_symmetric = tequal<T>(*lptr, *uptr, precision);
		}

		return is_symmetric;
	}

	template <typename T>
	bool matrix<T>::is_skew_symmetric() const {

		if (is_null() || !is_real() || !is_square())
			return false;
		else
			return transpose() == -(*this);
	}

	template <typename T>
	bool matrix<T>::is_unitary() const {

		// base template implementation assumes real-valued matrices - complex valued matrix implementation in template specialisation of is_unitary
		return is_orthogonal();
	}

	template <typename T>
	bool matrix<T>::is_orthogonal() const {

		if (is_null() || !is_real() || !is_square())
			return false;
		else
			return ((*this) * transpose()) == matrix<T>::I(n);
	}

	template <typename T>
	bool matrix<T>::is_normal() const {

		// base template implementation assumes real-valued matrices - complex valued matrix implementation in template specialisation of is_normal
		if (is_null() || !is_square())
			return false;
		else
			return (transpose() * (*this)) == ((*this) * transpose());
	}

	template <typename T>
	bool matrix<T>::is_posdef() const {

		if (is_null() || !is_square())
			return false;

		bool pd = true;
	
		for (unsigned int i=1; i<=n && pd; i++) {
		
			matrix<T> S = submatrix(1, 1, i, i);

			T det = S.det();

			//if (S.det() <= T(0))
			if (tless<T>(det, T(0), precision) || tequal<T>(det, T(0), precision))
				pd = false;
		}
	
		return pd;
	}

	template <typename T>
	bool matrix<T>::is_singular() const {

		return rank()<m;
	}

	template <typename T>
	bool matrix<T>::is_zero() const {

		if (is_null())
			return false;

		bool isZero = true;
		auto Aptr = M.get();

		for (unsigned int i=0; i<(n*m) && isZero; i++, Aptr++)
			isZero = tequal<T>(*Aptr, T(0), precision);

		return isZero;
	}


	template <typename T>
	unsigned int matrix<T>::rank() const {
		
		matrix<T> A = matrix<T>(*this);

		if (A.is_null())
			return 0;

		// create pivot index vector and initialise to 0
		auto P = std::unique_ptr<unsigned int, decltype(&::free)>((unsigned int*)calloc(n, sizeof(unsigned int)), ::free);

		auto pivotIndex = P.get();

		if (pivotIndex==nullptr)
			return 0;
		
		A.convertToEchelonForm(pivotIndex);

		unsigned int		rankA = 0;
	
		for (unsigned int i=0; i<n; i++) { // process each row - use pivotIndex to find rank
	
			if (pivotIndex[i]>0)
				rankA++;
		}

		return rankA;
	}


	template <typename T>
	unsigned int matrix<T>::nullity() const {

		if (is_null())
			return 0;
		else
			return m - rank();
	}


	template <typename T>
	T matrix<T>::trace() const {

		if (is_null() || !is_square())
			return T(0);

		T trace = T(0);

		for (unsigned int i=1; i<=n; i++)
			trace += element(i, i);

		return trace;
	}


	template <typename T>
	unsigned int matrix<T>::rows() const {

		return n;
	}

	template <typename T>
	unsigned int matrix<T>::columns() const {

		return m;
	}

	template <typename T>
	T& matrix<T>::operator()(unsigned int i, unsigned int j) {

		// cout << "non-const access 1\n";

		return element(i, j);
		// return *(M + (i-1) + ((j-1) * n));
	}
	
	template <typename T>
	T matrix<T>::operator()(unsigned int i, unsigned int j) const {

		// cout << "CONST access 1\n";

		return element(i, j);
		// return *(M + (i-1) + ((j-1) * n));
	}



	template <typename T>
	matrix<T> matrix<T>::row(unsigned int i) const {

		if (is_null() || !valid_row_index(i))
			return matrix<T>::nullmatrix();

		auto buffer = (T*)malloc(m * sizeof(T));

		if (!buffer)
			return matrix<T>::nullmatrix();

		auto Aptr = M.get() + (i-1);

		for (unsigned int k=0; k<m; k++, Aptr+=n)
			buffer[k] = *Aptr;

		return matrix<T>(1, m, matrix_ptr(buffer, ::free));
	}


	template <typename T>
	void matrix<T>::setRow(unsigned int i, const T *data) {
	
		if (!is_null() && valid_row_index(i) && data) {
		
			auto Aptr = M.get() + (i-1);
		
			for (unsigned int k=0;k<m;k++, Aptr+=n)
				*Aptr = data[k];
		}
	}


	template <typename T>
	void matrix<T>::setRow(unsigned int i, const matrix<T>& B) {

		if (!is_null() && valid_row_index(i) && !B.is_null() && B.n==1 && B.m==m) {
		
			auto Aptr = M.get() + (i-1);
			auto Bptr = B.M.get();

			for (unsigned int k=0;k<m;k++, Aptr+=n, Bptr++)
				*Aptr = *Bptr;
		}
	}


	template <typename T>
	matrix<T> matrix<T>::column(unsigned int j) const {

		if (is_null() || !valid_column_index(j))
			return matrix<T>::nullmatrix();

		auto buffer = (T*)malloc(n * sizeof(T));

		if (!buffer)
			return matrix<T>::nullmatrix();

		memcpy_s(buffer, n * sizeof(T), M.get() + (j-1) * n, n * sizeof(T));

		return matrix<T>(n, 1, matrix_ptr(buffer, ::free));
	}


	template <typename T>
	void matrix<T>::setColumn(unsigned int j, const T *data) {

		if (!is_null() && valid_column_index(j) && data) {
		
			auto Aptr = M.get() + (j-1) * n;
		
			memcpy_s(Aptr, n * sizeof(T), data, n * sizeof(T));
		}
	}


	template <typename T>
	void matrix<T>::setColumn(unsigned int j, const matrix<T>& B) {

		if (!is_null() && valid_column_index(j) && !B.is_null() && B.m==1 && B.n==n) {
		
			auto Aptr = M.get() + (j-1) * n;
			auto Bptr = B.M.get();
			
			memcpy_s(Aptr, n * sizeof(T), Bptr, n * sizeof(T));
		}
	}


	template <typename T>
	void matrix<T>::set_submatrix(unsigned int i, unsigned int j, const matrix<T>& B) {

		if (!is_null() && !B.is_null()) {

			auto Aptr = M.get() + (j-1) * n + (i-1);
			auto Bptr = B.M.get();

			for (unsigned int k=0; k<B.m; k++, Aptr+=n, Bptr+=B.n)
				memcpy_s(Aptr, B.n * sizeof(T), Bptr, B.n * sizeof(T));
		}
	}


	template <typename T>
	matrix<T> matrix<T>::submatrix(unsigned int i, unsigned int j, unsigned int num_rows, unsigned int num_cols) const {

		if (is_null() || num_rows==0 || num_cols==0)
			return matrix<T>::nullmatrix();

		auto buffer = (T*)malloc(num_rows * num_cols * sizeof(T));

		if (!buffer)
			return matrix<T>::nullmatrix();

		// populate buffer with specified submatrix
		auto Aptr = M.get() + ((j-1) * n) + (i-1);
		auto Sptr = buffer;
		
		// Copy each column (since matrix stored in column-major format)
		for (unsigned int k=0;k<num_cols;k++, Aptr+=n, Sptr+=num_rows)
			memcpy_s(Sptr, num_rows * sizeof(T), Aptr, num_rows * sizeof(T));

		return matrix<T>(num_rows, num_cols, matrix_ptr(buffer, ::free));
	}

	template <typename T>
	matrix<T> matrix<T>::remove_row_col(unsigned int i, unsigned int j) const {

		if (is_null())
			return matrix<T>::nullmatrix();

		unsigned int rs1 = n;
		unsigned int rs2 = 0;

		if (valid_row_index(i)) {

			rs1 = i-1;
			rs2 = n-i;
		}

		auto n_ = rs1 + rs2;
		auto m_ = valid_column_index(j) ? (m-1) : m;

		// return a null matrix if we're removing from a column or row vector
		if (n_==0 || m_==0)
			return matrix<T>::nullmatrix();

		auto buffer = (T*)malloc(n_ * m_ * sizeof(T));

		if (!buffer)
			return matrix<T>::nullmatrix();

		// populate buffer
		auto Aptr = M.get();
		auto Bptr = buffer;
		
		for (unsigned int k=1; k<=m; k++, Aptr+=n) { // process each column k in A
			
			if (k!=j) { // ignore column j
				
				if (rs1>0)
					memcpy_s(Bptr, rs1*sizeof(T), Aptr, rs1*sizeof(T));
				if (rs2>0)
					memcpy_s(Bptr+rs1, rs2*sizeof(T), Aptr+rs1+1, rs2*sizeof(T));

				Bptr += n_;
			}
		}

		return matrix<T>(n_, m_, matrix_ptr(buffer, ::free));
	}



	//
	// unary operators
	//

	template <typename T>
	matrix<T> matrix<T>::operator-() const {

		if (is_null())
			return matrix<T>::nullmatrix();

		auto buffer = (T*)malloc(n * m * sizeof(T));

		if (!buffer)
			return matrix<T>::nullmatrix();

		auto Aptr = M.get();

		for (unsigned int k=0; k<(n*m);k++)
			buffer[k] = -Aptr[k];

		return matrix<T>(n, m, matrix_ptr(buffer, ::free));
	}


	template <typename T>
	matrix<T> matrix<T>::transpose() const {

		if (is_null())
			return matrix<T>::nullmatrix();

		auto buffer = (T*)malloc(m * n * sizeof(T));
		
		if (!buffer)
			return matrix<T>::nullmatrix();

		auto Tptr = buffer;

		for (unsigned int i=0; i<n; i++) { // process each row of the original matrix

			auto Aptr = M.get() + i; // pointer to start of row

			for (unsigned int j=0; j<m; j++, Tptr++, Aptr+=n)
				*Tptr = *Aptr;
		}

		return matrix<T>(m, n, matrix_ptr(buffer, ::free));
	}


	template <typename T>
	matrix<T> matrix<T>::operator^(const int i) const { 
	
		if (!is_square() || i<-1)
			return matrix<T>::nullmatrix();

		switch (i) {

		case -1:
			return inv();

		case 0:
			return matrix<T>::identity(n);

		case 1:
			return matrix<T>(*this);

		default: // i>=2
			{
				matrix<T> A = matrix<T>(*this);

				for (int k=1; k<i; k++)
					A *= (*this);

				return A;
			}
		}
	}


	template <typename T>
	matrix<T> matrix<T>::inv() const {

		if (is_null() || !is_square())
			return matrix<T>::nullmatrix();

		matrix<T> I = matrix<T>::I(n);
		matrix<T> M = (*this) | I;

		// create pivotIndex and initialise indices to 0
		auto P = std::unique_ptr<unsigned int, decltype(&::free)>((unsigned int*)calloc(n, sizeof(unsigned int)), ::free);

		auto pivotIndex = P.get();

		if (pivotIndex==nullptr)
			return matrix<T>::nullmatrix();

		// row reduce matrix to echelon form (specifically triangle form since A is a square matrix)
		M.convertToEchelonForm(pivotIndex);

		// Let *this = A.  If a zero row exists in the A half of M (ie. in column indicies [1, m]) then A is singular and has no inverse
		// note: this test can be replaced by determining rank(A) from the echelon form - don't call rank() here as echelon form already exists
		bool isSingular = false;
	
		for (unsigned int i=0; i<M.n && !isSingular; i++) {
		
			if (pivotIndex[i]==0 || pivotIndex[i] > m)
				isSingular = true;
		}
	
		if (isSingular)
			return matrix<T>::nullmatrix();
	
		// convert to row canonical form
		M.convertToRowCanonicalForm(pivotIndex);

		// extract the right-hand part of M which represents the inverse of A
		return M.submatrix(1, m+1, n, n);
	}


	template <typename T>
	T matrix<T>::det() const {

		if (is_null() || !is_square())
			return T(0);

		switch(n) {
		
		case 1:

			return element(1, 1);
		
		case 2:
			{
				auto m = M.get();
				return m[0]*m[3] - m[2]*m[1];
			}

		case 3:
			{
				auto m = M.get();
				return m[0]*m[4]*m[8] + m[3]*m[7]*m[2] + m[6]*m[5]*m[1] - m[2]*m[4]*m[6] - m[5]*m[7]*m[0] - m[8]*m[3]*m[1];
			}
			
		default:
			{
				T	detA;

				// copy given matrix into D for in-place LUP decomposition
				matrix<T> D = matrix<T>(*this);

				if (D.is_null())
					return T(0);

				gu_lu_decomp_state lu_state = D.lup_decomposition_doolittle(NULL, &detA);

				// cout << "debug det: D = \n" << D << endl;

				if (lu_state==gu_lu_okay) {

					T *Dptr = D.M.get();
					for (unsigned int i=1; i<=D.n; i++, Dptr+=(D.n+1))
						detA *= *Dptr;
				
				} else {

					detA = T(0);
				}

				return detA;
			}
		}
	}


	template <typename T>
	T matrix<T>::norm(gu_norm_type t) const {

		T		sum, maxValue, *ptr;

		if (is_null())
			return T(0);

		switch(t) {

		case gu_norm_frobenius: // calculate the (Euclidean norm) based on each element of the matrix

			sum = T(0);
			ptr = M.get();

			for (int i=0; i<(int)(n * m); i++, ptr++)
				sum += abs(*ptr) * abs(*ptr);

			return sqrt(sum);


		case gu_norm_max: // return the value of the largest positive matrix element

			maxValue = T(0);
			ptr = M.get();

			for (int i=0; i<(int)(n * m); i++, ptr++)
				maxValue = (abs(*ptr) > maxValue) ? abs(*ptr) : maxValue;

			return maxValue;


		case gu_norm_op1: // Induced norm (operator norm) p = 1 (maximum absolute column sum of the matrix)

			maxValue = T(0);
			ptr = M.get();

			for (int j=0; j<(int)m; j++) { // process each column

				sum = T(0);

				for (int i=0; i<(int)n; i++, ptr++)
					sum += abs(*ptr);

				maxValue = (sum > maxValue) ? sum : maxValue;
			}

			return maxValue;


		case gu_norm_op_inf: // Induced norm (operator norm) p = +inf (maximum absolute row sum of the matrix)

			maxValue = T(0);

			for (int i=0; i<(int)n; i++) { // process each row

				ptr = M.get() + i;
				sum = T(0);

				for (int j=0; j<(int)m; j++, ptr+=n)
					sum += abs(*ptr);

				maxValue = (sum > maxValue) ? sum : maxValue;
			}

			return maxValue;


		default:
			return T(0);
		}
	}


	template <typename T>
	matrix<T> matrix<T>::conj() const {

		// base template implementation assumes real-valued matrices - complex valued matrix implementation in template specialisation of conj()
		return (*this);
	}


	template <typename T>
	matrix<T> matrix<T>::hconj() const {

		// base template implementation assumes real-valued matrices - complex valued matrix implementation in template specialisation of hconj()
		return transpose();
	}



	//
	// binary operators
	//

	// copy assign
	template <typename T>
	matrix<T> &matrix<T>::operator=(const matrix<T>& A) {

		if (A.is_null()) {

			n = 0;
			m = 0;
			M.reset(nullptr); // set matrix to a NULL matrix if A is a NULL matrix
		
		} else {

			auto copyBuffer = (T*)malloc(A.n * A.m * sizeof(T));

			if (copyBuffer) {

				n = A.n;
				m = A.m;

				memcpy_s(copyBuffer, n * m * sizeof(T), A.M.get(), n * m * sizeof(T));

				M.reset(copyBuffer);

			} else { // cannot create copy buffer - return a NULL matrix

				n = 0;
				m = 0;
				M.reset(nullptr);
			}
		}

		return *this;
	}

	
	// move assign
	template <typename T>
	matrix<T> &matrix<T>::operator=(matrix<T>&& A) {

		n = A.n;
		m = A.m;

		M = std::move(A.M);

		return *this;
	}
	

	template <typename T>
	bool matrix<T>::operator==(const matrix<T>& B) {

		// check if both matrices are NULL - if so return true
		if (is_null() && B.is_null())
			return true;

		// check matrix orders match (if one, but not both matrices are NULL then the orders are not equal so this condition does not have to be checked explicitly)
		if (n!=B.n || m!=B.m)
			return false;

		// check matrix values
		bool equal = true;

		auto Aptr = M.get();
		auto Bptr = B.M.get();

		for (unsigned int k=0;k<n*m && equal;k++)
			equal = tequal<T>(Aptr[k], Bptr[k], precision);

		return equal;
	}


	template <typename T>
	matrix<T> matrix<T>::operator+(const matrix<T>& B) const {

		if (is_null())
			return matrix<T>(B);
		else if (B.is_null())
			return matrix<T>(*this);
		else if (n!=B.n || m!=B.m)
			return matrix<T>::nullmatrix();
		else {

			auto buffer = (T*)malloc(n * m * sizeof(T));

			if (!buffer)
				return matrix<T>::nullmatrix();

			auto Aptr = M.get();
			auto Bptr = B.M.get();

			for (unsigned int k=0;k<n*m;k++)
				buffer[k] = Aptr[k] + Bptr[k];

			return matrix<T>(n, m, matrix_ptr(buffer, ::free));
		}
	}


	template <typename T>
	matrix<T>& matrix<T>::operator+=(const matrix<T>& B) {

		if (!B.is_null()) {

			if (n==B.n && m==B.m) {

				// if orders match and B is not null then it follows that A is also not null
				// therefore setup new matrix and add results

				auto buffer = (T*)malloc(n * m * sizeof(T));

				if (buffer) {

					auto Aptr = M.get();
					auto Bptr = B.M.get();

					for (unsigned int k=0;k<n*m;k++)
						buffer[k] = Aptr[k] + Bptr[k];

					M.reset(buffer);

				} else {

					make_null(); // cannot allocate buffer - set matrix to a NULL matrix
				}

			} else {

				if (is_null()) { // if A is NULL the result will be a copy of B ( 0 + x = x )

					auto buffer = (T*)malloc(B.n * B.m * sizeof(T));

					if (buffer) {
						
						memcpy_s(buffer, B.n * B.m * sizeof(T), B.M.get(), B.n * B.m * sizeof(T));
						
						n = B.n;
						m = B.m;
						M.reset(buffer);
					}

				} else { // if A is not NULL we're trying to add matrices of different order so result is NULL
					
					make_null();
				}
			}
		}

		return *this;
	}


	template <typename T>
	matrix<T> matrix<T>::operator-(const matrix<T>& B) const {

		if (is_null())
			return matrix<T>(B);
		else if (B.is_null())
			return matrix<T>(*this);
		else if (n!=B.n || m!=B.m)
			return matrix<T>::nullmatrix();
		else {

			auto buffer = (T*)malloc(n * m * sizeof(T));

			if (!buffer)
				return matrix<T>::nullmatrix();

			auto Aptr = M.get();
			auto Bptr = B.M.get();

			for (unsigned int k=0;k<n*m;k++)
				buffer[k] = Aptr[k] - Bptr[k];

			return matrix<T>(n, m, matrix_ptr(buffer, ::free));
		}
	}


	template <typename T>
	matrix<T>& matrix<T>::operator-=(const matrix<T>& B) {

		if (!B.is_null()) {

			if (n==B.n && m==B.m) {

				// if orders match and B is not null then it follows that A is also not null
				// therefore setup new matrix and add results

				auto buffer = (T*)malloc(n * m * sizeof(T));

				if (buffer) {

					auto Aptr = M.get();
					auto Bptr = B.M.get();

					for (unsigned int k=0;k<n*m;k++)
						buffer[k] = Aptr[k] - Bptr[k];

					M.reset(buffer);

				} else {

					make_null(); // cannot allocate buffer - set matrix to a NULL matrix
				}

			} else {

				if (is_null()) { // if A is NULL the result will be a copy of B ( 0 + x = x )

					auto buffer = (T*)malloc(B.n * B.m * sizeof(T));

					if (buffer) {
						
						memcpy_s(buffer, B.n * B.m * sizeof(T), B.M.get(), B.n * B.m * sizeof(T));
						
						n = B.n;
						m = B.m;
						M.reset(buffer);
					}

				} else { // if A is not NULL we're trying to add matrices of different order so result is NULL
					
					make_null();
				}
			}
		}

		return *this;
	}


	template <typename T>
	matrix<T> matrix<T>::operator*(T k) const {

		if (is_null())
			return matrix<T>::nullmatrix();

		auto buffer = (T*)malloc(n * m * sizeof(T));

		if (!buffer)
			return matrix<T>::nullmatrix();

		auto Aptr = M.get();

		for (unsigned int i=0; i<(n*m); i++)
			buffer[i] = Aptr[i] * k;

		return matrix<T>(n, m, matrix_ptr(buffer, ::free));
	}


	template <typename T>
	matrix<T>& matrix<T>::operator*=(T k) {

		if (!is_null()) {

			auto Aptr = M.get();

			for (unsigned int i=0; i<(n*m); i++)
				Aptr[i] *= k;
		}

		return *this;
	}


	template <typename T>
	matrix<T>  matrix<T>::operator*(const matrix<T>& B) const {

		if (is_null() || B.is_null() || m!=B.n)
			return matrix<T>::nullmatrix();

		auto buffer = (T*)malloc(n * B.m * sizeof(T));

		if (!buffer)
			return matrix<T>::nullmatrix();

		// multiplication kernel
		for (unsigned int i=0;i<n; i++) { // ith row in new matrix

			auto Mptr = buffer + i;
			auto Bptr = B.M.get();

			for (unsigned int j=0; j<B.m; j++, Mptr+=n) { // jth column in new matrix

				auto Aptr = M.get() + i;
			
				*Mptr = T(0);

				for (unsigned int k=0; k<m; k++, Aptr+=n, Bptr++)
					*Mptr += *Aptr * *Bptr;
			}
		}

		return matrix<T>(n, B.m, matrix_ptr(buffer, ::free));
	}


	template <typename T>
	matrix<T>& matrix<T>::operator*=(const matrix<T>& B) {

		if (is_null())
			return *this;

		if (B.is_null() || m!=B.n) {
			
			make_null();
			return *this;
		}

		auto buffer = (T*)malloc(n * B.m * sizeof(T));

		if (!buffer) {

			make_null();
			return *this;
		}

		// multiplication kernel
		for (unsigned int i=0;i<n; i++) { // ith row in new matrix

			auto Mptr = buffer + i;
			auto Bptr = B.M.get();

			for (unsigned int j=0; j<B.m; j++, Mptr+=n) { // jth column in new matrix

				auto Aptr = M.get() + i;
			
				*Mptr = T(0);

				for (unsigned int k=0; k<m; k++, Aptr+=n, Bptr++)
					*Mptr += *Aptr * *Bptr;
			}
		}

		// update this
		m = B.m;
		M.reset(buffer);

		return *this;
	}

	
	template <typename T>
	matrix<T> matrix<T>::operator|(const matrix<T>& B) const {
	
		if ((is_null() && B.is_null()) || (!is_null() && !B.is_null() && n!=B.n))
			return matrix<T>::nullmatrix();

		size_t sizeA = n * m;
		size_t sizeB = B.n * B.m;

		auto buffer = (T*)malloc(sizeA * sizeof(T) + sizeB * sizeof(T));

		if (!buffer)
			return matrix<T>::nullmatrix();

		// since matrices stored in column major format, we simply memcpy data from matrices 
		if (sizeA > 0) memcpy_s(buffer, sizeA * sizeof(T), M.get(), sizeA * sizeof(T));
		if (sizeB > 0) memcpy_s(buffer + sizeA, sizeB * sizeof(T), B.M.get(), sizeB * sizeof(T));

		return matrix<T>((n>0)?n:B.n, m+B.m, matrix_ptr(buffer, ::free));
	}


	template <typename T>
	matrix<T> matrix<T>::operator||(const matrix<T>& B) const {
	
		if ((is_null() && B.is_null()) || (!is_null() && !B.is_null() && m!=B.m))
			return matrix<T>::nullmatrix();

		size_t sizeA = n * m;
		size_t sizeB = B.n * B.m;

		auto buffer = (T*)malloc(sizeA * sizeof(T) + sizeB * sizeof(T));

		if (!buffer)
			return matrix<T>::nullmatrix();

		unsigned int n_ = n + B.n;
		unsigned int m_ = (m>0) ? m : B.m;

		auto Aptr = M.get();
		auto Bptr = B.M.get();
		auto Mptr = buffer;

		// since matrices stored in column major format we need to copy each column at a time
		for (unsigned int i=0; i<m_; i++, Mptr+=n_) {

			if (Aptr) {

				memcpy_s(Mptr, n * sizeof(T), Aptr, n * sizeof(T));
				Aptr += n;
			}

			if (Bptr) {

				memcpy_s(Mptr+n, B.n * sizeof(T), Bptr, B.n * sizeof(T));
				Bptr += B.n;
			}
		}

		return matrix<T>(n_, m_, matrix_ptr(buffer, ::free));
	}



	//
	// stream IO functions
	//

	template <typename T>
	std::ostream& operator<<(std::ostream& os, const matrix<T>& A) {

		if (A.is_null())
			os << setw(0) << "(NULL matrix)" << endl;
		else {

			auto buffer = A.M.get();
		
			// determine maximum string extent for matrix elements based on formatting flags of ostream os
			streamsize w = 0;

			stringstream sm;
			string str;

			sm.flags(os.flags());
			sm.width(0);
		
			for (unsigned int i=0;i<(A.n*A.m);i++) {
			
				sm << buffer[i] << endl; // render element to string stream sm
				sm >> str;
				w = max<streamsize>(w, str.length());
			}

			// print matrix order
			os << setw(0) << "(" << A.n << ", " << A.m << ")...\n" << std::right; // ensure right alignment for subsequent output

			for (unsigned int i=0;i<A.n;i++) { // print each row
			
				auto Aptr = buffer + i;
			
				for (unsigned int j=0;j<A.m;j++, Aptr+=A.n) // print each element in current row
					os << setw(w) << *Aptr << "  ";
				os << "\n";
			}
		
			os << endl;
		}

		return os;
	}

}