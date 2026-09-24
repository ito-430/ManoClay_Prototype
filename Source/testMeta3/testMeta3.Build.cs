using UnrealBuildTool;

public class testMeta3 : ModuleRules
{
    public testMeta3(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "HeadMountedDisplay",
            "GeometryFramework",       // DynamicMeshComponent用
			"GeometryScriptingCore",   // Geometry Script関数用
			"DynamicMesh",             // FDynamicMesh3 低レイヤーアクセス用
            "GeometryCore",       // 追加: 低レイヤーの形状生成コア
			"GeometryAlgorithms"  // 追加: ジオメトリ計算アルゴリズム
        });

        PrivateDependencyModuleNames.AddRange(new string[] { });
    }
}