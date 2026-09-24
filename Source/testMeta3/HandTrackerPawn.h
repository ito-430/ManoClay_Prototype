#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "MotionControllerComponent.h"
#include "HandTrackerPawn.generated.h"

UCLASS()
class TESTMETA3_API AHandTrackerPawn : public APawn
{
	GENERATED_BODY()

public:
	AHandTrackerPawn();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VR")
	USceneComponent* SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VR")
	UMotionControllerComponent* MotionControllerR;
};