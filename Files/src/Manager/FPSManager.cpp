#include <cmath>
#include <Dxlib.h>
#include <string>
#include "FPSManager.h"

FPSManager* FPSManager::instance_ = nullptr;

FPSManager::FPSManager(unsigned int fps)
    :
    showFlag_(false),
    fontHandle_(-1)
{
    Initialize(fps);

    // 垂直同期を無効化
    SetWaitVSyncFlag(false);
}

FPSManager::~FPSManager() {}

void FPSManager::Update(bool show_key)
{
    if (show_key) showFlag_ = !(showFlag_);
}

void FPSManager::Draw() const
{
    if (showFlag_)
        DrawFormatString(5, 5, 0xFFFFFFU,
            "AVG FPS: %.2f\nMIN FPS: %.2f\nMAX FPS: %.2f",
            averageFPS_, minFPS_, maxFPS_);
}

void FPSManager::CheckWait()
{
    // 現在時間
    auto nowTime = std::chrono::high_resolution_clock::now();

    // 前回からの経過時間
    std::chrono::duration<double> delta = nowTime - prevTime_;

    // 経過時間(秒)
    double deltaTime = delta.count();

    // 経過時間が理想時間よりも短ければ待機
    if (deltaTime < idealFrameSecond_)
    {
        // 待つべき時間(ミリ秒)
        double waitMiliSecond = (idealFrameSecond_ - deltaTime) * 1000.0;

        // Sleepで待ち時間分を待機
        if (waitMiliSecond >= 1.0)
        {
            // 指定ミリ秒数待つ(DxLib関数)
            WaitTimer(static_cast<int>(waitMiliSecond));
        }

        // 指定時間になるまでbusyになるが待つ
        while (deltaTime < idealFrameSecond_)
        {
            // 再計測
            nowTime = std::chrono::high_resolution_clock::now();
            delta = nowTime - prevTime_;
            deltaTime = delta.count();
        }
    }

    // 前回時間を更新
    prevTime_ = nowTime;
    // 経過時間を記録
    RegisterTime(deltaTime);

    // FPS計測(指定された最新フレーム数分の平均）
    {
        // 合計時間
        double total = 0.0, min = DBL_MIN, max = DBL_MAX;
        for (double time : timeList_)
        {
            total += time;
            min = min >= time ? min : time;
            max = max <= time ? max : time;
        }

        // 平均FPS
        averageFPS_ = static_cast<float>(timeList_.size() / total);
        // 最低FPS
        minFPS_ = static_cast<float>(1.0 / min);
        // 最高FPS
        maxFPS_ = static_cast<float>(1.0 / max);
    }
}

bool FPSManager::Release()
{
    timeList_.clear();

    return true;
}

template<typename T>
const T FPSManager::GetDeltaTime() const
{
    static_assert(std::is_floating_point<T>::value);

    if (timeList_.empty()) return T(0.0);

    // 可変フレームレート制御用のデルタタイム
    if (timeList_.back() < idealFrameSecond_ * DELTA_TIME_DELAY_MULT_MAX)
    {
        // IDEAL_FRAME_SECONDの2倍までは、可変フレームレートで対応する
        return static_cast<T>(timeList_.back());
    }
    // それ以上の場合、固定でIDEAL_FRAME_SECONDの2倍を返す（処理落ちする）
    // ※瞬間移動や判定抜けを防止するため
    return static_cast<T>(idealFrameSecond_ * DELTA_TIME_DELAY_MULT_MAX);
}
template const float FPSManager::GetDeltaTime() const;
template const double FPSManager::GetDeltaTime() const;
template const long double FPSManager::GetDeltaTime() const;

void FPSManager::SetDrawFont(int handle)
{
    fontHandle_ = handle;
}

void FPSManager::Initialize(unsigned int fps)
{
    targetFPS_ = fps < FPS_MAX ? fps : FPS_MAX;
    idealFrameSecond_ = 1.0 / targetFPS_;
    timeList_.clear();
    prevTime_ = {};

    averageFPS_ = minFPS_ = maxFPS_ = 0.0F;
}

void FPSManager::RegisterTime(const double delta_time)
{
    timeList_.emplace_back(delta_time);

    while (timeList_.size() > targetFPS_)
        timeList_.erase(timeList_.begin());
}
