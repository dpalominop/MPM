#ifndef SETUP_FUNCTIONS_H
#define SETUP_FUNCTIONS_H

#include "Common.h"
#include "Parameters.h"
#include "Particle.h"
#include <time.h>
#include "objloader.h"
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

inline void createModelGrid(std::vector<Particle>& particles, std::vector<unsigned int>& indices, solverParams* sp, float3 lower, int3 dims, float radius, float mass, float3 velocity) {
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
				/*tinyobj::real_t vx = attrib.vertices[3 * idx.vertex_index + 0];
				tinyobj::real_t vy = attrib.vertices[3 * idx.vertex_index + 1];
				tinyobj::real_t vz = attrib.vertices[3 * idx.vertex_index + 2];
				tinyobj::real_t nx = attrib.normals[3 * idx.normal_index + 0];
				tinyobj::real_t ny = attrib.normals[3 * idx.normal_index + 1];
				tinyobj::real_t nz = attrib.normals[3 * idx.normal_index + 2];
				tinyobj::real_t tx = attrib.texcoords[2 * idx.texcoord_index + 0];
				tinyobj::real_t ty = attrib.texcoords[2 * idx.texcoord_index + 1];

				float3 pos = lower + make_float3(float(vx), float(vy), float(vz));
				particles.push_back(Particle(pos, velocity, mass));*/
				
				indices.push_back(idx.vertex_index);
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

	for (int idx = 0; idx < attrib.vertices.size(); idx += 3) {
		tinyobj::real_t vx = attrib.vertices[idx + 0];
		tinyobj::real_t vy = attrib.vertices[idx + 1];
		tinyobj::real_t vz = attrib.vertices[idx + 2];
		
		float3 pos = lower + make_float3(float(vx), float(vy), float(vz));
		particles.push_back(Particle(pos, velocity, mass));
	}

	/*for (auto &pos : vertices) {
		particles.push_back(Particle(pos, velocity, mass));
	}*/
}


#endif