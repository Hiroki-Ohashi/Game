#pragma once
#include <vector>
#include <memory>
#include "EnemyBullet.h"
#include <Camera.h>

class EnemyBulletPool {
public:
	void Initialize(size_t poolSize);
	void Update();
	void Draw(Camera* camera, uint32_t index);

    // 使用可能な弾を取得
    EnemyBullet* GetBullet() {
        for (auto& bullet : bullets_) {
            if (!bullet->IsActive()) { // 非アクティブな弾を探す
                return bullet.get();
            }
        }
        return nullptr; // 使用可能な弾がない場合はnullptrを返す
    }
private:
	std::vector<std::unique_ptr<EnemyBullet>> bullets_;
};
