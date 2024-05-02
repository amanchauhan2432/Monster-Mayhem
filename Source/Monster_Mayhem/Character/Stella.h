#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Stella.generated.h"

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


	// Weapon Properties

	UPROPERTY(EditAnywhere, Category = Weapon)
	TSubclassOf<AActor> WeaponClass;

	UPROPERTY(EditAnywhere, Category = "Weapon")
	class UParticleSystem* MuzzleEffect;

	UPROPERTY(EditAnywhere, Category = "Weapon")
	class UParticleSystem* HitEffect;

	UPROPERTY(EditAnywhere, Category = "Weapon")
	class UParticleSystem* BulletTrailEffect;

	UPROPERTY(EditAnywhere, Category = "Weapon")
	class USoundBase* MuzzleSound;

	// Montages

	UPROPERTY(EditAnywhere, Category = Montages)
	class UAnimMontage* FireMontage;

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Input

	void Movement(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void Fire(const FInputActionValue& Value);


	void GetLineTrace(FVector InSocketLocation, FVector& TrailEndLocation);
};
