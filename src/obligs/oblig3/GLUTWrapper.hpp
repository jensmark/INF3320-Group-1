/* $Id: GLUTWrapper.hpp, v1.1 2011/09/05$
 *
 * Author: Atgeirr F Rasmussen, <atgeirr@sintef.no>
 * Reviewed by: Bartlomiej Siwek, <bartloms@ifi.uio.no>
 * 
 * Distributed under the GNU GPL.
 */

#ifndef GLUTWRAPPER_HEADER_H_
#define GLUTWRAPPER_HEADER_H_

#include "GLApp.hpp"

#include <string>
#include <cstdlib>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>

// A class that encapsulates GLUT functionality.
class GLUTWrapper {
 public:
  // The method that runs the application.
  static int run(int argc, char** argv, GLApp& the_app) {
    the_app_ = &the_app;
    
    glutInit(&argc, argv);
    bool init_ok = app().init(argc, argv);
    if (!init_ok) {
      return EXIT_FAILURE;
    }

    glutInitContextVersion(app().contextVersion()[0], app().contextVersion()[1]);
    glutInitContextFlags(app().contextFlags());
    glutInitDisplayMode(app().displayMode());
    glutInitWindowSize(app().initWindowSize()[0], app().initWindowSize()[1]);
    glutCreateWindow(app().windowName().c_str());
    
    // Init GLEW
    GLenum err = glewInit();
    if (GLEW_OK != err) {
      return EXIT_FAILURE;
    } 

    if(!GLEW_VERSION_3_1) {
      return EXIT_FAILURE;
    }
    
    glutDisplayFunc(displayCallback);
    glutReshapeFunc(reshapeCallback);
    glutKeyboardFunc(keyboardCallback);
    glutMouseFunc(mouseCallback);
    glutMotionFunc(motionCallback);
    
    app().initGL();
    
    glutMainLoop();
    
    return EXIT_SUCCESS;
  }

 private:
  static GLApp* the_app_;
  
  // Returns the singleton instance of the class.
  static GLApp& app() {
    return *the_app_;
  }

  // GLUT display callback.
  static void displayCallback() {
    app().display();
  }
  
  // GLUT window reshape callback.
  static void reshapeCallback(int w, int h) {
    app().reshape(w, h);
  }
  
  // GLUT keyboard callback.
  static void keyboardCallback(unsigned char key, int x, int y) {
    app().keyboard(key, x, y);
  }
  
  // GLUT mouse button callback.
  static void mouseCallback(int button, int state, int x, int y) {
    app().mouse(button, state, x, y);
  }
  
  // GLUT mouse move callback.
  static void motionCallback(int x, int y) {
    app().motion(x, y);
  }
};

GLApp* GLUTWrapper::the_app_ = 0;

#endif  // GLUTWRAPPER_HEADER_H_
