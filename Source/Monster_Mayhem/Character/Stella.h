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

	UPROPERTY(BlueprintReadOnly, Category = "Weapon")
	class AWeapon* EquippedWeapon;

	UPROPERTY(BlueprintReadWrite, Category = "Weapon")
	TArray<AItem*> Inventory;


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

	// Weapon Properties

	UPROPERTY(EditAnywhere, Category = Weapon)
	TSubclassOf<class AWeapon> WeaponClass;

	UPROPERTY(EditAnywhere, Category = "Weapon")
	class UParticleSystem* MuzzleEffect;

	UPROPERTY(EditAnywhere, Category = "Weapon")
	class UParticleSystem* HitEffect;

	UPROPERTY(EditAnywhere, Category = "Weapon")
	class UParticleSystem* BulletTrailEffect;

	UPROPERTY(BlueprintReadOnly, Category = "Weapon")
	bool bIsAiming;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TMap<EAmmoType, int32> AmmoMap;

	UPROPERTY(BlueprintReadWrite, Category = "Weapon")
	ECombatType CombatType = ECombatType::ECT_Unoccupied;

	// Montages

	UPROPERTY(EditAnywhere, Category = Montages)
	class UAnimMontage* FireMontage;

	UPROPERTY(EditAnywhere, Category = Montages)
	UAnimMontage* ReloadMontage;

	UPROPERTY(EditAnywhere, Category = Montages)
	UAnimMontage* EquipMontage;

	UPROPERTY(BlueprintAssignable)
	FPlaySlotAnim PlaySlotAnim;

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

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
	void GetLineTraceForBullet(FVector InSocketLocation, FVector& TrailEndLocation);
	void GetLineTraceForItem();

	FORCEINLINE FVector GetInterpTargetLocation();

	void InitializeAmmoMap();

	UFUNCTION(BlueprintCallable) // Anim Blueprint
	void Grab();

	UFUNCTION(BlueprintCallable) // Anim Blueprint
	void Release();
};
