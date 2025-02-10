#include "Title.h"
#include "DirectXCommon.h"
#include "Input.h"

void TitleScene::Initialize() {}

void TitleScene::Update(char* keys, char* preKeys) {
	if (preKeys[DIK_SPACE] == 0 && keys[DIK_SPACE]) {
		sceneNum = Game;
	}

}

void TitleScene::Draw() {
	//OutputDebugStringA(640, 350, "Title");
}