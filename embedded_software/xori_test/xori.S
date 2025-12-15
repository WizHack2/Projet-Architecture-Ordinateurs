.global _start
_start:
    li x1, 0              # On commence à 0
    
    # TEST 1 : Extension de signe (-1)
    # L'immédiat -1 (12 bits) = 0xFFF. 
    # S'il est bien étendu, il devient 0xFFFFFFFF.
    # 0 XOR 0xFFFFFFFF = 0xFFFFFFFF.
    xori x2, x1, -1       

    li x3, 0xFF           # 1111 1111
    # TEST 2 : Logique simple
    # 0xFF XOR 0x0F -> (1111 1111) XOR (0000 1111) = 1111 0000 (0xF0)
    xori x4, x3, 0x0F     

    ebreak