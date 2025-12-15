.global _start
_start:
    li x1, 1
    
    # TEST 1 : Décalage simple
    # 1 << 4 = 16 (0x10)
    slli x2, x1, 4

    li x3, 0xFFFFFFFF
    # TEST 2 : Perte de bits à gauche
    # On décale de 4. Les 4 bits de poids fort (gauche) disparaissent.
    # Les 4 bits de poids faible (droite) deviennent 0.
    slli x4, x3, 4

    ebreak