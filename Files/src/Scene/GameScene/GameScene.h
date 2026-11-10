#include <memory>
#include "../SceneBase.h"
#include "../../Object/Actor/Stage/Stage.h"

class Player;

class GameScene : public SceneBase
{
public:
    bool Init() override;
    void Update() override;
    void LateUpdate() override;
    void Draw() override;
    void DrawUI() override;
    bool Release() override;

private:
    static constexpr float LOSS_STABILITY_PER_SECOND = -1.0f;
    static constexpr float GAIN_STABILITY_PER_SECOND = -LOSS_STABILITY_PER_SECOND / 3.3f;
    static constexpr float MAX_STABILITY = 100.0f;

    static constexpr float SURVIVAL_TIME = 240.0f;

    std::shared_ptr<Player> player_;
    std::unique_ptr<Stage> stage_;

    float stability_ = MAX_STABILITY;
    int activeDevice_ = 0;

    float surviveTime_ = 0.0f;

};