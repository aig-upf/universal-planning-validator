(define (problem prob5x5-rule1022)
	(:domain cellular-automata)
	(:objects
		c2 c3 c4 - valid
	)
	(:init
		(next c0 c1)
		(next c1 c2)
		(next c2 c3)
		(next c3 c4)
		(next c4 cn)
		(next cn cnp1)

		(assigna c0 c0 v0)
		(assigna c0 c1 v0)
		(assigna c0 c2 v0)
		(assigna c0 c3 v0)
		(assigna c0 c4 v0)
		(assigna c0 cn v0)
		(assigna c0 cnp1 v0)
		(assigna c1 c0 v0)
		(assigna c1 cnp1 v0)
		(assigna c2 c0 v0)
		(assigna c2 cnp1 v0)
		(assigna c3 c0 v0)
		(assigna c3 cnp1 v0)
		(assigna c4 c0 v0)
		(assigna c4 cnp1 v0)
		(assigna cn c0 v0)
		(assigna cn cnp1 v0)
		(assigna cnp1 c0 v0)
		(assigna cnp1 c1 v0)
		(assigna cnp1 c2 v0)
		(assigna cnp1 c3 v0)
		(assigna cnp1 c4 v0)
		(assigna cnp1 cn v0)
		(assigna cnp1 cnp1 v0)

		(assigna c1 c1 v0)
		(assigna c1 c2 v0)
		(assigna c1 c3 v0)
		(assigna c1 c4 v0)
		(assigna c1 cn v0)
		(assigna c2 c1 v0)
		(assigna c2 c2 v0)
		(assigna c2 c3 v0)
		(assigna c2 c4 v0)
		(assigna c2 cn v0)
		(assigna c3 c1 v0)
		(assigna c3 c2 v0)
		(assigna c3 c3 v1)
		(assigna c3 c4 v0)
		(assigna c3 cn v0)
		(assigna c4 c1 v0)
		(assigna c4 c2 v0)
		(assigna c4 c3 v0)
		(assigna c4 c4 v0)
		(assigna c4 cn v0)
		(assigna cn c1 v0)
		(assigna cn c2 v0)
		(assigna cn c3 v0)
		(assigna cn c4 v0)
		(assigna cn cn v0)

		(program-phase)
	)
	(:goal (and
		(assigna c1 c1 v0)
		(assigna c1 c2 v0)
		(assigna c1 c3 v1)
		(assigna c1 c4 v0)
		(assigna c1 cn v0)
		(assigna c2 c1 v0)
		(assigna c2 c2 v1)
		(assigna c2 c3 v1)
		(assigna c2 c4 v1)
		(assigna c2 cn v0)
		(assigna c3 c1 v1)
		(assigna c3 c2 v1)
		(assigna c3 c3 v1)
		(assigna c3 c4 v1)
		(assigna c3 cn v1)
		(assigna c4 c1 v0)
		(assigna c4 c2 v1)
		(assigna c4 c3 v1)
		(assigna c4 c4 v1)
		(assigna c4 cn v0)
		(assigna cn c1 v0)
		(assigna cn c2 v0)
		(assigna cn c3 v1)
		(assigna cn c4 v0)
		(assigna cn cn v0)


		(execute-phase)
	))
)
