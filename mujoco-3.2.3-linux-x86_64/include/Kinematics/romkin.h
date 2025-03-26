#pragma once

//In arduino, functions and data are static because there is only one instance
//and we save some HEAP memory 
//in Android & Desktop aplications, is a normal templated class, to allow several 
//instances in the same aplication

#ifndef ARDUINO
#define STATIC 
#define INLINE_STATIC
#include <Kinematics/Eigen/Dense>
using Eigen::Matrix4d, Eigen::Vector3d, Eigen::Matrix3d;
	#ifndef M_PI
	constexpr double M_PI{ 3.14159265358979323846 };
	#endif
#else
	#define STATIC static
	#define INLINE_STATIC inline static
	#ifndef M_PI
	constexpr float M_PI{ 3.14159265358979323846F };
	#endif
#endif // !ARDUINO



template <class REAL>
struct _RomKin {
	using Real = REAL;
	struct DH { REAL Theta, D, A, Alpha;};

	//STATIC CONSTANTS
	//GEAR REDUCTION factor 
	static constexpr REAL factor_4 = 24.0 / 32.0 ;
	static constexpr REAL factor_6 = 34.0 / 51.0 ;
	static constexpr REAL rad2deg = 180.0 / M_PI;
	static constexpr REAL deg2rad = M_PI / 180.0;

	//STATIC CONFIGURABLE DATA
	INLINE_STATIC REAL L1{ 0.068 }, L2{ 0.236 }, L3{ 0.0149 }, L4{ 0.28 }, L5{ 0.022 }, L6{ 0.087 };
	INLINE_STATIC REAL alpha{}, beta{}, cosb{}, sinb{};
	INLINE_STATIC REAL Lc{}, Ld{};

	STATIC void init() { set_lenghts(L1, L2, L3, L4, L5, L6); }
	STATIC void set_lenghts(REAL l1, REAL l2, REAL l3, REAL l4, REAL l5, REAL l6);
	STATIC void set_lenghts(uint16_t l[6]);
	#ifndef ARDUINO
	  DH DH_data[6]{};
	#else
	  static DH DH_data[6];
	#endif // !ARDUINO

	//COMMON METHODS 
	STATIC bool IKwrist(REAL* q, REAL x, REAL y, REAL z, bool elbow = true);
	STATIC void q2m(REAL m[], REAL q[], bool gdl3 = false);
	STATIC void m2q(REAL q[], REAL m[], bool gdl3 = false);
        STATIC void mt2qt(REAL q[], REAL m[], bool gdl3 = false);
	STATIC void FKfast(const REAL* q, REAL m[][3], REAL p[]);
	STATIC bool IKfast(REAL* q, const REAL m[][3], const REAL p[], bool elbow = true, bool wrist = true);

	//NON ARDUINO METHODS  
#ifndef ARDUINO
	Vector3d FKwrist(REAL q1, REAL q2, REAL q3);
	Vector3d FKwristDH(REAL q1, REAL q2, REAL q3);
    Matrix4d FK(REAL q1, REAL q2, REAL q3, REAL q4, REAL q5, REAL q6);
    bool IK(REAL* q, Matrix4d T, bool elbow=true, bool wrist=true);
	Matrix4d rom_dh_matrix(double q, int index) {
		return dh_matrix(q + DH_data[index].Theta, DH_data[index].D, DH_data[index].A, DH_data[index].Alpha);
	}
	Matrix4d dh_matrix(REAL theta, REAL d, REAL a, REAL alpha);
#endif //!ARDUINO
};

#ifndef ARDUINO
// inline definition of utility function
template<class REAL>
inline Matrix4d _RomKin<REAL>::dh_matrix(REAL theta, REAL d, REAL a, REAL alpha) {
	Matrix4d T;
	T << cos(theta), -sin(theta) * cos(alpha), sin(theta)* sin(alpha), a* cos(theta),
		sin(theta), cos(theta)* cos(alpha), -cos(theta) * sin(alpha), a* sin(theta),
		0, sin(alpha), cos(alpha), d,
		0, 0, 0, 1;
	return T;
}
#endif //!ARDUINO

#ifndef ARDUINO
extern template struct _RomKin<double>;
using RomKin = _RomKin<double>;
#else
extern template struct _RomKin<float>;
using RomKin = _RomKin<float>;
#endif // !ARDUINO
