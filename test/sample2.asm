# comment
.symbol
    main

.data

.text
main:
            enter skalar1, skalar2, summe, produkt
            argv 100
            arg skalar1
            synce integer
            argv 15
            arg skalar2
            synce integer
            arg skalar1
            arg skalar2
            arg summe
            synce integer_add
            arg skalar1
            arg skalar2
            arg produkt
            synce integer_mul
            ret
