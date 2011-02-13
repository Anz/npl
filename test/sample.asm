# comment
main:
            enter integer var1, integer var2, list string
            sync print_hello_world
            sync print_hello_world
            sync print_hello_world
            argv 4
            arg string
            synce list_init
            argv 'b'
            arg var1
            synce integer
            sync print_hello_world
            argv 10
            arg var2
            synce integer
            arg var1
            arg var2
            arg var1
            synce integer_add
#            arg string
#            synce print2
            arg string
            synce list_release
#            jmp skip
            sync min
skip:
            sync max
            ret

min:
            enter integer a, integer b
            sync print_hello_world
            cmp a, b
            jle min_ret_a
            jg min_ret_b
min_ret_a:  ret a
min_ret_b:  ret b

max:
            enter integer a, integer b
            sync print_hello_world
            cmp a, b
            jle min_ret_a
            jg min_ret_b
            sync min
max_ret_a:  ret a
max_ret_b:  ret b

print_hello_world:
            enter list string
            argv 4
            arg string
            synce list_init
            argv '!'
            argv 'd'
            argv 'l'
            argv 'r'
            argv 'o'
            argv 'W'
            argv '_'
            argv 'o'
            argv 'l'
            argv 'l'
            argv 'e'
            argv 'H'
            argv 12
            arg string
            synce string_init
            arg string
            synce print
            arg string
            synce list_release
            ret

