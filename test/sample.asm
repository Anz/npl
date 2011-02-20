# comment
main:
            enter integer var1, integer var2, list string, string var1str
            call print_hello_world
            call print_hello_world
            call print_hello_world
            argv 4
            arg string
            calle list_init
            argv 'b'
            arg var1
            calle integer
            call print_hello_world
            argv 10
            arg var2
            calle integer
            arg var1
            arg var2
            arg var1
            calle integer_add
            arg var1
            arg var1str
            calle decimal_to_string
            arg var1str
            arg var1str
            argv 2
            calle print
            arg string
            calle list_release
            call min
skip:
            call max
            ret

min:
            enter integer a, integer b
            call print_hello_world
            cmp a, b
            jle min_ret_a
            jg min_ret_b
min_ret_a:  ret a
min_ret_b:  ret b

max:
            enter integer a, integer b
            call print_hello_world
            cmp a, b
            jle min_ret_a
            jg min_ret_b
            call min
max_ret_a:  ret a
max_ret_b:  ret b

print_hello_world:
            enter string str
            argv "hello_world_xD"
            arg str
            calle string_init
            arg str
            argv 1
            calle print
            arg str
            calle string_release
            ret

