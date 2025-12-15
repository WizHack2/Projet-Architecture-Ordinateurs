.global _start
_start:
    li x1, -1           # 0xFFFFFFFF (Max UInt)
    li x2, 10
    li x3, 0

    # 1. Modulo Non-Signé (Max % 10 = 5)
    remu x4, x1, x2

    # 2. Modulo par Zéro (= Dividende = Max)
    remu x5, x1, x3

    ebreak