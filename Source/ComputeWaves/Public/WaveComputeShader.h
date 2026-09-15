

#pragma once

#include "CoreMinimal.h"
#include "GlobalShader.h"
#include "ShaderParameterStruct.h"
#include "RenderGraphResources.h"
#include "WaveComputingSettings.h"

/**
 *
 */
class COMPUTEWAVES_API FWaveComputeCS : public FGlobalShader
{
public:

	DECLARE_GLOBAL_SHADER(FWaveComputeCS);
	SHADER_USE_PARAMETER_STRUCT(FWaveComputeCS, FGlobalShader);


	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )

		SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture2D<float4>, OutputTexture)
		SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture2D<float3>, OutputNormal)
		SHADER_PARAMETER(int32, WaveCount)
		SHADER_PARAMETER(float, WorldSize)
		SHADER_PARAMETER(int32, TextureRes)

		SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture2D<float4>, SecondOutputTexture)
		SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture2D<float3>, SecondOutputNormal)
		SHADER_PARAMETER(int32, SecondWaveCount)
		SHADER_PARAMETER(float, SecondWorldSize)
		SHADER_PARAMETER(int32, SecondTextureRes)

		SHADER_PARAMETER(float, Time)
		SHADER_PARAMETER(float, DeltaTime)
		SHADER_PARAMETER(float, FoamThreshold)
		SHADER_PARAMETER(float, FoamDecay)

		SHADER_PARAMETER_RDG_BUFFER_SRV(StructuredBuffer<FWaveParams>, Waves)

	END_SHADER_PARAMETER_STRUCT()

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return true;
	}

	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
	{
		FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);
	}
};
