.global _start
_start:
    li x1, 0x80000100     # Adresse RAM
    li x2, 0xFF102030     # 0xFF est tout en haut (offset +3)
    sw x2, 0(x1)          # Écriture

    # On charge l'octet 0xFF (offset 3) avec LBU
    lbu x3, 3(x1)         

    ebreak