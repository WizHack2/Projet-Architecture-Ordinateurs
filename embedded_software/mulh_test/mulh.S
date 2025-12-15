.global _start
_start:
    # TEST 1 : Petit nombre (Partie haute vide)
    li x1, 10
    mulh x2, x1, x1       # 10 * 10 = 100. Haut = 0.

    # TEST 2 : Débordement positif
    li x3, 0x10000        # 65536
    # 0x10000 * 0x10000 = 0x100000000 (1 suivi de 32 zéros)
    # Partie basse = 0, Partie haute = 1
    mulh x4, x3, x3

    # TEST 3 : Test Signé (Le Crash Test)
    li x5, -1             # 0xFFFFFFFF
    li x6, 2
    # -1 * 2 = -2
    # En 64 bits : 0xFFFFFFFF FFFFFFFE
    # Partie haute attendue : 0xFFFFFFFF (-1)
    mulh x7, x5, x6

    ebreak