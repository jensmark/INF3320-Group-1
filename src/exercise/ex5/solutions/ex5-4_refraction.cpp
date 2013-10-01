/* $Id: ex5-2_refraction.cpp,v 1.1 2006/08/17 12:06:39 dyken Exp $
 *
 * (C) 2005 Christopher Dyken, <dyken@cma.uio.no>
 *
 * Distributed under the GNU GPL.
 */

#include <cstdlib>
#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

// Vertex structures
struct ColorVertex2D {
  ColorVertex2D(float x, float y, float r, float g, float b)
      : position(x, y), color(r, g, b) {
  }
  
  ColorVertex2D(glm::vec2 p, glm::vec3 c)
      : position(p), color(c) {
  }
  
  glm::vec2 position;
  glm::vec3 color;
};

struct ColorVertex3D {
  ColorVertex3D(float x, float y, float z, float r, float g, float b)
      : position(x, y, z), color(r, g, b) {
  }
  
  ColorVertex3D(glm::vec3 p, glm::vec3 c)
      : position(p), color(c) {
  }
  
  glm::vec3 position;
  glm::vec3 color;
};

// Global variables
static const unsigned int BACKGROUND_VERTEX_COUNT = 8;
static const unsigned int LINES_VERTEX_COUNT = 4;
static GLuint backgroundVertexBufferId;
static GLuint linesVertexBufferId;

static glm::vec3 incident(0.0, 1.0, 0.0);
static glm::vec3 refracted(0.0, 0.0, 0.0);

static int width, height;
static int button_down = 0;

/** Reflect dir around normal vector n.
 * 
 * \param[in] n   Unit surface normal.
 * \param[in] dir Unit direction to reflect.
 * 
 * \returns       The reflected direction.
 */
glm::vec3 reflect(const glm::vec3& n, const glm::vec3& dir) {
  // skip
  return 2.0f*glm::dot(dir, n)*n - dir;
  // unskip
}

/** Check for total reflection.
 * 
 * This happens when light passes from a dense material to a lesser
 * dense material and the incident angle is greater than the
 * critical angle, given by
 * \[
 *    \sin\theta_{\text{critical}} = \frac{\eta_r}{\eta_i} = \frac{1.0}{\eta}
 * \]
 * 
 * \param[in] n    Unit surface normal, points to same halfspace as dir
 *                 (i.e. dot(n,dir) >= 0.0).
 * \param[in] dir  Unit direction to check.
 * \param[in] eta  Refraction index of material we go from divided by refraction
 *                 index of material we try to refract into.
 * 
 * \returns        If a total reflection occours or not.
 */
bool totalReflection(const glm::vec3& n, const glm::vec3& v, float eta) {
  // skip
  return (glm::length(glm::cross(n,v)) >= 1.0/eta);
  // unskip
}

/** Refract direction
 * 
 * \param[in] n    Unit surface normal, points to same halfspace as dir
 *                 (i.e. dot(n,dir) >= 0.0).
 * \param[in] dir  Unit direction to check.
 * \param[in] eta  Refraction index of material we go from divided by refraction
 *                 index of material we try to refract into.
 * 
 * \returns        If a total reflection occours or not.
 */
glm::vec3 refract(const glm::vec3& n, const glm::vec3& v, float eta) {
  // skip
  float cos_theta = glm::dot(n,v);
  glm::vec3 ret = -eta*v + (eta*cos_theta - glm::sqrt(1.0f - eta*eta*(1.0f-cos_theta*cos_theta)))*n;
  return ret;
  // unskip
}

/*************************************************************
 **                                                         **
 **                      Test program                       **
 **                                                         **
 *************************************************************/

void bindBackgroundData() {
  ColorVertex2D bg_vertices[BACKGROUND_VERTEX_COUNT] = {
    ColorVertex2D(-1.0, 1.0, 0.0, 0.5, 1.0),
    ColorVertex2D( 1.0, 1.0, 0.0, 0.5, 1.0),
    ColorVertex2D( 1.0, 0.0, 0.5, 0.5, 1.0),
    ColorVertex2D(-1.0, 0.0, 0.5, 0.5, 1.0),
    ColorVertex2D(-1.0, 0.0, 0.0, 0.2, 1.0),
    ColorVertex2D( 1.0, 0.0, 0.0, 0.2, 1.0),
    ColorVertex2D( 1.0,-1.0, 0.0, 0.0, 0.2),
    ColorVertex2D(-1.0,-1.0, 0.0, 0.0, 0.2),
  };
  
  glBindBuffer(GL_ARRAY_BUFFER, backgroundVertexBufferId);
  glBufferData(GL_ARRAY_BUFFER, BACKGROUND_VERTEX_COUNT * sizeof(ColorVertex2D), &bg_vertices[0], GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void bindLinesData() {
  ColorVertex3D lines_vertices[LINES_VERTEX_COUNT] = {
    ColorVertex3D(incident[0], incident[1], incident[2], 1.0, 1.0, 1.0),
    ColorVertex3D(0.0, 0.0, 0.0, 1.0, 1.0, 0.0),
    ColorVertex3D(refracted[0], refracted[1], refracted[2], 1.0, 1.0, 1.0),
    ColorVertex3D(0.0, 0.0, 0.0, 1.0, 1.0, 0.0),
  };
  
  glBindBuffer(GL_ARRAY_BUFFER, linesVertexBufferId);
  glBufferData(GL_ARRAY_BUFFER, LINES_VERTEX_COUNT * sizeof(ColorVertex3D), &lines_vertices[0], GL_STATIC_DRAW); 
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void update() {
  glm::vec3 n;
  float eta;

  // check if we're in air or in water....
  if (incident[1] >= 0.0) {
    eta = 1.0002926f/1.33157f;
    n = glm::vec3(0.0, 1.0, 0.0);
  } else {
    eta = 1.33157f/1.0002926f;
    n = glm::vec3(0.0, -1.0, 0.0);
  }
  
  if (totalReflection(n, -incident, eta)) {
    refracted = reflect(n, incident);
  } else {
    refracted = refract(n, incident, eta);
  }
  
  bindLinesData();
}

void myInit() {
  glGenBuffers(1, &backgroundVertexBufferId);
  glGenBuffers(1, &linesVertexBufferId);
  
  bindBackgroundData(); 
  update();
  
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_COLOR_ARRAY);
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  glLineWidth(3.0f);
}

void myReshape(int w, int h) {
  glViewport(0, 0, (GLsizei)w, (GLsizei)h);
  width = w;
  height = h;
}

void myMouseMotion(int x, int y) {
  if(button_down == 1) {
    incident = glm::vec3(((2.0*x)/width)-1.0, ((-2.0*y)/height)+1.0, 0.0);
    incident = glm::normalize(incident);
    update();
  }

  glutPostRedisplay();
}

void myMousePress(int b, int s, int x, int y) {
  if (s == GLUT_DOWN) {
    if (b == GLUT_LEFT_BUTTON) {
      button_down = 1;
    } else if (b == GLUT_RIGHT_BUTTON) {
      button_down = 2;
    }
    myMouseMotion(x, y);
  } else {
    myMouseMotion(x, y);
    button_down = 0;
  }
}

void myKeyboard(unsigned char key, int /*x*/, int /*y*/) {
  switch(key) {
    case 'q':
      std::exit(0);
      break;
  }
  glutPostRedisplay();
}

void myDisplay() {
  glClear(GL_COLOR_BUFFER_BIT);

  // draw background
  glBindBuffer(GL_ARRAY_BUFFER, backgroundVertexBufferId);
  glVertexPointer(2, GL_FLOAT, sizeof(ColorVertex2D), BUFFER_OFFSET(0));
  glColorPointer(3, GL_FLOAT, sizeof(ColorVertex2D), BUFFER_OFFSET(8));
  glDrawArrays(GL_QUADS, 0, BACKGROUND_VERTEX_COUNT);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // draw lines
  glBindBuffer(GL_ARRAY_BUFFER, linesVertexBufferId);
  glVertexPointer(3, GL_FLOAT, sizeof(ColorVertex3D), BUFFER_OFFSET(0));
  glColorPointer(3, GL_FLOAT, sizeof(ColorVertex3D), BUFFER_OFFSET(12));
  glDrawArrays(GL_LINES, 0, LINES_VERTEX_COUNT);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glutSwapBuffers();
}

void myShutdown() {
  glDeleteBuffers(1, &backgroundVertexBufferId);
  glDeleteBuffers(1, &linesVertexBufferId);
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
  glutMouseFunc(myMousePress);
  glutMotionFunc(myMouseMotion);
  
  // A nasty trick to get a shutdown handler
  atexit(myShutdown);
  
  // Application specific initialization
  myInit();
  
  // Run the GLUT main loop
  glutMainLoop();
  return 0;
}
