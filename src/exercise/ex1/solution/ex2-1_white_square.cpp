/* $Id: ex2-1_white_square.cpp, v1.3 2011/09/05$
 * 
 * Author: Christopher Dyken, <dyken@cma.uio.no>
 * Reviewed by: Bartlomiej Siwek, <bartloms@ifi.uio.no>
 *
 * Distributed under the GNU GPL.
 */

#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include "OpenGLError.hpp"

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

// Vertex structure
struct Vertex { 
  Vertex(float x, float y)
      : position(x, y) {
  }
  
  Vertex(const glm::vec2 &p)
      : position(p) {
  }
  
  glm::vec2 position;
};

// Buffer indentifiers
GLuint verticesVertexBufferObjectId;

void myInit() {
  // Geometry - vertices and indices
  Vertex vertices[4] = {
    Vertex(-0.5f,  0.5f),
    Vertex( 0.5f,  0.5f),
    Vertex( 0.5f, -0.5f),
    Vertex(-0.5f, -0.5f),
  };
  
  // Setup buffer objects
  glGenBuffers(1, &verticesVertexBufferObjectId);
   
  // Fill the VBO's with data 
  glBindBuffer(GL_ARRAY_BUFFER, verticesVertexBufferObjectId);
  glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
  glVertexPointer(2, GL_FLOAT, sizeof(Vertex), BUFFER_OFFSET(0));
   
  // Deactivate VBO
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  
  // Setup a clear color
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  
  // Setup a projection
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
  
  // Setup a view
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  
  CHECK_OPENGL;
}

void myReshape(int w, int h) {
  glViewport(0, 0, (GLsizei)w, (GLsizei)h);
  CHECK_OPENGL;
}

void myDisplay() {
  // Clear the backgound
  glClear(GL_COLOR_BUFFER_BIT);
 
  // Bind VBO's
  glBindBuffer(GL_ARRAY_BUFFER, verticesVertexBufferObjectId);
  
  // Draw
  glColor3f(1.0f, 1.0f, 1.0f);
  glEnableClientState(GL_VERTEX_ARRAY);
  glDrawArrays(GL_QUADS, 0, 4);
  
  // Unbind VBO's
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  
  // Switch the buffer
  glFlush();
  glutSwapBuffers();
   
  CHECK_OPENGL;  
}

void myShutdown() {
  glDeleteBuffers(1, &verticesVertexBufferObjectId);
}

int main(int argc, char **argv) {
  // Initialization of GLUT
  glutInit(&argc, argv);
  glutInitContextVersion(3, 1);
  glutInitContextFlags(GLUT_DEBUG);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
  glutInitWindowSize(512, 512);
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
  
  // Attach handlers
  glutDisplayFunc(myDisplay);
  glutReshapeFunc(myReshape);
  
  // A nasty trick to get a shutdown handler
  atexit(myShutdown);
  
  // Application specific initialization
  myInit();
  
  // Run the GLUT main loop
  glutMainLoop();
  return 0;
}
