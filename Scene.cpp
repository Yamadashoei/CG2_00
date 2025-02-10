#include "Scene.h"

int Scene::sceneNum = Title;

Scene::~Scene() {}

int Scene::GetScene() { return sceneNum; }