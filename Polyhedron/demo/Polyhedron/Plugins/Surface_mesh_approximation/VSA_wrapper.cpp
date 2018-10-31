#include "VSA_wrapper.h"

void VSA_wrapper::set_mesh(const SMesh &mesh) {
  Vertex_point_map vpm = get(boost::vertex_point, const_cast<SMesh &>(mesh));

  m_face_centers.clear();
  m_face_areas.clear();
  BOOST_FOREACH(face_descriptor f, faces(mesh)) {
    const halfedge_descriptor he = halfedge(f, mesh);
    const Point_3 &p0 = vpm[source(he, mesh)];
    const Point_3 &p1 = vpm[target(he, mesh)];
    const Point_3 &p2 = vpm[target(next(he, mesh), mesh)];

    m_face_centers.insert(std::pair<face_descriptor, Point_3>(
      f, CGAL::centroid(p0, p1, p2)));

    FT area(std::sqrt(CGAL::to_double(CGAL::squared_area(p0, p1, p2))));
    m_face_areas.insert(std::pair<face_descriptor, FT>(f, area));
  }

  if (m_l21_approx)
    delete m_l21_approx;
  if (m_pl21_metric)
    delete m_pl21_metric;
  if (m_l2_approx)
    delete m_l2_approx;
  if (m_pl2_metric)
    delete m_pl2_metric;
  if (m_compact_approx)
    delete m_compact_approx;
  if (m_pcompact_metric)
    delete m_pcompact_metric;

  m_pl21_metric = new L21_metric(mesh, vpm);
  m_l21_approx = new L21_approx(mesh, vpm, *m_pl21_metric);

  m_pl2_metric = new L2_metric(mesh, vpm);
  m_l2_approx = new L2_approx(mesh, vpm, *m_pl2_metric);

  m_pcompact_metric = new Compact_metric(m_center_pmap, m_area_pmap);
  m_compact_approx = new Compact_approx(mesh, vpm, *m_pcompact_metric);

  m_proxy_colors.clear();
  m_initialized = false;
}

void VSA_wrapper::set_metric(const Metric &m) {
  m_metric = m;
  m_proxy_colors.clear();
  m_initialized = false;
}

std::size_t VSA_wrapper::initialize_seeds(const VSA::Seeding_method method,
  const boost::optional<std::size_t> max_nb_of_proxies,
  const boost::optional<FT> min_error_drop,
  const std::size_t nb_relaxations) {
  std::size_t nb_initialized = 0;
  switch (m_metric) {
    case L21:
      nb_initialized = m_l21_approx->initialize_seeds(
        CGAL::parameters::seeding_method(method)
          .max_number_of_proxies(max_nb_of_proxies)
          .min_error_drop(min_error_drop)
          .number_of_relaxations(nb_relaxations));
      break;
    case L2:
      nb_initialized = m_l2_approx->initialize_seeds(
        CGAL::parameters::seeding_method(method)
          .max_number_of_proxies(max_nb_of_proxies)
          .min_error_drop(min_error_drop)
          .number_of_relaxations(nb_relaxations));
      break;
    case Compact:
      nb_initialized = m_compact_approx->initialize_seeds(
        CGAL::parameters::seeding_method(method)
          .max_number_of_proxies(max_nb_of_proxies)
          .min_error_drop(min_error_drop)
          .number_of_relaxations(nb_relaxations));
      break;
  }

  // generate proxy colors
  m_proxy_colors.clear();
  for (std::size_t i = 0; i < number_of_proxies(); ++i) {
    const std::size_t c = rand_0_255();
    m_proxy_colors.push_back(QColor::fromRgb(
      Color_cheat_sheet::r(c), Color_cheat_sheet::g(c), Color_cheat_sheet::b(c)));
  }

  m_initialized = true;
  return nb_initialized;
}

void VSA_wrapper::run(const std::size_t nb_iterations) {
  FT err(0.0);
  switch (m_metric) {
    case L21:
      m_l21_approx->run(nb_iterations);
      break;
    case L2:
      m_l2_approx->run(nb_iterations);
      break;
    case Compact:
      m_compact_approx->run(nb_iterations);
      break;
  }
}

std::size_t VSA_wrapper::add_one_proxy() {
  std::size_t nb_added = 0;
  switch (m_metric) {
    case L21:
      nb_added = m_l21_approx->add_to_furthest_proxies(1, 0);
      break;
    case L2:
      nb_added = m_l2_approx->add_to_furthest_proxies(1, 0);
      break;
    case Compact:
      nb_added = m_compact_approx->add_to_furthest_proxies(1, 0);
      break;
  }
  if (nb_added == 1) {
    const std::size_t c = rand_0_255();
    m_proxy_colors.push_back(QColor::fromRgb(
      Color_cheat_sheet::r(c), Color_cheat_sheet::g(c), Color_cheat_sheet::b(c)));
  }

  return nb_added;
}

std::size_t VSA_wrapper::teleport_one_proxy() {
  switch (m_metric) {
    case L21:
      return m_l21_approx->teleport_proxies(1, 0, true);
    case L2:
      return m_l2_approx->teleport_proxies(1, 0, true);
    case Compact:
      return m_compact_approx->teleport_proxies(1, 0, true);
  }
  return 0;
}

bool VSA_wrapper::split(const std::size_t px_idx, const std::size_t n, const std::size_t nb_relaxations) {
  bool splitted = false;
  switch (m_metric) {
    case L21:
      splitted = m_l21_approx->split(px_idx, n, nb_relaxations);
      break;
    case L2:
      splitted = m_l2_approx->split(px_idx, n, nb_relaxations);
      break;
    case Compact:
      splitted = m_compact_approx->split(px_idx, n, nb_relaxations);
      break;
  }
  if (splitted) {
    for (std::size_t i = m_proxy_colors.size(); i < number_of_proxies(); ++i) {
      const std::size_t c = rand_0_255();
      m_proxy_colors.push_back(QColor::fromRgb(
        Color_cheat_sheet::r(c), Color_cheat_sheet::g(c), Color_cheat_sheet::b(c)));
    }
  }

  return splitted;
}

bool VSA_wrapper::extract_mesh(const FT subdivision_ratio,
  const bool relative_to_chord,
  const bool with_dihedral_angle,
  const bool optimize_anchor_location,
  const bool pca_plane) {
  switch (m_metric) {
    case L21:
      return m_l21_approx->extract_mesh(
        CGAL::parameters::subdivision_ratio(subdivision_ratio).
        relative_to_chord(relative_to_chord).
        with_dihedral_angle(with_dihedral_angle).
        optimize_anchor_location(optimize_anchor_location).
        pca_plane(pca_plane));
    case L2:
      return m_l2_approx->extract_mesh(
        CGAL::parameters::subdivision_ratio(subdivision_ratio).
        relative_to_chord(relative_to_chord).
        with_dihedral_angle(with_dihedral_angle).
        optimize_anchor_location(optimize_anchor_location).
        pca_plane(pca_plane));
    case Compact:
      return m_compact_approx->extract_mesh(
        CGAL::parameters::subdivision_ratio(subdivision_ratio).
        relative_to_chord(relative_to_chord).
        with_dihedral_angle(with_dihedral_angle).
        optimize_anchor_location(optimize_anchor_location).
        pca_plane(pca_plane));
  }
  return false;
}

std::size_t VSA_wrapper::number_of_proxies() {
  switch (m_metric) {
    case L21:
      return m_l21_approx->number_of_proxies();
    case L2:
      return m_l2_approx->number_of_proxies();
    case Compact:
      return m_compact_approx->number_of_proxies();
  }
  return 0;
}
