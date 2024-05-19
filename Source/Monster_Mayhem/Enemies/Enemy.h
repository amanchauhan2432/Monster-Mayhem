#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "../Interfaces/BulletHitInterface.h"
#include "Enemy.generated.h"

UCLASS()
class MONSTER_MAYHEM_API AEnemy : public ACharacter, public IBulletHitInterface
{
	GENERATED_BODY()

public:
	AEnemy();

	UPROPERTY(EditAnywhere, Category = "Enemy Properties")
	class UParticleSystem* HitEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Enemy Properties")
	USoundBase* HitSound;

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void BulletHit_Implementation() override;
};
