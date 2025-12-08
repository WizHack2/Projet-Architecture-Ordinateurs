.global _start

_start:
    lui ra, 0x3         # ra = 0x1000
    lui sp, 0x4         # sp = 0x1000
    blt ra, sp, cible   # Si ra < sp, saut vers 'cible' (saut de 8 octets)
    lui sp, 0x2         # Cette ligne DOIT être sautée (sp restera à 0x4000)

cible:
    lui ra, 0x2         # ra deviendra 0x2000