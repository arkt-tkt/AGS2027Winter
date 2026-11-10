#pragma once
#include "../SceneBase.h"

class PauseLayer : public SceneBase
{
public:
    bool Init() override;
    void Update() override;
    void Draw() override;
    void DrawUI() override;
    bool Release() override;

private:

};

