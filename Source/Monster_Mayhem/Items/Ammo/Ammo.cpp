#include "Ammo.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "../../Character/Stella.h"
#include "Kismet/GameplayStatics.h"

AAmmo::AAmmo()
{
	AmmoMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Ammo Mesh"));
	SetRootComponent(AmmoMesh);
	AmmoMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	AmmoMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	AmmoMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);

	CollisionSphere->SetupAttachment(GetRootComponent());
	PickupWidget->SetupAttachment(GetRootComponent());
}

void AAmmo::BeginPlay()
{
	Super::BeginPlay();

	if (OutlineMaterial)
	{
		AmmoMesh->SetOverlayMaterial(OutlineMaterial);
	}
}

void AAmmo::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AAmmo::SetItemProperties(EItemState InState)
{
	switch (InState)
	{
	case EItemState::EIS_Pickup:

		AmmoMesh->SetSimulatePhysics(false);
		AmmoMesh->SetVisibility(true);
		AmmoMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		AmmoMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		AmmoMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);

		break;

	case EItemState::EIS_EquipInterping:

		AmmoMesh->SetSimulatePhysics(false);
		AmmoMesh->SetVisibility(true);
		AmmoMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		AmmoMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

		break;

	case EItemState::EIS_Pickedup:
		break;

	case EItemState::EIS_Equipped:

		AmmoMesh->SetSimulatePhysics(false);
		AmmoMesh->SetVisibility(true);
		AmmoMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		AmmoMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

		break;

	case EItemState::EIS_Falling:

		AmmoMesh->SetSimulatePhysics(true);
		AmmoMesh->SetVisibility(true);
		AmmoMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		AmmoMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		AmmoMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
		AmmoMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Block);

		break;

	case EItemState::EIS_MAX:
		break;

	default:
		break;
	}
}

void AAmmo::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnBeginOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	if (OverlappingPlayer)
	{
		StartInterpolation(OverlappingPlayer);
		UGameplayStatics::PlaySound2D(this, PickupSound);

		CollisionSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}
