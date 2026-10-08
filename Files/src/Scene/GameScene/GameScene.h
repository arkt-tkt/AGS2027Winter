#include <memory>
#include "../SceneBase.h"
#include "../../Object/Stage.h"

class Player;
class Stage;

class GameScene : public SceneBase {
public:
    bool Init() override;
    void Update() override;
    void Draw() override;
    void DrawUI() override;
    bool Release() override;

private:
    static constexpr float LOSS_STABILITY_PER_SECOND = -0.8f;
    static constexpr float GAIN_STABILITY_PER_SECOND = 0.2f;
    static constexpr float MAX_STABILITY = 100.0f;

    std::shared_ptr<Player> player_;
    std::unique_ptr<Stage> stage_;

    float stability_ = MAX_STABILITY;
    int activeDevice_ = 0;

};