.global _start      # Pour supprimer le warning "_start not found"

_start:
    lui sp, 0x1     # On met a 1 pour vérifier que ca va pas etre touché grace au saut de jal.
    jal ra, cible   # Saute vers le label 'cible', stocke PC+4 dans ra
    lui sp, 0x2             # (Cette ligne sera sautée)

cible:
    lui gp, 0x3   # On atterrit ici