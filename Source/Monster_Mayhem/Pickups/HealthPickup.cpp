#include "HealthPickup.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "../Character/Stella.h"

AHealthPickup::AHealthPickup()
{
	PrimaryActorTick.bCanEverTick = false;

	PickupMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Pickup Mesh"));
	SetRootComponent(PickupMesh);

	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Collision"));
	SphereCollision->SetupAttachment(PickupMesh);
	SphereCollision->SetSphereRadius(50.f);

}

void AHealthPickup::BeginPlay()
{
	Super::BeginPlay();
	
	SphereCollision->OnComponentBeginOverlap.AddDynamic(this, &AHealthPickup::OnBeginOverlap);
}

void AHealthPickup::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (auto Player = Cast<AStella>(OtherActor))
	{
		Player->Health = FMath::Clamp(Player->Health + 20, 0, Player->MaxHealth);
		Player->HealthPercent = Player->Health / Player->MaxHealth;

		Destroy();
	}
}