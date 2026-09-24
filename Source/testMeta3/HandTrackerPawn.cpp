#include "HandTrackerPawn.h"
#include "DrawDebugHelpers.h"
#include "Features/IModularFeatures.h"
#include "IHandTracker.h"
#include "HeadMountedDisplayTypes.h"
#include "ClayActor.h"
#include "Kismet/GameplayStatics.h"

AHandTrackerPawn::AHandTrackerPawn()
{
	PrimaryActorTick.bCanEverTick = true;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	RootComponent = SceneRoot;

	MotionControllerR = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("MotionControllerR"));
	MotionControllerR->SetupAttachment(RootComponent);
	MotionControllerR->SetTrackingMotionSource(FName("RightHand"));

	AutoPossessPlayer = EAutoReceiveInput::Player0;
}

void AHandTrackerPawn::BeginPlay()
{
	Super::BeginPlay();
}

void AHandTrackerPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	bool bFound = false;

	if (IModularFeatures::Get().IsModularFeatureAvailable(IHandTracker::GetModularFeatureName()))
	{
		IHandTracker* HandTracker = &IModularFeatures::Get().GetModularFeature<IHandTracker>(IHandTracker::GetModularFeatureName());

		if (HandTracker)
		{
			FTransform JointTransform;
			float JointRadius = 0.0f;

			bFound = HandTracker->GetKeypointState(EControllerHand::Right, EHandKeypoint::IndexTip, JointTransform, JointRadius);

			if (bFound)
			{
				FVector TipLocation = JointTransform.GetLocation();

				// 指先に赤い球体を描画
				DrawDebugSphere(
					GetWorld(),
					TipLocation,
					2.0f,
					12,
					FColor::Red,
					false,
					-1.0f,
					0,
					0.1f
				);

				// レベル上に存在する ClayActor を探して変形を適用
				AActor* FoundActor = UGameplayStatics::GetActorOfClass(GetWorld(), AClayActor::StaticClass());
				AClayActor* Clay = Cast<AClayActor>(FoundActor);
				if (Clay)
				{
					// ブラシ半径: 4cm, 押し込み強度: 1.0cm
					Clay->DeformClayAtLocation(TipLocation, 4.0f, 1.0f);
				}
			}
		}
	}
}