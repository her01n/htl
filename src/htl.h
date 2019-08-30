#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

/**
 * A point in 3D space.
 * The coordinates are integers, where unit is micrometer. 1 um = 0.0001 mm.
 * Vectors are also represented by this structure where applicable.
 */
typedef struct
{
  int x;
  int y;
  int z;
} HtlPoint;

HtlPoint
htl_point(int x, int y, int z);

/**
 * @return if the points are equal.
 */
bool
htl_point_equal(HtlPoint a, HtlPoint b);

/**
 * Adds two vectors together.
 */
HtlPoint
htl_point_add(HtlPoint v1, HtlPoint v2);

typedef struct
{
  HtlPoint a;
  HtlPoint b;
  HtlPoint c;
} HtlFace;

HtlFace
htl_face(HtlPoint a, HtlPoint b, HtlPoint c);

bool
htl_face_equal(HtlFace f1, HtlFace f2);

int
htl_face_point_orientation(HtlFace f, HtlPoint p);

HtlPoint
htl_face_lower_bound(HtlFace f);

HtlPoint
htl_face_higher_bound(HtlFace f);

typedef struct HtlSurface HtlSurface;

HtlSurface*
htl_surface_ref(HtlSurface* s);

void
htl_surface_unref(HtlSurface* s);

bool
htl_surface_equal(HtlSurface* s1, HtlSurface* s2);

HtlSurface*
htl_surface_add(HtlSurface* s, HtlFace f);

typedef bool (*HtlFaceTraverseFunc)(HtlFace face, void* data);

bool
htl_surface_traverse_faces(HtlSurface* surface,
                           HtlFaceTraverseFunc func,
                           void* data);

bool
htl_surface_contains_face(HtlSurface* surface, HtlFace face);

HtlPoint
htl_surface_lower_bound(HtlSurface* surface);

HtlPoint
htl_surface_higher_bound(HtlSurface* surface);

bool
htl_surface_traverse_faces_at(HtlSurface* surface,
                              HtlPoint lower,
                              HtlPoint higher,
                              HtlFaceTraverseFunc func,
                              void* data);
