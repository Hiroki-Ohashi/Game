#pragma once
#include "Enemy/BaseEnemy.h"

class FryEnemy : public BaseEnemy {
public:
    FryEnemy();
    ~FryEnemy() override = default;

    // 初期化処理
    void Initialize(Vector3 pos) override;
    // 更新処理
    void Update(Camera* camera) override;

    void UpdatePosition(Vector3 newPosition) {
        previousPosition_ = position_;
        position_ = newPosition;
    }
private:
    // 攻撃タイマー管理と攻撃判定処理
    void HandleAttack();

    // プレイヤーに近づいたときの移動処理
    void HandleMovement();

    // プレイヤーに向けて回転を行う
    void UpdateOrientationToPlayer();

    // 3Dワールド座標のレティクル位置をスクリーン座標に変換し、スプライトに反映
    void UpdateReticle(Camera* camera);

    // ロックオン状態に応じたテクスチャ切り替え
    void UpdateLockOnTexture();
private:
    // 発射タイマー
    int32_t attackTimer = 10;
    static const int kFireInterval = 60;
};
