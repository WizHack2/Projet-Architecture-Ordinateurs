.global _start
_start:
    li x1, 0xF0          # 1111 0000

    # TEST 1 : Fusion simple
    # 0xF0 OR 0x0F = 0xFF (1111 1111)
    ori x2, x1, 0x0F

    # TEST 2 : Extension de signe (Le Piège)
    # -1 en 12 bits (0xFFF) doit devenir 0xFFFFFFFF
    # 0 OR 0xFFFFFFFF = 0xFFFFFFFF
    ori x3, x0, -1

    ebreak