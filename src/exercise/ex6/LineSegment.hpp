/* $Id: LineSegment.hpp, v1.6 2011/09/05$
 *
 * Author: Christopher Dyken, <dyken@cma.uio.no>
 * Reviewed by: Bartlomiej Siwek, <bartloms@ifi.uio.no>
 * 
 * Distributed under the GNU GPL.
 */

#ifndef LINESEGMENT_H_
#define LINESEGMENT_H_

#include <limits>
#include <glm/glm.hpp>

namespace GfxMath {
  
/** Represents a 2D line segment.
 * 
 * Some computations can be speed up by using bounding boxes. However,
 * for sake of clarity, this is not utilized by this implementation.
 */
template<class type>
class LineSegment {
 public:
  typedef glm::detail::tvec2<type> Point; // Helper type.

  /** Creates a line segment between two points
   * 
   * \param p1 The first point.
   * \param p2 The second point.
   * \param epsilon A suitable tolerance; segments shorter than epsilon are
   *                considered degenerate.
   */
  LineSegment(const Point& p1, const Point& p2, type epsilon = std::numeric_limits<type>::epsilon())
      : m_degenerate(false), m_p1(p1), m_p2(p2) {
    // Explicit representation
    m_p1p2 = p2 - p1;

    // Check if line segment is degenerate
    if (glm::length(m_p1p2) < epsilon) {
      m_degenerate = true;
    }

    // implicit representation
    m_N[0] = -m_p1p2[1];
    m_N[1] =  m_p1p2[0];
    m_D    = -glm::dot(m_N, p1);
  }

  /** Calculate distance from a point to the line segment.
   * 
   * First, the point is projected onto the infinte line defined by the line
   * segment. If the projected point is contained in the segment, the distance
   * between the projected point and the point is used.
   * 
   * If the projected point is not contained in the segment, the distance
   * between the closest end-point and point is used.
   * 
   * \param   p The point.
   * \returns   The shortest distance from a point to the line segment. If
   *            the line segment is degeneratge, zero is returned.
   */
  type distance(const Point& p) const {
    if (m_degenerate) {
      return 0;
    }

    Point p1p  = p - m_p1;
    Point p1p2 = m_p2 - m_p1;
    type len = glm::dot(p1p2, p1p2);

    // shortest doesn't project onto the line segment
    type t = glm::dot(p1p,p1p2)/len;
    if (t < 0.0) {
      return glm::distance(p, m_p1);
    } else if (t > 1.0) {
      return glm::distance(p, m_p2);
    }

    Point i = (1.0f - t)*m_p1 + t*m_p2;
    return glm::distance(p, i);
  }

  /** Returns true if the line segment is degenerate.
   */
  bool isDegenerate() const {
    return m_degenerate;
  }

  /** Return line normal.
   */
  const glm::detail::tvec2<type>& getNormal() const {
    return m_N;
  }
    
  /** Return unscaled direction of line segment.
    */
  const glm::detail::tvec2<type>& getDirection() const {
    return m_p1p2;
  }

  /** Return the first point of the line segment.
   */
  const glm::detail::tvec2<type>& getPoint1() const {
    return m_p1;
  }

  /** Return the second point of the line segment.
   */
  const glm::detail::tvec2<type>& getPoint2() const {
    return m_p2;
  }

  /** Return the offset in the implicit equation of the line segment.
   */
  type getOffset() const {
    return m_D;
  }

protected:
  bool  m_degenerate;  // Flag to tag the line segment as degenerate.
  Point  m_p1;         // Endpoint at t=0.
  Point  m_p2;         // Endpoint at t=1.
  Point  m_p1p2;       // Vector from p1 to p2.
  Point  m_N;          // Normal vector component of implicit representation.
  type  m_D;           //Distance component of implicit representation.

  // Keep off.
  LineSegment();

  // Keep off.
  LineSegment(const LineSegment&);
};

/** Checks if two line segments intersects.
 * 
 * - Fails if either of the line segments are degenerate.
 * - Then the intersection of the two infinte lines are calculated and
 *   projected onto the line segments.
 * 
 * \param    t  The parameter position of intersection along l2.
 * \returns  True if segments intersect, false otherwise.
 */
template<class type>
bool intersects(const LineSegment<type>& l1, const LineSegment<type>& l2) {
  if(l1.isDegenerate() || l2.isDegenerate()) {
    return false;
  }
  type t = -(dot(l2.getNormal(), l1.getPoint1())+l2.getOffset())/dot(l2.getNormal(), l1.getDirection());
  if(t < 0 || t > 1) {
    return false;
  }
  t = -(dot(l1.getNormal(), l2.getPoint1())+l1.getOffset())/dot(l1.getNormal(), l2.getDirection());
  if(t < 0 || t > 1) {
    return false;
  }
  return true;
}

/** Checks if two line segments intersects, and if so, returns where.
 * 
 * - Fails if either of the line segments are degenerate.
 * - Then the intersection of the two infinte lines are calculated and
 *   projected onto the line segments.
 * 
 * \param   i The position of the intersection is stored here.
 * \returns   True if segments intersect, false otherwise.
 */
template<class type>
bool intersects(typename LineSegment<type>::Point& i, const LineSegment<type>& l1, const LineSegment<type>& l2) {
  if(l1.m_degenerate || l2.m_degenerate) {
    return false;
  }
  type t = -(dot(l2.getNormal(), l1.getPoint1)+l2.getOffset())/dot(l2.getNormal(), l1.getDirection());
  if(t < 0 || t > 1) {
    return false;
  }
  t = -(dot(l1.getNormal(), l2.getPoint1())+l1.getOffset())/dot(l1.getNormal(), l2.getDirection());
  if(t < 0 || t > 1) {
    return false;
  }
  i = lerp(l2.getPoint1(), l2.getPoint2(), t);      
  return true;
}

typedef LineSegment<float> LineSegment2f;  // Convenience typedef.
typedef LineSegment<double> LineSegment2d;  // Convenience typedef.

} // namespace GfxMath

#endif  // LINESEGMENT_H_
