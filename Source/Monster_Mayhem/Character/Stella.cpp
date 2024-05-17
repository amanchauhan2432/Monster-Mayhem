#include "Stella.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "EnhancedInputComponent.h"

#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundBase.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/SkeletalMeshSocket.h"

#include "../Items/Item.h"
#include "../Items/Weapons/Weapon.h"
#include "../Items/Ammo/Ammo.h"
#include "Components/WidgetComponent.h"

AStella::AStella()
{
	PrimaryActorTick.bCanEverTick = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("Camera Boom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->SocketOffset = FVector(0.f, 50.f, 50.f);

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Follow Camera"));
	FollowCamera->SetupAttachment(CameraBoom);

	HandSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Hand Scene Component"));

	GetCharacterMovement()->JumpZVelocity = 500.f;
}

void AStella::BeginPlay()
{
	Super::BeginPlay();
	
	InitializeAmmoMap();

	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(StellaContext, 0);
		}
	}

	if (WeaponClass)
	{
		if (AWeapon* Weapon = GetWorld()->SpawnActor<AWeapon>(WeaponClass))
		{
			PickupItem(Weapon);
		}
	}
}

void AStella::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AStella::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AStella::Movement);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AStella::Look);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &AStella::FireButtonPressed);
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Completed, this, &AStella::FireButtonReleased);
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Started, this, &AStella::Aim);
		EnhancedInputComponent->BindAction(PickupAction, ETriggerEvent::Started, this, &AStella::Pickup);
		EnhancedInputComponent->BindAction(DropAction, ETriggerEvent::Started, this, &AStella::Drop);
		EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Started, this, &AStella::Reload);
	}
}

void AStella::Movement(const FInputActionValue& Value)
{
	if (GetController())
	{
		FVector2D InValue = Value.Get<FVector2D>();
		
		FRotator Rotation(0.f, GetController()->GetControlRotation().Yaw, 0.f);
		FVector ForwardDirection = FRotationMatrix(Rotation).GetUnitAxis(EAxis::X);
		AddMovementInput(ForwardDirection, InValue.Y);

		FVector RightDirection = FRotationMatrix(Rotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(RightDirection, InValue.X);
	}
}

void AStella::Look(const FInputActionValue& Value)
{
	if (GetController())
	{
		FVector2D InValue = Value.Get<FVector2D>();

		AddControllerYawInput(InValue.X);
		AddControllerPitchInput(InValue.Y);
	}
}

void AStella::Fire()
{
	if (MuzzleEffect && MuzzleSound && FireMontage && HitEffect && BulletTrailEffect && EquippedWeapon->Ammo > 0)
	{
		GetMesh()->GetAnimInstance()->Montage_Play(FireMontage);

		FTransform SocketTransform = GetMesh()->GetSocketByName("MuzzleSocket")->GetSocketTransform(GetMesh());
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleEffect, SocketTransform);

		FVector TrailEndLocation;
		GetLineTraceForBullet(SocketTransform.GetLocation(), TrailEndLocation);

		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitEffect, TrailEndLocation);
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BulletTrailEffect, SocketTransform)->SetVectorParameter("Target", TrailEndLocation);

		UGameplayStatics::SpawnSound2D(this, MuzzleSound);

		EquippedWeapon->Ammo--;
	}
	if (HasAmmo() && CombatType != ECombatType::ECT_Reloading && EquippedWeapon->Ammo == 0)
	{
		PlayReloadMontage();
	}
}

void AStella::FireButtonPressed()
{
	if (CombatType == ECombatType::ECT_Unoccupied)
	{
		CombatType = ECombatType::ECT_Firing;
		Fire();
		GetWorldTimerManager().SetTimer(FireTimer, this, &AStella::Fire, 0.1f, true);
	}
}

void AStella::FireButtonReleased()
{
	GetWorldTimerManager().ClearTimer(FireTimer);
	CombatType = ECombatType::ECT_Unoccupied;
}

void AStella::Aim()
{
	if (!bIsAiming)
	{
		FollowCamera->SetFieldOfView(50.f);
		bIsAiming = true;
	}
	else
	{
		FollowCamera->SetFieldOfView(90.f);
		bIsAiming = false;
	}
}

void AStella::Pickup()
{
	if (LastTracedItem && LastTracedItem->PickupSound)
	{
		LastTracedItem->StartInterpolation(this);
		UGameplayStatics::PlaySound2D(this, LastTracedItem->PickupSound);
		LastTracedItem = nullptr;
	}
}

void AStella::Drop()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->ThrowWeapon();
	}
}

void AStella::Reload()
{
	if (CombatType == ECombatType::ECT_Unoccupied && HasAmmo() && EquippedWeapon->Ammo != EquippedWeapon->MagazineCapacity)
	{
		PlayReloadMontage();
	}
}

void AStella::PlayReloadMontage()
{
	if (ReloadMontage && EquippedWeapon)
	{
		CombatType = ECombatType::ECT_Reloading;
		GetMesh()->GetAnimInstance()->Montage_Play(ReloadMontage);
		GetMesh()->GetAnimInstance()->Montage_JumpToSection(EquippedWeapon->ReloadMontageSection);
	}
}

bool AStella::HasAmmo()
{
	if (EquippedWeapon)
	{
		return AmmoMap[EquippedWeapon->AmmoType] > 0;
	}

	return false;
}

void AStella::EndReload()
{
	CombatType = ECombatType::ECT_Unoccupied;

	if (EquippedWeapon)
	{
		const auto AmmoType = EquippedWeapon->AmmoType;
		int32 CarriedAmmo = AmmoMap[AmmoType];
		int32 MagEmptySpace = EquippedWeapon->MagazineCapacity - EquippedWeapon->Ammo;

		if (CarriedAmmo > MagEmptySpace)
		{
			EquippedWeapon->Ammo += MagEmptySpace;
			AmmoMap.Add(AmmoType, CarriedAmmo - MagEmptySpace);
		}
		else
		{
			EquippedWeapon->Ammo += CarriedAmmo;
			AmmoMap.Add(AmmoType, 0);
		}
	}
}

void AStella::PickupItem(AItem* ItemToPickup)
{
	if (AWeapon* InWeapon = Cast<AWeapon>(ItemToPickup))
	{
		Drop();

		InWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("WeaponSocket"));
		EquippedWeapon = Cast<AWeapon>(InWeapon);
		EquippedWeapon->SetItemState(EItemState::EIS_Equipped);
	}
	else if (AAmmo* InAmmo = Cast<AAmmo>(ItemToPickup))
	{
		if (EquippedWeapon)
		{
			int32 AmmoCount = AmmoMap[InAmmo->AmmoType];
			AmmoCount += EquippedWeapon->ItemCount;
			AmmoMap[EquippedWeapon->AmmoType] = AmmoCount;

			InAmmo->Destroy();

			if (EquippedWeapon->AmmoType == InAmmo->AmmoType && EquippedWeapon->Ammo == 0)
			{
				PlayReloadMontage();
			}
		}
	}
}

void AStella::GetStartEndForTrace(FVector& OutStart, FVector& OutEnd)
{
	FVector2D ViewportSize;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}
	FVector2D CrosshairLocation{ ViewportSize.X / 2.f, (ViewportSize.Y / 2.f) - 50.f };

	FVector WorldPosition, WorldDirection;
	UGameplayStatics::DeprojectScreenToWorld(UGameplayStatics::GetPlayerController(this, 0), CrosshairLocation, WorldPosition, WorldDirection);

	OutStart = WorldPosition;
	OutEnd = WorldPosition + WorldDirection * 50000.f;
}

void AStella::GetLineTraceForBullet(FVector InSocketLocation, FVector& TrailEndLocation)
{
	FVector Start, End;
	GetStartEndForTrace(Start, End);

	TrailEndLocation = End;

	// LineTrace from crosshair to aim location
	FHitResult HitResult;
	GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECollisionChannel::ECC_Visibility);
	if (HitResult.GetActor())
	{
		TrailEndLocation = HitResult.Location;
	}

	// LineTrace from weapon to aim location
	FHitResult WeaponHitResult;
	GetWorld()->LineTraceSingleByChannel(WeaponHitResult, InSocketLocation, End, ECollisionChannel::ECC_Visibility);
	if (WeaponHitResult.GetActor())
	{
		TrailEndLocation = WeaponHitResult.Location;
	}
}

void AStella::GetLineTraceForItem()
{
	FVector Start, End;
	GetStartEndForTrace(Start, End);

	FHitResult ItemHitResult;
	GetWorld()->LineTraceSingleByChannel(ItemHitResult, Start, End, ECollisionChannel::ECC_Visibility);

	if (AItem* Item = Cast<AItem>(ItemHitResult.GetActor()))
	{
		Item->PickupWidget->SetVisibility(true);
		LastTracedItem = Item;
	}
	else if (LastTracedItem)
	{
		LastTracedItem->PickupWidget->SetVisibility(false);
		LastTracedItem = nullptr;
	}
}

FVector AStella::GetInterpTargetLocation()
{
	return FollowCamera->GetComponentLocation() + FollowCamera->GetForwardVector() * 200.f + FVector(0.f, 0.f, 20.f);
}

void AStella::InitializeAmmoMap()
{
	AmmoMap.Add(EAmmoType::EAT_9mm, 120);
	AmmoMap.Add(EAmmoType::EAT_Rifle, 120);
}

void AStella::Grab()
{
	if (EquippedWeapon && HandSceneComponent)
	{
		FTransform BoneTransform = EquippedWeapon->ItemMesh->GetBoneTransform(EquippedWeapon->ClipBoneName);

		FAttachmentTransformRules Rules(EAttachmentRule::KeepRelative, true);
		HandSceneComponent->AttachToComponent(GetMesh(), Rules, FName("hand_l"));
		HandSceneComponent->SetWorldTransform(BoneTransform);
		EquippedWeapon->bMagMoving = true;
	}
}

void AStella::Release()
{
	EquippedWeapon->bMagMoving = false;
}
