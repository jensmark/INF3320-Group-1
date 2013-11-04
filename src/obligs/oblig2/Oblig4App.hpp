/* $Id: Oblig3App.hpp, v1.1 2011/09/20$
 *
 * Author: Atgeirr F Rasmussen <atgeirr@sintef.no>
 * Reviewed by: Bartlomiej Siwek, <bartloms@ifi.uio.no>
 *
 * Distributed under the GNU GPL.
 */

#ifndef OBLIG4APP_HEADER_H_
#define OBLIG4APP_HEADER_H_

#include "GLApp.hpp"
#include "SimpleViewer.hpp"
#include "TriMesh.hpp"

class Oblig4App : public GLApp {
 public:
  Oblig4App();
  ~Oblig4App();
  
  void initGL();
  void display();
  void reshape(int w, int h);
  void keyboard(unsigned char key, int /*x*/, int /*y*/);
  void mouse(int b, int s, int x, int y);
  void motion(int x, int y);
  int displayMode();
  std::string windowName();

 protected:
  void setupLightParameters();
  void setupMaterials();
  void setupBoundingBox(const glm::vec3& low, const glm::vec3& high);
  
  GfxUtil::SimpleViewer m_viewer_;
  std::vector<GfxUtil::TriMesh*> m_meshes_;
  int m_current_level_;
};

#endif // OBLIG4APP_HEADER_H_
