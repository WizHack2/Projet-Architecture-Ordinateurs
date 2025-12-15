.global _start
_start:
    li x1, 0x55555555     # 0101...
    li x2, 0xAAAAAAAA     # 1010...

    # TEST 1 : Fusion parfaite
    # Les bits de x1 et x2 sont complémentaires.
    # Résultat : Tous les bits à 1.
    or x3, x1, x2

    # TEST 2 : Identité (Le Piège vs XOR)
    # 1 OR 1 = 1
    # Si c'était XOR, ça ferait 0.
    # Si c'était ADD, ça ferait une retenue (x2 * 2).
    or x4, x2, x2

    ebreak