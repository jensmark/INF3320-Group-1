/* $Id: TexturedVertex.hpp, v1.0 2011/09/20$
 *
 * Author: Bartlomiej Siwek, <bartloms@ifi.uio.no>
 * 
 * Distributed under the GNU GPL.
 */

#ifndef TEXTUREDVERTEX_HEADER_H_
#define TEXTUREDVERTEX_HEADER_H_

#include <glm/glm.hpp>
#include <GL/glew.h>

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

struct TexturedVertex {
  TexturedVertex()
      : position(), normal(), texcoord() {
  }
  
  TexturedVertex(float x, float y, float z,
                 float nx, float ny, float nz,
                 float s, float t)
      : position(x, y, z), normal(nx, ny, nz), texcoord(s, t) {
  }
  
  TexturedVertex(glm::vec3 p, glm::vec3 n, glm::vec2 tc)
      : position(p), normal(n), texcoord(tc) {
  }
  
  glm::vec3 position;
  glm::vec3 normal;
  glm::vec2 texcoord;
};

#endif  // TEXTUREDVERTEX_HEADER_H_
