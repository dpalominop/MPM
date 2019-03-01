#pragma once

#include <vector>
#include "Common.h"

bool loadOBJ(
	const char * path,
	std::vector < float3 > & out_vertices,
	std::vector < float2 > & out_uvs,
	std::vector < float3 > & out_normals
);

