!test.f90

program main
	use wigcpp
	implicit none
	real :: result

	call wigcpp_global_init(2 * 100, 9)

	result = clebsch_gordan(35, 37, 3, 5, 66, 8)
	print *, "result of clebsch gordan <17.5, 18.5; 1.5, 2.5 | 33, 4> is", result

	result = wigner3j(2 * 1, 2 * 1, 2 * 2, 0, 0, 0)
	print *, "result of wigner 3j(1, 1, 2, 0, 0, 0) is", result

	result = wigner6j(2*2, 2*2, 2*2, 2*2, 2*2, 2*2)
	print *, "result of wigner 6j(2, 2, 2, 2, 2, 2) is", result

	result = wigner9j(2*20, 2*20, 2*20, 2*20, 2*20, 2*20, 2*20, 2*20, 2*20)
	print *, "result of wigner 9j(20, 20, 20, 20, 20, 20, 20, 20, 20) is", result

end program main