#pragma once
#include <cstdint>

// プレイヤー陣営
const uint32_t kcollisionAttributePlayer = 0b1;
// 敵陣営
const uint32_t kcollisionAttributeEnemy = 0b1 << 1;
// オブジェクト
const uint32_t kcollisionAttributeObject = 0b1 << 2;

