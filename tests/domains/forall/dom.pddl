(define (domain forall-test-1)
    (:requirements :typing :universal-preconditions :negative-preconditions)
    (:types knight - object)
    (:predicates
        (has-shouted ?a - knight)
        (is-ready ?a - knight)
    )
    (:action get-ready
        :parameters (?a - knight)
        :precondition (not (is-ready ?a))
        :effect (is-ready ?a)
    )
    (:action shout
        :parameters (?a - knight)
        :precondition (forall (?a2 - knight) (is-ready ?a2))
        :effect (has-shouted ?a)
    )
)
