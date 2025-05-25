#include "BaseEnemy.h"
#include "Player/Player.h"
#include "GameScene.h"

void BaseEnemy::Draw(Camera* camera) {
    if (!isDead_) {
        model_->Draw(camera, enemyTex);
    }
}

void BaseEnemy::DrawUI() {
    if (worldtransform_.translate.z - player_->GetPos().z <= kMaxAttack &&
        worldtransform_.translate.z > player_->GetPos().z)
    {
        if (!isDead_) {
            enemySprite_->Draw();
        }
    }
}

void BaseEnemy::DrawParticle(Camera* camera) {
    if (isDead_) {
        particle_->Draw(camera, bakuhatuTex);
    }
}

void BaseEnemy::OnCollision() {
    isDead_ = true;
    isLockOn_ = false;
    isPossibillityLock = false;
    SetEnemySpeed({ 0.0f, 0.0f, 0.0f });

    if (particle_) {
        emitter.transform.translate = worldtransform_.translate;
        particle_->SetEmitter(emitter);
        particle_->EmitOnce(emitter);
    }
}

void BaseEnemy::Attack() {
    if (!player_ || !gameScene_) return;

    Vector3 end = player_->GetPos();
    Vector3 start = GetPos();

    Vector3 diff = {
        end.x - start.x,
        end.y - start.y,
        end.z - start.z
    };
    diff = Normalize(diff);

    Vector3 velocity_ = diff;

    // 弾を生成して初期化
    std::unique_ptr<EnemyBullet> newBullet = std::make_unique<EnemyBullet>();
    newBullet->SetPlayer(player_);
    newBullet->Initialize(worldtransform_.translate, velocity_);

    // 弾を登録
    gameScene_->AddEnemyBullet(std::move(newBullet));
}

Vector3 BaseEnemy::GetWorldPosition() const {
    Vector3 worldPos;
    worldPos.x = worldtransform_.matWorld.m[3][0];
    worldPos.y = worldtransform_.matWorld.m[3][1];
    worldPos.z = worldtransform_.matWorld.m[3][2];
    return worldPos;
}
