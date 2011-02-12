# comment
main:
            enter integer var1, integer var2, list string
            synce print
            synce print
            synce print
            argv 4
            arg string
            synce list_init
            argv 3
            arg var1
            synce integer
            arg var1
            arg string
            synce list_add
            argv 10
            arg var2
            synce integer
            arg var1
            arg var2
            arg var1
            synce integer_add
            arg string
            synce list_release
#            jmp skip
            sync min
skip:
            sync max
            ret

min:
            enter integer a, integer b
            synce print
            cmp a, b
            jle min_ret_a
            jg min_ret_b
min_ret_a:  ret a
min_ret_b:  ret b

max:
            enter integer a, integer b
            synce print
            cmp a, b
            jle min_ret_a
            jg min_ret_b
            sync min
max_ret_a:  ret a
max_ret_b:  ret b
