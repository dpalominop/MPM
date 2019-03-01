#ifndef SETUP_FUNCTIONS_H
#define SETUP_FUNCTIONS_H

#include "Common.h"
#include "Parameters.h"
#include "Particle.h"
#include <time.h>
#include "objloader.h"
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

inline void createModelGrid(std::vector<Particle>& particles, solverParams* sp, float3 lower, int3 dims, float radius, float mass, float3 velocity) {
	// Read our .obj file
	std::vector< float3 > vertices;
	std::vector< float2 > uvs;
	std::vector< float3 > normals; // No las usaremos por ahora
	//bool res = loadOBJ("D:/Maestria/Graficos/Snow/Snow/models/bunny.obj", vertices, uvs, normals);

	std::string inputfile = "D:/Maestria/Graficos/Snow/Snow/models/bunny_small.obj";
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;

	std::string warn;
	std::string err;
	bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, inputfile.c_str());

	if (!err.empty()) { // `err` may contain warning message.
		std::cerr << err << std::endl;
	}

	if (!ret) {
		exit(1);
	}

	// Loop over shapes
	for (size_t s = 0; s < shapes.size(); s++) {
		// Loop over faces(polygon)
		size_t index_offset = 0;
		for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
			int fv = shapes[s].mesh.num_face_vertices[f];

			// Loop over vertices in the face.
			for (size_t v = 0; v < fv; v++) {
				// access to vertex
				tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
				tinyobj::real_t vx = attrib.vertices[3 * idx.vertex_index + 0];
				tinyobj::real_t vy = attrib.vertices[3 * idx.vertex_index + 1];
				tinyobj::real_t vz = attrib.vertices[3 * idx.vertex_index + 2];
				tinyobj::real_t nx = attrib.normals[3 * idx.normal_index + 0];
				tinyobj::real_t ny = attrib.normals[3 * idx.normal_index + 1];
				tinyobj::real_t nz = attrib.normals[3 * idx.normal_index + 2];
				tinyobj::real_t tx = attrib.texcoords[2 * idx.texcoord_index + 0];
				tinyobj::real_t ty = attrib.texcoords[2 * idx.texcoord_index + 1];

				float3 pos = lower + make_float3(float(vx), float(vy), float(vz));
				particles.push_back(Particle(pos, velocity, mass));
				// Optional: vertex colors
				// tinyobj::real_t red = attrib.colors[3*idx.vertex_index+0];
				// tinyobj::real_t green = attrib.colors[3*idx.vertex_index+1];
				// tinyobj::real_t blue = attrib.colors[3*idx.vertex_index+2];
			}
			index_offset += fv;

			// per-face material
			shapes[s].mesh.material_ids[f];
		}
	}


	/*for (auto &pos : vertices) {
		particles.push_back(Particle(pos, velocity, mass));
	}*/
}


inline void createParticleGrid(std::vector<Particle>& particles, solverParams* sp, float3 lower, int3 dims, float radius, float mass, float3 velocity) {
	//srand(int(time(NULL)));
	srand(16);

	for (int x = 0; x < dims.x; x++) {
		for (int y = 0; y < dims.y; y++) {
			for (int z = 0; z < dims.z; z++) {
				float r1 = 0.001f + static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
				float r2 = 0.001f + static_cast <float>(rand()) / static_cast <float> (RAND_MAX);
				float r3 = 0.001f + static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
				float3 jitter = make_float3(r1, r2, r3) * radius;
				float3 pos = lower + make_float3(float(x), float(y), float(z)) * radius + jitter;
				particles.push_back(Particle(pos, velocity, mass));
			}
		}
	}
}

//Some method for creating a snowball
inline void createSnowball(std::vector<Particle>& particles, float3 center, int3 dims, float radius, float mass, float3 velocity) {
	float sphereRadius = radius * (float)dims.x / 2.0f;
	for (int x = -dims.x/2; x <= dims.x/2; x++) {
		for (int y = -dims.y/2; y <= dims.y/2; y++) {
			for (int z = -dims.z/2; z <= dims.z/2; z++) {
				// generate a jittered point
				float r1 = 0.001f + static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
				float r2 = 0.001f + static_cast <float>(rand()) / static_cast <float> (RAND_MAX);
				float r3 = 0.001f + static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
				float3 jitter = make_float3(r1, r2, r3) * radius;

				float3 pos = center + make_float3(float(x), float(y), float(z)) * radius + jitter;
				// see if pos is inside the sphere
				if (length(pos - center) < sphereRadius) {
					particles.push_back(Particle(make_float3(pos.x, pos.y, pos.z), velocity, mass));
				}
			}
		}
	}


}


#endif