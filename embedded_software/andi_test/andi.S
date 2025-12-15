.global _start
_start:
    li x1, 0xFF          # 1111 1111

    # TEST 1 : Masquage simple (Garder les 4 bits de poids faible)
    # 0xFF AND 0x0F = 0x0F
    andi x2, x1, 0x0F

    li x3, 0x12345678
    # TEST 2 : Extension de signe (Le Piège)
    # -1 en 12 bits (0xFFF) devient 0xFFFFFFFF après extension.
    # X AND 0xFFFFFFFF = X (Identité)
    andi x4, x3, -1

    ebreak