// ============================================================================
//
// Copyright (c) 1998 The CGAL Consortium
//
// This software and related documentation is part of an INTERNAL release
// of the Computational Geometry Algorithms Library (CGAL). It is not
// intended for general use.
//
// ----------------------------------------------------------------------------
//
// release       :
// release_date  :
//
// file          : include/CGAL/Triangulation_cell.h
// revision      : $Revision$
// author(s)     : Monique Teillaud <Monique.Teillaud@sophia.inria.fr>
//
// coordinator   : Mariette Yvinec  <Mariette.Yvinec@sophia.inria.fr>
//
// ============================================================================

#ifndef CGAL_TRIANGULATION_CELL_H
#define CGAL_TRIANGULATION_CELL_H

#include <CGAL/Pointer.h>
#include <CGAL/Triangulation_short_names.h>
#include <CGAL/Triangulation_data_structure.h>


template < class Gt, class Tds >
class CGAL_Triangulation_vertex;

template < class Gt, class Tds >
class CGAL_Triangulation_vertex_handle;

template < class Gt, class Tds >
class CGAL_Triangulation_cell_handle;

template < class Gt, class Tds >
class CGAL_Triangulation_cell  
  : public Tds::Cell
{
public:

  typedef typename Gt::Point Point;

  typedef typename Tds::Vertex Vtds;
  typedef typename Tds::Cell Ctds;

  typedef CGAL_Triangulation_vertex<Gt,Tds> Vertex;
  typedef CGAL_Triangulation_cell<Gt,Tds> Cell;

  typedef CGAL_Triangulation_vertex_handle<Gt,Tds> Vertex_handle;
  typedef CGAL_Triangulation_cell_handle<Gt,Tds> Cell_handle;
  //  typedef triple<Cell_handle, int, int>     Edge;

  inline
  CGAL_Triangulation_cell()
    : Ctds()
  { }

//   inline 
//   CGAL_Triangulation_cell(Ctds f)
//     : Ctds(f)
//   {}
// 
  
  inline
  CGAL_Triangulation_cell(Tds& tds)
    : Ctds(tds)
  { }

  inline
  CGAL_Triangulation_cell(Tds& tds,
			  const Vertex_handle & v0,
			  const Vertex_handle & v1,
			  const Vertex_handle & v2,
			  const Vertex_handle & v3)
    : Ctds(tds, &(*v0), &(*v1), &(*v2), &(*v3))
  {}
    
  inline
  CGAL_Triangulation_cell(Tds& tds,
			  const Vertex_handle & v0,
			  const Vertex_handle & v1,
			  const Vertex_handle & v2,
			  const Vertex_handle & v3,
			  const Cell_handle & n0,
			  const Cell_handle & n1,
			  const Cell_handle & n2,
			  const Cell_handle & n3)
    : Ctds(tds, &(*v0), &(*v1), &(*v2), &(*v3), &(*n0), &(*n1), &(*n2), &(*n3)) 
  {}

  // Vertex access functions
  inline Vertex_handle vertex(int i) const
  {
    return  ((Vertex *)(Ctds::vertex(i)));
  }
    
  inline bool has_vertex(const Vertex_handle & v) const
  {
    return (Ctds::has_vertex( & (*v)) );
  }
    
  inline bool has_vertex(const Vertex_handle & v, int & i) const
  {
    return Ctds::has_vertex( &(*v), i);
  }

  inline int index(const Vertex_handle & v) const
  {
    return Ctds::index( &(*v));
  }

  //ACCESS FUNCTIONS
  inline
  Cell_handle neighbor(int i)
  {
    return (Cell *)(Ctds::neighbor(i));
  }

  inline int index(Cell_handle c) const
  {
    return Ctds::index( &(*c));
  }
  
  inline bool has_neighbor(Cell_handle c)
  {
    return Ctds::has_neighbor( &(*c));
  }

  inline bool has_neighbor(Cell_handle c, int &i)
  {
    return Ctds::has_neighbor( &(*c), i);
  }
 
  inline Cell_handle handle() const
  {
    return Cell_handle(this);
  }

 //Setting
  void set_vertices(Vertex_handle v0,
		    Vertex_handle v1,
		    Vertex_handle v2,
		    Vertex_handle v3)
  {
    Ctds::set_vertices(&(*v0), &(*v1), &(*v2), &(*v3));
  }
    
  void set_neighbors(Cell_handle n0,
		     Cell_handle n1,
		     Cell_handle n2,
		     Cell_handle n3)
  {
    Ctds::set_neighbors(&(*n0), &(*n1), &(*n2), &(*n3));
  }
    
  void set_vertices() 
  {
    Ctds::set_vertices();
  }
    
  void set_neighbors() 
  {
    Ctds::set_neighbors();
  }
    
  void set_vertex(int i, Vertex_handle v)
  {
    Ctds::set_vertex(i, &(*v));
  }
    
  void set_neighbor(int i, Cell_handle n)
  {
    Ctds::set_neighbor(i, &(*n));
  }

};

#endif CGAL_TRIANGULATION_CELL_H
