#pragma once

// Provide basic wrapper around an array with 2D access capability (see lecture slides)

template <class T>
struct Array2D {

	T*		data;	// Array data
	int		w, h;	// number of array elements across (width) and down (height)

	// Constructor - Setup array data (in C/C++ the memory is uninitialised)
	Array2D(int w, int h) {

		this->w = w;
		this->h = h;
		this->data = (T*)malloc(w * h * sizeof(T));
	}

	// Operator to access elements in the array (see usage in gldemo.cpp)
	T &operator()(int x, int y) {

		T* addr = this->data + (y * w) + x;

		return *addr;
	}
};
