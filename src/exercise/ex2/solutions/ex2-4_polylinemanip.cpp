/* $Id: ex2-4_polylinemanip.cpp, v1.3 2011/09/05$
 *
 * Author: Christopher Dyken, <dyken@cma.uio.no>
 * Reviewed by: Bartlomiej Siwek, <bartloms@ifi.uio.no>
 *
 * Distributed under the GNU GPL.
 */

#include <cstdlib>
#include <iostream>
#include <vector>
#include <limits>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include "OpenGLError.hpp"
#include "LineSegment.hpp"

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

// Vertex structure
struct Point { 
  Point(float x, float y)
      : position(x, y) {
  }
  
  Point(const glm::vec2 &p)
      : position(p) {
  }
  
  glm::vec2 position;
};

// Array of points defining our polygon.
static std::vector<Point> points;

// Buffer indentifiers
GLuint verticesVertexBufferObjectId;

// The currently selected point, -1 if no point is selected.
static int selected = -1;

// Maintain the width and height of the window.
static int width, height;

void bindBuffersToData() {
  // Fill the VBO's with data
  glBindBuffer(GL_ARRAY_BUFFER, verticesVertexBufferObjectId);
  glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(Point), &points[0], GL_STATIC_DRAW);
  glVertexPointer(2, GL_FLOAT, sizeof(Point), BUFFER_OFFSET(0));
    
  // Deactivate VBO
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  
  CHECK_OPENGL;
}

void myInit()
{
  // Create 10 points in a circle
  unsigned int n_points = 10;
  for(unsigned int i = 0;i < n_points;i++) {
    points.push_back(Point(0.75*cos(2.0*M_PI*((float)i/(float)n_points)),
                           0.75*sin(2.0*M_PI*((float)i/(float)n_points))
                          ));
  }
  
  // Setup buffer objects
  glGenBuffers(1, &verticesVertexBufferObjectId);
   
  // Bind buffers to data
  bindBuffersToData();
  
  // Setup a clear color
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  
  CHECK_OPENGL;
}

void myReshape(int w, int h) {
  glViewport(0, 0, (GLsizei)w, (GLsizei)h);
  width = w;
  height = h;
  CHECK_OPENGL;
}

/** Returns the position in modelview space that corresponds to a mouse position.
 * 
 * Since all matrices are set to unity, we only have to reverse
 * the viewport transformation.
 */
glm::vec2 getModelViewPoint(int x, int y) {
  return glm::vec2((2.0f*x)/(float)width-1, (-2.0f*y)/(float)height+1);  
}

/** Checks if the given position corresponds to selection of a point. 
 * 
 * Runs through all points and finds the point closest to p. If the
 * distance between the closest point and p is less than a given
 * tolerance, the index of the closest point is returned. Otherwise,
 * -1 is return to indicate that no points matched for selection.
 */
int checkForPointSelection(const glm::vec2& p) {
  int point = -1;
  float dist = std::numeric_limits<float>::max();

  for(size_t i = 0; i < points.size(); i++) {
    float d = glm::distance(points[i].position, p);
    if(d < 0.1 && d < dist) {
      dist = d;
      point = i;
    }
  }
  
  return point;
}

/** Checks if the given position corresponds to selection of a line segment.
 * 
 * Similar to checkForPointSelection. Returns the index of the first point
 * of the selected line segment, otherwise returns -1 to indicate that
 * no linesegments matched for selection.
 */
int checkForLineSegSelection(const glm::vec2& p) {
  int lineseg = -1;
  float dist = std::numeric_limits<float>::max();

  for(unsigned int i=0; i<points.size(); i++) {
    glm::vec2& p1 = points[i].position;
    glm::vec2& p2 = points[(i+1)%points.size()].position;
    GfxMath::LineSegment2f l(p1, p2);
    float d = l.distance(p);
    if( d < 0.1 && d < dist ) {
      dist = d;
      lineseg = i;
    }
  }
  return lineseg;
}

/** Handle mouse presses.
 * 
 * If left mousebutton is pressed down, check if a point should be selected,
 * and if so, select this point. Otherwise, check if a line segment should
 * be selected, and if so, insert a new point in the middle of this line segment
 * with the position of the mouse pointer and selected this new point.
 * 
 * If the right mousebutton is pressed down, check if a point should be selected,
 * and if so, remove the selected point.
 * 
 * If the mouse button is released, mark that no point is selected.
 * 
 */
void myMousePress(int b, int s, int x, int y) {
  glm::vec2 p = getModelViewPoint(x,y);

  if( s == GLUT_DOWN ) {
    if( b == GLUT_LEFT_BUTTON ) {
      selected = checkForPointSelection( p );
      if(selected == -1 ) {
        int lineseg = checkForLineSegSelection( p );
        if(lineseg != -1) {
          points.insert(points.begin()+lineseg+1, 1, Point(p));
          selected = lineseg+1;
          bindBuffersToData();
        }
      }
    } else if ( b == GLUT_RIGHT_BUTTON ) {
      int selected = checkForPointSelection(p);
      if(selected != -1 && points.size() > 2) {
        points.erase(points.begin() + selected);
        bindBuffersToData();
      }
    }
  } else {
    selected = -1;
  }

  glutPostRedisplay();
}

void myMouseMotion(int x, int y) {
  if(selected != -1) {
    points[selected].position = getModelViewPoint( x, y );
    bindBuffersToData();
  }
  
  glutPostRedisplay();
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

  // Bind VBO's
  glBindBuffer(GL_ARRAY_BUFFER, verticesVertexBufferObjectId);
  
  // Render polygon
  glColor3f(0.8f, 0.8f, 0.0f);
  glLineWidth(1.0f);
  glEnableClientState(GL_VERTEX_ARRAY);
  glDrawArrays(GL_LINE_LOOP, 0, points.size());

  // Render points
  glColor3f(1.0f, 1.0f, 0.0f);  
  glPointSize(10.0f);
  glEnableClientState(GL_VERTEX_ARRAY);
  glDrawArrays(GL_POINTS, 0, points.size());

  if(selected != -1) {
    glColor3f(1.0f, 1.0f, 1.0f);
    glPointSize(10.0f);
    glEnableClientState(GL_VERTEX_ARRAY);
    glDrawArrays(GL_POINTS, selected, 1);
  }
  
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
