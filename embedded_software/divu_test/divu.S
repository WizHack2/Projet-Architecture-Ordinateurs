.global _start
_start:
    li x1, -1           # 0xFFFFFFFF (Max UInt)
    li x2, 2
    li x3, 0            # Zéro pour le crash test

    # TEST 1 : Grand nombre / Petit nombre
    # Si c'était signé : -1 / 2 = 0
    # En non-signé : 4 294 967 295 / 2 = 2 147 483 647
    divu x4, x1, x2

    # TEST 2 : Petit / "Grand" (Négatif en apparence)
    # 2 / 0xFFFFFFFF
    # Résultat attendu : 0
    divu x5, x2, x1

    # TEST 3 : Division par Zéro (Le Crash Test)
    # RISC-V spec : x / 0 = 0xFFFFFFFF (Max UInt) pour DIVU
    # Si ton ému plante, tu n'as pas géré le cas `if (rs2 == 0)`
    divu x6, x1, x3

    ebreak