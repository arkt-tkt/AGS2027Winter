#include "../Application.h"
#include "../Manager/SceneManager.h"
#include "SceneBase.h"

SceneBase::SceneBase() :
    app_(Application::GetInstance()),
    scnMng_(SceneManager::GetInstance())
{
	myScene_ = nextScene_ = SCENE::NONE;
    variable_ = 0;
}

bool SceneBase::Assign(SCENE scene, int var)
{
    myScene_ = nextScene_ = scene;
    variable_ = var;

    return true;
}

SceneBase::SCENE SceneBase::GetMyScene() const
{
    return myScene_;
}

SceneBase::SCENE SceneBase::GetNextScene() const
{
    return nextScene_;
}

void SceneBase::ResetNextScene()
{
    nextScene_ = myScene_;
}
