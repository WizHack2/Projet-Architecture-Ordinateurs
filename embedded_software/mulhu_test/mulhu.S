.global _start
_start:
    li x1, -1             # 0xFFFFFFFF
                          # En signé : -1
                          # En non-signé : 4 294 967 295 (Max UInt)

    # LE TEST CRITIQUE : Max * Max
    # Math : (2^32 - 1) * (2^32 - 1)
    # Résultat 64 bits : 0xFFFFFFFE 00000001
    # Partie haute attendue : 0xFFFFFFFE
    mulhu x2, x1, x1

    # Comparaison avec MULH (pour mémoire)
    # Si c'était signé : (-1) * (-1) = 1 -> Haut = 0
    mulh x3, x1, x1

    ebreak