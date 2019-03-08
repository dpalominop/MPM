#version 450 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;

uniform mat4 mView;
uniform mat4 projection;

out vec3 ourColor;
out vec2 TexCoord;

void main() {
    gl_Position = projection * mView * vec4(aPos, 1.0);
    //coord = position;
	ourColor = aColor;
    TexCoord = aTexCoord;
}  