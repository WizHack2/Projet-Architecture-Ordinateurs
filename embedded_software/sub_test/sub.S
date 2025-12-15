.global _start
_start:
    li x1, 20
    li x2, 10

    # TEST 1 : Positif (20 - 10)
    sub x3, x1, x2

    # TEST 2 : Négatif (10 - 20)
    sub x4, x2, x1

    ebreak