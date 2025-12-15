.global _start
_start:
    li x1, 20
    li x2, -5
    li x3, 0            # Zéro pour le crash test

    # TEST 1 : Division signée standard
    # 20 / -5 = -4
    div x4, x1, x2

    # TEST 2 : Division par Zéro (Le Crash Test 1)
    # RISC-V ne plante pas, il retourne -1 (0xFFFFFFFF).
    # Si ton ému plante ici, tu as oublié le `if (rs2 == 0)`.
    div x5, x1, x3

    # TEST 3 : Overflow (Le Crash Test 2)
    # Min_Int / -1 provoque un overflow car Max_Int = Min_Int - 1.
    # RISC-V retourne Min_Int (0x80000000).
    li x6, 0x80000000   # -2147483648
    li x7, -1
    div x8, x6, x7

    ebreak