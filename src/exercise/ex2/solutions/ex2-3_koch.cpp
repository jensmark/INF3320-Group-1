/* $Id: ex2-3_koch.cpp, v1.4 2011/09/05$
 *
 * Author: Christopher Dyken, <dyken@cma.uio.no>
 * Reviewed by: Bartlomiej Siwek, <bartloms@ifi.uio.no>
 *
 * Distributed under the GNU GPL.
 */

#include <cstdlib>
#include <iostream>
#include <vector>
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

// Koch snowflake level
static int level = 3;
static int point_count = 0;

// Buffer indentifiers
GLuint verticesVertexBufferObjectId;

void buildLine(glm::vec2 &a, glm::vec2 &b, int level, std::vector<Vertex> &vertices) {
  if(level) {
      glm::vec2 t = 0.288675f * (b - a); // sqrt(3)/6

      glm::vec2 ab = 2.0f / 3.0f * a + 1.0f / 3.0f * b;
      glm::vec2 m  = 0.5f * a + 0.5f * b + glm::vec2(t[1], -t[0]);
      glm::vec2 ba = 1.0f / 3.0f * a + 2.0f / 3.0f * b;
    
      buildLine(a, ab, level-1, vertices);
      buildLine(ab, m, level-1, vertices);
      buildLine(m, ba, level-1, vertices);
      buildLine(ba, b, level-1, vertices);
  } else {
    vertices.push_back(Vertex(a));
    ++point_count;
  }
}

void rebuildKochSnowflake() {
  // Geometry
  std::vector<Vertex> vertices;
  point_count = 0;
  
  glm::vec2 a(cos(M_PI*(1.0/6.0)), sin(M_PI*(1.0/6.0)));
  glm::vec2 b(cos(M_PI*(5.0/6.0)), sin(M_PI*(5.0/6.0)));
  glm::vec2 c(cos(M_PI*(9.0/6.0)), sin(M_PI*(9.0/6.0)));

  buildLine(a, b, level, vertices);
  buildLine(b, c, level, vertices);
  buildLine(c, a, level, vertices);  
  
  // Fill the VBO's with data  
  glBindBuffer(GL_ARRAY_BUFFER, verticesVertexBufferObjectId);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
  glVertexPointer(2, GL_FLOAT, sizeof(Vertex), BUFFER_OFFSET(0));
  
  // Deactivate VBO
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  
  CHECK_OPENGL;
}

void myInit() {  
  // Setup buffer objects
  glGenBuffers(1, &verticesVertexBufferObjectId);
   
  // Construct the koch snowflake
  rebuildKochSnowflake();
  
  // Setup a clear color
  glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
  
  // Setup a projection
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(-1.2, 1.2, -1.2, 1.2, -1.0, 1.0);
  
  // Setup a view
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  
  CHECK_OPENGL;
}

void myReshape(int w, int h)
{
  glViewport(0, 0, (GLsizei)w, (GLsizei)h);
  CHECK_OPENGL;
}

void
myKeyboard(unsigned char key, int /*x*/, int /*y*/)
{
  switch(key) {
    case 'q':
      std::exit(0);
      break;
    case '+':
      level++;
      rebuildKochSnowflake();
      break;
    case '-':
      level = level > 0 ? level-1 : 0;
      rebuildKochSnowflake();
      break;
  }
  
  glutPostRedisplay();
}

void myDisplay()
{
  glClear(GL_COLOR_BUFFER_BIT);

  // Bind VBO's
  glBindBuffer(GL_ARRAY_BUFFER, verticesVertexBufferObjectId);
  
  // Draw
  glColor3f(0.0f, 0.0f, 0.0f);
  glLineWidth(2.0f);
  glEnableClientState(GL_VERTEX_ARRAY);
  glDrawArrays(GL_LINE_LOOP, 0, point_count);
  
  // Unbind VBO's
  glBindBuffer(GL_ARRAY_BUFFER, 0);

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
  glutKeyboardFunc(myKeyboard);

  // A nasty trick to get a shutdown handler
  atexit(myShutdown);
  
  // Application specific initialization
  myInit();
  
  // Run the GLUT main loop
  glutMainLoop();
  return 0;
}
