.global _start
_start:
    # 1. Chargement simple (x0 + 10)
    addi x1, x0, 10      # x1 = 10

    # 2. Addition standard (10 + 20)
    addi x2, x1, 20      # x2 = 30

    # 3. Soustraction via immédiat négatif (10 + (-4))
    addi x3, x1, -4      # x3 = 6

    ebreak