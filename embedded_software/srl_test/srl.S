.global _start
_start:
    li x1, 0xF0000000     # Nombre négatif
    li x2, 4              # Shift normal
    
    # TEST 1 : Comportement Logique
    # Attendu : 0x0F000000 (Zéros entrants)
    srl x3, x1, x2

    li x4, 36             # 36 = 32 + 4.
                          # Les 5 bits de poids faible valent 4.
    
    # TEST 2 : Masquage du shift
    # Attendu : Le même résultat (décalage de 4, pas de 36 ou 0)
    srl x5, x1, x4

    ebreak