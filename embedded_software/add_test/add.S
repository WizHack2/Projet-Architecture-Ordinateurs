.global _start
_start:
    li x1, 10
    li x2, -1             # 0xFFFFFFFF (La plus grande valeur non-signée)

    # TEST 1 : Addition simple
    add x3, x1, x1        # 10 + 10 = 20

    # TEST 2 : Dépassement / Signe
    # 10 + (-1) = 9
    # En binaire : 10 + 0xFFFFFFFF = 0x100000009 -> on garde les 32 bits du bas -> 9
    add x4, x1, x2        

    ebreak