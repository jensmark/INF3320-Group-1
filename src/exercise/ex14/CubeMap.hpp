#ifndef CUBEMAP_H__
#define CUBEMAP_H__

#include <string>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <stdio.h>
#include <vector>
#include <glm/gtc/type_ptr.hpp>

#include "OpenGLError.hpp"
#include "TGALoader.h"

const GLubyte quad_indices[]= {
	0, 2, 1, 2, 0, 3, //north face
	1, 6, 5, 6, 1, 2, //west face
	5, 7, 4, 7, 5, 6, //south face
	4, 3, 0, 3, 4, 7, //east face
	3, 6, 2, 6, 3, 7, //up face
	0, 5, 4, 5, 0, 1, //down face
};
static const GLfloat quad_vertices[] =  {
	-1.f, -1.f,  1.f,
	 1.f, -1.f,  1.f,
	 1.f,  1.f,  1.f,
	-1.f,  1.f,  1.f,

	-1.f, -1.f, -1.f,
	 1.f, -1.f, -1.f,
	 1.f,  1.f, -1.f,
	-1.f,  1.f, -1.f,
};

class CubeMap {
public:
	CubeMap(std::string base_filename){
		//Load cubemap from file
		cubemap = loadCubeMap(base_filename, "tga");

		//Create program
		std::string vert = "\
			varying vec3 tex_coord;\
			uniform mat4 tex_rotate;\
			void main() {\
				vec4 pos = gl_ProjectionMatrix*gl_ModelViewMatrix * vec4(gl_Vertex.xyz*5.0, 1.0);\
				tex_coord = -gl_Vertex.xyz;\
				gl_Position = pos;\
			}";
		
		std::string frag = "\
			uniform samplerCube cube_tex;\
			varying vec3 tex_coord;\
			void main() {\
				gl_FragColor = textureCube(cube_tex, tex_coord);\
			}";
		
		const char* vertSource = vert.c_str();
		
		GLint vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
  		glShaderSource(vertexShaderId, 1, &vertSource, NULL);
  		glCompileShader(vertexShaderId);
		  		
  		const char* fragSource = frag.c_str();
  		
  		GLint fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
  		glShaderSource(fragmentShaderId, 1, &fragSource, NULL);
  		glCompileShader(fragmentShaderId);
		
		cubemap_program = glCreateProgram();
    	glAttachShader(cubemap_program, vertexShaderId);
    	glAttachShader(cubemap_program, fragmentShaderId);
    	glLinkProgram(cubemap_program);
		
		// Create buffer objects
		glGenBuffers(1, &vertices_buffer);
		glBindBuffer(GL_ARRAY_BUFFER, vertices_buffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), &quad_vertices[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		
		glGenBuffers(1, &indices_buffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_buffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quad_indices), &quad_indices[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
	~CubeMap(){
		glDeleteBuffers(1, &vertices_buffer);
		glDeleteBuffers(1, &indices_buffer);
	}

	void bind(){
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);
	}
	void unbind(){
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	}

	void render(){
		glMatrixMode(GL_MODELVIEW);
		
		glm::mat4 transform;
		glGetFloatv(GL_MODELVIEW_MATRIX, glm::value_ptr(transform));
		
		transform[3][0] = 0;
		transform[3][1] = 0;
		transform[3][2] = 0;
		
		glPushMatrix();
		
		glLoadMatrixf(glm::value_ptr(transform));
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);
		
		glEnableClientState(GL_VERTEX_ARRAY);

		glUseProgram(cubemap_program);
		glUniform1i(glGetUniformLocation(cubemap_program,"cube_tex"), 0);

		glBindBuffer(GL_ARRAY_BUFFER, vertices_buffer);
		glVertexPointer(3, GL_FLOAT, 0, NULL);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_buffer);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, NULL);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
		glUseProgram(0);
		
		glDisableClientState(GL_VERTEX_ARRAY);
		
		glPopMatrix();
	}

private:
	GLuint loadCubeMap(std::string base_filename, std::string extension) {
		const char name_exts[6][5] = {"posx", "negx", "posy", "negy", "posz", "negz"};
		const GLenum faces[6] = {GL_TEXTURE_CUBE_MAP_POSITIVE_X, GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
			GL_TEXTURE_CUBE_MAP_POSITIVE_Y, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
			GL_TEXTURE_CUBE_MAP_POSITIVE_Z, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z};
		GLuint cube_map_name;

		//Allocate texture name and set parameters
		glGenTextures(1, &cube_map_name);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cube_map_name);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		//Load each face and set texture
		for (int i=0; i<6; ++i) {
			std::stringstream filename;

			filename << base_filename << name_exts[i] << "." << extension;
			tgaInfo* img = tgaLoad(filename.str().c_str()); 
			if(img->status != 0){
				std::cout << img->status << std::endl;
			}
			
			glTexImage2D(faces[i], 0, GL_RGB, img->width, img->height, 
				0, GL_RGB, GL_UNSIGNED_BYTE, (void*)img->imageData);
			tgaDestroy(img);
		}

		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

		return cube_map_name;
	}

	GLuint cubemap;
	GLuint vertices_buffer;
	GLuint indices_buffer;
	GLint cubemap_program;
};

#endif
