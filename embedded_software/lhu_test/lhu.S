.global _start
_start:
    li x1, 0x80000100     # Adresse RAM
    li x2, 0xFFFF1234     # 0xFFFF est en haut (offset +2)
    sw x2, 0(x1)          # Écriture

    # On charge le demi-mot 0xFFFF (offset 2) avec LHU
    lhu x3, 2(x1)         

    ebreak