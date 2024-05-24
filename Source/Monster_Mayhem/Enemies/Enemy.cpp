#include "Enemy.h"
#include "Components/CapsuleComponent.h"
#include "Components/BoxComponent.h"
#include "../Character/Stella.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "EnemyController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"

AEnemy::AEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Ignore);

	AgroSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Agro Sphere"));
	AgroSphere->SetupAttachment(GetRootComponent());
	AgroSphere->SetSphereRadius(650.f);

	CombatSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Combat Sphere"));
	CombatSphere->SetupAttachment(GetRootComponent());
	CombatSphere->SetSphereRadius(150.f);

	LeftWeaponCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Left Weapon Collision"));
	LeftWeaponCollision->SetupAttachment(GetMesh(), FName("LeftWeaponBone"));
	LeftWeaponCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LeftWeaponCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	LeftWeaponCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	LeftWeaponCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	RightWeaponCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Right Weapon Collision"));
	RightWeaponCollision->SetupAttachment(GetMesh(), FName("RightWeaponBone"));
	RightWeaponCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RightWeaponCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	RightWeaponCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	RightWeaponCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	HealthBarComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("Health Bar Component"));
	HealthBarComponent->SetupAttachment(GetRootComponent());
	HealthBarComponent->SetWidgetSpace(EWidgetSpace::Screen);
	HealthBarComponent->SetVisibility(false);

	GetCharacterMovement()->bOrientRotationToMovement = true;
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Ignore);

	AgroSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::OnBeginOverlap);
	AgroSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemy::OnEndOverlap);

	CombatSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::OnBeginCombat);
	CombatSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemy::OnEndCombat);

	LeftWeaponCollision->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::OnBeginLeftAttack);
	RightWeaponCollision->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::OnBeginRightAttack);

	Health = MaxHealth;
	HealthPercent = Health / MaxHealth;

	EnemyController = Cast<AEnemyController>(GetController());
	if (EnemyController)
	{
		EnemyController->BlackboardComponent->SetValueAsVector(TEXT("PatrolTarget1"), PatrolTarget1->GetActorLocation());
		EnemyController->BlackboardComponent->SetValueAsVector(TEXT("PatrolTarget2"), PatrolTarget2->GetActorLocation());
		EnemyController->RunBehaviorTree(BehaviorTree);
	}
}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemy::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	auto Player = Cast<AStella>(OtherActor);
	if (Player && EnemyController)
	{
		EnemyController->BlackboardComponent->SetValueAsObject(FName("Target"), Player);
	}
}

void AEnemy::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	EnemyController->BlackboardComponent->ClearValue(FName("Target"));
}

void AEnemy::OnBeginCombat(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	auto Player = Cast<AStella>(OtherActor);
	if (Player && EnemyController)
	{
		EnemyController->BlackboardComponent->SetValueAsBool(FName("InAttackRange"), true);
	}
}

void AEnemy::OnEndCombat(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	EnemyController->BlackboardComponent->SetValueAsBool(FName("InAttackRange"), false);
}

void AEnemy::OnBeginLeftAttack(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	auto Player = Cast<AStella>(OtherActor);
	if (Player)
	{
		UGameplayStatics::ApplyDamage(Player, BaseDamage, GetController(), this, UDamageType::StaticClass());
	}
}

void AEnemy::OnBeginRightAttack(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	auto Player = Cast<AStella>(OtherActor);
	if (Player)
	{
		UGameplayStatics::ApplyDamage(Player, BaseDamage, GetController(), this, UDamageType::StaticClass());
	}
}

void AEnemy::BulletHit_Implementation(FVector HitLocation)
{
	if (HitEffect && HitSound)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitEffect, HitLocation);

		UGameplayStatics::SpawnSoundAtLocation(this, HitSound, HitLocation);
	}
	if (FMath::FRand() > 0.92)
	{
		SetIsBulletHit(true);
		PlayMontage(HitReactMontage);
	}
	ShowWidget();
}

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	Health = FMath::Clamp(Health - DamageAmount, 0.f, MaxHealth);
	HealthPercent = Health / MaxHealth;

	if (EnemyController)
	{
		EnemyController->BlackboardComponent->SetValueAsObject(FName("Target"), DamageCauser);
	}

	if (Health == 0)
	{
		Die();
	}

	return Health;
}

void AEnemy::Die()
{
	if (HealthBarComponent)
	{
		HealthBarComponent->SetVisibility(false);
		DeadAnimIndex = FMath::RandRange(1, 2);
		FName SectionName = FName(FString::Printf(TEXT("%d"), DeadAnimIndex));
		PlayMontage(DeathMontage, SectionName);

		EnemyController->BlackboardComponent->SetValueAsBool(FName("DeadState"), true);
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		UpdateLeftWeaponCollision(ECollisionEnabled::NoCollision);
		UpdateRightWeaponCollision(ECollisionEnabled::NoCollision);

		GetWorldTimerManager().SetTimer(DestroyHandle, this, &AEnemy::DestroyEnemy, 5.f);
	}
}

void AEnemy::DestroyEnemy()
{
	Destroy();
}

void AEnemy::ShowWidget()
{
	if (HealthBarComponent)
	{
		GetWorldTimerManager().ClearTimer(HealthBarHandle);
		HealthBarComponent->SetVisibility(true);
		GetWorldTimerManager().SetTimer(HealthBarHandle, this, &AEnemy::HideWidget, 5.f);
	}
}

void AEnemy::HideWidget()
{
	if (HealthBarComponent)
	{
		HealthBarComponent->SetVisibility(false);
	}
}

void AEnemy::PlayMontage(UAnimMontage* InMontage, FName InName)
{
	if (InMontage)
	{
		GetMesh()->GetAnimInstance()->Montage_Play(InMontage);
		GetMesh()->GetAnimInstance()->Montage_JumpToSection(InName, InMontage);
	}
}

void AEnemy::SetIsBulletHit(bool bBulletehit)
{
	if (EnemyController)
	{
		EnemyController->BlackboardComponent->SetValueAsBool(FName("BulletHit"), bBulletehit);
	}
}

void AEnemy::UpdateLeftWeaponCollision(ECollisionEnabled::Type InType)
{
	LeftWeaponCollision->SetCollisionEnabled(InType);
}

void AEnemy::UpdateRightWeaponCollision(ECollisionEnabled::Type InType)
{
	RightWeaponCollision->SetCollisionEnabled(InType);
}
