These are simple programs for LLVM development 

        ./c_expr -c " cos(pi* (2+0.45 ) )"
                call(cos,operator(* ,identifier(pi) ,operator(+ ,value(2.000000) ,value(0.450000))))
                cos((pi*(2.000000+0.450000)))
                eval -> 0.156434
        ./c_expr -c "1*2+3+4+5"
                operator(+ ,operator(+ ,operator(+ ,operator(* ,value(1.000000) ,value(2.000000)) ,value(3.000000)) ,value(4.000000)) ,value(5.000000))
                ((((1.000000*2.000000)+3.000000)+4.000000)+5.000000)
                eval -> 14.000000
        ./c_expr -c "1+2*3+4+5"
                operator(+ ,operator(+ ,operator(+ ,value(1.000000) ,operator(* ,value(2.000000) ,value(3.000000))) ,value(4.000000)) ,value(5.000000))
                (((1.000000+(2.000000*3.000000))+4.000000)+5.000000)
                eval -> 16.000000
        ./c_expr -c "1+2+3*4+5"
                operator(+ ,operator(+ ,operator(+ ,value(1.000000) ,value(2.000000)) ,operator(* ,value(3.000000) ,value(4.000000))) ,value(5.000000))
                (((1.000000+2.000000)+(3.000000*4.000000))+5.000000)
                eval -> 20.000000
        ./c_expr -c "1+2+3+4*5"
                operator(+ ,operator(+ ,operator(+ ,value(1.000000) ,value(2.000000)) ,value(3.000000)) ,operator(* ,value(4.000000) ,value(5.000000)))
                (((1.000000+2.000000)+3.000000)+(4.000000*5.000000))
                eval -> 26.000000
        ./c_expr -c "1*2+3+4*5"
                operator(+ ,operator(+ ,operator(* ,value(1.000000) ,value(2.000000)) ,value(3.000000)) ,operator(* ,value(4.000000) ,value(5.000000)))
                (((1.000000*2.000000)+3.000000)+(4.000000*5.000000))
                eval -> 25.000000
        ./c_expr -c "1*(2+3)+4*5"
                operator(+ ,operator(* ,value(1.000000) ,operator(+ ,value(2.000000) ,value(3.000000))) ,operator(* ,value(4.000000) ,value(5.000000)))
                ((1.000000*(2.000000+3.000000))+(4.000000*5.000000))
                eval -> 25.000000

