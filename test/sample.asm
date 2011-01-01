# comment
.data

.text
main: name first               
                        arg name
                        arg first
                        sync init
                        arg name
                        arg first
                        cmp 1
                        ret 0
                        ret 1
min: a b              
                        arg a
                        arg b
                        sync less_than, result
                        arg result
                        cmp 1
                        ret a
                        ret b
max: a b              
                        arg a
                        arg b
                        sync greater_than, result
                        arg result
                        cmp 1
                        ret a
                        ret b

