/* $Id: ColoredCube.hpp, v1.1 2011/09/20$
 *
 * Author: Christopher Dyken, <dyken@cma.uio.no>
 * Reviewed by: Bartlomiej Siwek, <bartloms@ifi.uio.no>
 *
 * Distributed under the GNU GPL.
 */

#ifndef COLOREDCUBE_HEADER_H_
#define COLOREDCUBE_HEADER_H_

#include <GL/glew.h>

namespace GfxUtil {

class ColoredCube {
 public:
  ColoredCube();
  ~ColoredCube();
  
  void init(float size = 1.0f);
  void render();

 protected:
  GLuint m_vertices_vbo_id_;
  GLuint m_indices_vbo_id_;
  
  unsigned int m_vertex_count_;
  unsigned int m_index_count_;
};

}

#endif  // COLOREDCUBE_HEADER_H_
