/* $Id: TriMesh.hpp, v1.1 2011/10/17$
 *
 * Author: Christopher Dyken, <dyken@cma.uio.no>
 * Reviewed by: Bartlomiej Siwek, <bartloms@ifi.uio.no>
 *
 * Distributed under the GNU GPL.
 */

#ifndef GFXUTIL_TRIMESH_H
#define GFXUTIL_TRIMESH_H

#include <vector>
#include <string>
#include <GL/glew.h>
#include <glm/glm.hpp>

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

namespace GfxUtil {

/** Triangulation data structure using half-edges. */
class TriMesh {
 public:
  struct Node;
  struct Triangle;
  struct HalfEdge;

  /** Default constructor. */
  TriMesh();

  /** Constructor reading from a *.msh-file. */
  TriMesh(const std::string &filename);

  /** Constructor from a list of points and triangle indices. */
  TriMesh(const std::vector<glm::vec3> &points, const std::vector<int> &triangles);

  /** Destructor. */
  ~TriMesh();

  /** Reads a mesh from a .msh-file. Call this if you used the default constructor. */
  void readMesh(const std::string& filename);

  /** Returns the minimum x,y,z-values of the vertices. */
  const glm::vec3& getBBoxMin() const { return m_bbox_min; }

  /** Returns the maximum x,y,z-values of the vertices. */
  const glm::vec3& getBBoxMax() const { return m_bbox_max; }

  /** Finds suitable normal vectors for triangles
     *  and vertices. */
  void computeNormals();

  /** Renders the mesh according to the given RenderMode. */
  void render();

  /** Refines the mesh one step. */
  TriMesh* subdivide();

 protected:
  struct SortElement;  /// Helper struct for building connectivity.

  /** Builds the data structure from an indexed point set. */
  void buildTriangulation(const std::vector<glm::vec3>& points,
                          const std::vector<int>& indices);

  /** Calculates bounding-box. */
  void calcBBox();

  /** Finds the connectivity of the mesh, that is,
   *  finds twin half-edges. */
  void buildConnectivity();

  /** Refines the mesh one step using Loop-subdivision. */
  TriMesh* subdivideLoop();

  /** Refines the mesh one step using \sqrt{3}-subdivision. */
  TriMesh* subdivideSqrt3();
  
  /** Prepares VBO and EBO for drawing */
  void prepareBufferObjects(const std::vector<int>& indices);
  
  glm::vec3              m_bbox_min;   /// Minimum values of bounding box.
  glm::vec3              m_bbox_max;   /// Maximum values of bounding box.

  std::vector<Node>      m_nodes;      /// The nodes of this mesh.
  std::vector<Triangle*> m_triangles;  /// The triangles of this mesh.
  std::vector<HalfEdge*> m_halfedges;  /// The half-edges of this mesh.
  
  GLuint                 m_vertices_vbo_id;  /// Vertex buffer object id
  GLuint                 m_indices_vbo_id;   /// Element buffer object id
};


/** Struct containing per half-edge data. */
struct TriMesh::HalfEdge {
  /** Constructor.
   * \param from The source node of the triangle.
   * \param next The next half-edge in triangle loop.
   * \param face The triangle of which this half-edge belongs.
   * \param twin The other half-edge of this edge, or NULL if this half-edge
   *             is on the boundary.
   */
  HalfEdge(Node *from, Triangle *face, HalfEdge *next, HalfEdge *twin)
      : m_from_(from), m_face_(face), m_next_(next), m_twin_(twin) {
  }

  /** Returns true if half-edge is part of a boundary edge. */
  bool isBoundary() {
    return m_twin_ == NULL;
  }

  /** Get next half-edge in triangle loop. */
  HalfEdge* getNext() {
    return m_next_;
  }

  /** Get twin half-edge in neighbouring triangle. */
  HalfEdge* getTwin() {
    return m_twin_;
  }

  /** Returns the previous half-edge in the triangle loop.
   * \note Assumes that triangles are used. */
  HalfEdge* getPrev() {
    return m_next_->m_next_;
  }

  /** Returns the next half-edge from the same node in a counter-clockwise order.
   * 
   * This function  is used to iterate over all the connecting edges or
   * neighbouring vertices of a vertex.
   */
  HalfEdge* getVtxRingNext() {
    return getPrev()->m_twin_;
  }

  /** Returns the next half-edge from the same node in a clockwise order.
   * 
   * This function  is used to iterate over all the connecting edges or
   * neighbouring vertices of a vertex.
   */
  HalfEdge* getVtxRingPrev() {
    if(m_twin_ == NULL) {
      return NULL;
    }
    return m_twin_->m_next_;
  }

  /** Returns the triangle of the half-edge. */
  Triangle* getTriangle() {
    return m_face_;
  }

  /** Returns the source node of this half-edge. */
  Node* getSourceNode() {
    return m_from_;
  }

  /** Returns the destination node of this half-edge. */
  Node* getDestinationNode() {
    return m_next_->m_from_;
  }
      
  Node*     m_from_;  ///< The source node of this half-edge.
  Triangle* m_face_;  ///< The triangle of this half-edge.
  HalfEdge* m_next_;  ///< The next half-edge of this triangle.
  HalfEdge* m_twin_;  ///< The other half-edge of this edge.

  size_t    m_ix_;
};

/** Struct containing per node data. */
struct TriMesh::Node {
  /** Constructor.
   *  \param p The geometric position of the node. */
  Node(const glm::vec3 &p)
      : m_pos_(p), m_he_(NULL) {
  }

  /** Returns one half-edge from the node, if node is on boundary,
   *  it returns the first half-edge in anti-clockwise order. */
  HalfEdge *getLeadingHalfEdge() {
    return m_he_;
  }

  /** Returns true if node is on boundary. */
  bool isBoundary() {
    return m_he_->getVtxRingPrev() == NULL;
  }

  glm::vec3 m_pos_;  /// Position of vertex.
  glm::vec3 m_N_;    /// Vertex shading normal.
  HalfEdge* m_he_;   /// Leading half-edge.

  size_t    m_ix_;
};

/** Struct containing per triangle data. */
struct TriMesh::Triangle {
  /** Constructor
   * \param he One of the half-edges of this triangle.
   */
  Triangle(HalfEdge *he)
      : m_he_(he) {
  }

  /** Return node no ix from triangle.
   * \param ix is either 0, 1 or 2.
   */
  Node* getNode(size_t ix) {
    HalfEdge *he = m_he_;
    for(size_t i=0; i<ix; i++) {
      he = he->getNext();
    }
    return he->getSourceNode();
  }

  /** Returns one of the half-edges of this triangle. */
  HalfEdge* getLeadingHalfEdge() {
    return m_he_;
  }

  glm::vec3      m_N_;   /// Shading normal of triangle
  HalfEdge*      m_he_;  /// One of the half-edges of this triangle.
  size_t         m_ix_;
};

}  // GfxUtil

#endif
