.global _start

.section .text
_start:
    # 1. Initialisation de la pile (nécessaire pour la conversion int->string)
    li sp, 0x80004000   

    # 2. Calcul de Fibonacci (F12)
    li t0, 0            # a = 0
    li t1, 1            # b = 1
    li t2, 12           # n = 12 itérations
    li t3, 0            # i = 0

fibo_loop:
    beq t3, t2, print_result
    add t4, t0, t1      # temp = a + b
    mv  t0, t1          # a = b
    mv  t1, t4          # b = temp
    addi t3, t3, 1      # i++
    j fibo_loop

print_result:
    # Le résultat est dans t0 (C'est F_n, selon l'algo ci-dessus t0 aura la valeur précédente, t1 la courante)
    # Pour F(12), t1 contient 144. On va afficher t1.
    
    # Affiche le texte "Resultat: "
    la a0, msg_result
    call put_string

    # Affiche le nombre contenu dans t1
    mv a0, t1
    call print_int

    # Nouvelle ligne
    la a0, msg_newline
    call put_string

    # Fin
    ebreak


# --- Fonction : Affiche une chaîne (terminée par 0) ---
put_string:
    li a2, 0x10000000   # Adresse de sortie (UART/Terminal)
ps_loop:
    lbu a1, 0(a0)       # Charge le caractère
    beqz a1, ps_end     # Si 0, fin
    sb a1, 0(a2)        # Écrit le caractère
    addi a0, a0, 1      # Avance
    j ps_loop
ps_end:
    ret


# --- Fonction : Affiche un entier (a0) en décimal ---
# Utilise DIVU et REMU que vous venez d'implémenter !
print_int:
    li a2, 0x10000000   # Adresse de sortie
    mv t0, a0           # Nombre à afficher
    
    # Cas particulier : si nombre est 0
    bnez t0, pi_convert
    li t1, '0'
    sb t1, 0(a2)
    ret

pi_convert:
    # On empile les chiffres (car on les obtient à l'envers : unités, dizaines...)
    li t2, 10           # Diviseur
    li t3, 0            # Compteur de chiffres empilés

pi_loop_div:
    beqz t0, pi_print_stack
    remu t4, t0, t2     # t4 = nombre % 10 (chiffre)
    divu t0, t0, t2     # t0 = nombre / 10 (reste)
    
    addi t4, t4, '0'    # Convertir en ASCII ('0' = 48)
    addi sp, sp, -4     # Pousser sur la pile
    sw t4, 0(sp)
    
    addi t3, t3, 1      # Incrémenter compteur
    j pi_loop_div

pi_print_stack:
    # On dépile et on affiche
    beqz t3, pi_end
    lw t1, 0(sp)        # Récupérer chiffre
    addi sp, sp, 4      # Libérer pile
    sb t1, 0(a2)        # Afficher
    addi t3, t3, -1
    j pi_print_stack

pi_end:
    ret


.section .data
msg_result:
    .string "Fibonacci(12) = "
msg_newline:
    .string "\n"