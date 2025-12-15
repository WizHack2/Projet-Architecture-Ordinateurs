.global _start
_start:
    li ra, 0x80000100     # Adresse RAM
    li sp, 0xFF102030     # Donnée (0xFF est négatif, 0x30 est positif)
    sw sp, 0(ra)          # Écriture en mémoire

    lb gp, 0(ra)          # Charge l'octet 0x30 (Positif)
    lb tp, 3(ra)          # Charge l'octet 0xFF (Négatif)

    ebreak