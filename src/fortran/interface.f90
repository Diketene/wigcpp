module wigcpp
  use, intrinsic :: iso_c_binding
  implicit none
  private

  public :: wigcpp_global_init, clebsch_gordan, wigner3j, wigner6j, wigner9j

  interface
    subroutine wigcpp_global_init(max_two_j, wigner_type) bind(c, name="wigcpp_global_init")
      import c_int
      integer(c_int), value :: max_two_j, wigner_type
    end subroutine

    function clebsch_gordan(two_j1, two_j2, two_m1, two_m2, two_J, two_M) bind(c, name="clebsch_gordan")
      import c_int, c_double
      integer(c_int), value :: two_j1, two_j2, two_m1, two_m2, two_J, two_M
      real(c_double) :: clebsch_gordan
    end function

    function wigner3j(two_j1, two_j2, two_j3, two_m1, two_m2, two_m3) bind(c, name="wigner3j")
      import c_int, c_double
      integer(c_int), value :: two_j1, two_j2, two_j3, two_m1, two_m2, two_m3
      real(c_double) :: wigner3j
    end function

    function wigner6j(two_j1, two_j2, two_j3, two_j4, two_j5, two_j6) bind(c, name="wigner6j")
      import c_int, c_double
      integer(c_int), value :: two_j1, two_j2, two_j3, two_j4, two_j5, two_j6
      real(c_double) :: wigner6j
    end function

    function wigner9j(two_j1, two_j2, two_j3, two_j4, two_j5, two_j6, two_j7, two_j8, two_j9) bind(c, name="wigner9j")
      import c_int, c_double
      integer(c_int), value :: two_j1, two_j2, two_j3, two_j4, two_j5, two_j6, two_j7, two_j8, two_j9
      real(c_double) :: wigner9j
    end function
  end interface
end module wigcpp