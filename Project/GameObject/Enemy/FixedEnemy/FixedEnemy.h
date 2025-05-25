#pragma once
#include "Enemy/BaseEnemy.h"

class FixedEnemy : public BaseEnemy {
public:
    FixedEnemy();
    ~FixedEnemy() override = default;

    // 初期化処理
    void Initialize(Vector3 pos) override;
    // 更新処理
    void Update(Camera* camera) override;

    void UpdatePosition(Vector3 newPosition) {
        previousPosition_ = position_;
        position_ = newPosition;
    }
private:
    // 攻撃に関するタイミングやロジック処理
    void HandleAttackLogic();

    // プレイヤーの方向を向くように回転調整
    void UpdateRotationToPlayer();

    // 3D→2Dレティクル座標変換およびスプライト位置更新
    void UpdateReticlePosition(Camera* camera);

    // ロックオン状態に応じたスプライトテクスチャ変更
    void UpdateReticleTexture();
private:
    // 発射タイマー
    int32_t attackTimer = 10;
    static const int kFireInterval = 60;
    int rensya = 10;
    int rensyanum = 0;
    const int rensyaNumSpeed = 1;
    const int kMaxRensyaNum = 3;
};
