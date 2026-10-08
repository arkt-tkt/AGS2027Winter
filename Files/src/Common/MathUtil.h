#pragma once
#include <algorithm>
#include <cmath>
#include <concepts>
#include <map>
#include <numbers>
#include <type_traits>

namespace MathUtil
{
	// π
	template <typename T>
	constexpr T PI_V = static_cast<T>(3.141592653589793);

	constexpr auto PI = PI_V<double>;
	constexpr auto PI_F = PI_V<float>;
	constexpr auto PI_L = PI_V<long double>;

	// 関数メモ　cpprefjp.github.ioより引用
	// ライブラリ上で既に実装されている実用的な関数をリストアップしました
	// MathUtilに関数が無い場合、代わりにこちらを使用してください
	//
	// std::modf(f, *i)
	// 浮動小数点数を、整数部と小数部に分解する。引数 f の整数部を i に書き込む。
	// ※整数部は0の方向に丸めた値になる。
	//
	// std::ceil(x)
	// 引数 x 以上で最小の整数値を得る。
	//
	// std::floor(x)
	// 引数 x 以下で最大の整数値を得る。
	//
	// std::clamp(v, low, high) ※C++17で追加
	// 値を範囲内に収める。この関数は、 v の値を範囲 [low, high] に収める。
	//
	// std::lerp(a, b, t) ※C++20で追加
	// 二点 a と b の間を、時間 t で線形補間 (linear interpolate) する。

	// 以下の2つの関数定義文は同じ動作をする、下の定義文はC++20以降でのみ動作する
	//template<std::floating_point _Ty> _Ty Func(_Ty vars);
	//auto Func(_Ty auto vars);

	// 値の範囲収め (C++17未満のバージョンでも実行可能)
	template<typename T>
	T Clamp(T v, T low, T high)
	{
		static_assert(std::is_arithmetic<T>::value);
		
		if (low > high) return v;

		return (std::min)((std::max)(v, low), high);
	}

	// 線形保管 (C++20未満のバージョンでも実行可能)
	template<typename T, typename fp>
	T Lerp(T a, T b, fp t)
	{
		static_assert(std::is_arithmetic<T>::value);
		static_assert(std::is_floating_point<fp>::value);
		
		return a + (b - a) * t;
	}

	// 弧度を0～360に収める
	template<typename T>
	T DegIn360(T degree)
	{
		static_assert(std::is_arithmetic<T>::value);

		auto deg360 = T(360);

		while (degree >= deg360)
			degree -= deg360;

		while (degree < 0.0f)
			degree += deg360;

		return degree;
	}

	// ラジアン度を0～2πに収める
	template<typename T>
	T RadIn2PI(T radian)
	{
		static_assert(std::is_floating_point<T>::value);

		auto rad2pi = PI_V<T> * T(2.0f);

		while (radian >= rad2pi)
			radian -= rad2pi;

		while (radian < 0.0f)
			radian += rad2pi;

		return radian;
	}

	// 弧度からラジアン度に変換
	template<typename T>
	T DegToRad(T degree)
	{
		static_assert(std::is_floating_point<T>::value);

		return degree * PI_V<T> / T(180.0f);
	}

	// ラジアン度から弧度に変換
	template<typename T>
	T RadToDeg(T radian)
	{
		static_assert(std::is_floating_point<T>::value);

		return radian / PI_V<T> * T(180.0f);
	}

	// 少数部を四捨五入
	template<typename T>
	int Round(T n)
	{
		static_assert(std::is_floating_point<T>::value);

		auto i = int(n);
		return n - T(i) >= T(0.5f) ? i + 1 : i;
	}
	// 少数部を切り捨て
	template<typename T>
	int RoundDown(T n)
	{
		static_assert(std::is_floating_point<T>::value);

		return std::floor(n);
	}
	// 少数部を切り上げ
	template<typename T>
	int RoundUp(T n)
	{
		static_assert(std::is_floating_point<T>::value);

		return std::ceil(n);
	}

	// 少数部のみを返す (符号付き)
	template<typename T>
	T Decimal(T n)
	{
		static_assert(std::is_floating_point<T>::value);

		return std::modf(n);
	}

	// 少数部のみを返す (絶対値)
	template<typename T>
	T Fraction(T n)
	{
		static_assert(std::is_floating_point<T>::value);

		return n - std::floor(n);
	}

	// 絶対値の小さい方の値を返す
	template<typename T>
	T AbsMin(T x, T y)
	{
		static_assert(std::is_arithmetic<T>::value);

		return std::abs(x) < std::abs(y) ? x : y;
	}

	// 絶対値の大きい方の値を返す
	template<typename T>
	T AbsMax(T x, T y)
	{
		static_assert(std::is_arithmetic<T>::value);

		return std::abs(x) > std::abs(y) ? x : y;
	}

	// 値 v が範囲 [low, high] の内にあるかどうかを返す
	template<typename T>
	bool Within(T v, T low, T high)
	{
		static_assert(std::is_arithmetic<T>::value);

		return low <= high && low <= v && v <= high;
	}
	// 値 v が範囲 [low, high] の内にあるかどうかを返す
	// ポインタ ptr には範囲内にクランプした値 v を入れる
	template<typename T>
	bool Within(T v, T low, T high, T* ptr)
	{
		static_assert(std::is_arithmetic<T>::value);

		bool ret = Within(v, low, high);
		
		if (ptr != nullptr)
		{
			(*ptr) = std::clamp(v, low, high);
		}

		return ret;
	}

	// 角度差を求める (ラジアン度)
	template<typename T>
	T AngleDifference(T from, T to)
	{
		static_assert(std::is_floating_point<T>::value);

		return std::atan2(std::sin(to - from), std::cos(to - from));
	}

	// 2つのラジアン度の最短回転方向を求める
	template<typename T>
	T NearAroundDirection(T from, T to)
	{
		static_assert(std::is_floating_point<T>::value);

		// 戻り値
		T ret = 0.0f;

		// ラジアンを調整
		from = RadIn2PI(from);
		to = RadIn2PI(to);
		
		// 差を求める
		T diff = to - from;
		
		if (diff >= 0.0f)
		{
			if (diff > PI_V<T>) ret = -1.0f;
			else ret = 1.0f;
		}
		else
		{
			if (diff < PI_V<T>) ret = 1.0f;
			else ret = -1.0f;
		}

		return ret;
	}

	// 線形補間 (ラジアン度)
	template<typename T, typename fp>
	T LerpRad(T start, T end, fp rate)
	{
		static_assert(std::is_floating_point<T>::value);
		static_assert(std::is_floating_point<fp>::value);

		// 角度差を求める
		T diff = AngleDifference(start, end);

		// 線形補間
		T ret = start + diff * rate;

		// 0～2πに収めて返す
		return RadIn2PI(ret);
	}

	// 色線形補間
	unsigned int LerpColor(const unsigned int& a, const unsigned int& b, float t);

	// map を用いた色線形補完
	// map には少なくとも[0.0]と[1.0]に対応する要素が入っているべきである
	unsigned int LerpColorMap(const std::map<float, unsigned int>& map, float rate);

};
