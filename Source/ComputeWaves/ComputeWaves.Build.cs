using UnrealBuildTool;

public class ComputeWaves: ModuleRules
{
    public ComputeWaves(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] {"Core", "CoreUObject", "Engine"});

        PrivateDependencyModuleNames.AddRange(new string[] { "RenderCore", "RHI", "Projects" });
    }
}
