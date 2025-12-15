.global _start
_start:
    li x1, 0x80000100     # Adresse RAM
    li x2, 0xDEADBEEF     # Une valeur de 32 bits bien reconnaissable
    sw x2, 0(x1)          # Écriture

    lw x3, 0(x1)          # Lecture

    ebreak