#pragma once
#include <Camera.h>
#include <TextureManager.h>
#include <WorldTransform.h>
#include <Model.h>
#include "EnemyBullet.h"
#include "Collider.h"
#include "Particle.h"
#include "Sprite.h"
#include "CollisionConfig.h"

using namespace Engine;

class Player;
class GameScene;

class BaseEnemy : public Collider {
public:
    virtual ~BaseEnemy() = default;

    // 初期化処理
    virtual void Initialize(Vector3 pos) = 0;
    // 更新処理
    virtual void Update(Camera* camera) = 0;
    // 描画処理
    virtual void Draw(Camera* camera);
    virtual void DrawUI();
    virtual void DrawParticle(Camera* camera);
    // 当たり判定処理
    virtual void OnCollision() override;

    // Getter
    Vector3 GetPos() const { return position_; }
    Vector3 GetPrePos() const { return previousPosition_; }
    Vector3 GetScreenPos() { return positionReticle; }
    bool GetIsLockOn() const { return isLockOn_; }
    Vector3 GetWorldPosition() const override;

    // Setter
    void SetPlayer(Player* player) { player_ = player; }
    void SetGameScene(GameScene* scene) { gameScene_ = scene; }
    void SetEnemySpeed(Vector3 speed) { enemySpeed = speed; }
    void SetIsDead(bool isDead) { isDead_ = isDead; }
    bool IsDead() const { return isDead_; }
    void SetPosition(Vector3 pos) {
        worldtransform_.translate = pos;
        posParam = pos;
    }
    void SetRotation(Vector3 rotate) { worldtransform_.rotate = rotate; }
    void SetScale(Vector3 scale) { worldtransform_.scale = scale; }
    void SetisLockOn(bool isLockOn) { isLockOn_ = isLockOn; }

protected:
    // モデル・位置情報
    std::unique_ptr<Model> model_;
    WorldTransform worldtransform_;
    EulerTransform transform_;
    const float kMaxAttack = 2500.0f;
    Vector3 position_;
    Vector3 previousPosition_;

    // ロックオン用スプライト
    std::unique_ptr<Sprite> enemySprite_;
    Vector3 positionReticle;

    // パーティクル
    std::unique_ptr<Particles> particle_;
    Emitter emitter;

    // テクスチャ
    uint32_t lockTex = 0;
    uint32_t lockOnTex = 0;
    uint32_t enemyTex = 0;
    uint32_t enemyBulletTex = 0;
    uint32_t bakuhatuTex = 0;

    // 状態フラグ
    bool isDead_ = false;
    bool isLockOn_ = false;
    bool isPossibillityLock = false;

    // 攻撃処理
    int attackTimer = 0;
    int rensya = 0;
    int rensyanum = 0;

    // 移動
    Vector3 enemySpeed{0.0f,0.0f,20.0f};
    float speedY = 0.0f;
    Vector3 posParam{};

    // 外部参照
    Player* player_ = nullptr;
    GameScene* gameScene_ = nullptr;

    // シングルトン呼び出し
    TextureManager* textureManager_ = TextureManager::GetInstance();

    // 共通攻撃処理
    void Attack();
};
