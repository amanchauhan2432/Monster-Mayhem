#include "Item.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "../Character/Stella.h"
#include "Weapons/Weapon.h"
#include "Kismet/GameplayStatics.h"

AItem::AItem()
{
	PrimaryActorTick.bCanEverTick = true;

	ItemMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Item Mesh"));
	SetRootComponent(ItemMesh);
	ItemMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	ItemMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);

	CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Collision Sphere"));
	CollisionSphere->SetupAttachment(GetRootComponent());
	CollisionSphere->SetSphereRadius(300.f);

	PickupWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("Pickup Widget"));
	PickupWidget->SetupAttachment(GetRootComponent());
	PickupWidget->SetWidgetSpace(EWidgetSpace::Screen);
}

void AItem::BeginPlay()
{
	Super::BeginPlay();
	
	if (PickupWidget)
	{
		PickupWidget->SetVisibility(false);
	}

	CollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &AItem::OnBeginOverlap);
	CollisionSphere->OnComponentEndOverlap.AddDynamic(this, &AItem::OnEndOverlap);
}

void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (OverlappingPlayer)
	{
		OverlappingPlayer->GetLineTraceForItem();
	}

	InterpItem(DeltaTime);
}

void AItem::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	OverlappingPlayer = Cast<AStella>(OtherActor);
}

void AItem::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	OverlappingPlayer = nullptr;
}

void AItem::SetItemState(EItemState InState)
{
	ItemState = InState;
	SetItemProperties(ItemState);
}

void AItem::SetItemProperties(EItemState InState)
{
	switch (InState)
	{
	case EItemState::EIS_Pickup:

		ItemMesh->SetSimulatePhysics(false);
		ItemMesh->SetVisibility(true);
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		ItemMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);

		CollisionSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
		CollisionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

		break;

	case EItemState::EIS_EquipInterping:

		ItemMesh->SetSimulatePhysics(false);
		ItemMesh->SetVisibility(true);
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

		PickupWidget->SetVisibility(false);

		CollisionSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		CollisionSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		break;

	case EItemState::EIS_Pickedup:
		break;

	case EItemState::EIS_Equipped:

		ItemMesh->SetSimulatePhysics(false);
		ItemMesh->SetVisibility(true);
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

		PickupWidget->SetVisibility(false);

		CollisionSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		CollisionSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		break;

	case EItemState::EIS_Falling:

		ItemMesh->SetSimulatePhysics(true);
		ItemMesh->SetVisibility(true);
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		ItemMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
		ItemMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Block);

		CollisionSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		CollisionSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		break;

	case EItemState::EIS_MAX:
		break;

	default:
		break;
	}
}

void AItem::ThrowWeapon()
{
	ThrowRotation = GetActorRotation();
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	SetItemProperties(EItemState::EIS_Falling);

	ItemMesh->AddForce(ItemMesh->GetRightVector() * 300000.f);

	GetWorldTimerManager().SetTimer(WeaponThrowHandle, this, &AItem::StopFalling, 0.5f);
}

void AItem::StopFalling()
{
	SetItemState(EItemState::EIS_Pickup);
	SetActorRotation(ThrowRotation);
}

void AItem::StartInterpolation(AStella* InCharacter)
{
	Character = InCharacter;
	GetWorldTimerManager().SetTimer(InterpTimerHandle, this, &AItem::FinishInterpolation, 0.7f);
	bCanInterp = true;
	SetItemState(EItemState::EIS_EquipInterping);
}

void AItem::FinishInterpolation()
{
	bCanInterp = false;
	
	if (Character && EquipSound)
	{
		Character->PickupItem(this);
		UGameplayStatics::PlaySound2D(this, EquipSound);
		SetItemState(EItemState::EIS_Equipped);
	}
}

void AItem::InterpItem(float DeltaTime)
{
	if (bCanInterp && Character && ItemZCurve)
	{
		const float ElapsedTime = GetWorldTimerManager().GetTimerElapsed(InterpTimerHandle);

		FVector InterpedLocation{ FMath::FInterpTo(GetActorLocation().X, Character->GetInterpTargetLocation().X, DeltaTime, 30.f),
								  FMath::FInterpTo(GetActorLocation().Y, Character->GetInterpTargetLocation().Y, DeltaTime, 30.f),
								  FMath::FInterpTo(GetActorLocation().Z, Character->GetInterpTargetLocation().Z * ItemZCurve->GetFloatValue(ElapsedTime), DeltaTime, 30.f)};
		SetActorLocation(InterpedLocation);
	}
}
