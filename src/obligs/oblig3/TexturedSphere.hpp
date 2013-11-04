/* $Id: TexturedSphere.hpp, v1.0 2011/09/20$
 *
 * Author: Bartlomiej Siwek, <bartloms@ifi.uio.no>
 *
 * Distributed under the GNU GPL.
 */

#ifndef TEXTUREDSPHERE_HEADER_H_
#define TEXTUREDSPHERE_HEADER_H_

#include <vector>
#include <GL/glew.h>
#include "TexturedVertex.hpp"

namespace GfxUtil {

class TexturedSphere {
 public:
  TexturedSphere();
  ~TexturedSphere();
  
  void init(int stacks = 12, int slices = 12, float size = 1.0f);
  void render();

 protected:
  void build(int stacks, int slices, std::vector<TexturedVertex> *vertices, std::vector<unsigned int> *indices);
  
  GLuint m_vertices_vbo_id_;
  GLuint m_indices_vbo_id_;
  unsigned int m_vertex_count_;
  unsigned int m_index_count_;
};

}

#endif  // TEXTUREDSPHERE_HEADER_H_
