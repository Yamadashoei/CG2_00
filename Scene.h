#pragma once

enum Snece {
	Title,
	Game
};

class Scene {
public:
	virtual void Initialize() = 0;
	virtual void Update(char* keys, char* preKeys) = 0;
	virtual void Draw() = 0;

	virtual ~Scene();

	int GetScene();

protected:
	static int sceneNum;

};