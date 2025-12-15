.global _start
_start:
    li x1, 10           # Une petite valeur
    li x2, -1           # 0xFFFFFFFF (Max int en non-signé)

    # 1. Test classique : 10 < 20 ? (Vrai)
    sltiu x3, x1, 20    

    # 2. Test classique : 10 < 5 ? (Faux)
    sltiu x4, x1, 5     

    # 3. LE TEST CRITIQUE (Signé vs Non-Signé)
    # On demande : Est-ce que x2 (-1) est plus petit que 10 ?
    # En math normal (signé) : OUI (-1 < 10)
    # En non-signé (SLTIU)   : NON (4294967295 > 10)
    sltiu x5, x2, 10

    ebreak