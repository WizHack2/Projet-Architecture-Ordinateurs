.global _start

_start:
    lui ra, 0xFFFFF
    lui sp, 0x1
    bgeu ra, sp, cible
    lui gp, 0x2
    jal x0, fin

cible:
    lui gp, 0x1 # Faut etre la a la fin

fin:
    nop