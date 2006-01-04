
#ifndef GPS_BPS_BASE_VISITOR_H
#define GPS_BPS_BASE_VISITOR_H

#include <CGAL/Unique_hash_map.h> 

CGAL_BEGIN_NAMESPACE

template <class Arrangement_>
class Gps_bfs_base_visitor
{
  typedef  Arrangement_                                  Arrangement;
  typedef typename Arrangement::Face_iterator            Face_iterator;
  typedef typename Arrangement::Halfedge_iterator        Halfedge_iterator;
public:
  typedef Unique_hash_map<Halfedge_iterator, unsigned int> Edges_hash;
  typedef Unique_hash_map<Face_iterator, unsigned int>     Faces_hash;

protected:
  Edges_hash*    m_edges_hash;
  Faces_hash*    m_faces_hash;

public:

  Gps_bfs_base_visitor(Edges_hash* edges_hash, Faces_hash* faces_hash): 
    m_edges_hash(edges_hash),
    m_faces_hash(faces_hash)
  {}


  void flip_face(Face_iterator f1, Face_iterator f2, Halfedge_iterator he)
  {
    CGAL_assertion(m_edges_hash->is_defined(he) && 
                   m_edges_hash->is_defined(he->twin()) &&
                   m_faces_hash->is_defined(f1) &&
                   !m_faces_hash->is_defined(f2));

    // IC of f2 (inside counter)
    unsigned int ic_f2 = 
      (*m_faces_hash)[f1] - (*m_edges_hash)[he] + (*m_edges_hash)[he->twin()];
    (*m_faces_hash)[f2] = ic_f2;
  }

protected:

  // compute the inside count of a face
  unsigned int compute_ic(Face_iterator f1, Face_iterator f2, Halfedge_iterator he)
  {
    CGAL_assertion(m_edges_hash->is_defined(he) && 
                   m_edges_hash->is_defined(he->twin()) &&
                   m_faces_hash->is_defined(f1) &&
                   !m_faces_hash->is_defined(f2));
    unsigned int ic_f2 = 
      (*m_faces_hash)[f1] - (*m_edges_hash)[he] + (*m_edges_hash)[he->twin()];
    (*m_faces_hash)[f2] = ic_f2;

    return (ic_f2);
  }
};

CGAL_END_NAMESPACE

#endif
