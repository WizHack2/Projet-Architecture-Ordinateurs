.global _start
_start:
    li x1, 10
    li x2, -1           # 0xFFFFFFFF (C'est le MAX possible en non-signé)

    # TEST 1 : Est-ce que 10 < MAX ? (Vrai)
    sltu x3, x1, x2

    # TEST 2 : Est-ce que MAX < 10 ? (Faux)
    sltu x4, x2, x1

    ebreak