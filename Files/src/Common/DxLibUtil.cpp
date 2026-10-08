#include "MathUtil.h"
#include "DxLibUtil.h"

FLOAT2 DxLibUtil::Vec2ToFLT2(const Vector2& v)
{
	return FLOAT2(v.x, v.y);
}

Vector2 DxLibUtil::FLT2ToVec2(const FLOAT2& f)
{
	return Vector2(f.u, f.v);
}

VECTOR DxLibUtil::Vec3ToVEC(const Vector3& v)
{
	return VECTOR(v.x, v.y, v.z);
}

Vector3 DxLibUtil::VECToVec3(const VECTOR& v)
{
	return Vector3(v.x, v.y, v.z);
}

MATRIX DxLibUtil::Mat4x4ToMAT(const Matrix4x4& m)
{
	MATRIX ret = MGetIdent();
	for (int i = 0; i < 4; ++i) for (int j = 0; j < 4; ++j)
	{
		ret.m[i][j] = m.m[j][i];
	}
	return ret;
}

Matrix4x4 DxLibUtil::MATToMat4x4(const MATRIX& m)
{
	Matrix4x4 ret = IdentityMatrix();
	for (int i = 0; i < 4; ++i) for (int j = 0; j < 4; ++j)
	{
		ret.m[i][j] = m.m[j][i];
	}
	return ret;
}

Quaternion DxLibUtil::GetRotation(const MATRIX& m)
{
	return GetRotation(MATToMat4x4(m));
}

VECTOR DxLibUtil::VZero()
{
	return { 0.0f, 0.0f, 0.0f };
}

VECTOR DxLibUtil::VGetIdentX()
{
	return { 1.0f, 0.0f, 0.0f };
}

VECTOR DxLibUtil::VGetIdentY()
{
	return { 0.0f, 1.0f, 0.0f };
}

VECTOR DxLibUtil::VGetIdentZ()
{
	return { 0.0f, 0.0f, 1.0f };
}

VECTOR DxLibUtil::VGetIdent()
{
	return { 1.0f, 1.0f, 1.0f };
}

VECTOR DxLibUtil::VDegToRad(const VECTOR& In)
{
	float mult = DX_PI_F / 180.0f;
	return VScale(In, mult);
}

VECTOR DxLibUtil::VRadToDeg(const VECTOR& In)
{
	float mult = 180.0f / DX_PI_F;
	return VScale(In, mult);
}

VECTOR DxLibUtil::VLerp(const VECTOR& In1, const VECTOR& In2, float lerp)
{
	VECTOR v = VZero();

	v.x = MathUtil::Lerp(In1.x, In2.x, lerp);
	v.y = MathUtil::Lerp(In1.y, In2.y, lerp);
	v.z = MathUtil::Lerp(In1.z, In2.z, lerp);

	return v;
}

VECTOR DxLibUtil::VLerpRad(const VECTOR& In1, const VECTOR& In2, float lerp)
{
	VECTOR v = VZero();

	v.x = MathUtil::LerpRad(In1.x, In2.x, lerp);
	v.y = MathUtil::LerpRad(In1.y, In2.y, lerp);
	v.z = MathUtil::LerpRad(In1.z, In2.z, lerp);

	return v;
}

VECTOR DxLibUtil::VMinus(const VECTOR& In)
{
	return { -In.x, -In.y, -In.z };
}

VECTOR DxLibUtil::VInverse(const VECTOR& In)
{
	return VMinus(In);
}

bool DxLibUtil::VEquals(const VECTOR& In1, const VECTOR& In2)
{
	return In1.x == In2.x && In1.y == In2.y && In1.z == In2.z;
}

MATRIX DxLibUtil::MGetRotXYZ(const VECTOR& euler)
{
	MATRIX ret = MGetIdent();
	ret = MMult(ret, MGetRotX(euler.x));
	ret = MMult(ret, MGetRotY(euler.y));
	ret = MMult(ret, MGetRotZ(euler.z));
	return ret;
}

MATRIX DxLibUtil::Multiplication(const VECTOR& childEuler, const VECTOR& parentEuler)
{
	return MMult(MGetRotXYZ(childEuler), MGetRotXYZ(parentEuler));
}

MATRIX DxLibUtil::Multiplication(const MATRIX& child, const MATRIX& parent)
{
	return MMult(child, parent);
}

VECTOR DxLibUtil::GetScreenPosToWorldPos(const VECTOR& worldPos, int scr_x, int scr_y)
{   
	// ビュー行列と射影行列の取得
	MATRIX view = GetCameraViewMatrix();
	MATRIX proj = GetCameraProjectionMatrix();

	// ビューポート行列（スクリーン行列）の作成
	float w = static_cast<float>(scr_x) / 2.0f;
	float h = static_cast<float>(scr_y) / 2.0f;

	MATRIX viewport = {
		w , 0 , 0 , 0 ,
		0 ,-h , 0 , 0 ,
		0 , 0 , 1 , 0 ,
		w , h , 0 , 1
	};

	VECTOR screenPos, tmp = worldPos;
	// ビュー変換とプロジェクション変換
	tmp = VTransform(tmp, view);
	tmp = VTransform(tmp, proj);
	// zで割って-1~1の範囲に収める
	//tmp.x /= tmp.z; tmp.y /= tmp.z; tmp.z /= tmp.z;
	tmp = VScale(tmp, 1 / tmp.z);
	// スクリーン変換
	screenPos = VTransform(tmp, viewport);

	return screenPos;
}
