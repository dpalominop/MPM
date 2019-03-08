#ifndef SCENE_H
#define SCENE_H

#include "../includes/Headers.h"
#include "Parameters.h"
#include "Model.hpp"
#include "Particle.h"

class Scene {
public:
	Scene(std::string name) : name(name) {}
	virtual void init(std::vector<Particle>& particles, solverParams* sp) {

		sp->deltaT = 5e-5f;
		sp->radius = 0.022f;
		sp->compression = 0.019f;
		sp->stretch = 0.0075f;
		sp->hardening = 100.0f;
		sp->young = 1500.0f;
		sp->poisson = 0.4f;
		sp->alpha = 0.95f;
		sp->density = 30.0f;

		sp->lambda = getLambda(sp->poisson, sp->young);
		sp->mu = getMu(sp->poisson, sp->young);

		sp->gravity = make_float3(0, -9.8f, 0);

		sp->frictionCoeff = 1.0f;
		sp->stickyWalls = false;
	}

	float getLambda(float poisson, float young) {
		return (poisson * young) / ((1 + poisson) * (1 - 2 * poisson));
	}

	float getMu(float poisson, float young) {
		return young / (2 * (1 + poisson));
	}

	float getMass(float radius, float density) {
		return pow(radius, 3) * density / 4;
	}

private:
	std::string name;
};

class Model : public Scene {
public:
	Model(std::string name) : Scene(name) {}

	virtual void init(std::vector<Particle>& particles, std::vector<unsigned int>& indices, solverParams* sp) {
		Scene::init(particles, sp);

		const float restDistance = sp->radius * 1.f;

		int3 dims = make_int3(150);
		int3 modelDims = make_int3(40);

		sp->boxCorner1 = make_float3(0, 0.0f, 0);
		sp->boxCorner2 = make_float3((dims.x) * sp->radius, (dims.y) * sp->radius, (dims.z) * sp->radius);

		float3 lower = make_float3(dims.x / 2 * sp->radius, 0.5f, dims.z / 2 * sp->radius);
		createModelGrid(particles, indices, sp, lower, modelDims, restDistance, getMass(sp->radius, sp->density), make_float3(0, -5, 0));

		sp->numParticles = int(particles.size());
		sp->numIndices = int(indices.size());
		sp->gridSize = dims.x * dims.y * dims.z;
		sp->gBounds = dims;
	}
};

#endif