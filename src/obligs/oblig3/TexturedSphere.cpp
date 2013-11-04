/* $Id: TexturedSphere.cpp, v1.0 2011/09/20$
 *
 * Author: Bartlomiej Siwek, <bartloms@ifi.uio.no>
 *
 * Distributed under the GNU GPL.
 */

#include "TexturedSphere.hpp"
#include <GL/glew.h>
#include <vector>
#include <GL/gl.h>
#include "TexturedVertex.hpp"

namespace GfxUtil {

TexturedSphere::TexturedSphere()
    : m_vertices_vbo_id_(0), m_indices_vbo_id_(0),
      m_vertex_count_(0), m_index_count_(0) {
}

TexturedSphere::~TexturedSphere() { 
  if(m_indices_vbo_id_ != 0) {  
    glDeleteBuffers(1, &m_indices_vbo_id_);
  }
  
  if(m_vertices_vbo_id_ != 0) {  
    glDeleteBuffers(1, &m_vertices_vbo_id_);
  }
}

void TexturedSphere::init(int stacks, int slices, float size) {
  // Build the sphere
  std::vector<TexturedVertex> vertices;
  std::vector<unsigned int> indices;
  build(stacks, slices, &vertices, &indices);
  
  // Scale
  for(unsigned int i = 0;i < m_vertex_count_;++i) {
    vertices[i].position *= size;
  }
  
  // Generate VBO ids 
  glGenBuffers(1, &m_vertices_vbo_id_);
  glGenBuffers(1, &m_indices_vbo_id_);
   
  // Bind VBOs and provide data to them  
  glBindBuffer(GL_ARRAY_BUFFER, m_vertices_vbo_id_);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indices_vbo_id_);
  
  glBufferData(GL_ARRAY_BUFFER, m_vertex_count_ * sizeof(TexturedVertex), &vertices[0], GL_STATIC_DRAW);    
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_index_count_ * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
  
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void TexturedSphere::render() {  
  // Bind VAO and VBO's and call a drawing function  
  glBindBuffer(GL_ARRAY_BUFFER, m_vertices_vbo_id_);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indices_vbo_id_);
 
  // Setup the strides, offsets etc.
  glVertexPointer(3, GL_FLOAT, sizeof(TexturedVertex), BUFFER_OFFSET(0));
  glNormalPointer(GL_FLOAT, sizeof(TexturedVertex), BUFFER_OFFSET(12));
  glTexCoordPointer(2, GL_FLOAT, sizeof(TexturedVertex), BUFFER_OFFSET(24));
  
  // Enable the above arrays
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_NORMAL_ARRAY);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  
  // TexturedVertex::BindVertexAttributes(0, 0, 0);
  glDrawElements(GL_TRIANGLES, m_index_count_, GL_UNSIGNED_INT, BUFFER_OFFSET(0));
  
  // Unbind VBOs
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  
  // Disbale arrays
  glDisableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_NORMAL_ARRAY);
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

void TexturedSphere::build(int stacks, int slices, std::vector<TexturedVertex> *vertices,
                           std::vector<unsigned int> *indices) {
  // Vertical step
  float phi_step = M_PI / stacks;

  // do not count the poles as rings
  int num_rings = stacks-1;

  // Compute vertices for each stack ring.
  for(unsigned int i = 1; i <= num_rings; ++i) {
    // Current vertical angle
    float phi = i*phi_step;

    // Vertices of ring
    float theta_step = 2.0f*M_PI / slices;
    for(unsigned int j = 0; j <= slices; ++j) {
      // Current horizontal angle
      float theta = j*theta_step;

      TexturedVertex v;

      // Convert from spherical to cartesian coords
      v.position[0] = glm::sin(phi) * glm::cos(theta);
      v.position[1] = glm::cos(phi);
      v.position[2] = glm::sin(phi) * glm::sin(theta);
      
      // Normal is equal to position for unit sphere
      v.normal = v.position;

      // Compute texture coords
      v.texcoord[0] = 1.0f - theta / (2.0f * M_PI);
      v.texcoord[1] = phi / M_PI;

      vertices->push_back(v);
    }
  }

  // Poles
  vertices->push_back(TexturedVertex(0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f));
  vertices->push_back(TexturedVertex(0.0f,  1.0f, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f, 0.0f));

  // Pole indices
  unsigned int north_pole_index = vertices->size()-1;
  unsigned int south_pole_index = vertices->size()-2;

  unsigned int num_ring_vertices = slices+1;

  // Compute indices for inner stacks (not connected to poles).
  for(unsigned int i = 0; i < stacks-2; ++i) {
    for(unsigned int j = 0; j < slices; ++j) {
      indices->push_back( i    * num_ring_vertices + j);
      indices->push_back( i    * num_ring_vertices + j+1);
      indices->push_back((i+1) * num_ring_vertices + j);

      indices->push_back((i+1) * num_ring_vertices + j);
      indices->push_back(  i   * num_ring_vertices + j+1);
      indices->push_back((i+1) * num_ring_vertices + j+1);
    }
  }

  // Compute indices for top stack
  for(unsigned int i = 0; i < slices; ++i) {
    indices->push_back(north_pole_index);
    indices->push_back(i+1);
    indices->push_back(i);
  }

  // Compute indices for bottom stack
  unsigned int base_index = (num_rings-1)*num_ring_vertices;
  for(unsigned int i = 0; i < slices; ++i) {
    indices->push_back(south_pole_index);
    indices->push_back(base_index+i);
    indices->push_back(base_index+i+1);
  }
  
  // Set the sizes
  m_vertex_count_ = vertices->size();
  m_index_count_ = indices->size();
}

}
