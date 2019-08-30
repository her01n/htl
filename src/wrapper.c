#include <htl.h>
#include <libguile.h>

static HtlPoint
scm_to_point(SCM value)
{
  HtlPoint point;
  point.x = scm_to_int(scm_cadr(value));
  point.y = scm_to_int(scm_caddr(value));
  point.z = scm_to_int(scm_cadddr(value));
  return point;
}

static SCM
scm_from_point(HtlPoint value)
{
  SCM point = scm_from_utf8_symbol("point");
  SCM x = scm_from_int(value.x);
  SCM y = scm_from_int(value.y);
  SCM z = scm_from_int(value.z);
  return scm_list_4(point, x, y, z);
}

static HtlFace
scm_to_face(SCM value)
{
  HtlFace face;
  face.a = scm_to_point(scm_car(value));
  face.b = scm_to_point(scm_cadr(value));
  face.c = scm_to_point(scm_caddr(value));
  return face;
}

static SCM
scm_from_face(HtlFace face)
{
  SCM a = scm_from_point(face.a);
  SCM b = scm_from_point(face.b);
  SCM c = scm_from_point(face.c);
  return scm_list_3(a, b, c);
}

SCM
htl_face_equal_wrapper(SCM sf1, SCM sf2)
{
  HtlFace nf1 = scm_to_face(sf1);
  HtlFace nf2 = scm_to_face(sf2);
  return scm_from_bool(htl_face_equal(nf1, nf2));
}

SCM
htl_face_point_orientation_wrapper(SCM sf, SCM sp)
{
  HtlFace nf = scm_to_face(sf);
  HtlPoint np = scm_to_point(sp);
  return scm_from_int(htl_face_point_orientation(nf, np));
}

SCM
htl_face_lower_bound_wrapper(SCM sf)
{
  HtlFace nf = scm_to_face(sf);
  HtlPoint nb = htl_face_lower_bound(nf);
  return scm_from_point(nb);
}

SCM
htl_face_higher_bound_wrapper(SCM sf)
{
  HtlFace nf = scm_to_face(sf);
  HtlPoint nb = htl_face_higher_bound(nf);
  return scm_from_point(nb);
}

static HtlSurface*
scm_to_surface(SCM value)
{
  return (HtlSurface*)scm_to_pointer(scm_cadr(value));
}

static SCM
scm_from_surface(HtlSurface* surface)
{
  SCM tag = scm_from_utf8_symbol("surface");
  SCM pointer =
    scm_from_pointer(surface, (scm_t_pointer_finalizer)htl_surface_unref);
  return scm_list_2(tag, pointer);
}

SCM
htl_surface_add_all(SCM ss, SCM sfs)
{
  HtlSurface* ns = scm_to_surface(ss);
  htl_surface_ref(ns);
  SCM i = sfs;
  while (scm_is_pair(i)) {
    HtlFace nf = scm_to_face(scm_car(i));
    i = scm_cdr(i);
    HtlSurface* next = htl_surface_add(ns, nf);
    htl_surface_unref(ns);
    ns = next;
  }
  return scm_from_surface(ns);
}

static bool
add_face(HtlFace face, SCM* list)
{
  *list = scm_cons(scm_from_face(face), *list);
  return true;
}

SCM
htl_surface_faces(SCM ss)
{
  HtlSurface* ns = scm_to_surface(ss);
  SCM list = scm_list_n(SCM_UNDEFINED);
  htl_surface_traverse_faces(ns, (HtlFaceTraverseFunc)add_face, &list);
  return list;
}

SCM
htl_surface_equal_wrapper(SCM ss1, SCM ss2)
{
  HtlSurface* ns1 = scm_to_surface(ss1);
  HtlSurface* ns2 = scm_to_surface(ss2);
  bool equal = htl_surface_equal(ns1, ns2);
  return scm_from_bool(equal);
}

SCM
htl_surface_contains_face_wrapper(SCM ss, SCM sf)
{
  HtlSurface* ns = scm_to_surface(ss);
  HtlFace nf = scm_to_face(sf);
  return scm_from_bool(htl_surface_contains_face(ns, nf));
}

SCM
htl_surface_lower_bound_wrapper(SCM ss)
{
  HtlSurface* ns = scm_to_surface(ss);
  HtlPoint nb = htl_surface_lower_bound(ns);
  return scm_from_point(nb);
}

SCM
htl_surface_higher_bound_wrapper(SCM ss)
{
  HtlSurface* ns = scm_to_surface(ss);
  HtlPoint nb = htl_surface_higher_bound(ns);
  return scm_from_point(nb);
}

SCM
htl_surface_faces_at(SCM ss, SCM sl, SCM sh)
{
  HtlSurface* ns = scm_to_surface(ss);
  HtlPoint nl = scm_to_point(sl);
  HtlPoint nh = scm_to_point(sh);
  SCM list = scm_list_n(SCM_UNDEFINED);
  htl_surface_traverse_faces_at(ns, nl, nh, (HtlFaceTraverseFunc)add_face, &list);
  return list;
}

void
init_htl()
{
  scm_c_define_gsubr("htl-face-equal", 2, 0, 0, htl_face_equal_wrapper);
  scm_c_define_gsubr(
    "htl-face-point-orientation", 2, 0, 0, htl_face_point_orientation_wrapper);
  scm_c_define_gsubr("htl-face-lower-bound", 1, 0, 0, htl_face_lower_bound_wrapper);
  scm_c_define_gsubr("htl-face-higher-bound", 1, 0, 0, htl_face_higher_bound_wrapper);
  scm_c_define_gsubr("htl-surface-add-all", 2, 0, 0, htl_surface_add_all);
  scm_c_define_gsubr("htl-surface-faces", 1, 0, 0, htl_surface_faces);
  scm_c_define_gsubr("htl-surface-equal", 2, 0, 0, htl_surface_equal_wrapper);
  scm_c_define_gsubr(
    "htl-surface-contains-face", 2, 0, 0, htl_surface_contains_face_wrapper);
  scm_c_define_gsubr("htl-surface-lower-bound", 1, 0, 0, htl_surface_lower_bound_wrapper);
  scm_c_define_gsubr("htl-surface-higher-bound", 1, 0, 0, htl_surface_higher_bound_wrapper);
  scm_c_define_gsubr("htl-surface-faces-at", 3, 0, 0, htl_surface_faces_at);
}
