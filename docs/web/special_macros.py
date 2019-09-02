def declare_variables(variables, macro):

    @macro
    def add2(x):
        return(2 + x)
    @macro
    def add3(x):
            return(3 + x)
