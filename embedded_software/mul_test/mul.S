.global _start
_start:
    li x1, 10
    li x2, -5             # Négatif

    # TEST 1 : Signe (10 * -5 = -50)
    mul x3, x1, x2

    # TEST 2 : Overflow (Le Piège)
    # 65536 (0x10000) * 65536 (0x10000) = 4,294,967,296 (0x100000000)
    # Le résultat dépasse 32 bits. MUL ne garde que le bas.
    li x4, 0x10000
    mul x5, x4, x4

    ebreak