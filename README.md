PrimeAndCompositeTautology2

This project aims to generate CNF formulas that are difficult for SAT solvers to solve.
In particular, it aims to create CNF formulas that establish an exponential lower bound for Subset Propagation Redundancy (SPR).

The CNF represents the following proposition:
A natural number with n bits cannot be both prime and composite at the same time.

A Ruby script is used to generate the CNF:

ruby prime_and_composite_tautology.rb 4

As a bonus, a script has also been created to generate a CNF that determines whether a given number can be factored into prime numbers.

ruby prime_factoring_cnf.rb 57

You can merge the execution results of CaDiCaL with the CNF comments.

cadical prime_factoring_57.cnf > cadical_result.txt
ruby merge_result.rb cadical_result.txt prime_factoring_57.cnf
