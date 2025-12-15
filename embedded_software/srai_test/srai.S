.global _start
_start:
    # Cas Négatif (Le test critique)
    li x1, 0xF0000000     # 1111 0000 ... (Bit de signe = 1)
    srai x2, x1, 4        # On décale de 4

    # Cas Positif (Vérification)
    li x3, 0x10000000     # 0001 0000 ... (Bit de signe = 0)
    srai x4, x3, 4

    ebreak