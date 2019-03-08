#include "../includes/Renderer.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../includes/stb_image.h"

using namespace std;

static const float radius = 0.027f;
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

	GLfloat vertices[] = {
		// positions          // colors           // texture coords (note that we changed them to 'zoom in' on our texture image)
		 sp->boxCorner2.x, sp->boxCorner1.y, sp->boxCorner2.z,   1.0f, 0.0f, 0.0f,   0.95f, 0.95f, // top right
		 sp->boxCorner2.x, sp->boxCorner1.y, sp->boxCorner1.z,   0.0f, 1.0f, 0.0f,   0.95f, 0.10f, // bottom right
		sp->boxCorner1.x, sp->boxCorner1.y, sp->boxCorner1.z,   0.0f, 0.0f, 1.0f,   0.10f, 0.10f, // bottom left
		sp->boxCorner1.x, sp->boxCorner1.y, sp->boxCorner2.z,   1.0f, 1.0f, 0.0f,   0.10f, 0.95f  // top left 
	};

	//Grass
	glGenVertexArrays(1, &floorBuffers.vao);

	glGenBuffers(1, &floorBuffers.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, floorBuffers.vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &floorBuffers.ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, floorBuffers.ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// texture coord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	// load and create a texture 
	// -------------------------
	unsigned int texture1;
	// texture 1
	// ---------
	glGenTextures(1, &texture1);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture1);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	int widthi, heighti, nrChannels;
	stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
	// The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
	unsigned char *data = stbi_load("models/grass.png", &widthi, &heighti, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, widthi, heighti, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);


}

Renderer::~Renderer() {
	if (modelBuffers.vao != 0) {
		glDeleteVertexArrays(1, &modelBuffers.vao);
		glDeleteBuffers(1, &modelBuffers.positions);
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
	/*glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	*/
	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// texture coord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glUniform1i(glGetUniformLocation(plane.program, "texture1"), 0);

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

	glDrawElements(GL_POINTS, sp->numIndices, GL_UNSIGNED_INT, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}