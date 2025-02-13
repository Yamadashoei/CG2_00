#pragma once
#include "Vector3.h"

// 変換用構造体
// スケール、回転、平行移動をまとめて扱う
struct Transform {
    Vector3 scale;      // スケール（拡大・縮小）
    Vector3 rotate;     // 回転（ラジアン単位）
    Vector3 translate;  // 平行移動
};
