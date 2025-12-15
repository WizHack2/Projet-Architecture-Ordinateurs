.global _start
_start:
    li x1, 20
    li x2, -7
    li x3, -20
    li x4, 0            # Zéro

    # 1. Signe suit le dividende (20 % -7 = 6)
    rem x5, x1, x2
    # 2. Signe suit le dividende (-20 % -7 = -6)
    rem x6, x3, x2
    # 3. Div par Zéro (= Dividende = 20)
    rem x7, x1, x4
    
    ebreak