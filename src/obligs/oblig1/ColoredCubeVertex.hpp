/* $Id: ColoredCubeVertex.hpp, v1.0 2011/09/20$
 *
 * Author: Bartlomiej Siwek, <bartloms@ifi.uio.no>
 * 
 * Distributed under the GNU GPL.
 */

#ifndef COLOREDCUBEVERTEX_HEADER_H_
#define COLOREDCUBEVERTEX_HEADER_H_

#include <glm/glm.hpp>

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

struct ColoredCubeVertex {
  ColoredCubeVertex(float x, float y, float z,
                    float r, float g, float b)
      : position(x, y, z), color(r, g, b) {
  }
  
  ColoredCubeVertex(glm::vec3 p, glm::vec3 c)
      : position(p), color(c) {
  }
  
  glm::vec3 position;
  glm::vec3 color;
};

#endif  // COLOREDCUBEVERTEX_HEADER_H_
