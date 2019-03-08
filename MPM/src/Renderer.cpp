#include "../includes/Renderer.h"

using namespace std;

static const float radius = 0.005f;

Renderer::Renderer(int width, int height, solverParams* sp) :
width(width),
height(height),
plane(Shader("shaders/plane.vert", "shaders/plane.frag")),
model(Shader("shaders/model.vert", "shaders/model.frag"))
{
	this->sp = sp;
	aspectRatio = float(width) / float(height);

	GLfloat floorVertices[] = {
		sp->boxCorner2.x, sp->boxCorner1.y, sp->boxCorner2.z,
		sp->boxCorner2.x, sp->boxCorner1.y, sp->boxCorner1.z,
		sp->boxCorner1.x, sp->boxCorner1.y, sp->boxCorner1.z,
		sp->boxCorner1.x, sp->boxCorner1.y, sp->boxCorner2.z
	};

	GLfloat wallVertices[] = {
		sp->boxCorner1.x, sp->boxCorner1.y, sp->boxCorner2.z,
		sp->boxCorner1.x, sp->boxCorner1.y, sp->boxCorner1.z,
		sp->boxCorner1.x, sp->boxCorner2.y, sp->boxCorner1.z,
		sp->boxCorner1.x, sp->boxCorner2.y, sp->boxCorner2.z
	};

	GLuint indices[] = {
		0, 1, 3,
		1, 2, 3,
	};

	//Wall
	glGenVertexArrays(1, &wallBuffers.vao);

	glGenBuffers(1, &wallBuffers.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, wallBuffers.vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(wallVertices), wallVertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &wallBuffers.ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, wallBuffers.ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	//Floor
	glGenVertexArrays(1, &floorBuffers.vao);

	glGenBuffers(1, &floorBuffers.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, floorBuffers.vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(floorVertices), floorVertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &floorBuffers.ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, floorBuffers.ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

}

Renderer::~Renderer() {
	if (modelBuffers.vao != 0) {
		glDeleteVertexArrays(1, &modelBuffers.vao);
		glDeleteBuffers(1, &modelBuffers.positions);
	}

	if (wallBuffers.vao != 0) {
		glDeleteVertexArrays(1, &wallBuffers.vao);
		glDeleteBuffers(1, &wallBuffers.vbo);
		glDeleteBuffers(1, &wallBuffers.ebo);
	}

	if (floorBuffers.vao != 0) {
		glDeleteVertexArrays(1, &floorBuffers.vao);
		glDeleteBuffers(1, &floorBuffers.vbo);
		glDeleteBuffers(1, &floorBuffers.ebo);
	}
}

void Renderer::setProjection(glm::mat4 projection) {
	this->projection = projection;
}

void Renderer::initModelBuffers(int numParticles, int numIndices, std::vector<unsigned int> indices) {
	glGenVertexArrays(1, &modelBuffers.vao);

	glGenBuffers(1, &modelBuffers.positions);
	glBindBuffer(GL_ARRAY_BUFFER, modelBuffers.positions);
	glBufferData(GL_ARRAY_BUFFER, numParticles * 3 * sizeof(float), 0, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	cudaGraphicsGLRegisterBuffer(&resource, modelBuffers.positions, cudaGraphicsRegisterFlagsWriteDiscard);

	// create index buffer
	glGenBuffers(1, &modelBuffers.indices);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, modelBuffers.indices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices* sizeof(unsigned int), 0, GL_DYNAMIC_DRAW);

	// fill with indices for rendering mesh as triangle strips
	GLuint *ebo = (GLuint *)glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY);
	if (!ebo)
	{
		return;
	}

	for (auto& val : indices) {
		*ebo++ = val;
	}

	glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	modelBuffers.numParticles = numParticles;
	modelBuffers.numIndices = numIndices;
}

void Renderer::render(Camera& cam) {
	//Set model view matrix
	mView = cam.getMView();

	//Clear buffer
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Plane
	//renderPlane(wallBuffers);
	renderPlane(floorBuffers);

	//Model
	renderModel(cam);
}

void Renderer::renderPlane(planeBuffers &buf) {
	glUseProgram(plane.program);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	plane.setUniformmat4("mView", mView);
	plane.setUniformmat4("projection", projection);

	glBindVertexArray(buf.vao);
	glBindBuffer(GL_ARRAY_BUFFER, buf.vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buf.ebo);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void Renderer::renderModel(Camera& cam) {
	glUseProgram(model.program);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	model.setUniformmat4("mView", mView);
	model.setUniformmat4("projection", projection);
	model.setUniformf("pointRadius", radius);
	model.setUniformf("pointScale", width / aspectRatio * (1.0f / tanf(cam.zoom * 0.5f)));

	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
	glEnable(GL_CULL_FACE);

	//Draw the model
	glBindVertexArray(modelBuffers.vao);
	glBindBuffer(GL_ARRAY_BUFFER, modelBuffers.positions);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, modelBuffers.indices);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);


	//glDrawArrays(GL_POINTS, 0, GLsizei(modelBuffers.numParticles));
	

	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawElements(GL_POINTS, sp->numIndices, GL_UNSIGNED_INT, 0);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}