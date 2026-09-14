#include "WaveComputeShader.h"

IMPLEMENT_GLOBAL_SHADER(FWaveComputeCS, "/ComputeWavesShaders/Private/WaveCompute.usf", "MainCS", SF_Compute);