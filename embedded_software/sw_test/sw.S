.global _start
_start:
    li x1, 0x80000100     # Adresse RAM
    li x2, 0x12345678     # Valeur test
    sw x2, 0(x1)          # On écrit le mot

    lw x3, 0(x1)          # On relit le mot entier
    ebreak