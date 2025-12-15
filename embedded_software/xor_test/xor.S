.global _start
_start:
    li x1, 0xAAAAAAAA     # Motif alterné (1010...)
    li x2, 0xFFFFFFFF     # Masque plein (1111...)

    # TEST 1 : Annulation
    # X XOR X = 0
    xor x3, x1, x1

    # TEST 2 : Inversion
    # 1010... XOR 1111... = 0101... (0x55555555)
    xor x4, x1, x2

    # TEST 3 : Distinction avec OR
    # 1 XOR 1 = 0 (Si c'était OR, ça ferait 1)
    li x5, 0x3
    li x6, 0x1
    xor x7, x5, x6        # 0011 XOR 0001 = 0010

    ebreak