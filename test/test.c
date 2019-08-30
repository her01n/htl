#include <assert.h>
#include <stdio.h>

#include <htl.h>

int 
main (int argc, char** argv)
{
  assert (htl_point_equal (htl_point (1, 2, 3), htl_point (1, 2, 3)));
  assert (! htl_point_equal (htl_point (1, 2, 3), htl_point (1, 2, 4)));
  
  assert (htl_point_equal (htl_point (5, 7, 9), htl_point_add (htl_point(1, 2, 3), htl_point (4, 5, 6))));
  printf ("All tests ok.\n");
  return 0;
}

