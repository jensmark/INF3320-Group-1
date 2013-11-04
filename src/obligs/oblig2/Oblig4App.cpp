/* $Id: Oblig3App.cpp, v1.1 2011/09/20$
 *
 * Author: Atgeirr F Rasmussen <atgeirr@sintef.no>
 * Reviewed by: Bartlomiej Siwek, <bartloms@ifi.uio.no>
 *
 * Distributed under the GNU GPL.
 */

#include "Oblig4App.hpp"

#include <iostream>
#include <cstdlib>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "OpenGLError.hpp"
#include "ReadTextfile.hpp"

Oblig4App::Oblig4App()
    : m_viewer_(), m_meshes_(), m_current_level_() {
}

Oblig4App::~Oblig4App() {
  for(int mesh_index = 0;mesh_index < m_meshes_.size();++mesh_index) {
    delete m_meshes_[mesh_index];
  }
}

void Oblig4App::initGL() {
  glClearColor(0.0f, 0.0f, 0.5f, 0.0f);
  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_LIGHTING);
  
  std::string mesh_filename = "share/cube.msh";
  
  m_meshes_.reserve(10);
  m_meshes_.push_back(new GfxUtil::TriMesh(mesh_filename));
  setupBoundingBox(m_meshes_[0]->getBBoxMin(), m_meshes_[0]->getBBoxMax());
  
  m_current_level_ = 0;
  
  setupLightParameters();
}

void Oblig4App::display() {
  // Clear buffers
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Set up projection matrix
  glMatrixMode(GL_PROJECTION);
  glm::mat4x4 projection_matrix = m_viewer_.getProjectionMatrix();
  glLoadMatrixf(glm::value_ptr(projection_matrix));

  // Set the matrix mode to Modelview
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  // Light stationary with regard to camera (because MV matrix is empty)
  GLfloat light0_position[] = { 2.0f,  2.0f,  2.0f,  1.0f };
  glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
  
  // Set the MV matrix
  glm::mat4x4 model_view_matrix = m_viewer_.getModelViewMatrix();
	model_view_matrix = glm::translate(model_view_matrix, glm::vec3(0.0, 0.0, -1.0));
	model_view_matrix = glm::scale(model_view_matrix, glm::vec3(0.5, 0.5, 0.5));
  glLoadMatrixf(glm::value_ptr(model_view_matrix));
  
  // Position the directional light (stationary to the object)
  GLfloat light1_position[] = { 0.0f, -1.0f,  0.0f,  0.0f };
  glLightfv(GL_LIGHT1, GL_POSITION, light1_position);
  
  // Render the mesh
  setupMaterials();
  m_meshes_[m_current_level_]->render();
  
  glutSwapBuffers();
  CHECK_OPENGL;
}

void Oblig4App::reshape(int w, int h) {
  glViewport(0, 0, w, h);
  m_viewer_.setWindowSize(w,h);
}

void Oblig4App::keyboard(unsigned char key, int /*x*/, int /*y*/) { 
  switch (key) {
    case 'q':
      std::exit(0);
      break;
    case '+':
      ++m_current_level_;
      if (m_current_level_ >= int(m_meshes_.size())) {
        m_meshes_.push_back(m_meshes_.back()->subdivide());
      }
      break;
    case '-':
      if (m_current_level_) --m_current_level_;
      break;
  }
  glutPostRedisplay();
}

void Oblig4App::mouse(int b, int s, int x, int y) {
  if (s == GLUT_DOWN) {
    switch (b) {
      case GLUT_LEFT_BUTTON:   m_viewer_.rotationBegin(x, y); break;
      case GLUT_MIDDLE_BUTTON: m_viewer_.panBegin(x, y); break;
      case GLUT_RIGHT_BUTTON:  m_viewer_.zoomBegin(x, y); break;
    }
  } else {
    m_viewer_.resetState(x,y);
  }

  glutPostRedisplay();
}

void Oblig4App::motion(int x, int y) {
  m_viewer_.motion(x, y);
  glutPostRedisplay();
}


int Oblig4App::displayMode() {
  return GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH;
}

std::string Oblig4App::windowName() {
  return "Oblig 4";
}

void Oblig4App::setupLightParameters() {
  //Light atributes
  GLfloat black_light[] = {  0.0f,  0.0f,  0.0f,  1.0f };
  GLfloat red_light[]  = {  1.0f,  0.0f,  0.0f,  1.0f };
  GLfloat white_light[] = {  1.0f,  1.0f,  1.0f,  1.0f };
  
  //Setup light 0
  glLightfv(GL_LIGHT0, GL_DIFFUSE,  white_light);
  glLightfv(GL_LIGHT0, GL_SPECULAR, white_light);
  glLightfv(GL_LIGHT0, GL_AMBIENT,  black_light);
  
  // Enable the light
  glEnable(GL_LIGHT0);
  
    //Setup light 1
  glLightfv(GL_LIGHT1, GL_DIFFUSE,  red_light);
  glLightfv(GL_LIGHT1, GL_SPECULAR, black_light);
  glLightfv(GL_LIGHT1, GL_AMBIENT,  black_light);
  
  // Enable the light
  glEnable(GL_LIGHT1);
}

void Oblig4App::setupMaterials() {
  //Setup material properties
  GLfloat mat_ambient[]   = { 0.5, 0.3, 1.0, 1.0 };
  GLfloat mat_diffuse[]   = { 0.3, 0.5, 1.0, 1.0 };
  GLfloat mat_specular[]  = { 1.0, 1.0, 1.0, 1.0 };
  GLfloat mat_shininess   = 50.0 ;
  
  //Setup material
  glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
  glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
  glMaterialf(GL_FRONT, GL_SHININESS, mat_shininess);
}

void Oblig4App::setupBoundingBox(const glm::vec3& low, const glm::vec3& high) {
  glm::vec3 origin = 0.5f * (low + high);
  float radius = 0.5f * glm::length(high - low);
  m_viewer_.setViewVolume(origin, radius);
}
