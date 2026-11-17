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

unsigned int DxLibUtil::LerpColor(const unsigned int& a, const unsigned int& b, float t)
{
	unsigned int red, gre, blu;

	red = std::lerp((a & 0xff0000u) / 0x10000u, (b & 0xff0000u) / 0x10000u, t);
	gre = std::lerp((a & 0xff00u) / 0x100u, (b & 0xff00u) / 0x100u, t);
	blu = std::lerp(a & 0xffu, b & 0xffu, t);
	
	return (red * 0x10000u) + (gre * 0x100u) + blu;
}

unsigned int DxLibUtil::LerpColorMap(const std::map<float, unsigned int>& map, float rate)
{
	std::pair<float, unsigned int> lPair(-1.0f, 0x0u), rPair(-1.0f, 0x0u);

	for (auto it = map.begin(); it != map.end(); )
	{
		if ((*it).first <= rate)
		{
			lPair = (*it);
			++it;
		}
		else if ((*it).first > lPair.first)
		{
			rPair = (*it);
			break;
		}
	}

	// lPair が無い場合
	if (lPair.first == -1.0f)
	{
		// map 内の要素数がゼロである可能性が濃いため、確認する事
		return 0xff00ffu;
	}

	// rPair が無い、または何故か lPair と同じ場合
	if (rPair.first == -1.0f || lPair.first == rPair.first)
	{
		// 前者は正常動作でも起こり得るため、中身が確実に存在する lPair.second を返す
		return lPair.second;
	}

	// ここから LerpColor 関数に渡すための変換処理
	rPair.first -= lPair.first;
	float dRate = (rate - lPair.first) / rPair.first;

	// 戻り値をそのまま返す
	return LerpColor(lPair.second, rPair.second, dRate);
}

COLOR_F DxLibUtil::UInt2ColorF(const unsigned int& color_code)
{
	auto red = (color_code & 0xff0000u) / 0x10000u;
	auto green = (color_code & 0xff00u) / 0x100u;
	auto blue = color_code & 0xffu;
	return COLOR_F(float(red) / 255.0f, float(green) / 255.0f, float(blue) / 255.0f, 1.0f);
}

void DxLibUtil::DrawStringToCenterPos(int x, int y, const char* string, unsigned int color)
{
	int handleId = GetDefaultFontHandle();

	DrawString(x - MathUtil::Round(GetFontSizeToHandle(handleId) * GetStringLength(string) * 0.5f), y, string, color);
}
