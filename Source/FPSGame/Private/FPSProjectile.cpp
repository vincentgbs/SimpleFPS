// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "FPSProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
// needs to be added to include the correct class, outside of tutorial
#include "Runtime/Engine/Classes/Materials/MaterialInstanceDynamic.h"

AFPSProjectile::AFPSProjectile() 
{
	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->SetCollisionProfileName("Projectile");
	CollisionComp->OnComponentHit.AddDynamic(this, &AFPSProjectile::OnHit);	// set up a notification for when this component hits something blocking

	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	// Set as root component
	RootComponent = CollisionComp;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;

	// Die after 3 seconds by default
	InitialLifeSpan = 3.0f;
}


void AFPSProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Only add impulse and destroy projectile if we hit a physics
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr) && OtherComp->IsSimulatingPhysics())
	{
		// impact of hit on the component that is hit
		OtherComp->AddImpulseAtLocation(GetVelocity() * 100.0f, GetActorLocation());
		
		// reduce scale of the component that is hit
		FVector Scale = OtherComp->GetComponentScale();
		Scale *= 0.8f;

		// if the smallest measurement of the scale is "small enough" 0.5f, destory the other actor
		if (Scale.GetMin() < 0.5f)
		{
			OtherActor->Destroy();
		}
		else
		{
			OtherComp->SetWorldScale3D(Scale);
		}

		UMaterialInstanceDynamic* MatInst = OtherComp->CreateAndSetMaterialInstanceDynamic(0);
		if (MatInst)
		{
			MatInst->SetVectorParameterValue("Color", FLinearColor::MakeRandomColor());
		}

		Destroy();
	}
}