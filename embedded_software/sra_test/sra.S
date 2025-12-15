.global _start
_start:
    li x1, 0xF0000000     # Nombre négatif (Signe = 1)
    li x2, 4              # Shift de 4
    
    # TEST 1 : Extension du signe
    # Attendu : 0xFF000000 (Les 4 bits vides deviennent des 1)
    sra x3, x1, x2

    li x4, 36             # 36 = 32 + 4.
                          # Les 5 bits de poids faible valent 4.
    
    # TEST 2 : Masquage du shift
    # Attendu : Pareil (décalage de 4) -> 0xFF000000
    sra x5, x1, x4

    ebreak