#include "EnemyBulletPool.h"

void EnemyBulletPool::Initialize(size_t poolSize)
{
    // プールを初期化
    bullets_.resize(poolSize);
    for (auto& bullet : bullets_) {
        bullet = std::make_unique<EnemyBullet>();
    }
}

void EnemyBulletPool::Update()
{
    for (auto& bullet : bullets_) {
        if (bullet->IsActive()) {
            bullet->Update();
        }
    }
}

void EnemyBulletPool::Draw(Camera* camera, uint32_t index)
{
    for (auto& bullet : bullets_) {
        if (bullet->IsActive()) {
            bullet->Draw(camera, index);
        }
    }
}
