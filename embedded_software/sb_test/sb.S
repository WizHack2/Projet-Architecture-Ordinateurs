.global _start
_start:
    li x1, 0x80000100     # Adresse RAM
    li x2, 0xFFFFFFFF     # Fond blanc (tout à 1)
    sw x2, 0(x1)          # On remplit la mémoire

    li x3, 0xAA           # Motif à écrire (10101010)
    sb x3, 1(x1)          # On écrit 0xAA à l'offset +1

    # On relit tout le mot pour voir si on a cassé les voisins
    lw x4, 0(x1)

    ebreak