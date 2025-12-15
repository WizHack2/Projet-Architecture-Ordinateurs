.global _start
_start:
    li x1, 0x80000100     # Adresse RAM
    li x2, 0xFFFFFFFF     # Fond blanc (tout à 1)
    sw x2, 0(x1)          # On remplit la mémoire

    li x3, 0xCAFE         # Motif à écrire
    sh x3, 2(x1)          # On écrit 0xCAFE à l'offset +2 (partie haute)

    # On relit tout le mot
    lw x4, 0(x1)

    ebreak