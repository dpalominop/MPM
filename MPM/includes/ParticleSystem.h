#ifndef PARTICLE_SYSTEM_H
#define PARTICLE_SYSTEM_H

#include "../includes/Headers.h"
#include "../includes/Particle.h"
#include "../includes/Cell.h"
#include "../includes/Parameters.h"
#include <time.h>

class ParticleSystem {
public:
	ParticleSystem(std::vector<Particle>& particles, solverParams& params);
	~ParticleSystem();

	void updateWrapper(solverParams& params);
	void getPositionsWrapper(float* positionsPtr);

private:
	Particle* particles;
	Cell* cells;

	#define cudaCheck(x) { cudaError_t err = x; if (err != cudaSuccess) { printf("Cuda error: %d in %s at %s:%d\n", err, #x, __FILE__, __LINE__); assert(0); } }
};

#endif