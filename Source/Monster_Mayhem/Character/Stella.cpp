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

AStella::AStella()
{
	PrimaryActorTick.bCanEverTick = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("Camera Boom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->SocketOffset = FVector(0.f, 50.f, 50.f);

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Follow Camera"));
	FollowCamera->SetupAttachment(CameraBoom);

	GetCharacterMovement()->JumpZVelocity = 500.f;
}

void AStella::BeginPlay()
{
	Super::BeginPlay();
	
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(StellaContext, 0);
		}
	}

	if (WeaponClass)
	{
		AActor* Weapon = GetWorld()->SpawnActor<AActor>(WeaponClass);
		if (Weapon)
		{
			Weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("WeaponSocket"));
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
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &AStella::Fire);
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

void AStella::Fire(const FInputActionValue& Value)
{
	if (MuzzleEffect && MuzzleSound && FireMontage && HitEffect && BulletTrailEffect)
	{
		GetMesh()->GetAnimInstance()->Montage_Play(FireMontage);

		FTransform SocketTransform = GetMesh()->GetSocketByName("MuzzleSocket")->GetSocketTransform(GetMesh());
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleEffect, SocketTransform);

		FVector TrailEndLocation;
		GetLineTrace(SocketTransform.GetLocation(), TrailEndLocation);

		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitEffect, TrailEndLocation);
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BulletTrailEffect, SocketTransform)->SetVectorParameter("Target", TrailEndLocation);

		UGameplayStatics::SpawnSound2D(this, MuzzleSound);
	}
}

void AStella::GetLineTrace(FVector InSocketLocation, FVector& TrailEndLocation)
{
	FVector2D ViewportSize;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}
	FVector2D CrosshairLocation{ ViewportSize.X / 2.f, (ViewportSize.Y / 2.f) - 50.f };

	FVector WorldPosition, WorldDirection;
	UGameplayStatics::DeprojectScreenToWorld(UGameplayStatics::GetPlayerController(this, 0), CrosshairLocation, WorldPosition, WorldDirection);


	// LineTrace from crosshair to aim location
	FHitResult HitResult;
	FVector Start = WorldPosition;
	FVector End = WorldPosition + WorldDirection * 50000.f;
	TrailEndLocation = End;

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

