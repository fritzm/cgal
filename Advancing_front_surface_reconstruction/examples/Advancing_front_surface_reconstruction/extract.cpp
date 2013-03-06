#define NOLAZY
#define BLIND


#include <cstdio>
#include <cstring>
#include <iostream>
#include <fstream>
#include <strstream>
#include <cassert>
#include <vector>
#include <list>


// Kernel
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>

#include <CGAL/Triangulation_data_structure_3.h>
#include <CGAL/Delaunay_triangulation_3.h>
#include <CGAL/Triangulation_hierarchy_3.h>

#include <CGAL/AFSR_vertex_base_with_id_3.h>
#include <CGAL/Triangulation_cell_base_3.h>
#include <CGAL/AFSR_cell_base_3.h>
#include <CGAL/Advancing_front_surface_reconstruction.h>
#include <CGAL/AFSR_options.h>
#include <CGAL/IO/Advancing_front_surface_reconstruction.h>





typedef CGAL::Exact_predicates_inexact_constructions_kernel Kernel;

typedef Kernel::Point_3  Point;
typedef Kernel::Vector_3 Vector;
typedef Kernel::Point_2  Point_2;

typedef CGAL::AFSR_vertex_base_with_id_3<Kernel> LVb;
typedef CGAL::Triangulation_hierarchy_vertex_base_3<LVb> HVb;

typedef CGAL::Triangulation_cell_base_3<Kernel> Cb;
typedef CGAL::AFSR_cell_base_3<Cb> LCb;


typedef CGAL::Triangulation_data_structure_3<HVb,LCb> Tds;


typedef CGAL::Delaunay_triangulation_3<Kernel,Tds> Delaunay_Triangulation_3;
typedef CGAL::Triangulation_hierarchy_3<Delaunay_Triangulation_3> Triangulation_3;
typedef Triangulation_3::Vertex_handle Vertex_handle;

typedef CGAL::Advancing_front_surface_reconstruction<Kernel,Triangulation_3> Surface;
typedef CGAL::AFSR_options Options;


//---------------------------------------------------------------------

bool
file_input(const Options& opt, std::vector<Point>& points)
{
  const char* finput = opt.finname;
  std::size_t number_of_points =  opt.number_of_points;
  bool xyz = opt.xyz;
  
  std::ios::openmode mode = (opt.binary) ? std::ios::binary : std::ios::in;
  std::ifstream is(finput, mode);

  if(opt.binary){
    CGAL::set_binary_mode(is);
  }
  if(is.fail())
    {
      std::cerr << "+++unable to open file for input" << std::endl;
      exit(0);
      return false;
    }
  else
    std::cout << "Input from file : " << finput << std::endl;

  std::size_t n;
  if(! xyz){
    is >> n;
    std::cout << "   reading " << n << " points" << std::endl;
    points.reserve(n);
     CGAL::cpp11::copy_n(std::istream_iterator<Point>(is), n, std::back_inserter(points));
  } else {
    // we do not know beforehand how many points we will read
    std::istream_iterator<Point> it(is), eof;
    char ignore[256];
    while(it!= eof){
      points.push_back(*it);
      is.getline(ignore,256);
      it++;
    }
    n = points.size();
  }


  if ( (number_of_points > 0 ) && (number_of_points < n ))
    {
      points.erase(points.begin()+number_of_points, points.begin()+n);

      std::cout << std::endl 
		<< "   and randomize a sub-sample of " << number_of_points 
		<< " points." <<
	std::endl << std::endl;
    }
  return true;
}



void usage(char* program)
{
  std::cerr << std::endl << "NAME     " << std::endl
	    << program << "     - surface extension -" << std::endl << std::endl;

  std::cerr << std::endl << "OPTIONS" << std::endl
	    << "     -shuffle         : random shuffle" << std::endl
	    << "     -xyz             : input data in xyz format" << std::endl
	    << "     -no_border -nb   : " << std::endl
	    << "     -in fname        : reads points from file ./fname" << std::endl
	    << "     -out fname       : writes points to file ./fname" << std::endl
	    << "     -out_format -of  : choose file format for output (iv, wrl, oogl, medit," << std::endl
	    << "                                                     ply, stl, all, none)" << std::endl
            << "     -rgb r g b       : color of the surface" << std::endl
            << "     -no_header       : The Vrml header and footer are not written" << std::endl	    
	    << "     -area  a         : No faces larger than area * average_area" << std::endl
	    << "     -perimeter  p    : No faces larger than perimeter * average_perimeter" << std::endl   
	    << "     -abs_area  a     : No faces larger than abs_area" << std::endl
	    << "     -abs_perimeter p : No faces with perimeter longer than abs_perimeter" << std::endl
	    << "     -number_of_points n : set a number of points for a sub-sample" << std::endl

            << "\n     Options for internal use" << std::endl

	    << "     -sect_in fname   : reads points from sections file ./fname" << std::endl
	    << "     -binary          : binary I/O" << std::endl
	    << "     -delta x         : set the delta constant" << std::endl
	    << "     -ki x y          : set the K interval (default : [1.1  5])" << std::endl
	    << "     -ks x            : set the K step (default : .1)" << std::endl
	    << "     -k  x            : set the K constant (only one pass)" << std::endl
	    << "     -Delaunay        : display the underlying Delaunay triangulation" << std::endl
	    << "     -max_of_connected_components x     : set the max of connected components" << std::endl
	    << "                                          (default : non-active)" << std::endl
            << "     -post x          : set a number for the post process" << std::endl
	    << "     -contours        : display contours" << std::endl;
}



bool
parse(int argc, char* argv[], Options &opt)
{
  std::strcpy(opt.program, argv[0]);
  --argc;
  argv++;
  if(argc == 0)
    std::cout << "nothing ???" << std::endl;
  
  while ((argc > 0) && (argv[0][0] == '-')){
    if ((!std::strcmp(argv[0], "-D")) || (!std::strcmp(argv[0], "-Delaunay"))) {
      opt.Delaunay = true;
      argv++;
      argc--;
      std::cout << "-D ";
    }
    else if ((!std::strcmp(argv[0], "-c")) || (!std::strcmp(argv[0], "-contours"))) {
      opt.contour = true;
      argv++;
      argc--;
      std::cout << "-c ";
    }
    else if ((!std::strcmp(argv[0], "-s")) || (!std::strcmp(argv[0], "-shuffle"))) {
      opt.shuffle = true;
      argv++;
      argc--;
      std::cout << "-s ";
    }
    else if ((!std::strcmp(argv[0], "-b")) || (!std::strcmp(argv[0], "-binary"))) {
      opt.binary = true;
      argv++;
      argc--;
      std::cout << "-b ";
    }
    else if ((!std::strcmp(argv[0], "-x")) || (!std::strcmp(argv[0], "-xyz"))) {
      opt.xyz = true;
      argv++;
      argc--;
      std::cout << "-x ";
    }
    else if ((!std::strcmp(argv[0], "-nb")) || (!std::strcmp(argv[0], "-no_border"))) {
      opt.K = HUGE_VAL;
      opt.K_init = opt.K;
      argv++;
      argc--;
      std::cout << "-nb ";
    }
    else if ((!std::strcmp(argv[0], "-nh")) || (!std::strcmp(argv[0], "-no_header"))) {
      opt.no_header = true;
      argv++;
      argc--;
      std::cout << "-nh ";
    }
    else if ((!std::strcmp(argv[0], "-d")) || (!std::strcmp(argv[0], "-delta"))){
      if (sscanf(argv[1], "%lf", &opt.delta) != 1) {
	std::cerr << "Argument for delta must be a number"
		  << std::endl;
      }
      argv += 2;
      argc -= 2;
      std::cout << "-d " << opt.delta << " "; 
    }  
    else if ((!std::strcmp(argv[0], "-a")) || (!std::strcmp(argv[0], "-area"))){
      if (sscanf(argv[1], "%lf", &opt.area) != 1) {
	std::cerr << "Argument for area must be a number"
		  << std::endl;
      }
      argv += 2;
      argc -= 2;
      std::cout << "-a " << opt.area << " "; 
    }   
    else if ((!std::strcmp(argv[0], "-pe")) || (!std::strcmp(argv[0], "-perimeter"))){
      if (sscanf(argv[1], "%lf", &opt.perimeter) != 1) {
	std::cerr << "Argument for perimeter must be a number"
		  << std::endl;
      }
      argv += 2;
      argc -= 2;
      std::cout << "-perimeter " << opt.perimeter << " "; 
    }    
    else if ((!std::strcmp(argv[0], "-aa")) || (!std::strcmp(argv[0], "-abs_area"))){
      if (sscanf(argv[1], "%lf", &opt.abs_area) != 1) {
	std::cerr << "Argument for abs_area must be a number"
		  << std::endl;
      }
      argv += 2;
      argc -= 2;
      std::cout << "-abs_area " << opt.abs_area << " "; 
    }     
    else if ((!std::strcmp(argv[0], "-ae")) || (!std::strcmp(argv[0], "-abs_perimeter"))){
      if (sscanf(argv[1], "%lf", &opt.abs_perimeter) != 1) {
	std::cerr << "Argument for abs_perimeter must be a number"
		  << std::endl;
      }
      argv += 2;
      argc -= 2;
      std::cout << "-abs_perimeter " << opt.abs_perimeter << " "; 
    }  
    else if ((!std::strcmp(argv[0], "-ki"))){
      if ((sscanf(argv[1], "%lf", &opt.K_init) != 1)||
	  (sscanf(argv[2], "%lf", &opt.K) != 1)){
	std::cerr << "Argument for K must be a number"
		  << std::endl;
      }
      argv += 3;
      argc -= 3;
      std::cout << "-ki " << opt.K_init << " " << opt.K << " ";
    }  
    else if ((!std::strcmp(argv[0], "-rgb"))){
      if ((sscanf(argv[1], "%lf", &opt.red) != 1)||
	  (sscanf(argv[2], "%lf", &opt.green) != 1) ||
	  (sscanf(argv[3], "%lf", &opt.blue) != 1)){
	std::cerr << "Argument for rgb must be three numbers"
		  << std::endl;
      }
      argv += 4;
      argc -= 4;
      std::cout << "-rgb " << opt.red << " " << opt.green << " " << opt.blue << " " ;
    }
    else if ((!std::strcmp(argv[0], "-ks"))){
      if (sscanf(argv[1], "%lf", &opt.K_step) != 1) {
	std::cerr << "Argument for K must be a number"
		  << std::endl;
      }
      argv += 2;
      argc -= 2;
      std::cout << "-ks " << opt.K_step << " ";
    }  
    else if ((!std::strcmp(argv[0], "-k"))){
      if (sscanf(argv[1], "%lf", &opt.K) != 1) {
	std::cerr << "Argument for K must be a number"
		  << std::endl;
      }
      opt.K_init = opt.K;
      argv += 2;
      argc -= 2;
      std::cout << "-k " << opt.K_init << " ";
    } 
    else if ((!std::strcmp(argv[0], "-n")) || (!std::strcmp(argv[0], "-number_of_points"))){
      if (sscanf(argv[1], "%d", &opt.number_of_points) != 1) {
	std::cerr << "Argument for the number of points must be a number"
		  << std::endl;
      }
      argv += 2;
      argc -= 2;
      std::cout << "-n " << opt.number_of_points << " ";
    }  
    else if ((!std::strcmp(argv[0], "-m")) || (!std::strcmp(argv[0], "-max_of_connected_components"))){
      if (sscanf(argv[1], "%d", &opt.max_connected_comp) != 1) {
	std::cerr << "Argument for the number of connected components must be a number"
		  << std::endl;
      }
      /*
      if(opt.max_connected_comp < 1) {
	std::cerr << "Argument for the number of connected components must be a positive number"
		  << "It is set to 1" << std::endl;
	opt.max_connected_comp = 1;
      }
      */
      argv += 2;
      argc -= 2;
      std::cout << "-m " << opt.max_connected_comp << " ";
    }
    else if ((!std::strcmp(argv[0], "-p")) || (!std::strcmp(argv[0], "-post"))){
      if (sscanf(argv[1], "%d", &opt.NB_BORDER_MAX) != 1) {
	std::cerr << "Argument for post process must be a number"
		  << std::endl;
      }
      argv += 2;
      argc -= 2;
      std::cout << "-p " << opt.NB_BORDER_MAX << " ";
    }
    else if ((!std::strcmp(argv[0], "-i")) || (!std::strcmp(argv[0], "-in"))) {
      std::strcpy(opt.finname, argv[1]);
      opt.file_input = true;
      argv += 2;
      argc -= 2;
      std::cout << "-i " << opt.finname << " ";
    }
    else if ((!std::strcmp(argv[0], "-s")) || (!std::strcmp(argv[0], "-sect_in"))) {
      std::strcpy(opt.finname, argv[1]);
      opt.Section_file = true;
      opt.file_input = true;
      argv += 2;
      argc -= 2;
      std::cout << "-s " << opt.finname << " ";
    }
    else if ((!std::strcmp(argv[0], "-o")) || (!std::strcmp(argv[0], "-out"))) {
      std::strcpy(opt.foutname, argv[1]);
      opt.file_output = true;
      argv += 2;
      argc -= 2;
      std::cout << "-o " << opt.foutname << " ";
    }
    else if ((!std::strcmp(argv[0], "-of")) || (!std::strcmp(argv[0], "-out_format"))) {
      if (!std::strcmp(argv[1], "wrl"))
	opt.out_format = 0;
      else if (!std::strcmp(argv[1], "oogl"))
	opt.out_format = 1;
      else if (!std::strcmp(argv[1], "medit"))
	opt.out_format = 2;
      else if (!std::strcmp(argv[1], "ply"))
	opt.out_format = 3;
      else if(!std::strcmp(argv[1], "iv"))
	opt.out_format = 4;
      else if(!std::strcmp(argv[1], "stl"))
	opt.out_format = 5;
      else if (!std::strcmp(argv[1], "all"))
	opt.out_format = -1;
      else if (!std::strcmp(argv[1], "none"))
	opt.out_format = -2;
      else
	std::cout << "unrecognized file format." << std::endl;
      opt.file_output = true;
      std::cout << "-of " << argv[1] << " ";
      argv += 2;
      argc -= 2;
    }
    else if ((!std::strcmp(argv[0], "-?")) ||
	     (!std::strcmp(argv[0], "-h")) ||
	     (!std::strcmp(argv[0], "-help"))) {
      usage(opt.program);
      return false;
    }
    else {
      std::cerr << "unrecognized option " << argv[0] << std::endl;
      usage(opt.program);
      return false;
    }
  }
  if(argc > 0){
    std::cerr << "unrecognized option " << argv[0] << std::endl;
    usage(opt.program);
    return false;
  }
  return true;
}


//___________________________________________
int main(int argc,  char* argv[])
{
  CGAL::Timer timer, total;
  total.start();
  timer.start();
  //parse command line
  Options opt;
  std::cout << "Option line for this execution is :" << std::endl;
  if (!parse(argc, argv, opt))
    exit(0);
  std::cout << std::endl << std::endl;
  
  Triangulation_3 dt;

  std::vector<Point> points;

  file_input(opt, points);

  std::cout << "Time for reading "  << timer.time() << " sec." << std::endl;


  std::vector<int> index(points.size());
  for(int i = 0; i < points.size(); i++){
    index[i] = i;
  }
  if(opt.shuffle){ 
    std::cout << "Random shuffling" << std::endl;
    std::random_shuffle(index.begin(), index.end());
  }

  std::cout << "Compute Delaunay Tetrahedrization " << std::endl; 
  CGAL::Timer t1;
  t1.start();
  
  for(int i =0; i < points.size(); i++){
    Vertex_handle vh = dt.insert(points[index[i]]);
    vh->id() = index[i];
  } 
  t1.stop();
  std::cout << "   Inserted " << dt.number_of_vertices() << " points, "
	    <<  dt.number_of_cells() << " cells computed in "
	    << t1.time() << " sec." << std::endl;
 
  if (dt.dimension() < 3) {
    std::cout << "-- 2D sample of points ???"  << std::endl;
    exit(0);
  }
  
  index.clear();
  points.clear();

  {
  Surface S(dt, opt);

  const Surface::TDS_2&  tds_2 = S.tds_2();

  for(Surface::TDS_2::Face_iterator fit = tds_2.faces_begin(); fit != tds_2.faces_end(); ++fit){
    if( fit->is_on_surface()){
      Surface::Facet facet = fit->facet();
      Surface::Cell_handle ch = facet.first;
      int fi = facet.second;
      int i,j,k;
      Surface::Vertex_handle vh3 = ch->vertex(Triangulation_3::vertex_triple_index(fi,0));
      for(i=0; i<3 ; i++){
	if(fit->vertex(i)->vertex_3()== vh3){
	  break;
	}
      }
      vh3 = ch->vertex(Triangulation_3::vertex_triple_index(fi,1));
      for(j=0; j<3; j++){
	if(fit->vertex(j)->vertex_3()== vh3){
	  break;
	}
      }
	    vh3 = ch->vertex(Triangulation_3::vertex_triple_index(fi,2));
      for(k=0; k<3 ; k++){
	if (fit->vertex(k)->vertex_3()== vh3){
	  break;
	}      
      }
      
      if((i+j+k) != 3){
	std::cerr << "i = " << i << "; j = " << j << "; k = " << k << std::endl; 
      }
      
    }
    
  }
  

  std::cout << "Total time: " << timer.time() << " sec." << std::endl; 
  //  write_to_file_vrml2(opt.foutname, S, opt.contour, opt.red, opt.green, opt.blue, opt.no_header);
  write_to_file(opt.foutname, S, opt.contour, opt.out_format, opt.red, opt.green, opt.blue, opt.no_header);


  std::cout << "   "  << S.number_of_outliers()
	    << " outliers." << std::endl; 
  std::cout << "   Reconstructed surface: " << S.number_of_facets() << 
    " facets, " << S.number_of_vertices() << " vertices." << std::endl;
  std::cout << "   "  << S.number_of_border_edges() << 
    " border edges." << std::endl;
  std::cout << "   number of connected components <= " 
	    << (std::max)(1, S.number_of_connected_components()-1)
	    << std::endl << std::endl;
  }
  total.stop();
  std::cout << "Total = " << total.time() << " sec." << std::endl;
  return 0;
}




