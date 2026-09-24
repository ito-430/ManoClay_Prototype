#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/DynamicMeshComponent.h"
#include "ClayActor.generated.h"

UCLASS()
class TESTMETA3_API AClayActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AClayActor();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Clay")
	UDynamicMeshComponent* DynamicMeshComponent;

	// 指先座標・半径・押し込み量を渡して変形させる関数
	UFUNCTION(BlueprintCallable, Category = "Clay")
	void DeformClayAtLocation(FVector BrushWorldLocation, float BrushRadius, float PushStrength);
};