#include "ClayActor.h"
#include "DynamicMesh/DynamicMesh3.h"
#include "Generators/SphereGenerator.h"
#include "GeometryScript/MeshNormalsFunctions.h"

using namespace UE::Geometry;

AClayActor::AClayActor()
{
	PrimaryActorTick.bCanEverTick = false;

	DynamicMeshComponent = CreateDefaultSubobject<UDynamicMeshComponent>(TEXT("DynamicMesh"));
	RootComponent = DynamicMeshComponent;
}

void AClayActor::BeginPlay()
{
	Super::BeginPlay();

	UDynamicMesh* TargetMesh = DynamicMeshComponent->GetDynamicMesh();
	if (TargetMesh)
	{
		TargetMesh->EditMesh([&](FDynamicMesh3& Mesh)
			{
				// 低レイヤーのジェネレータで半径20cm、48x48分割の球体を直接構築
				FSphereGenerator SphereGen;
				SphereGen.Radius = 20.0;
				SphereGen.NumPhi = 48;
				SphereGen.NumTheta = 48;
				SphereGen.Generate();

				Mesh.Copy(&SphereGen);
			});

		// 法線の初期計算
		FGeometryScriptCalculateNormalsOptions NormalsOptions;
		UGeometryScriptLibrary_MeshNormalsFunctions::RecomputeNormals(TargetMesh, NormalsOptions);

		DynamicMeshComponent->NotifyMeshModified();
	}
}

void AClayActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AClayActor::DeformClayAtLocation(FVector BrushWorldLocation, float BrushRadius, float PushStrength)
{
	if (!DynamicMeshComponent) return;

	UDynamicMesh* DynMesh = DynamicMeshComponent->GetDynamicMesh();
	if (!DynMesh) return;

	FTransform ActorTransform = GetActorTransform();
	FVector LocalBrushPos = ActorTransform.InverseTransformPosition(BrushWorldLocation);
	float LocalRadius = BrushRadius / ActorTransform.GetScale3D().GetMax();
	float LocalRadiusSq = LocalRadius * LocalRadius;

	bool bMeshModified = false;

	DynMesh->EditMesh([&](FDynamicMesh3& Mesh)
		{
			FAxisAlignedBox3d LocalQueryBox(
				(FVector3d)LocalBrushPos - FVector3d(LocalRadius),
				(FVector3d)LocalBrushPos + FVector3d(LocalRadius)
			);

			for (int32 VertID : Mesh.VertexIndicesItr())
			{
				FVector3d VertPos = Mesh.GetVertex(VertID);

				// AABB 粗判定
				if (!LocalQueryBox.Contains(VertPos))
				{
					continue;
				}

				// 球半径内判定
				double DistSq = FVector3d::DistSquared(VertPos, (FVector3d)LocalBrushPos);
				if (DistSq < LocalRadiusSq)
				{
					double Dist = FMath::Sqrt(DistSq);

					// Falloff 計算
					float Falloff = FMath::Clamp(1.0f - (float)(Dist / LocalRadius), 0.0f, 1.0f);
					Falloff = 0.5f * (1.0f - FMath::Cos(Falloff * PI));

					FVector3d PushDir = VertPos - (FVector3d)LocalBrushPos;
					if (PushDir.SquaredLength() > KINDA_SMALL_NUMBER)
					{
						PushDir.Normalize();
					}
					else
					{
						PushDir = FVector3d::UpVector;
					}

					FVector3d Offset = PushDir * (PushStrength * Falloff);
					Mesh.SetVertex(VertID, VertPos + Offset);

					bMeshModified = true;
				}
			}
		});

	if (bMeshModified)
	{
		FGeometryScriptCalculateNormalsOptions NormalsOptions;
		UGeometryScriptLibrary_MeshNormalsFunctions::RecomputeNormals(DynMesh, NormalsOptions);
		DynamicMeshComponent->NotifyMeshModified();
	}
}