/* $Id: SimpleViewer.hpp, v1.1 2011/09/20$
 *
 * Author: Christopher Dyken, <dyken@cma.uio.no>
 * Reviewed by: Bartlomiej Siwek, <bartloms@ifi.uio.no>
 *
 * Distributed under the GNU GPL.
 */

#ifndef SIMPLEVIEWER_HEADER_H_
#define SIMPLEVIEWER_HEADER_H_

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace GfxUtil {

class SimpleViewer {
 public:
  SimpleViewer();
  void setWindowSize(int w, int h);
  void setViewVolume(glm::vec3 origin, float radius);
   
  void rotationBegin(int x, int y);
  void panBegin(int x, int y);
  void zoomBegin(int x, int y);
  void resetState(int x, int y);
  void motion(int x, int y);

  glm::mat4x4 getProjectionMatrix();
  glm::mat4x4 getModelViewMatrix();
  void renderDebugGraphics();

private:
  enum { NONE, ROTATING, PANNING, ZOOMING } m_state;
 
  glm::vec2 getNormalizedCoords(int x, int y);
  glm::vec3 getPointOnUnitSphere(glm::vec2 p);
  glm::fquat getGreatCircleRotation(glm::vec3 a, glm::vec3 b);
  glm::fquat getAxisAngleRotation(glm::vec3& axis, float angle);
  
  // Window size and aspect
  int   m_window_width;   // Width of window
  int   m_window_height;  // Height of window
  float m_window_aspect;  // Aspect ratio of window.
  
  // View volume
  glm::vec3 m_viewvolume_origin;
  float     m_viewvolume_radius;
  float     m_viewvolume_epsilon;  // Small value used for epsilon tests.

  // Camera
  glm::fquat m_camera_orientation; // Orientation of camera.
  glm::vec3  m_camera_position;    // Pivot point.
  float      m_camera_distance;    // Distance from pivot point along local -Z.

  // Mouse position
  glm::vec2 m_unit_mousepos_i;
  glm::vec2 m_unit_mousepos_c;

  // Rotation state data
  glm::fquat m_rotation_orientation_i; // Orientation when mousebutton was pressed.
  glm::vec3  m_rotation_axis;          // Currently defined rotation axis.
  float      m_rotation_angle;         // Currently defined rotation angle.
  glm::vec3  m_rotation_mousepos_i;    // Mouseposition when mousebutton was pressed.
  glm::vec3  m_rotation_mousepos_c;    // Current mouseposition.

  // Pan state data
  glm::vec3 m_pan_position_i;

  // Zoom state data
  float m_zoom_distance_i;
};

}  // namespace GfxUtil

#endif  // SIMPLEVIEWER_HEADER_H_
