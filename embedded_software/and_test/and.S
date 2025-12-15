.global _start
_start:
    li x1, 0xFFFFFFFF
    li x2, 0x12345678

    # TEST 1 : Identité
    # X AND 1111... = X
    and x3, x1, x2

    li x4, 0x0F
    li x5, 0xF0
    
    # TEST 2 : Intersection nulle
    # 00001111 AND 11110000 = 0
    and x6, x4, x5

    li x7, 2      # 0010
    li x8, 1      # 0001
    
    # TEST 3 : Le piège du C (&& vs &)
    # Bitwise : 0010 & 0001 = 0
    # Logical : 2 && 1 = Vrai (1)
    and x9, x7, x8

    ebreak