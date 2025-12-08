.global _start

_start:
    # 1. Charger un nombre "négatif" (Bit 31 à 1)
    lui ra, 0xFFFFF     # ra = 0xFFFFF000.
                        # En signé : Nombre très négatif.
                        # En unsigned : Nombre très grand.

    # 2. Charger un nombre positif petit
    lui sp, 0x00001     # sp = 0x00001000.

    # 3. Test BLTU
    # Question : Est-ce que (Unsigned)Grand < (Unsigned)Petit ?
    # Réponse attendue : NON. Le saut ne doit PAS se faire.
    bltu ra, sp, cible

    # CAS NORMAL (Succès pour BLTU)
    lui gp, 0x1        # On passe ici, c'est bon.
    jal x0, fin         # On finit.

cible:
    # CAS D'ERREUR (Si on arrive ici, c'est que BLTU s'est comporté comme BLT)
    lui gp, 0x2       # Echec.

fin:
    nop