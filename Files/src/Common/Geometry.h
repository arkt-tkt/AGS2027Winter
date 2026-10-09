#pragma once
#include <map>

#pragma region 色
struct Color
{
	float r, g, b;

	inline Color() : r(0), g(0), b(0) {}
	inline Color(float r, float g, float b) : r(r), g(g), b(b) {}
	Color(unsigned int c);

	Color operator/(float f);

	Color Add(const Color& c, bool limit = true) const;

	// unsigned int型を返す
	unsigned int GetColorHex() const;
};

// 線形補間（色）
Color LerpColor(const Color& start, const Color& end, float rate);

Color ColorMapCalc(const std::map<float, Color>& map, float time);
#pragma endregion

#pragma region 2次元ベクトル
// 2次元ベクトル
struct Vector2
{
	float x, y;

	constexpr Vector2() : x(0.0f), y(0.0f) {}
	constexpr Vector2(float x, float y) : x(x), y(y) {}

	// 単項プラス演算子
	Vector2 operator+() const;
	// 単項マイナス演算子
	Vector2 operator-() const;
	// 代入
	Vector2& operator=(const float& f);
	// 代入
	Vector2& operator=(const Vector2& v);
	
	// ベクトル加算
	Vector2& operator+=(const Vector2& v);
	// ベクトル減算
	Vector2& operator-=(const Vector2& v);
	// スカラー倍
	Vector2& operator*=(const float& f);
	// スカラー割
	Vector2& operator/=(const float& f);
	
	// 長さ
	float Length() const;
	// 長さの2乗(sqrt関数なし、高速)
	float LengthSquare() const;
	// 大きさ
	float Magnitude() const;

	// 正規化
	Vector2& Normalize();
	// 正規化済みベクトル
	Vector2 Normalized() const;

	// 角度（ラジアン度）
	float Angle() const;
	// 角度（弧度）
	float AngleDegree() const;
};

// 別名
using Position2 = Vector2;

// ベクトル同士の加算
Vector2 operator+(const Vector2& va, const Vector2& vb);
// ベクトル同士の減算
Vector2 operator-(const Vector2& va, const Vector2& vb);
// スカラー倍ベクトル
Vector2 operator*(const Vector2& v, const float& f);
// スカラー割ベクトル
Vector2 operator/(const Vector2& v, const float& f);
// 内積（ドット積）
float operator*(const Vector2& va, const Vector2& vb);
// 外積（クロス積）
float operator%(const Vector2& va, const Vector2& vb);
// ベクトル同士の比較
bool operator<(const Vector2& va, const Vector2& vb);
// ベクトル同士の比較
bool operator>(const Vector2& va, const Vector2& vb);
// ベクトル同士の比較
bool operator<=(const Vector2& va, const Vector2& vb);
// ベクトル同士の比較
bool operator>=(const Vector2& va, const Vector2& vb);
// ベクトル同士の比較
bool operator==(const Vector2& va, const Vector2& vb);

// 内積（ドット積）
float Dot(const Vector2& va, const Vector2& vb);
// 外積（クロス積）
float Cross(const Vector2& va, const Vector2& vb);
// 線形補間
Vector2 Lerp(const Vector2& start, const Vector2& end, const float& rate);
// 角度からベクトルを取得
Vector2 GetVector2FromAngle(float radian, float length);
#pragma endregion

#pragma region 3次元ベクトル
// 3次元ベクトル
struct Vector3
{
	float x, y, z;

	constexpr Vector3() : x(0.0f), y(0.0f), z(0.0f) {}
	constexpr Vector3(float x, float y, float z) : x(x), y(y), z(z) {}

	// 単項プラス演算子
	Vector3 operator+() const;
	// 単項マイナス演算子
	Vector3 operator-() const;
	// 代入
	Vector3& operator=(const float& f);
	// 代入
	Vector3& operator=(const Vector3& v);

	// ベクトル加算
	Vector3& operator+=(const Vector3& v);
	// ベクトル減算
	Vector3& operator-=(const Vector3& v);
	// スカラー倍
	Vector3& operator*=(const float& f);
	// スカラー割
	Vector3& operator/=(const float& f);

	Vector2 XY() const;
	Vector2 XZ() const;
	Vector2 YZ() const;

	// 長さ
	float Length() const;
	// 長さの2乗(sqrt関数なし、高速)
	float LengthSquare() const;
	// 大きさ
	float Magnitude() const;
	// 大きさの2乗(sqrt関数なし、高速)
	float MagnitudeSquare() const;

	// 正規化
	Vector3& Normalize();
	// 正規化済みベクトル
	Vector3 Normalized() const;
};

// 別名
using Position3 = Vector3;

// ベクトル同士の加算
Vector3 operator+(const Vector3& va, const Vector3& vb);
// ベクトル同士の減算
Vector3 operator-(const Vector3& va, const Vector3& vb);
// スカラー倍ベクトル
Vector3 operator*(const Vector3& v, const float& f);
// スカラー割ベクトル
Vector3 operator/(const Vector3& v, const float& f);
// 内積（ドット積）
float operator*(const Vector3& va, const Vector3& vb);
// 外積（クロス積）
Vector3 operator%(const Vector3& va, const Vector3& vb);
// ベクトル同士の比較
bool operator<(const Vector3& va, const Vector3& vb);
// ベクトル同士の比較
bool operator>(const Vector3& va, const Vector3& vb);
// ベクトル同士の比較
bool operator<=(const Vector3& va, const Vector3& vb);
// ベクトル同士の比較
bool operator>=(const Vector3& va, const Vector3& vb);
// ベクトルの各要素と浮動小数の比較
bool operator==(const Vector3& v, const float& f);
// ベクトル同士の比較
bool operator==(const Vector3& va, const Vector3& vb);

// 内積（ドット積）
float Dot(const Vector3& va, const Vector3& vb);
// 外積（クロス積）
Vector3 Cross(const Vector3& va, const Vector3& vb);

// 線形補間
Vector3 Lerp(const Vector3& start, const Vector3& end, const float& rate);
// 線形補間（ラジアン度）
Vector3 LerpRad(const Vector3& start, const Vector3& end, const float& rate);

// 2つのベクトルのコサイン類似度
float CosSimilar(const Vector3& va, const Vector3& vb);
#pragma endregion

#pragma region 4x4行列
// 4x4行列
struct Matrix4x4
{
	float m[4][4] = {};

	// 行列の結合
	Matrix4x4& operator*=(const Matrix4x4& mat);

	Matrix4x4 Transposed();
};

// 3次元ベクトルとの計算
Vector3 operator*(const Matrix4x4& m, const Vector3& v);
// 行列の結合
Matrix4x4 operator*(const Matrix4x4& ma, const Matrix4x4& mb);
// 行列の結合（拡縮行列を無視）
Matrix4x4 operator%(const Matrix4x4& ma, const Matrix4x4& mb);
// 行列の結合（拡縮行列を無視）
Matrix4x4 MultRP(const Matrix4x4& ma, const Matrix4x4& mb);

// 単位行列
Matrix4x4 IdentityMatrix();
// 転置行列
Matrix4x4 TransposeMatrix(const Matrix4x4& m);

// 拡縮行列
Matrix4x4 ScaleMatrix(const float& x, const float& y, const float& z);
// 拡縮行列
Matrix4x4 ScaleMatrix(const Vector3& v);

// X軸中心の回転行列
Matrix4x4 RotationMatrixX(const float& angle);
// Y軸中心の回転行列
Matrix4x4 RotationMatrixY(const float& angle);
// Z軸中心の回転行列
Matrix4x4 RotationMatrixZ(const float& angle);
// XYZ順の回転行列
Matrix4x4 RotationMatrixXYZ(const float& x, const float& y, const float& z);
// XYZ順の回転行列
Matrix4x4 RotationMatrixXYZ(const Vector3& v);

// 平行移動行列
Matrix4x4 TranslationMatrix(const float& x, const float& y, const float& z);
// 平行移動行列
Matrix4x4 TranslationMatrix(const Vector3& v);

// オイラー角と無回転状態での正面方向から、現在の正面方向を取得
Vector3 GetForwardVector(const Vector3& v, const Vector3& zero);
#pragma endregion

#pragma region クォータニオン
// クォータニオン
struct Quaternion
{
	static constexpr float kEpsilonNormalSqrt = 1.0e-15f;

	double w, x, y, z;

	// デフォルトコンストラクタ
	inline Quaternion() : w(1.0), x(0.0), y(0.0), z(0.0) {};
	// 関数用コンストラクタ
	inline Quaternion(double w, double x, double y, double z) : w(w), x(x), y(y), z(z) {};

	inline Quaternion(const Vector3& v) { *this = Quaternion().Euler(v); }

	// オイラー角(要素の集合)から、Quaternionに変換
	Quaternion& Euler(double rad_x, double rad_y, double rad_z);
	// オイラー角(3次元ベクトル)から、Quaternionに変換
	Quaternion& Euler(const Vector3& rad);

	// 平方和(各要素の2乗の和)の平方根
	double Magnitude() const;
	// 平方和(各要素の2乗の和)
	double MagnitudeSquare() const;

	// Quaternionから、オイラー角(3次元ベクトル)に変換
	Vector3 ToEuler() const;

	// Quaternionから、行列に変換
	Matrix4x4 ToMatrix() const;

	// Quaternion計算用
	Vector3 XYZ() const;

	// 正規化
	Quaternion& Normalize();
	// 正規化済み
	Quaternion Normalized() const;

	// 逆クォータニオン化済み
	Quaternion Inversed() const;

	// 代入
	Quaternion& operator=(const Quaternion& q);
	// クォータニオンの直接加算
	Quaternion& operator+=(const Quaternion& q);
	// スカラー倍
	Quaternion& operator*=(float f);
	// クォータニオンの合成
	Quaternion& operator*=(const Quaternion& q);
	// スカラー割
	Quaternion& operator/=(float f);

	// クォータニオンの合成
	Quaternion Multiplication(const Quaternion& q) const;
};

// クォータニオンの直接加算
Quaternion operator+(const Quaternion& qa, const Quaternion& qb);
// スカラー倍
Quaternion operator*(const Quaternion& q, double d);
// 3次元ベクトルとの計算
Vector3 operator*(const Quaternion& q, const Vector3& v);
// クォータニオンの合成
Quaternion operator*(const Quaternion& qa, const Quaternion& qb);
// スカラー割
Quaternion operator/(const Quaternion& q, double d);

// 内積(ドット積)
double Dot(const Quaternion& qa, const Quaternion& qb);

// 指定した軸を指定した角度回転させる、クォータニオンを生成
// @param axis 軸（ゼロベクトルを元に、指定したい軸だけ 1.0f を入れる）
// @param rad ラジアン角
Quaternion AngleAxis(Vector3 axis, double rad);

// 移動方向(3次元ベクトル)から、クォータニオンに変換
Quaternion LookRotation(const Vector3& dir);
// 移動方向(3次元ベクトル)から、クォータニオンに変換
Quaternion LookRotation(Vector3 dir, Vector3 up);

// 行列から、クォータニオンに変換
Quaternion GetRotation(const Matrix4x4& mat);
#pragma endregion
