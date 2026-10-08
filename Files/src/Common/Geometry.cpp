#include <algorithm>
#include <array>
#include <cmath>
#include <numbers>
#include "MathUtil.h"
#include "Geometry.h"

#pragma region 色
Color::Color(unsigned int c)
{
	r = (float)(c / 0x10000);
	g = (float)(c / 0x100 % 0x100);
	b = (float)(c % 0x100);
}

Color Color::operator/(float f)
{
	return { r / f, g / f, b / f };
}

Color Color::Add(const Color& c, bool limit) const
{
	Color ret = {};

	ret.r = r + c.r;
	ret.g = g + c.g;
	ret.b = b + c.b;

	if (limit)
	{
#ifndef _HAS_CXX20
		ret.r = std::min(std::max(ret.r, 0.0f), 255.0f);
		ret.g = std::min(std::max(ret.g, 0.0f), 255.0f);
		ret.b = std::min(std::max(ret.b, 0.0f), 255.0f);
#else
		ret.r = std::clamp(ret.r, 0.0f, 255.0f);
		ret.g = std::clamp(ret.g, 0.0f, 255.0f);
		ret.b = std::clamp(ret.b, 0.0f, 255.0f);
#endif
	}

	return ret;
}

unsigned int Color::GetColorHex() const
{
	unsigned int ret = 0u;
	ret += (unsigned int)(r * 0x10000u);
	ret += (unsigned int)(g * 0x100u);
	ret += (unsigned int)(b);
	return ret;
}

Color LerpColor(const Color& start, const Color& end, float rate)
{
	rate = std::clamp(rate, 0.0f, 1.0f);

	Color ret = start;
	ret.r += rate * (end.r - start.r);
	ret.g += rate * (end.g - start.g);
	ret.b += rate * (end.b - start.b);

	return ret;
}

Color ColorMapCalc(const std::map<float, Color>& map, float time)
{
	if (map.empty()) return Color();
	std::pair<float, Color> lastPair = (*map.begin());

	for (auto it = ++map.begin(); it != map.end(); ++it)
	{
		std::pair<float, Color> nextPair = (*it);

		if (time < nextPair.first)
		{
			return LerpColor(lastPair.second, nextPair.second,
				(time - lastPair.first) / (nextPair.first - lastPair.first));
		}

		lastPair = nextPair;
	}

	return (*map.end()).second;
}
#pragma endregion

#pragma region ２次元ベクトル
Vector2 Vector2::operator+() const
{
	return { +x, +y };
}

Vector2 Vector2::operator-() const
{
	return { -x, -y };
}

Vector2& Vector2::operator=(const float& f)
{
	x = f;
	y = f;
	return *this;
}

Vector2& Vector2::operator=(const Vector2& v)
{
	x = v.x;
	y = v.y;
	return *this;
}

Vector2& Vector2::operator+=(const Vector2& v)
{
	x += v.x;
	y += v.y;
	return *this;
}

Vector2& Vector2::operator-=(const Vector2& v)
{
	x -= v.x;
	y -= v.y;
	return *this;
}

Vector2& Vector2::operator*=(const float& f)
{
	x *= f;
	y *= f;
	return *this;
}

Vector2& Vector2::operator/=(const float& f)
{
	x /= f;
	y /= f;
	return *this;
}

float Vector2::Length() const
{
	return std::hypot(x, y);
}

float Vector2::LengthSquare() const
{
	return x * x + y * y;
}

float Vector2::Magnitude() const
{
	return Length();
}

Vector2& Vector2::Normalize()
{
	float len = Length();
	if (len != 0.0f)
	{
		x /= len;
		y /= len;
	}
	return *this;
}

Vector2 Vector2::Normalized() const
{
	float len = Length();
	if (len == 0.0f) return Vector2();
	return *this / len;
}

float Vector2::Angle() const
{
	return std::atan2(y, x);
}

float Vector2::AngleDegree() const
{
	return Angle() / MathUtil::PI_V<float> * 180.0f;
}

Vector2 operator+(const Vector2& va, const Vector2& vb)
{
	return Vector2(va) += vb;
}

Vector2 operator-(const Vector2& va, const Vector2& vb)
{
	return Vector2(va) -= vb;
}

Vector2 operator*(const Vector2& v, const float& f)
{
	return Vector2(v) *= f;
}

Vector2 operator/(const Vector2& v, const float& f)
{
	return Vector2(v) /= f;
}

float operator*(const Vector2& va, const Vector2& vb)
{
	return Dot(va, vb);
}

float operator%(const Vector2& va, const Vector2& vb)
{
	return Cross(va, vb);
}

bool operator<(const Vector2& va, const Vector2& vb)
{
	return va.x < vb.x && va.y < vb.y;
}

bool operator>(const Vector2& va, const Vector2& vb)
{
	return vb < va;
}

bool operator<=(const Vector2& va, const Vector2& vb)
{
	return va.x <= vb.x && va.y <= vb.y;
}

bool operator>=(const Vector2& va, const Vector2& vb)
{
	return vb <= va;
}

bool operator==(const Vector2& va, const Vector2& vb)
{
	return va.x == vb.x && va.y == vb.y;
}

float Dot(const Vector2& va, const Vector2& vb)
{
	return va.x * vb.x + va.y * vb.y;
}

float Cross(const Vector2& va, const Vector2& vb)
{
	return va.x * vb.y - vb.x * va.y;
}

Vector2 Lerp(const Vector2& start, const Vector2& end, const float& rate)
{
	Vector2 ret = start;
	ret.x += (end.x - start.x) * rate;
	ret.y += (end.y - start.y) * rate;

	return ret;
}

Vector2 GetVector2FromAngle(float radian, float length)
{
	return Vector2(std::cos(radian), std::sin(radian)) * length;
}
#pragma endregion

#pragma region ３次元ベクトル
Vector3 Vector3::operator+() const
{
	return { +x, +y, +z };
}

Vector3 Vector3::operator-() const
{
	return { -x, -y, -z };
}

Vector3& Vector3::operator=(const float& f)
{
	x = y = z = f;
	return *this;
}

Vector3& Vector3::operator=(const Vector3& v)
{
	x = v.x;
	y = v.y;
	z = v.z;
	return *this;
}

Vector3& Vector3::operator+=(const Vector3& v)
{
	x += v.x;
	y += v.y;
	z += v.z;
	return *this;
}

Vector3& Vector3::operator-=(const Vector3& v)
{
	x -= v.x;
	y -= v.y;
	z -= v.z;
	return *this;
}

Vector3& Vector3::operator*=(const float& f)
{
	x *= f;
	y *= f;
	z *= f;
	return *this;
}

Vector3& Vector3::operator/=(const float& f)
{
	x /= f;
	y /= f;
	z /= f;
	return *this;
}

Vector2 Vector3::XY() const
{
	return Vector2(x, y);
}

Vector2 Vector3::XZ() const
{
	return Vector2(x, z);
}

Vector2 Vector3::YZ() const
{
	return Vector2(y, z);
}

float Vector3::Length() const
{
	return std::hypot(x, y, z);
}

float Vector3::LengthSquare() const
{
	return x * x + y * y + z * z;
}

float Vector3::Magnitude() const
{
	return Length();
}

float Vector3::MagnitudeSquare() const
{
	return LengthSquare();
}

Vector3& Vector3::Normalize()
{
	float len = Length();
	if (len != 0.0f)
	{
		x /= len;
		y /= len;
		z /= len;
	}
	return *this;
}

Vector3 Vector3::Normalized() const
{
	float len = Length();
	if (len == 0.0f) return Vector3();
	return *this / len;
}

Vector3 operator+(const Vector3& va, const Vector3& vb)
{
	return Vector3(va) += vb;
}

Vector3 operator-(const Vector3& va, const Vector3& vb)
{
	return Vector3(va) -= vb;
}

Vector3 operator*(const Vector3& v, const float& f)
{
	return Vector3(v) *= f;
}

Vector3 operator/(const Vector3& v, const float& f)
{
	return Vector3(v) /= f;
}

float operator*(const Vector3& va, const Vector3& vb)
{
	return Dot(va, vb);
}

Vector3 operator%(const Vector3& va, const Vector3& vb)
{
	return Cross(va, vb);
}

bool operator<(const Vector3& va, const Vector3& vb)
{
	return va.x < vb.x && va.y < vb.y && va.z < vb.z;
}

bool operator>(const Vector3& va, const Vector3& vb)
{
	return vb < va;
}

bool operator<=(const Vector3& va, const Vector3& vb)
{
	return va.x <= vb.x && va.y <= vb.y && va.z <= vb.z;
}

bool operator>=(const Vector3& va, const Vector3& vb)
{
	return vb <= va;
}

bool operator==(const Vector3& v, const float& f)
{
	return v.x == f && v.y == f && v.z == f;
}

bool operator==(const Vector3& va, const Vector3& vb)
{
	return va.x == vb.x && va.y == vb.y && va.z == vb.z;
}

float Dot(const Vector3& va, const Vector3& vb)
{
	return va.x * vb.x + va.y * vb.y + va.z * vb.z;
}

Vector3 Cross(const Vector3& va, const Vector3& vb)
{
	return { va.y * vb.z - va.z * vb.y,
	         va.z * vb.x - va.x * vb.z,
	         va.x * vb.y - va.y * vb.x };
}

Vector3 Lerp(const Vector3& start, const Vector3& end, const float& rate)
{
	Vector3 ret = start;
	ret.x += (end.x - start.x) * rate;
	ret.y += (end.y - start.y) * rate;
	ret.z += (end.z - start.z) * rate;

	return ret;
}

Vector3 LerpRad(const Vector3& start, const Vector3& end, const float& rate)
{
	Vector3 ret = {};
	ret.x = MathUtil::LerpRad(start.x, end.x, rate);
	ret.y = MathUtil::LerpRad(start.y, end.y, rate);
	ret.z = MathUtil::LerpRad(start.z, end.z, rate);

	return ret;
}

float CosSimilar(const Vector3& va, const Vector3& vb)
{
	if (va == 0.0f || vb == 0.0f) return 0.0f;

	return Dot(va, vb) / (va.Length() * vb.Length());
}
#pragma endregion

#pragma region ４x４行列
Matrix4x4& Matrix4x4::operator*=(const Matrix4x4& mat)
{
	Matrix4x4 result = {};
	for (int i = 0; i < 4; i++) for (int j = 0; j < 4; j++) for (int k = 0; k < 4; k++)
	{
		result.m[i][j] += m[i][k] * mat.m[k][j];
	}

	for (int i = 0; i < 4; i++) for (int j = 0; j < 4; j++)
	{
		m[i][j] = result.m[i][j];
	}
	
	return *this;
}

Matrix4x4 Matrix4x4::Transposed()
{
	return {
		m[0][0], m[1][0], m[2][0], m[3][0],
		m[0][1], m[1][1], m[2][1], m[3][1],
		m[0][2], m[1][2], m[2][2], m[3][2],
		m[0][3], m[1][3], m[2][3], m[3][3]
	};
}

Vector3 operator*(const Matrix4x4& m, const Vector3& v)
{
	Vector3 result = {};
	
	result.x = m.m[0][0] * v.x + m.m[0][1] * v.y + m.m[0][2] * v.z + m.m[0][3];
	result.y = m.m[1][0] * v.x + m.m[1][1] * v.y + m.m[1][2] * v.z + m.m[1][3];
	result.z = m.m[2][0] * v.x + m.m[2][1] * v.y + m.m[2][2] * v.z + m.m[2][3];

	return result;
}

Matrix4x4 operator*(const Matrix4x4& ma, const Matrix4x4& mb)
{
	Matrix4x4 result = {};
	for (int i = 0; i < 4; i++) for (int j = 0; j < 4; j++) for (int k = 0; k < 4; k++)
	{
		result.m[i][j] += ma.m[i][k] * mb.m[k][j];
	}

	return result;
}

Matrix4x4 operator%(const Matrix4x4& ma, const Matrix4x4& mb)
{
	return MultRP(ma, mb);
}

Matrix4x4 MultRP(const Matrix4x4& ma, const Matrix4x4& mb)
{
	Matrix4x4 result = {};
	for (int i = 0; i < 3; i++) for (int j = 0; j < 3; j++) for (int k = 0; k < 3; k++)
	{
		result.m[i][j] += ma.m[i][k] * mb.m[k][j];
	}
	result.m[3][3] += ma.m[3][3] * mb.m[3][3];
	
	result.m[0][3] += ma.m[0][3] + mb.m[0][3];
	result.m[1][3] += ma.m[1][3] + mb.m[1][3];
	result.m[2][3] += ma.m[2][3] + mb.m[2][3];

	result.m[3][0] += ma.m[3][0] + mb.m[3][0];
	result.m[3][1] += ma.m[3][1] + mb.m[3][1];
	result.m[3][2] += ma.m[3][2] + mb.m[3][2];

	return result;
}

Matrix4x4 IdentityMatrix()
{
	return {
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};
}

Matrix4x4 TransposeMatrix(const Matrix4x4& m)
{
	return {
		m.m[0][0], m.m[1][0], m.m[2][0], m.m[3][0],
		m.m[0][1], m.m[1][1], m.m[2][1], m.m[3][1],
		m.m[0][2], m.m[1][2], m.m[2][2], m.m[3][2],
		m.m[0][3], m.m[1][3], m.m[2][3], m.m[3][3]
	};
}

Matrix4x4 ScaleMatrix(const float& x, const float& y, const float& z)
{
	return {
		x,    0.0f, 0.0f, 0.0f,
		0.0f, y,    0.0f, 0.0f,
		0.0f, 0.0f, z,    0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};
}

Matrix4x4 ScaleMatrix(const Vector3& v)
{
	return ScaleMatrix(v.x, v.y, v.z);
}

Matrix4x4 RotationMatrixX(const float& angle)
{
	return
	{
		1.0f, 0.0f,        0.0f,         0.0f,
		0.0f, cosf(angle), -sinf(angle), 0.0f,
		0.0f, sinf(angle), cosf(angle),  0.0f,
		0.0f, 0.0f,        0.0f,         1.0f
	};
	/*
	{
		1.0f, 0.0f,         0.0f,        0.0f,
		0.0f, cosf(angle),  sinf(angle), 0.0f,
		0.0f, -sinf(angle), cosf(angle), 0.0f,
		0.0f, 0.0f,         0.0f,        1.0f
	};
	*/
}

Matrix4x4 RotationMatrixY(const float& angle)
{
	return
	{
		cosf(angle),  0.0f, sinf(angle), 0.0f,
		0.0f,         1.0f, 0.0f,        0.0f,
		-sinf(angle), 0.0f, cosf(angle), 0.0f,
		0.0f,         0.0f, 0.0f,        1.0f
	};
	/*
	{
		cosf(angle), 0.0f, -sinf(angle), 0.0f,
		0.0f,        1.0f, 0.0f,         0.0f,
		sinf(angle), 0.0f, cosf(angle),  0.0f,
		0.0f,        0.0f, 0.0f,         1.0f
	};
	*/
}

Matrix4x4 RotationMatrixZ(const float& angle)
{
	return
	{
		cosf(angle), -sinf(angle), 0.0f, 0.0f,
		sinf(angle), cosf(angle),  0.0f, 0.0f,
		0.0f,        0.0f,         1.0f, 0.0f,
		0.0f,        0.0f,         0.0f, 1.0f
	};
	/*
	{
		cosf(angle),  sinf(angle), 0.0f, 0.0f,
		-sinf(angle), cosf(angle), 0.0f, 0.0f,
		0.0f,         0.0f,        1.0f, 0.0f,
		0.0f,         0.0f,        0.0f, 1.0f
	};
	*/
}

Matrix4x4 RotationMatrixXYZ(const float& x, const float& y, const float& z)
{
	Matrix4x4 result = IdentityMatrix();
	result *= RotationMatrixX(x);
	result *= RotationMatrixY(y);
	result *= RotationMatrixZ(z);
	return result;
}

Matrix4x4 RotationMatrixXYZ(const Vector3& v)
{
	return RotationMatrixXYZ(v.x, v.y, v.z);
}

Matrix4x4 TranslationMatrix(const float& x, const float& y, const float& z)
{
	Matrix4x4 result = IdentityMatrix();
	
	result.m[0][3] = x;
	result.m[1][3] = y;
	result.m[2][3] = z;
	
	return result;
}

Matrix4x4 TranslationMatrix(const Vector3& v)
{
	return TranslationMatrix(v.x, v.y, v.z);
}

Vector3 GetForwardVector(const Vector3& v, const Vector3& zero)
{
	Matrix4x4 rotMat = RotationMatrixXYZ(v);
	return (rotMat * zero).Normalized();
}

#pragma endregion

#pragma region クォータニオン
void Quaternion::Euler(double rad_x, double rad_y, double rad_z)
{
	rad_x = MathUtil::RadIn2PI(rad_x);
	rad_y = MathUtil::RadIn2PI(rad_y);
	rad_z = MathUtil::RadIn2PI(rad_z);

	double cosZ = cos(rad_z / 2.0f);
	double sinZ = -sin(rad_z / 2.0f); // 左手座標系用の改良
	//double sinZ = sin(rad_z / 2.0f);
	double cosX = cos(rad_x / 2.0f);
	double sinX = -sin(rad_x / 2.0f); // 左手座標系用の改良
	//double sinX = sin(rad_x / 2.0f);
	double cosY = cos(rad_y / 2.0f);
	double sinY = -sin(rad_y / 2.0f); // 左手座標系用の改良
	//double sinY = sin(rad_y / 2.0f);

	w = cosX * cosY * cosZ + sinX * sinY * sinZ;
	x = sinX * cosY * cosZ + cosX * sinY * sinZ;
	y = cosX * sinY * cosZ - sinX * cosY * sinZ;
	z = cosX * cosY * sinZ - sinX * sinY * cosZ;
}

void Quaternion::Euler(const Vector3& rad)
{
	Euler(rad.x, rad.y, rad.z);
}

double Quaternion::Magnitude() const
{
	return sqrt(MagnitudeSquare());
}

double Quaternion::MagnitudeSquare() const
{
	return x * x + y * y + z * z + w * w;
}

Vector3 Quaternion::ToEuler() const
{
	Vector3 ret;

	double r11 = 2 * (x * z + w * y);
	double r12 = w * w - x * x - y * y + z * z;
	double r21 = -2 * (y * z - w * x);
	double r31 = 2 * (x * y + w * z);
	double r32 = w * w - x * x + y * y - z * z;

	r21 = MathUtil::Clamp(r21, -1.0, 1.0); // 誤差修正用

	ret.x = static_cast<float>(std::asin(r21));
	ret.y = static_cast<float>(std::atan2(r11, r12));
	ret.z = static_cast<float>(std::atan2(r31, r32));

	return ret;
}

Matrix4x4 Quaternion::ToMatrix() const
{
	Matrix4x4 mat = {};

	struct XYZW { float x, y, z, w; };

	XYZW fq = { (float)x, (float)y, (float)z, (float)w };

	float sx = fq.x * fq.x * 2.0f;
	float sy = fq.y * fq.y * 2.0f;
	float sz = fq.z * fq.z * 2.0f;
	float cx = fq.y * fq.z * 2.0f;
	float cy = fq.x * fq.z * 2.0f;
	float cz = fq.x * fq.y * 2.0f;
	float wx = fq.w * fq.x * 2.0f;
	float wy = fq.w * fq.y * 2.0f;
	float wz = fq.w * fq.z * 2.0f;

	mat.m[0][0] = 1.0f - (sy + sz);	mat.m[0][1] = cz + wz;			mat.m[0][2] = cy - wy;			mat.m[0][3] = 0.0f;
	mat.m[1][0] = cz - wz;			mat.m[1][1] = 1.0f - (sx + sz);	mat.m[1][2] = cx + wx;			mat.m[1][3] = 0.0f;
	mat.m[2][0] = cy + wy;			mat.m[2][1] = cx - wx;			mat.m[2][2] = 1.0f - (sx + sy);	mat.m[2][3] = 0.0f;
	mat.m[3][0] = 0.0f;				mat.m[3][1] = 0.0f;				mat.m[3][2] = 0.0f;				mat.m[3][3] = 1.0f;

	return mat;
}

Vector3 Quaternion::XYZ() const
{
	return { (float)x, (float)y, (float)z };
}

Quaternion& Quaternion::Normalize()
{
	double mag = Magnitude();
	w /= mag;
	x /= mag;
	y /= mag;
	z /= mag;
	return *this;
}

Quaternion Quaternion::Normalized() const
{
	double mag = Magnitude();
	return *this / mag;
}

Quaternion Quaternion::Inversed() const
{
	double n = 1.0f / MagnitudeSquare();
	Quaternion tmp = { w, -x, -y, -z };
	return { tmp.w * n, tmp.x * n, tmp.y * n, tmp.z * n };
}

Quaternion& Quaternion::operator+=(const Quaternion& q)
{
	w += q.w;
	x += q.x;
	y += q.y;
	z += q.z;
	return *this;
}

Quaternion& Quaternion::operator*=(float f)
{
	w *= f;
	x *= f;
	y *= f;
	z *= f;
	return *this;
}

Quaternion& Quaternion::operator*=(const Quaternion& q)
{
	Quaternion ret;
	double d1, d2, d3, d4;

	// wの計算 
	d1 = w * q.w;
	d2 = -x * q.x;
	d3 = -y * q.y;
	d4 = -z * q.z;
	ret.w = d1 + d2 + d3 + d4;

	// xの計算 
	d1 = w * q.x;
	d2 = x * q.w;
	d3 = y * q.z;
	d4 = -z * q.y;
	ret.x = d1 + d2 + d3 + d4;

	// yの計算
	d1 = w * q.y;
	d2 = y * q.w;
	d3 = z * q.x;
	d4 = -x * q.z;
	ret.y = d1 + d2 + d3 + d4;

	// zの計算
	d1 = w * q.z;
	d2 = z * q.w;
	d3 = x * q.y;
	d4 = -y * q.x;
	ret.z = d1 + d2 + d3 + d4;

	return *this = ret;
}

Quaternion& Quaternion::operator/=(float f)
{
	w /= f;
	x /= f;
	y /= f;
	z /= f;
	return *this;
}

Quaternion Quaternion::Multiplication(const Quaternion& q) const
{
	return Quaternion(*this) *= q;
}

Quaternion operator+(const Quaternion& qa, const Quaternion& qb)
{
	return Quaternion(qa) += qb;
}

Quaternion operator*(const Quaternion& q, double d)
{
	return Quaternion(q) *= d;
}

Vector3 operator*(const Quaternion& q, const Vector3& v)
{
	// 位置情報に回転情報を反映させる
	// pos' = q・pos・q(-1)
	Quaternion tmp = Quaternion();
	tmp *= q;
	tmp *= Quaternion(0.0f, v.x, v.y, v.z);
	tmp *= q.Inversed();
	return { (float)tmp.x, (float)tmp.y, (float)tmp.z };
}

Quaternion operator*(const Quaternion& qa, const Quaternion& qb)
{
	return Quaternion(qa) *= qb;
}

Quaternion operator/(const Quaternion& q, double d)
{
	return Quaternion(q) /= d;
}

double Dot(const Quaternion& qa, const Quaternion& qb)
{
	return (qa.w * qb.w + qa.x * qb.x + qa.y * qb.y + qa.z * qb.z);
}

Quaternion AngleAxis(Vector3 axis, double rad)
{
	Quaternion ret = {};

	double norm;
	double c, s;

	ret.w = 1.0;
	ret.x = ret.y = ret.z = 0.0;

	norm = (double)axis.x * (double)axis.x + (double)axis.y * (double)axis.y + (double)axis.z * (double)axis.z;
	if (norm <= 0.0f) return ret;

	norm = 1.0 / sqrt(norm);
	axis.x = (float)(axis.x * norm);
	axis.y = (float)(axis.y * norm);
	axis.z = (float)(axis.z * norm);

	c = cos(0.5f * rad);
	//s = -sin(0.5f * rad); // 左手座標系用の改良？
	s = sin(0.5f * rad);

	ret.w = c;
	ret.x = s * axis.x;
	ret.y = s * axis.y;
	ret.z = s * axis.z;

	return ret;
}

Quaternion LookRotation(const Vector3& dir)
{
	Vector3 up = { 0.0f, 1.0f, 0.0f };
	return LookRotation(dir, up);
}

Quaternion LookRotation(Vector3 dir, Vector3 up)
{
	dir.Normalize();
	Vector3 right = Cross(up, dir);

	// dirとupがほぼ平行な場合の回避処理
	if (right.MagnitudeSquare() < 1.0e-6f)
	{
		// 進行方向がY軸寄りならX軸を、それ以外ならY軸を仮のUpベクトルとする
		up = (std::abs(dir.y) > 0.99f) ? Vector3(1.0f, 0.0f, 0.0f) : Vector3(0.0f, 1.0f, 0.0f);
		right = Cross(up, dir);
	}

	right.Normalize();
	up = Cross(dir, right);

	auto m00 = right.x;
	auto m10 = right.y;
	auto m20 = right.z;
	auto m01 = up.x;
	auto m11 = up.y;
	auto m21 = up.z;
	auto m02 = dir.x;
	auto m12 = dir.y;
	auto m22 = dir.z;

	float num8 = (m00 + m11) + m22;
	Quaternion quaternion = {};

	if (num8 > 0.0f) {
		double num = sqrt(num8 + 1.0);
		quaternion.w = num * 0.5;
		num = 0.5 / num;
		quaternion.x = ((double)m12 - m21) * num;
		quaternion.y = ((double)m20 - m02) * num;
		quaternion.z = ((double)m01 - m10) * num;

		quaternion.x = -quaternion.x; // 左手座標系用の改良
		quaternion.y = -quaternion.y; // 左手座標系用の改良
		quaternion.z = -quaternion.z; // 左手座標系用の改良
		return quaternion.Normalized();
	}

	if ((m00 >= m11) && (m00 >= m22)) {
		auto num7 = sqrt(((1.0f + m00) - m11) - m22);
		auto num4 = 0.5f / num7;
		quaternion.x = 0.5 * num7;
		quaternion.y = ((double)m01 + m10) * num4;
		quaternion.z = ((double)m02 + m20) * num4;
		quaternion.w = ((double)m12 - m21) * num4;

		quaternion.x = -quaternion.x; // 左手座標系用の改良
		quaternion.y = -quaternion.y; // 左手座標系用の改良
		quaternion.z = -quaternion.z; // 左手座標系用の改良
		return quaternion.Normalized();
	}

	if (m11 > m22) {
		auto num6 = sqrt(((1.0f + m11) - m00) - m22);
		auto num3 = 0.5f / num6;
		quaternion.x = ((double)m10 + m01) * num3;
		quaternion.y = 0.5 * num6;
		quaternion.z = ((double)m21 + m12) * num3;
		quaternion.w = ((double)m20 - m02) * num3;

		quaternion.x = -quaternion.x; // 左手座標系用の改良
		quaternion.y = -quaternion.y; // 左手座標系用の改良
		quaternion.z = -quaternion.z; // 左手座標系用の改良
		return quaternion.Normalized();
	}

	auto num5 = sqrt(((1.0f + m22) - m00) - m11);
	auto num2 = 0.5f / num5;
	quaternion.x = ((double)m20 + m02) * num2;
	quaternion.y = ((double)m21 + m12) * num2;
	quaternion.z = 0.5 * num5;
	quaternion.w = ((double)m01 - m10) * num2;

	quaternion.x = -quaternion.x; // 左手座標系用の改良
	quaternion.y = -quaternion.y; // 左手座標系用の改良
	quaternion.z = -quaternion.z; // 左手座標系用の改良
	return quaternion.Normalized();
}

Quaternion GetRotation(const Matrix4x4& mat)
{
	Quaternion ret;

	float s;
	float tr = mat.m[0][0] + mat.m[1][1] + mat.m[2][2] + 1.0f;

	if (tr >= 1.0f) {
		s = 0.5f / sqrtf(tr);
		ret.w = 0.25f / s;
		ret.x = (mat.m[1][2] - mat.m[2][1]) * s;
		ret.y = (mat.m[2][0] - mat.m[0][2]) * s;
		ret.z = (mat.m[0][1] - mat.m[1][0]) * s;
	}
	else {
		float max;
		max = mat.m[1][1] > mat.m[2][2] ? mat.m[1][1] : mat.m[2][2];

		if (max < mat.m[0][0]) {
			s = sqrtf(mat.m[0][0] - (mat.m[1][1] + mat.m[2][2]) + 1.0f);

			float x = s * 0.5f;
			s = 0.5f / s;
			ret.x = x;
			ret.y = (mat.m[0][1] + mat.m[1][0]) * s;
			ret.z = (mat.m[2][0] + mat.m[0][2]) * s;
			ret.w = (mat.m[1][2] - mat.m[2][1]) * s;
		}
		else
			if (max == mat.m[1][1]) {
				s = sqrtf(mat.m[1][1] - (mat.m[2][2] + mat.m[0][0]) + 1.0f);

				float y = s * 0.5f;
				s = 0.5f / s;
				ret.x = (mat.m[0][1] + mat.m[1][0]) * s;
				ret.y = y;
				ret.z = (mat.m[1][2] + mat.m[2][1]) * s;
				ret.w = (mat.m[2][0] - mat.m[0][2]) * s;
			}
			else {
				s = sqrtf(mat.m[2][2] - (mat.m[0][0] + mat.m[1][1]) + 1.0f);

				float z = s * 0.5f;
				s = 0.5f / s;
				ret.x = (mat.m[2][0] + mat.m[0][2]) * s;
				ret.y = (mat.m[1][2] + mat.m[2][1]) * s;
				ret.z = z;
				ret.w = (mat.m[0][1] - mat.m[1][0]) * s;
			}
	}


	ret.x = -ret.x; // 左手座標系用の改良
	ret.y = -ret.y; // 左手座標系用の改良
	ret.z = -ret.z; // 左手座標系用の改良
	return ret;
}
#pragma endregion
