#pragma once
#include <DxLib.h>
#include "Geometry.h"

namespace DxLibUtil
{
	// Geometry → DxLib変換
	FLOAT2 Vec2ToFLT2(const Vector2& v);
	// DxLib → Geometry変換
	Vector2 FLT2ToVec2(const FLOAT2& f);

	// Geometry → DxLib変換
	VECTOR Vec3ToVEC(const Vector3& v);
	// DxLib → Geometry変換
	Vector3 VECToVec3(const VECTOR& v);

	// Geometry → DxLib変換(転置含む)
	MATRIX Mat4x4ToMAT(const Matrix4x4& m);
	// DxLib → Geometry変換(転置含む)
	Matrix4x4 MATToMat4x4(const MATRIX& m);

	// GetRotation関数のオーバーロード
	Quaternion GetRotation(const MATRIX& m);
	// 暗黙でないゼロベクトル
	VECTOR VZero();
	// X方向のみの単位ベクトル
	VECTOR VGetIdentX();
	// Y方向のみの単位ベクトル
	VECTOR VGetIdentY();
	// Z方向のみの単位ベクトル
	VECTOR VGetIdentZ();
	// 単位ベクトル
	VECTOR VGetIdent();

	// 弧度からラジアン度に変換
	VECTOR VDegToRad(const VECTOR& In);
	// ラジアン度から弧度に変換
	VECTOR VRadToDeg(const VECTOR& In);

	// 線形補間
	VECTOR VLerp(const VECTOR& In1, const VECTOR& In2, float lerp);
	// 線形補間(ラジアン)
	VECTOR VLerpRad(const VECTOR& In1, const VECTOR& In2, float lerp);

	// VECTOR 型の単項マイナス演算
	VECTOR VMinus(const VECTOR& In);
	// VECTOR 型の単項マイナス演算
	VECTOR VInverse(const VECTOR& In);

	// VECTOR 型の等価演算
	bool VEquals(const VECTOR& In1, const VECTOR& In2);

	// オイラー角からXYZ順の回転行列を取得
	MATRIX MGetRotXYZ(const VECTOR& euler);

	// 親子のオイラー角を合成する
	MATRIX Multiplication(const VECTOR& childEuler, const VECTOR& parentEuler);
	// 親子の回転行列を合成する
	MATRIX Multiplication(const MATRIX& child, const MATRIX& parent);

	VECTOR GetScreenPosToWorldPos(const VECTOR& worldPos, int scr_x, int scr_y);

	void DrawStringToCenterPos(int center_x, int y, const char* string, unsigned int color);
};

using namespace DxLibUtil;
