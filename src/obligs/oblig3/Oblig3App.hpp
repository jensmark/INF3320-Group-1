/* $Id: Oblig3App.hpp, v1.1 2011/09/20$
 *
 * Author: Atgeirr F Rasmussen <atgeirr@sintef.no>
 * Reviewed by: Bartlomiej Siwek, <bartloms@ifi.uio.no>
 *
 * Distributed under the GNU GPL.
 */

#ifndef OBLIG3APP_HEADER_H_
#define OBLIG3APP_HEADER_H_

#include "GLApp.hpp"
#include "SimpleViewer.hpp"
#include "TexturedSphere.hpp"
#include "Shader.h"

class Oblig3App : public GLApp {
 public:
  void initGL();
  void display();
  void reshape(int w, int h);
  void keyboard(unsigned char key, int /*x*/, int /*y*/);
  void mouse(int b, int s, int x, int y);
  void motion(int x, int y);
  int displayMode();
  std::string windowName();

 protected:
  void setupShaders();
  void setupLightParameters();
  GfxUtil::SimpleViewer m_viewer_;
  GfxUtil::TexturedSphere m_sphere_;
  GfxUtil::Shader m_shader_;
};

#endif // OBLIG3APP_HEADER_H_
