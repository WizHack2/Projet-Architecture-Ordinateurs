.global _start
_start:
    li x1, -1             # 0xFFFFFFFF
                          # Signé : -1
                          # Non-signé : Max Int

    li x2, 0x80000000     # 1000...0000
                          # Signé : -2147483648 (Min Int)
                          # Non-signé : 2147483648 (Grand nombre positif)

    # LE TEST CRITIQUE
    # On fait : (-1) * (2147483648)
    # Résultat math : -2147483648
    # En 64 bits Hex : 0xFFFFFFFF 80000000
    # On garde le haut : 0xFFFFFFFF
    mulhsu x3, x1, x2

    ebreak