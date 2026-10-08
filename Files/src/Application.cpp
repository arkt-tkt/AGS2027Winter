#include <DxLib.h>
#include <EffekseerForDXLib.h>
#include <random>
#include <stdio.h>
#include "Common/Geometry.h"
#include "Manager/AudioManager.h"
#include "Manager/FPSManager.h"
#include "Manager/InputManager.h"
#include "Manager/ResourceManager.h"
#include "Manager/SceneManager.h"
#include "Manager/ScoreManager.h"
#include "Application.h"

Application* Application::instance_ = nullptr;

const std::string Application::PATH_RESOURCE = "Data/";
const std::string Application::PATH_BGM = Application::PATH_RESOURCE + "BGM/";
const std::string Application::PATH_EFFECT = Application::PATH_RESOURCE + "Effect/";
const std::string Application::PATH_FONT = Application::PATH_RESOURCE + "Font/";
const std::string Application::PATH_IMAGE = Application::PATH_RESOURCE + "Image/";
const std::string Application::PATH_MODEL = Application::PATH_RESOURCE + "Model/";
const std::string Application::PATH_SE = Application::PATH_RESOURCE + "SE/";
const std::string Application::PATH_SHADER = Application::PATH_RESOURCE + "Shader/";
const std::string Application::PATH_TEXT = Application::PATH_RESOURCE + "Text/";

const bool Application::USE_3D_FLAG = false;

Application::Application()
{
}

bool Application::Init()
{
	if (InitSystem() == false) return false;

	preDrawScreen_ = MakeScreen(RESOLUTION_WIDTH, RESOLUTION_HEIGHT, TRUE);

	// ResourceManager
	ResourceManager::CreateInstance();
	ResourceManager::GetInstance().Init();

	// AudioManager
	AudioManager::CreateInstance();

	// FPSManager
	FPSManager::CreateInstance(GetRefreshRate());

	// InputManager
	InputManager::CreateInstance(DX_INPUT_PAD2);

	// ScoreManager
	ScoreManager::CreateInstance(DX_INPUT_PAD2);
	ScoreManager::GetInstance().Init();

	// SceneManager
	SceneManager::CreateInstance();
	SceneManager::GetInstance().Init();

	// 乱数生成処理の初期化
	// 非決定的な乱数
	std::random_device rd;
	// 乱数rdをシード値とした、擬似乱数生成（メルセンヌ・ツイスター）
	std::mt19937 mt(rd());
	// 一様分布生成器（今回はint型を指定）
	std::uniform_int_distribution<int> randDist(0, 9999);
	// 乱数をSRand関数に渡す
	SRand(randDist(mt));

	return true;
}

void Application::GameLoop()
{
	while (!ProcessMessage() && !exit_)
	{
		// 更新処理
		Update();

		// 描画処理
		Draw();
		
		// 待機チェック
		FPSManager::GetInstance().CheckWait();
	}
}

bool Application::Release()
{
	DeleteGraph(preDrawScreen_);

	SceneManager::GetInstance().Release();
	SceneManager::DeleteInstance();

	AudioManager::DeleteInstance();

	FPSManager::GetInstance().Release();
	FPSManager::DeleteInstance();

	InputManager::GetInstance().Release();
	InputManager::DeleteInstance();

	ResourceManager::GetInstance().Release();
	ResourceManager::DeleteInstance();

	ScoreManager::GetInstance().Release();
	ScoreManager::DeleteInstance();

#ifdef APPLICATION_USE_EFFEKSEER
	Effkseer_End();
#endif

	// DxLib の解放
	DxLib_End();

	return true;
}

void Application::Exit()
{
	exit_ = true;
}

int Application::GetPreDrawScreen() const
{
	return preDrawScreen_;
}

bool Application::InitSystem()
{
	// ウインドウの名称
	(SetWindowText)("DOUBLE FORCE CRISIS");

	// 画面設定
	int w = GetSystemMetrics(SM_CXSCREEN), h = GetSystemMetrics(SM_CYSCREEN);
	SetGraphMode(w, h, 32);

	// ウィンドウモード
	ChangeWindowMode(TRUE);

	// ボーダーレス化
	SetWindowStyleMode(1);

	// 文字コード指定
	//SetUseCharCodeFormat(DX_CHARCODEFORMAT_UTF8);

	// DirectX のバージョン
	SetUseDirect3DVersion(DX_DIRECT3D_11);

	// DxLib の初期化
	if (DxLib_Init() == -1) return false;

	// 3D 描画機能の有効化
	if (USE_3D_FLAG)
	{
		// ※SetUseLighting関数以外の設定はSetDrawScreen関数の呼び出し時にリセットされるため、
		// Cameraオブジェクト内で毎フレーム再設定するように変更

		// ライトの設定
		SetUseLighting(TRUE);
	}

#ifdef APPLICATION_USE_EFFEKSEER
	// Effekseerの初期化
	InitEffekseer();
#endif

	return true;
}

#ifdef APPLICATION_USE_EFFEKSEER
bool Application::InitEffekseer()
{
	SetChangeScreenModeGraphicsSystemResetFlag(FALSE);

	Effekseer_Init(8000);

	Effekseer_InitDistortion();

	Effekseer_SetGraphicsDeviceLostCallbackFunctions();

	if (USE_3D_FLAG)
	{
		Effekseer_Sync3DSetting();
	}
	else
	{
		int w, h;
		GetWindowSize(&w, &h);
		Effekseer_Set2DSetting(w, h);
	}

	return true;
}
#endif

void Application::Update()
{
	InputManager::GetInstance().Update();

	FPSManager::GetInstance().Update(
		InputManager::GetInstance().CheckDownMap(0, InputManager::TAGS::DEBUG));

	SceneManager::GetInstance().Update();

	ScoreManager::GetInstance().Update();
}

void Application::Draw()
{
	// 描画先の画面をクリア
	ClearDrawScreen();

	SceneManager::GetInstance().Draw();

	FPSManager::GetInstance().Draw();

#ifdef _DEBUG
	Vector2 center = { RESOLUTION_WIDTH / 2.0f, RESOLUTION_HEIGHT / 2.0f };
	DrawCircleAA(center.x, center.y, 3.0f, 12, 0x00FFFFU, true);
#endif

	SetDrawScreen(DX_SCREEN_BACK);

	ClearDrawScreen();

	int x, y;
	GetWindowSize(&x, &y);
	double ext = (std::min)(
		static_cast<double>(x) / RESOLUTION_WIDTH,
		static_cast<double>(y) / RESOLUTION_HEIGHT);
	DrawRotaGraph(x / 2, y / 2, ext, 0.0, preDrawScreen_, FALSE);

	// 裏画面を表画面に転写
	ScreenFlip();

	SetDrawScreen(preDrawScreen_);
}
