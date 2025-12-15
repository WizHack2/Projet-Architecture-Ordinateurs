.global _start
_start:
    li x1, 0x80000100     # Adresse RAM
    li x2, 0xFFFF1234     # Donnée (0xFFFF est négatif, 0x1234 est positif)
    sw x2, 0(x1)          # Écriture en mémoire

    lh x3, 0(x1)          # Charge le demi-mot bas (0x1234 -> Positif)
    lh x4, 2(x1)          # Charge le demi-mot haut (0xFFFF -> Négatif)

    ebreak