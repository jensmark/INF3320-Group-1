/* $Id: GLApp.hpp, v1.1 2011/09/05$
 *
 * Author: Atgeirr F Rasmussen, <atgeirr@sintef.no>
 * Reviewed by: Bartlomiej Siwek, <bartloms@ifi.uio.no>
 *
 * Distributed under the GNU GPL.
 */

#ifndef GLAPP_HEADER_H_
#define GLAPP_HEADER_H_

#include <string>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>

// A base class suitable for use with GLUTWrapper.
class GLApp {
 public:
    /* A virtual destructor is necessary, so that any inherited class destructor
     * will be called through the instance pointer. Override if you claim any
     * non-automatic resources in the class.
     */
    virtual ~GLApp() {
    }
    
    /* Override init to do your own setup. Do not call OpenGL functions here, since
     * at this point the GL context does not exist. Put OpenGL init in initGL() instead.
     */
    virtual bool init(int, char**) {
      return true;
    }
    
    /* Override init to do your own GL setup. This function will be called after
     * GLUT has created an OpenGL context.
     */
    virtual void initGL() {
    }

    // Override to specify a different GL display mode.
    virtual int displayMode() {
      return GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH;
    }

    // Override to specify a different GL context flags.
    virtual int contextFlags() {
      return GLUT_DEBUG;
    }
    
    // Override to specify a different OpenGL vesion.
    virtual glm::ivec2 contextVersion() {
      return glm::ivec2(3, 1);
    }
    
    // Override to specify a different default window size.
    virtual glm::ivec2 initWindowSize() {
      return glm::ivec2(512, 512);
    }
    
    // Override to specify a different window name.
    virtual std::string windowName() {
      return "OpenGL application";
    }

    // ---- Callback overrides ----

    // Override to customize display.
    virtual void display() {
    }
    
    // Override to customize actions on window resizing.
    virtual void reshape(int, int) {
    }
    
    // Override to do your own keyboard processing.
    virtual void keyboard(unsigned char, int, int) {
    }
    
    // Override to do your own mouse button processing.
    virtual void mouse(int, int, int, int) {
    }
    
    // Override to do your own mouse drag processing.
    virtual void motion(int, int) {
    }
};

#endif  // GLAPP_HEADER_H_
