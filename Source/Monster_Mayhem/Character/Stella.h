#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "../AmmoType.h"
#include "Stella.generated.h"

UENUM(BlueprintType)
enum class ECombatType : uint8
{
	ECT_Unoccupied UMETA(DisplayName = "Unoccupied"),
	ECT_Firing UMETA(DisplayName = "Firing"),
	ECT_Reloading UMETA(DisplayName = "Reloading"),

	EAT_MAX UMETA(DisplayName = "DefaultMAX")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPlaySlotAnim, int32, LastSlotIndex);

UCLASS()
class MONSTER_MAYHEM_API AStella : public ACharacter
{
	GENERATED_BODY()

public:
	AStella();

	UPROPERTY(EditAnywhere)
	class USpringArmComponent* CameraBoom;

	UPROPERTY(EditAnywhere)
	class UCameraComponent* FollowCamera;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Properties")
	USceneComponent* HandSceneComponent;

	class AItem* LastTracedItem{ nullptr };

	UPROPERTY(EditAnywhere, Category = "Weapon")
	TSubclassOf<class AWeapon> WeaponClass;

	UPROPERTY(BlueprintReadOnly, Category = "Weapon")
	class AWeapon* EquippedWeapon;

	UPROPERTY(BlueprintReadWrite, Category = "Weapon")
	TArray<AItem*> Inventory;

	// Weapon Properties

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	class UParticleSystem* MuzzleEffect;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	class UParticleSystem* HitEffect;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	class UParticleSystem* BulletTrailEffect;

	UPROPERTY(BlueprintReadOnly, Category = "Weapon Properties")
	bool bIsAiming;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Properties")
	TMap<EAmmoType, int32> AmmoMap;

	UPROPERTY(BlueprintReadWrite, Category = "Weapon Properties")
	ECombatType CombatType = ECombatType::ECT_Unoccupied;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Properties")
	class USoundBase* HitSound;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Properties")
	UParticleSystem* BloodParticle;

	UPROPERTY(BlueprintReadOnly, Category = "Health")
	float Health = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Health")
	float MaxHealth = 100.f;

	UPROPERTY(BlueprintReadOnly, Category = "Health")
	float HealthPercent = 1.f;



	// Input
	UPROPERTY(EditAnywhere, Category = Input)
	class UInputMappingContext* StellaContext;

	UPROPERTY(EditAnywhere, Category = Input)
	class UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* FireAction;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* AimAction;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* PickupAction;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* ReloadAction;

	UPROPERTY(EditAnywhere, Category = "Input | Weapon Selection")
	UInputAction* EAction;

	UPROPERTY(EditAnywhere, Category = "Input | Weapon Selection")
	UInputAction* OneAction;

	UPROPERTY(EditAnywhere, Category = "Input | Weapon Selection")
	UInputAction* TwoAction;

	UPROPERTY(EditAnywhere, Category = "Input | Weapon Selection")
	UInputAction* ThreeAction;

	UPROPERTY(EditAnywhere, Category = "Input | Weapon Selection")
	UInputAction* FourAction;

	UPROPERTY(EditAnywhere, Category = "Input | Weapon Selection")
	UInputAction* FiveAction;

	// Montages

	UPROPERTY(EditAnywhere, Category = Montages)
	class UAnimMontage* FireMontage;

	UPROPERTY(EditAnywhere, Category = Montages)
	UAnimMontage* ReloadMontage;

	UPROPERTY(EditAnywhere, Category = Montages)
	UAnimMontage* EquipMontage;

	UPROPERTY(EditAnywhere, Category = Montages)
	UAnimMontage* HitReactMontage;

	UPROPERTY(EditAnywhere, Category = Montages)
	UAnimMontage* DeathMontage;

	UPROPERTY(BlueprintAssignable)
	FPlaySlotAnim PlaySlotAnim;

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	// Input

	void Movement(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);

	void Fire();
	void FireButtonPressed();
	void FireButtonReleased();
	FTimerHandle FireTimer;

	void Aim();

	void Pickup();
	void Drop();

	void Reload();
	void PlayReloadMontage();
	bool HasAmmo();

	UFUNCTION(BlueprintCallable) // Anim Blueprint
	void EndReload();

	void EKeyPressed();
	void OneKeyPressed();
	void TwoKeyPressed();
	void ThreeKeyPressed();
	void FourKeyPressed();
	void FiveKeyPressed();

	void SwapWeapon(int32 InSlotIndex);
	void PickupItem(AItem* ItemToPickup);

	void EquipWeapon(AWeapon* InWeapon);

	void GetStartEndForTrace(FVector& OutStart, FVector& OutEnd);
	void GetLineTraceForBullet(FVector InSocketLocation, FVector& TrailEndLocation, FHitResult& OutHitResult);
	void GetLineTraceForItem();

	FVector GetInterpTargetLocation();

	void InitializeAmmoMap();

	UFUNCTION(BlueprintCallable) // Anim Blueprint
	void Grab();

	UFUNCTION(BlueprintCallable) // Anim Blueprint
	void Release();

	UFUNCTION(BlueprintImplementableEvent)
	void ShowEndWidget();
};
