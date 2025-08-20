#ifndef __WIGCPP_H__
#define __WIGCPP_H__

#ifdef __cplusplus
extern "C"{
#endif
  void wigcpp_global_init(int max_two_j, int wigner_type);
  double wig3jj(int two_j1, int two_j2, int two_j3, int two_m1, int two_m2, int two_m3);
  double wig6jj(int two_j1, int two_j2, int two_j3, int two_j4, int two_j5, int two_j6);
  double wig9jj(int two_j1, int two_j2, int two_j3, int two_j4, int two_j5, int two_j6, int two_j7, int two_j8, int two_j9);
#ifdef __cplusplus
}
#endif
#endif