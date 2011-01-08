# comment
.symbol
    main
    min
    max

.data

.text
main:
            enter var1, var2
            synce print
            synce print
            synce print
            argv 32
            arg var1
            synce integer
            argv 10
            arg var2
            synce integer
            arg var1
            arg var2
            arg var1
            synce integer_mul
            sync min
            sync max
            ret

min:
            enter a, b
            cmp a, b
            jle min_ret_a
            jg min_ret_b
min_ret_a:  ret a
min_ret_b:  ret b

max:
            enter a, b
            cmp a, b
            jle min_ret_a
            jg min_ret_b
            sync min
max_ret_a:  ret a
max_ret_b:  ret b
