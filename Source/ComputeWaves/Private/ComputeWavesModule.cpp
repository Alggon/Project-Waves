#include "ComputeWavesModule.h"
#include "Modules/ModuleManager.h"
#include "Misc/Paths.h"
#include "ShaderCore.h"

IMPLEMENT_MODULE(FComputeWaves, ComputeWaves);

void FComputeWaves::StartupModule()
{
	FString ShaderDir = FPaths::Combine(FPaths::ProjectDir(), TEXT("Source/ComputeWaves/Shaders"));
	AddShaderSourceDirectoryMapping(TEXT("/ComputeWavesShaders"), ShaderDir);
}

void FComputeWaves::ShutdownModule()
{
}
