#pragma once
#include <memory>
#include "Scene.h" 
#include "Title.h"

#include "DirectXCommon.h"
#include "Input.h"

class GameManager {
public:

	GameManager();
	~GameManager();

	int Run();

private:
	std::unique_ptr<Scene> sceneArr_[2];

	int currentSceneNum_;
	int prevSceneNum_;

};