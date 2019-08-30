#include <glib.h>

#include <htl.h>

HtlPoint
htl_point(int x, int y, int z)
{
  HtlPoint point = { x, y, z };
  return point;
}

bool
htl_point_equal(HtlPoint a, HtlPoint b)
{
  return a.x == b.x && a.y == b.y && a.z == b.z;
}

HtlPoint
htl_point_add(HtlPoint v1, HtlPoint v2)
{
  return htl_point(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
}

HtlFace
htl_face(HtlPoint a, HtlPoint b, HtlPoint c)
{
  HtlFace face = { a, b, c };
  return face;
}

bool
htl_face_equal(HtlFace f1, HtlFace f2)
{
  return (htl_point_equal(f1.a, f2.a) && htl_point_equal(f1.b, f2.b) &&
          htl_point_equal(f1.c, f2.c)) ||
         (htl_point_equal(f1.a, f2.b) && htl_point_equal(f1.b, f2.c) &&
          htl_point_equal(f1.c, f2.a)) ||
         (htl_point_equal(f1.a, f2.c) && htl_point_equal(f1.b, f2.a) &&
          htl_point_equal(f1.c, f2.b));
}

static int
min(int a, int b, int c)
{
  if (a <= b && a <= c)
    return a;
  if (b <= a && b <= c)
    return b;
  return c;
}

static int
max(int a, int b, int c)
{
  if (a >= b && a >= c)
    return a;
  if (b >= a && b >= c)
    return b;
  return c;
}

HtlPoint
htl_face_lower_bound(HtlFace face)
{
  HtlPoint low;
  low.x = min(face.a.x, face.b.x, face.c.x);
  low.y = min(face.a.y, face.b.y, face.c.y);
  low.z = min(face.a.z, face.b.z, face.c.z);
  return low;
}

HtlPoint
htl_face_higher_bound(HtlFace face)
{
  HtlPoint high;
  high.x = max(face.a.x, face.b.x, face.c.x);
  high.y = max(face.a.y, face.b.y, face.c.y);
  high.z = max(face.a.z, face.b.z, face.c.z);
  return high;
}

static int
det3(int a, int b, int c, int d, int e, int f, int g, int h, int i)
{
  return a * (e * i - f * h) - b * (d * i - f * g) + c * (d * h - e * g);
}

int
htl_face_point_orientation(HtlFace f, HtlPoint p)
{
  // we want to return the determinant of this matrix:
  // p.x    p.y    p.z    1
  // f.a.x  f.a.y  f.a.z  1
  // f.b.x  f.b.y  f.b.z  1
  // f.c.x  f.c.y  f.c.z  1
  return p.x * det3(f.a.y, f.a.z, 1, f.b.y, f.b.z, 1, f.c.y, f.c.z, 1) -
         p.y * det3(f.a.x, f.a.z, 1, f.b.x, f.b.z, 1, f.c.x, f.c.z, 1) +
         p.z * det3(f.a.x, f.a.y, 1, f.b.x, f.b.y, 1, f.c.x, f.c.y, 1) -
         det3(f.a.x, f.a.y, f.a.z, f.b.x, f.b.y, f.b.z, f.c.x, f.c.y, f.c.z);
}

typedef enum
{
  LOW_X,
  LOW_Y,
  LOW_Z,
  HIGH_X,
  HIGH_Y,
  HIGH_Z,
} dimension;

struct HtlSurface
{
  gatomicrefcount ref_count;
  HtlFace face;
  dimension dimension;
  int value;
  HtlSurface* low;
  HtlSurface* high;
};

HtlSurface*
htl_surface_ref(HtlSurface* surface)
{
  if (surface)
    g_atomic_ref_count_inc(&surface->ref_count);
  return surface;
}

void
htl_surface_unref(HtlSurface* surface)
{
  if (surface && g_atomic_ref_count_dec(&surface->ref_count)) {
    htl_surface_unref(surface->low);
    htl_surface_unref(surface->high);
    free(surface);
  }
}

static int
value(HtlPoint low, HtlPoint high, dimension dimension)
{
  switch (dimension) {
    case LOW_X:
      return low.x;
    case LOW_Y:
      return low.y;
    case LOW_Z:
      return low.z;
    case HIGH_X:
      return high.x;
    case HIGH_Y:
      return high.y;
    case HIGH_Z:
      return high.z;
  }
  fprintf(stderr, "wrong dimension: %d\n", dimension);
  abort();
  return 0;
}

// TODO can i make this not recursive?
// TODO balancing?
static HtlSurface*
surface_add(HtlSurface* surface,
            HtlFace face,
            HtlPoint low,
            HtlPoint high,
            dimension next)
{
  if (surface == NULL) {
    surface = (HtlSurface*)malloc(sizeof(HtlSurface));
    g_atomic_ref_count_init(&surface->ref_count);
    surface->face = face;
    surface->dimension = next;
    surface->value = value(low, high, next);
    surface->low = NULL;
    surface->high = NULL;
    return surface;
  } else if (htl_face_equal(surface->face, face)) {
    return htl_surface_ref(surface);
  } else {
    dimension next = (surface->dimension + 1) % 6;
    HtlSurface* copy = (HtlSurface*)malloc(sizeof(HtlSurface));
    g_atomic_ref_count_init(&copy->ref_count);
    copy->face = surface->face;
    copy->dimension = surface->dimension;
    copy->value = surface->value;
    if (value(low, high, surface->dimension) > surface->value) {
      copy->low = htl_surface_ref(surface->low);
      copy->high = surface_add(surface->high, face, low, high, next);
    } else {
      copy->low = surface_add(surface->low, face, low, high, next);
      copy->high = htl_surface_ref(surface->high);
    }
    return copy;
  }
}

HtlSurface*
htl_surface_add(HtlSurface* surface, HtlFace face)
{
  HtlPoint low = htl_face_lower_bound(face);
  HtlPoint high = htl_face_higher_bound(face);
  return surface_add(surface, face, low, high, LOW_X);
}

bool
htl_surface_traverse_faces(HtlSurface* surface,
                           HtlFaceTraverseFunc func,
                           void* data)
{
  bool result = true;
  if (surface) {
    if (result)
      result = func(surface->face, data);
    if (result)
      result = htl_surface_traverse_faces(surface->low, func, data);
    if (result)
      result = htl_surface_traverse_faces(surface->high, func, data);
  }
  return result;
}

static bool
not_equals_face(HtlFace f1, HtlFace* f2)
{
  return !htl_face_equal(f1, *f2);
}

bool
htl_surface_contains_face(HtlSurface* surface, HtlFace face)
{
  return !htl_surface_traverse_faces(
    surface, (HtlFaceTraverseFunc)not_equals_face, &face);
}

static bool
contains_face(HtlFace face, HtlSurface* surface)
{
  return htl_surface_contains_face(surface, face);
}

bool
htl_surface_equal(HtlSurface* s1, HtlSurface* s2)
{
  return htl_surface_traverse_faces(
           s1, (HtlFaceTraverseFunc)contains_face, s2) &&
         htl_surface_traverse_faces(s2, (HtlFaceTraverseFunc)contains_face, s1);
}

static int
get_lowest_value(HtlSurface* surface, dimension d)
{
  int face = INT32_MAX;
  int low = INT32_MAX;
  int high = INT32_MAX;
  if (surface) {
    face = value(htl_face_lower_bound(surface->face), htl_point(0, 0, 0), d);
    if (surface->low)
      low = get_lowest_value(surface->low, d);
    if (surface->dimension != d && surface->high)
      high = get_lowest_value(surface->high, d);
  }
  return min(face, low, high);
}

HtlPoint
htl_surface_lower_bound(HtlSurface* surface)
{
  HtlPoint bound;
  bound.x = get_lowest_value(surface, LOW_X);
  bound.y = get_lowest_value(surface, LOW_Y);
  bound.z = get_lowest_value(surface, LOW_Z);
  return bound;
}

static int
get_highest_value(HtlSurface* surface, dimension d)
{
  int face = INT32_MIN;
  int low = INT32_MIN;
  int high = INT32_MIN;
  if (surface) {
    face = value(htl_point(0, 0, 0), htl_face_higher_bound(surface->face), d);
    if (surface->dimension != d && surface->low)
      low = get_highest_value(surface->low, d);
    if (surface->high)
      high = get_highest_value(surface->high, d);
  }
  return max(face, low, high);
}

HtlPoint
htl_surface_higher_bound(HtlSurface* surface)
{
  HtlPoint bound;
  bound.x = get_highest_value(surface, HIGH_X);
  bound.y = get_highest_value(surface, HIGH_Y);
  bound.z = get_highest_value(surface, HIGH_Z);
  return bound;
}

static bool
bounding_box_intersects(HtlPoint al, HtlPoint ah, HtlPoint bl, HtlPoint bh)
{
  return (al.x <= bh.x) && (al.y <= bh.y) && (al.z <= bh.z) && (bl.x <= ah.x) &&
         (bl.y <= ah.y) && (bl.z <= ah.z);
}

bool
htl_surface_traverse_faces_at(HtlSurface* surface,
                              HtlPoint lower,
                              HtlPoint higher,
                              HtlFaceTraverseFunc func,
                              void* data)
{
  bool result = true;
  if (surface) {
    HtlPoint face_lower = htl_face_lower_bound(surface->face);
    HtlPoint face_higher = htl_face_higher_bound(surface->face);
    if (bounding_box_intersects(face_lower, face_higher, lower, higher)) {
      result = func(surface->face, data);
    }
    bool go_low = true, go_high = true;
    switch (surface->dimension) {
      case LOW_X:
        go_high = (surface->value < higher.x);
        break;
      case LOW_Y:
        go_high = (surface->value < higher.y);
        break;
      case LOW_Z:
        go_high = (surface->value < higher.z);
        break;
      case HIGH_X:
        go_low = (surface->value >= lower.x);
        break;
      case HIGH_Y:
        go_low = (surface->value >= lower.y);
        break;
      case HIGH_Z:
        go_low = (surface->value >= lower.z);
        break;
    }
    if (result && go_low)
      result =
        htl_surface_traverse_faces_at(surface->low, lower, higher, func, data);
    if (result && go_high)
      result =
        htl_surface_traverse_faces_at(surface->high, lower, higher, func, data);
  }
  return result;
}
