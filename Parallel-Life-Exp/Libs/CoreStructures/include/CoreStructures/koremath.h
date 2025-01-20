//
//  koremath.h
//

#pragma once

#include <cmath>
#include <cstdarg>
#include <limits>
#include <algorithm>
#include <functional>

namespace gsl {

	#ifndef gu_byte
	
		typedef unsigned char gu_byte; // byte [0, 255] representation
	
	#endif


	// trignometric constants extending definitions in <cmath>
	#define gu_pi				3.141593f	// pi
	#define	gu_radian			0.017453f	// pi / 180
	#define gu_radian_rcp		57.295779f	// radian reciprocal = 180 / pi


	// vector component values for modelling in R2, R3 (homogeneous and non-homogeneous) and R4
	typedef enum{_x=0, _y=1, _z=2, _w=3, _u=0, _v=1, _r=0, _g=1, _b=2, _a=3, _s=0, _t=1, _p=2, _q=3} vec_comp;



	//
	// floating point threshold and equality / inequality function definitions
	//

	// floating point equality and inequality functions to address numerical imprecision in IEEE based floating point numbers.  Equality is determined by fabs(a - b) laying in the range [0, precision).  precision is a log base 10 number eg. 1e+x.  The default value for precision = gu_epsilon so fpequal(a, b) compares a and b with respect to gu_epsilon.  Given { x <= y <---> (y-x)>=0.0 } and { x >= y <---> (x-y)>=0.0 } we do not need to specify specific inequality functions for <= and >= when dealing with floating point numbers

	#define	gu_epsilon			std::numeric_limits<float>::epsilon() // single precision floating point error tolerance
	#define	gu_epsilon_d		std::numeric_limits<double>::epsilon() // double precision floating point error tolerance


	// a == b (single precision)
	inline bool fequal(float a, float b, float precision = gu_epsilon) {
	
		return fabs(a - b) < precision;
	}

	// a == b (double precision)
	inline bool fequal(double a, double b, double precision = gu_epsilon_d) {

		return fabs(a - b) < precision;
	}

	// a < b
	inline bool fless(float a, float b, float precision = gu_epsilon) {
	
		return (b - a) >= precision;
	}

	inline bool fless(double a, double b, double precision = gu_epsilon_d) {
	
		return (b - a) >= precision;
	}

	// a > b
	inline bool fgreater(float a, float b, float precision = gu_epsilon) {
	
		return (a - b) >= precision;
	}

	inline bool fgreater(double a, double b, double precision = gu_epsilon_d) {
	
		return (a - b) >= precision;
	}



	//
	// Template interface for equality / inequality testing.  This provides a single interface with specialisations for float and double precision testing.  Since C++ does not allow operators on intrinsic types to be changed, to avoid creating wrapper classes around intrinsic types a set of templated functions with specialisations are defined to encapsulate the equality / inequality operators.  Templated types can use these overloaded functions instead of using operators directly.  This allows specialisations to be applied where specific equality / inequality testing is needed on floating point numbers.  The precision limit for these functions is based on the double float type
	//

	// a == b
	template <class T, class U>
	inline bool tequal(const T& a, const U& b, const double precision = gu_epsilon_d) {

		return a==b;
	}

	template <>
	inline bool tequal<float, float>(const float& a, const float& b, const double precision) {

		return fequal(a, b, (float)precision);
	}

	template <>
	inline bool tequal<double, double>(const double& a, const double& b, const double precision) {

		return fequal(a, b, precision);
	}

	// a < b
	template <class T, class U>
	inline bool tless(const T& a, const U& b, const double precision = gu_epsilon_d) {

		return a < b;
	}

	template <>
	inline bool tless<float, float>(const float& a, const float& b, const double precision) {

		return fless(a, b, (float)precision);
	}

	template <>
	inline bool tless<double, double>(const double& a, const double& b, const double precision) {

		return fless(a, b, precision);
	}

	// a > b
	template <class T, class U>
	inline bool tgreater(const T& a, const U& b, const double precision = gu_epsilon_d) {

		return a > b;
	}

	template <>
	inline bool tgreater<float, float>(const float& a, const float& b, const double precision) {

		return fgreater(a, b, (float)precision);
	}

	template <>
	inline bool tgreater<double, double>(const double& a, const double& b, const double precision) {

		return fgreater(a, b, precision);
	}



	// va_arg extraction (with float overload)
	template <typename T>
	void extract_va_arg(va_list *arg_list, T *value) {
	
		*value = T(va_arg(*arg_list, T));
	}

	template <>
	void extract_va_arg<float>(va_list *arg_list, float *value) {
	
		float v = float(va_arg(*arg_list, double));
		*value = v;
	}



	// clamp given value x into [lower, upper] range
	template <class T>
	inline T clamp(T x, T lower, T upper) {

		return std::max<T>(lower, std::min<T>(x, upper));
	}



	// return the square of two numbers
	template <class T>
	inline T sqr(T x) {
	
		return x * x;
	}



	// return the sign of x (base template version)
	template <class T>
	inline T sgn(T x) {

		if (x < T(0))
			return T(-1);
		else if (x == T(0))
			return T(0);
		else
			return T(1);
	}

	// float specialisation of sgn
	template <> inline float sgn<float>(float x) { return (fequal(x, 0.0f)) ? 0.0f : ((x < 0.0f) ? -1.0f : 1.0f); }

	// double specialisation of sgn
	template <> inline double sgn<double>(double x) { return (fequal(x, 0.0)) ? 0.0 : ((x < 0.0) ? -1.0 : 1.0); }



	// return true if number if a single bit (log base 2 aligned)
	inline bool isLogBase2(unsigned int x) {
	
		//return ((-x)&x)==x;
		return ((~x+1)&x)==x; // ~x+1 = -x (avoid compiler warning attemping negation of unsigned int)
	}

	// find the log base 2 of v (base 2 number index of x)
	unsigned int logbase2(unsigned int x);

	// round-up to next largest log base 2 number index for x
	inline unsigned int roundBase2(unsigned int x) {
	
		return (x==0) ? 0 : (0x01 << ((isLogBase2(x)) ? logbase2(x) : logbase2(x)+1));
	}



	// linear interpolation over interval [a, b] where x lies in the interval [0, 1].  letp performs linear extrapolation if x lies in the intervals [-inf, a) or (b, +inf]
	inline float lerp(float a, float b, float x) {

		return a * (1.0f - x) + b * x;
	}

	inline double lerp(double a, double b, double x) {

		return a * (1.0 - x) + b * x;
	}



	// round x to nearest whole number
	inline float fround(float x) {

		return floor(x + 0.5f);
	}

	inline double fround(double x) {

		return floor(x + 0.5);
	}



	// 1/sqrt(x)
	inline float invsqrt(float x) {
	
		float h = 0.5f * x;
		int i = *(int *)&x;
		i = 0x5f3759df - (i>>1);
		x = *(float *)&i;
		x = x * (1.5f - h * x * x);
		return x;
	}

	// x * 1/sqrt(x) = 1 / (1/sqrt(x)) = sqrt(x)
	inline float fastsqrt(float x) {
	
		return x * invsqrt(x);
	}



	// Peachey's modified modulus function returns a mod b but ensures correct ordering for negative values of a (see Ebert et al. p.32).  It is assumed b > 0
	float modP(float a, float b);
	double modP(double a, double b);



	// the step function returns step(x, a) in the range [0, 1] where step(x, a) = 0 if x<a and step(x, a) = 1 otherwise
	inline float step(float x, float a) {

		return (x < a) ? 0.0f : 1.0f;
	}

	inline double step(double x, double a) {

		return (x < a) ? 0.0 : 1.0;
	}

	// the inverse step function returns 1 if x < a and 0 otherwise
	inline float inv_step(float x, float a) {

		return 1.0f - step(x, a);
	}

	inline double inv_step(double x, double a) {

		return 1.0 - step(x, a);
	}



	// ramp(x, a, b) returns a monotonically (linearly) increasing value in the range [0, 1] over the interval [a, b] (or [b, a] if b < a).  If x < a then ramp() returns 0.0 or if x >= b then ramp() returns 1.0.  If a == b (as determined by tequal) then the function reduces to the step function
	float ramp(float x, float a, float b);
	double ramp(double x, double a, double b);

	// inv_ramp returns 1.0 - ramp(x, a, b) which results in a monotonically decreasing (linear) value in the range [0, 1] over the interval [a, b]
	inline float inv_ramp(float x, float a, float b) {

		return 1.0f - ramp(x, a, b);
	}

	inline double inv_ramp(double x, double a, double b) {

		return 1.0 - ramp(x, a, b);
	}



	// smoothstep(x, a, b) returns a monotonically increasing sigmoidal curve in the range [0, 1] over the interval [a, b] (or [b, a] if b < a).  If x < a then smoothstep() returns 0.0 or if x >= b then smoothstep() returns 1.0.  If a == b (as determined by tequal) then the function reduces to the step function
	float smoothstep(float x, float a, float b);
	double smoothstep(double x, double a, double b);

	// inv_smoothstep returns 1.0 - smoothstep(x, a, b) which results in a monotonically decreasing sigmoidal curve in the range [0, 1] over the interval [a, b]
	inline float inv_smoothstep(float x, float a, float b) {

		return 1.0f - smoothstep(x, a, b);
	}

	inline double inv_smoothstep(double x, double a, double b) {

		return 1.0 - smoothstep(x, a, b);
	}



	// pulse() returns 1.0 when x lies in the interval [a, b] where a <= x <= b (or [b, a] where b <= x <= a if b < a), otherwise pulse() returns 0.0.  The bounary of the pulse region is determined by tequal.  note: Peachey's pulse macro (see Ebert et al p.28) does not work for a singularity at x = a = b so this case is handled separately
	float pulse(float x, float a, float b);
	double pulse(double x, double a, double b);



	// gamma correction transfer function maps x->gamma(x, g) where g = (0, +inf].  When g=1.0, x->x
	inline float gamma(float x, float g) {

		return pow(x, 1.0f/g);
	}

	inline double gamma(double x, double g) {

		return pow(x, 1.0/g);
	}



	// Perlin and Hoffert's modified gamma transfer function (Ebert et al [89]) where x->bias(x, b) and b = (0, 1).  When b=0.5, x->x 
	inline float bias(float x, float b) {

		return pow(x, log(b)/log(0.5f));
	}

	inline double bias(double x, double b) {

		return pow(x, log(b)/log(0.5));
	}



	// Perlin and Hoffert's gain transfer function (Ebert et al [89]) where x->gain(x, g) and g = (0, 1).  When g=0.5, x->x
	inline float gain(float x, float g) {

		return (x<0.5f) ? bias(1.0f-g, 2.0f*x) / 2.0f : 1.0f - bias(1.0f-g, 2.0f - 2.0f*x) / 2.0f;
	}

	inline double gain(double x, double g) {

		return (x<0.5) ? bias(1.0-g, 2.0*x) / 2.0 : 1.0 - bias(1.0-g, 2.0 - 2.0*x) / 2.0;
	}



	// return the gaussian g(x) normalised to 1.0 at x=u with standard deviation sigma.  It is assumed sigma != 0.0
	inline float gaussian(float x, float sigma, float u=0.0f) {

		return exp(-(sqr(x - u)) / (2.0f * sqr(sigma)));
	}

	inline double gaussian(double x, double sigma, double u=0.0) {

		return exp(-(sqr(x - u)) / (2.0 * sqr(sigma)));
	}



	// logistic curve f(x/sigma) where x=[-inf, +inf] and f(x/sigma) = (0, 1).  x is scaled by 1/sigma where sigma = (0, +inf].  It is assumed sigma != 0.0
	inline float sigmoidL(float x, float sigma=1.0f) {

		return 1.0f / (1.0f + exp(-x / sigma));
	}

	inline double sigmoidL(double x, double sigma=1.0) {

		return 1.0 / (1.0 + exp(-x / sigma));
	}



	// error function approximation f(x/sigma) where x=[-inf, +inf] and f(x/sigma) = (-1, 1).  x is scaled by 1/sigma where sigma = (0, +inf].  It is assumed sigma != 0.0
	inline float sigmoidE(float x, float sigma=1.0f) {

		return tanh(x/sigma);
	}

	inline double sigmoidE(double x, double sigma=1.0) {

		return tanh(x/sigma);
	}



	// cspline evaluates a 1D parametric cubic hermite (interpolating) spline.  This is a cardinal spline with tension coefficient c, where c = [0, 1].  When c = 0.0 (the default) cspline reduces to the Catmull-Rom interpolating spline.  When c=1.0 the resulting tangents are zero and the interpolation reduces to linear interpolation.  [Monotonicity can be approaximated when c>=0.5 (CHECK THIS)].  The spline parameter t is clamped to the interval [0, 1] which spans the control points (knot values) in p.  The knot values are uniformly distributed along the extent of the curve.  cspline models a 1D parametric curve so at least 4 knot values must be defined.  If not, the default value of T is returned.  cspline is templated and it is assumed operators <+ - * /> are defined on type T
	template <class T, class U>
	inline U __cspline(T t, int nKnots, const U *p, T c = T(0)) {

		if (nKnots < 4)
			return U();

		t = clamp<T>(t, T(0), T(1));

		int nSpans = nKnots - 3;

		t *= T(nSpans);

		int span = int(t);

		// make sure case where x=1 does not map to the wrong span (special case to deal with end of curve parameter value 1.0)
		if (span>=nSpans)
			span = nKnots - 4;

		int k = span + 1;

		t -= T(span); // local curve segment (span) parameter
		T td = T(1) / T(nSpans); // uniform distance between each t[i] is 1.0 / nSpans

		T tk = (T(k) - T(1)) * td;

		U m0 = (p[k+1] - p[k-1]) / ((tk+td) - (tk-td)) * (T(1) - c); // gradient m0
		U m1 = (p[k+2] - p[k]) / ((tk + td*T(2)) - tk) * (T(1) - c); // gradient m1

		// cubic coefficients (calculated using Horner's rule)
		T h00 = (((T(2)*t - T(3)) * t) * t) + T(1);
		T h10 = (((t - T(2)) * t) + T(1)) * t;
		T h01 = (-T(2)*t + T(3)) * t * t;
		T h11 = (t - T(1)) * t * t;

		return p[k]*h00 + m0*td*h10 + p[k+1]*h01 + m1*td*h11;
	}

}