module wigcpp
  use, intrinsic :: iso_c_binding
  implicit none
  private

  public :: wigcpp_global_init, wig3jj, wig6jj, wig9jj

  interface
    subroutine wigcpp_global_init(max_two_j, wigner_type) bind(c, name="wigcpp_global_init")
      import c_int
      integer(c_int), value :: max_two_j, wigner_type
    end subroutine

    function wig3jj(two_j1, two_j2, two_j3, two_m1, two_m2, two_m3) bind(c, name="wig3jj")
      import c_int, c_double
      integer(c_int), value :: two_j1, two_j2, two_j3, two_m1, two_m2, two_m3
      real(c_double) :: wig3jj
    end function

    function wig6jj(two_j1, two_j2, two_j3, two_j4, two_j5, two_j6) bind(c, name="wig6jj")
      import c_int, c_double
      integer(c_int), value :: two_j1, two_j2, two_j3, two_j4, two_j5, two_j6
      real(c_double) :: wig6jj
    end function

    function wig9jj(two_j1, two_j2, two_j3, two_j4, two_j5, two_j6, two_j7, two_j8, two_j9) bind(c, name="wig9jj")
      import c_int, c_double
      integer(c_int), value :: two_j1, two_j2, two_j3, two_j4, two_j5, two_j6, two_j7, two_j8, two_j9
      real(c_double) :: wig9jj
    end function
  end interface
end module wigcpp