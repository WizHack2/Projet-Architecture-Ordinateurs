lui sp, 0x1     # On met a 1 pour vérifier que ca va pas etre touché grace au saut de jal.
jalr ra, 0x4(sp)   # Saute de 0x4 + sp, stocke PC+4 dans ra
lui sp, 0x2             # (Cette ligne sera sautée)