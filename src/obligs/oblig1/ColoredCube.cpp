/* $Id: ColoredCube.cpp, v1.1 2011/09/20$
 *
 * Author: Christopher Dyken, <dyken@cma.uio.no>
 * Reviewed by: Bartlomiej Siwek, <bartloms@ifi.uio.no>
 *
 * Distributed under the GNU GPL.
 */

#include "ColoredCube.hpp"
#include <GL/glew.h>
#include "ColoredCubeVertex.hpp"

namespace GfxUtil {

ColoredCube::ColoredCube()
    : m_vertices_vbo_id_(0), m_indices_vbo_id_(0),
      m_vertex_count_(0), m_index_count_(0) {
}

ColoredCube::~ColoredCube() {
  if(m_indices_vbo_id_ != 0) {
    glDeleteBuffers(1, &m_indices_vbo_id_);
  }
  
  if(m_vertices_vbo_id_ != 0) {  
    glDeleteBuffers(1, &m_vertices_vbo_id_);
  }
}

void ColoredCube::init(float size) {
  // The vertex and index data
  m_vertex_count_ = 8;
  ColoredCubeVertex vertices[] = {
    ColoredCubeVertex(-1.0,  1.0,  1.0, 0.0, 0.0, 0.0),
    ColoredCubeVertex( 1.0,  1.0,  1.0, 0.0, 0.0, 1.0),
    ColoredCubeVertex( 1.0, -1.0,  1.0, 0.0, 1.0, 0.0),
    ColoredCubeVertex(-1.0, -1.0,  1.0, 0.0, 1.0, 1.0),
    ColoredCubeVertex(-1.0,  1.0, -1.0, 1.0, 0.0, 0.0),
    ColoredCubeVertex( 1.0,  1.0, -1.0, 1.0, 0.0, 1.0),
    ColoredCubeVertex( 1.0, -1.0, -1.0, 1.0, 1.0, 0.0),
    ColoredCubeVertex(-1.0, -1.0, -1.0, 1.0, 1.0, 1.0),
  };
  
  m_index_count_ = 24;
  unsigned int indices[] = {
    0, 3, 2, 1,
    1, 2, 6, 5,
    5, 6, 7, 4,
    4, 7, 3, 0,
    0, 1, 5, 4,
    2, 3, 7, 6,
  };
  
  // Scale the box
  for(unsigned int i = 0;i < m_vertex_count_;++i) {
    vertices[i].position *= size;
  }
  
  // Generate VAO and VBO ids 
  glGenBuffers(1, &m_vertices_vbo_id_);
  glGenBuffers(1, &m_indices_vbo_id_);
   
  // Bind VAO and VBOs and provide data to them  
  glBindBuffer(GL_ARRAY_BUFFER, m_vertices_vbo_id_);
  glBufferData(GL_ARRAY_BUFFER, m_vertex_count_ * sizeof(ColoredCubeVertex), &vertices[0], GL_STATIC_DRAW);
  glVertexPointer(3, GL_FLOAT, sizeof(ColoredCubeVertex), BUFFER_OFFSET(0));
  glColorPointer(3, GL_FLOAT, sizeof(ColoredCubeVertex), BUFFER_OFFSET(12));
  
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indices_vbo_id_);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_index_count_ * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
  
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void ColoredCube::render() {
  // Bind VAO and VBO's and call a drawing function  
  glBindBuffer(GL_ARRAY_BUFFER, m_vertices_vbo_id_);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indices_vbo_id_);
  
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_COLOR_ARRAY);

  glDrawElements(GL_QUADS, m_index_count_, GL_UNSIGNED_INT, BUFFER_OFFSET(0));
  
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

}
