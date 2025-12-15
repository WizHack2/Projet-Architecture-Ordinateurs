.global _start
_start:
    li x1, 10
    li x2, 20
    li x3, -10          # Valeur négative

    # TEST 1 : 10 < 20 ? (Vrai)
    slt x4, x1, x2

    # TEST 2 : 20 < 10 ? (Faux)
    slt x5, x2, x1

    # TEST 3 : -10 < 10 ? (Vrai)
    # Si c'était non-signé, -10 serait immense, donc > 10.
    slt x6, x3, x1

    ebreak