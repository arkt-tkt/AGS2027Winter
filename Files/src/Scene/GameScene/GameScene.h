#include <memory>
#include "../SceneBase.h"
#include "../../Shader/PostEffect.h"
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

    bool IsGodMode() const { return godMode_; }

private:
    static constexpr float GAUSSIAN_SIGMA_RATIO = 2.57f;
    static constexpr float GAUSSIAN_STEPS = 4.0f;

    static constexpr float SURVIVAL_TIME = 240.0f;

    std::shared_ptr<Player> player_;
    std::unique_ptr<Stage> stage_;

    std::unique_ptr<PostEffect> gaussian_;
    std::unique_ptr<PostEffect> screenEffect_;

    int effectScreen1Id_;
    int effectScreen2Id_;
    float sigma_ = 0.0f;

    float surviveTime_ = 0.0f;

    bool godMode_ = false;

};