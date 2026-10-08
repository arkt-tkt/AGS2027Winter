#pragma once
#include <chrono>
#include <list>

class FPSManager final {
public:
	// シングルトン
	static void CreateInstance(unsigned int fps = STANDARD_FPS) { if (instance_ == nullptr) instance_ = new FPSManager(fps); }
	static FPSManager& GetInstance() { return *instance_; }
	static void DeleteInstance() { if (instance_ != nullptr) delete instance_; instance_ = nullptr; }

private:
	// インスタンス
	static FPSManager* instance_;

	// シングルトン
	FPSManager(const FPSManager&) = delete;
	FPSManager& operator=(const FPSManager&) = delete;
	FPSManager(FPSManager&&) = delete;
	FPSManager& operator=(FPSManager&&) = delete;

public:
	// 更新
	void Update(bool show_key);

	// 描画
	void Draw() const;

	// 待機
	void CheckWait();

	// 解放
	bool Release();

	// デルタタイム取得（実際にFPSから算出）
	template<typename T>
	T GetDeltaTime() const;

	// 描画用フォントを設定
	void SetDrawFont(int handle);

private:
	// 標準目標FPS
	static constexpr unsigned int STANDARD_FPS = 60U;
	// 最大目標FPS
	static constexpr unsigned int MAX_FPS = 300U;

	// 目標FPS
	unsigned int targetFPS_;
	// フレーム毎の理想時間
	double idealFrameSecond_;

	// タイマー記録用リスト
	std::list<double> timeList_;
	// 直前フレームの時間
	std::chrono::high_resolution_clock::time_point prevTime_;

	// FPS表示可視化フラグ
	bool showFlag_;
	// 平均FPS
	float averageFPS_;
	// 最低FPS
	float minFPS_;
	// 最高FPS
	float maxFPS_;

	// 描画用フォントハンドルID
	int fontHandle_;

	// コンストラクタ
	FPSManager(unsigned int fps);
	// デストラクタ
	~FPSManager();

	// 初期化
	void Initialize(unsigned int fps);

	// 時間記録処理
	void RegisterTime(const double delta_time);

};
