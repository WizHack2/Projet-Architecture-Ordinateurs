.global _start
_start:
    li x1, 10            # Valeur de référence

    slti x2, x1, 20      # 10 < 20 ? (Vrai)
    slti x3, x1, 5       # 10 < 5 ?  (Faux)
    
    # Test Signé (Important)
    li x4, -10
    slti x5, x4, 0       # -10 < 0 ? (Vrai)

    ebreak