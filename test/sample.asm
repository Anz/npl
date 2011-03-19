# comment
.data
    string      hello_world     "hello world! :D"
    integer     random_number   42

.text

@main
            arg random_number
            init integer i

            arg hello_world
            init string str
            
            arg str
            call print
