.global _start
_start:
    li x1, 0xF0000000     # Nombre négatif (bit de signe à 1)

    # On décale de 4 vers la droite.
    # Attendu : Les 4 bits de gauche deviennent 0.
    srli x2, x1, 4        

    ebreak