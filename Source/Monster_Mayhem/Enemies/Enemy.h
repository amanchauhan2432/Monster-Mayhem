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

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class USphereComponent* AgroSphere;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class USphereComponent* CombatSphere;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UBoxComponent* LeftWeaponCollision;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UBoxComponent* RightWeaponCollision;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UWidgetComponent* HealthBarComponent;

	class AEnemyController* EnemyController;

	UPROPERTY(EditAnywhere, Category = "Enemy Properties")
	class UParticleSystem* HitEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Enemy Properties")
	USoundBase* HitSound;

	UPROPERTY(BlueprintReadOnly, Category = "Health")
	float Health = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Health")
	float MaxHealth = 100.f;

	UPROPERTY(BlueprintReadOnly, Category = "Health")
	float HealthPercent = 1.f;

	UPROPERTY(BlueprintReadOnly)
	int32 DeadAnimIndex = 1;

	FTimerHandle HealthBarHandle;
	FTimerHandle DestroyHandle;

	UPROPERTY(EditDefaultsOnly)
	class UBehaviorTree* BehaviorTree;

	UPROPERTY(EditInstanceOnly, Category = "Patrolling")
	AActor* PatrolTarget1;

	UPROPERTY(EditInstanceOnly, Category = "Patrolling")
	AActor* PatrolTarget2;

	// Montages

	UPROPERTY(EditDefaultsOnly)
	class UAnimMontage* DeathMontage;

	UPROPERTY(EditDefaultsOnly)
	class UAnimMontage* HitReactMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Properties")
	float BaseDamage = 10.f;

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
	virtual void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	virtual void OnBeginCombat(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnEndCombat(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	virtual void OnBeginLeftAttack(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void OnBeginRightAttack(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	virtual void BulletHit_Implementation(FVector HitLocation) override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	void Die();
	void DestroyEnemy();

	void ShowWidget();
	void HideWidget();

	UFUNCTION(BlueprintCallable)
	void PlayMontage(UAnimMontage* InMontage, FName InName = NAME_None);

	UFUNCTION(BlueprintCallable)
	void SetIsBulletHit(bool bBulletehit);

	UFUNCTION(BlueprintCallable)
	void UpdateLeftWeaponCollision(ECollisionEnabled::Type InType);

	UFUNCTION(BlueprintCallable)
	void UpdateRightWeaponCollision(ECollisionEnabled::Type InType);
};
