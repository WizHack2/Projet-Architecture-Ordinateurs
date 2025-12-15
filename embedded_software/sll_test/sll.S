.global _start
_start:
    li x1, 1            # Valeur à décaler
    li x2, 4            # Décalage normal (4)
    
    # TEST 1 : Normal
    # 1 << 4 = 16 (0x10)
    sll x3, x1, x2

    li x4, 36           # 36 = 100100 en binaire.
                        # Les 5 derniers bits sont 00100 (4).
    
    # TEST 2 : Masquage du shift (Le Piège)
    # Le CPU doit ignorer le bit 32 et ne voir que le 4.
    # Résultat attendu : 1 << 4 = 16.
    sll x5, x1, x4

    ebreak