!test.f90

program main
	use wigcpp
	implicit none
	real :: result

	call wigcpp_ensure_global(2 * 20, 9)

	write(*, '(A, G0)') 'Initialized the global factorial pool, size: (40, 9)'

	result = clebsch_gordan(35, 37, 3, 5, 66, 8)
	write(*, '(A,G0)') 'result of clebsch gordan <17.5, 18.5; 1.5, 2.5 | 33, 4> is ', result

	result = wigner3j(2 * 1, 2 * 1, 2 * 2, 0, 0, 0)
	write(*, '(A,G0)') 'result of wigner 3j(1, 1, 2, 0, 0, 0) is ', result

	result = wigner6j(2*2, 2*2, 2*2, 2*2, 2*2, 2*2)
	write(*, '(A, G0)') 'result of wigner 6j(2, 2, 2, 2, 2, 2) is ', result

	result = wigner9j(2*20, 2*20, 2*20, 2*20, 2*20, 2*20, 2*20, 2*20, 2*20)
	write(*, '(A, G0)') 'result of wigner 9j(20, 20, 20, 20, 20, 20, 20, 20, 20) is ', result

	call wigcpp_ensure_global(2 * 100, 9);
	write(*, '(A, G0)') 'Expanded the global factorial pool to size (200, 9)'

	result = wigner9j(90, 180, 200, 100, 80, 140, 160, 130, 150)
	write(*, '(A, G0)') 'result of wigner 9j(45, 90, 100, 50, 40, 70, 80, 65, 75) is ', result 

end program main