/* $Id: ex15_mandel_brot.cpp, v1 2013/11/16$
 * 
 * Author: Jens Kristoffer Reitan Markussen, <jenskre@ifi.uio.no>
 *
 * Distributed under the GNU GPL.
 */

#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/freeglut_ext.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "OpenGLError.hpp"
#include "ReadTextfile.hpp"

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

//Buffer objects
GLuint indicesBO = 0;
GLuint verticesBO = 0;

//Shaders source
std::string vertexShader = "";
std::string fragmentShader = "";

//Shader OpenGL identifies
GLint vertexShaderId = -1;
GLint fragmentShaderId = -1;
GLint shaderProgramId = -1;

//Fractal positions
glm::vec2 position_start = glm::vec2(0.0f);
glm::vec2 position = glm::vec2(0.0f);

//Mouse position
glm::vec2 mouse_start = glm::vec2(0.0f);

//Zoom level
float zoom = 1.0f;

//Is user interacting
bool moving = false;

//Fractal iterations
int iterations = 128;

//Window size
unsigned int window_width = 800;
unsigned int window_height = 600;

//Vertices to render a quad
static GLfloat quad_vertices[] =  {
	-1.f, -1.f,
	 1.f, -1.f,
	 1.f,  1.f,
	-1.f,  1.f,
};

//Indices to create the quad
static GLubyte quad_indices[] = {
	0, 1, 2, //triangle 1
	2, 3, 0, //triangle 2
};

void setupBuffers()
{
  glGenBuffers(1, &verticesBO);
  glGenBuffers(1, &indicesBO);
   
  // Fill the VBO's with data 
  glBindBuffer(GL_ARRAY_BUFFER, verticesBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*8, &quad_vertices[0], GL_STATIC_DRAW);
   
  // Deactivate VBO
  glBindBuffer(GL_ARRAY_BUFFER, 0);
   
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLubyte)*6, &quad_indices[0], GL_STATIC_DRAW);
   
  // Deactivate VBO
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void setupShaders()
{  
  //Variable to hold compile status
  GLint compileStatus;
  
  //Vertex shader
  const GLchar *vertexShaderSourceTemp = vertexShader.c_str();
  
  vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShaderId, 1, &vertexShaderSourceTemp, NULL);
  glCompileShader(vertexShaderId);
  
  glGetShaderiv(vertexShaderId, GL_COMPILE_STATUS, &compileStatus);
  if(!compileStatus)
  {
    GLint length;
    GLchar *log;
    
    glGetShaderiv(vertexShaderId, GL_INFO_LOG_LENGTH, &length);
    log = new GLchar[length];
    glGetShaderInfoLog(vertexShaderId, length, &length, log);
    std::cout << "vertex shader compile log:" << std::endl << log << std::endl;
    delete [] log;
    
    glDeleteShader(vertexShaderId);
    vertexShaderId = -1;
  }
  
  //Fragment shader
  const GLchar *fragmentShaderSourceTemp = fragmentShader.c_str();  
  
  fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShaderId, 1, &fragmentShaderSourceTemp, NULL);
  glCompileShader(fragmentShaderId);
  
  glGetShaderiv(fragmentShaderId, GL_COMPILE_STATUS, &compileStatus);
  if(!compileStatus)
  {
    GLint length;
    GLchar *log;
    
    glGetShaderiv(fragmentShaderId, GL_INFO_LOG_LENGTH, &length);
    log = new GLchar[length];
    glGetShaderInfoLog(fragmentShaderId, length, &length, log);
    std::cout << "Fragment shader compile log:" << std::endl << log << std::endl;
    delete [] log;
    
    glDeleteShader(fragmentShaderId);
    fragmentShaderId = -1;
  }
  
  //Shader program
  if(vertexShaderId != -1 && fragmentShaderId != -1)
  {
    shaderProgramId = glCreateProgram();
    glAttachShader(shaderProgramId, vertexShaderId);
    glAttachShader(shaderProgramId, fragmentShaderId);
    glLinkProgram(shaderProgramId);
    
    GLint linkStatus;
    glGetProgramiv(shaderProgramId, GL_LINK_STATUS, &linkStatus);
    if(!linkStatus)
    {
      GLint length;
      GLchar *log;
      
      glGetProgramiv(shaderProgramId, GL_INFO_LOG_LENGTH, &length);
      log = new GLchar[length];
      glGetProgramInfoLog(shaderProgramId, length, &length, log);
      std::cout << "Shader program link log:" << std::endl << log << std::endl;
      delete [] log;
      
      glDeleteProgram(shaderProgramId);
      shaderProgramId = -1;      
    }
  }
  
  glUseProgram(shaderProgramId);
	glUniform2fv(glGetUniformLocation(shaderProgramId,"position"), 1, glm::value_ptr(position));
	glUniform1f(glGetUniformLocation(shaderProgramId,"zoom"), zoom);
	glUniform1f(glGetUniformLocation(shaderProgramId,"iterations"), iterations);
	glUseProgram(0);
}


void myInit() {
  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);
	glEnable(GL_CULL_FACE);
	glEnable(GL_MULTISAMPLE);

  //Compile shaders
  setupShaders();
  
  //Create buffer objects
  setupBuffers();
  
  CHECK_OPENGL;
}

void myDisplay() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
 	
 	glUseProgram(shaderProgramId);
	glUniform2fv(glGetUniformLocation(shaderProgramId,"position"), 1, glm::value_ptr(position));
	glUniform1f(glGetUniformLocation(shaderProgramId,"zoom"), zoom);
	glUniform1f(glGetUniformLocation(shaderProgramId,"iterations"), iterations);
  
  glEnableClientState(GL_VERTEX_ARRAY);
  
  glBindBuffer(GL_ARRAY_BUFFER, verticesBO);
  glVertexPointer(2, GL_FLOAT, 0, BUFFER_OFFSET(0));
  
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesBO);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, BUFFER_OFFSET(0));
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  
  glDisableClientState(GL_VERTEX_ARRAY);
  
  glUseProgram(0);
  
  glFlush();
  glutSwapBuffers();  
  CHECK_OPENGL;  
}

void myShutdown() {
  glDeleteBuffers(1, &indicesBO);
  glDeleteBuffers(1, &verticesBO);
  
  glDeleteProgram(shaderProgramId);
}

void myMousePress(int b, int s, int x, int y) {
  if( s == GLUT_DOWN ) {
    if(b == GLUT_LEFT_BUTTON ) {
      moving = true;
			mouse_start = glm::vec2(-x, y);
    }else if(b == 3){
    	zoom *= 1.0/1.1;
    }else if(b == 4){
    	zoom *= 1.1;
    }
  }else{
  	moving = false;
  	position_start = position;
  }
  glutPostRedisplay();
}

void myMouseMotion(int x, int y) {
	if (moving) {
		position = position_start + 2.0f*
			(glm::vec2(-x, y) - mouse_start)/static_cast<float>(window_width)*zoom;
		//std::cout << position.x << ", " << position.y << std::endl;
	}
  
  glutPostRedisplay();
}

void myKeyboard(unsigned char key, int /*x*/, int /*y*/)
{
	switch(key){
		case 'q':
			exit(0);
			break;
		case '+':
			iterations = iterations*1.1;
			std::cout << iterations << " iterations" << std::endl;
			break;
		case '-':
			iterations = iterations*1.0/1.1;
			std::cout << iterations << " iterations" << std::endl;
			break;
	}
	glutPostRedisplay();
}

int main(int argc, char **argv) {
  //Load vertex shader
  vertexShader = GfxUtil::ReadTextfile("mandelbrot.vert");
  
  //Load fragment shader
  fragmentShader = GfxUtil::ReadTextfile("mandelbrot.frag");
  
  glutInit(&argc, argv);
  glutInitContextVersion(3, 1);
  glutInitContextFlags(GLUT_DEBUG);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_MULTISAMPLE);
  glutInitWindowPosition(0,0);
  glutInitWindowSize(window_width, window_height);
  glutCreateWindow( __FILE__ );

  // Init GLEW
  GLenum err = glewInit();
  if (GLEW_OK != err) {
    std::cerr << "Error: " << glewGetErrorString(err) << std::endl;
    return 1;
  } 

  if(!GLEW_VERSION_3_1) {
  	std::cerr << "Driver does not support OpenGL 3.1" << std::endl;  
    return 1;
  } 
	
  glutDisplayFunc(myDisplay);
  glutKeyboardFunc(myKeyboard);
  glutMouseFunc(myMousePress);
  glutMotionFunc(myMouseMotion);
  
  // A nasty trick to get a shutdown handler
  atexit(myShutdown);
  
  myInit();
  glutMainLoop();
  return 0;
}
