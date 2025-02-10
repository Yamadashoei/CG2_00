#include "GameManager.h"

GameManager::GameManager() {

	sceneArr_[Title] = std::make_unique<TitleScene>();
	sceneArr_[Game] = std::make_unique<TitleScene>();


	prevSceneNum_ = 0;
	currentSceneNum_ = Title;


}


GameManager::~GameManager() {}


int GameManager::Run() {

	// 
	char keys[256] = { 0 };
	char preKeys[256] = { 0 };

	while (true) {
		// 
		memcpy(preKeys, keys, 256);
		Input::GetHitKeyStateAll(keys);

		Input::BeginFrame();

		prevSceneNum_ = currentSceneNum_;
		currentSceneNum_ = sceneArr_[currentSceneNum_]->GetScene();

		if (prevSceneNum_ != currentSceneNum_) {
			sceneArr_[currentSceneNum_]->Initialize();
		}

		sceneArr_[currentSceneNum_]->Update(keys, preKeys);

		sceneArr_[currentSceneNum_]->Draw();

		DirectXCommon::EndFrame();



		if (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0) {
			break;
		}
	}

	return 0;
}