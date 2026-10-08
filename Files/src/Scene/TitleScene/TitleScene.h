#include "../SceneBase.h"

class TitleScene : public SceneBase {
public:
    bool Init() override;
    void Update() override;
    void Draw() override;
    void DrawUI() override;
    bool Release() override;

private:

};