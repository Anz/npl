# comment
.symbol
    main
    min
    max

.data

.text
main:
            enter args
            synce print
            synce print
            synce print
            init i, integer
            arg i
            synce integer
            arg i
            arg 32
            synce integer_set
            arg i
            arg 42
            sync min, low
            arg i
            arg 42
            sync max, high
            ret 0

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
