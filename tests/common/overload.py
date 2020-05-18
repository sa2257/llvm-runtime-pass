import os,sys,inspect
currentdir = os.path.dirname(os.path.abspath(inspect.getfile(inspect.currentframe())))
parentdir = os.path.dirname(currentdir)
sys.path.insert(0,parentdir) 
from Q import processingElement

dict_1in = {
        1: 5, 
        2: 4,
        3: 3,
        4: 2,
        5: 1
}

dict_2in = {
        1: {1: 5, 2: 4, 3: 3, 4: 2, 5: 1}, 
        2: {1: 4, 2: 3, 3: 2, 4: 1, 5: 5}, 
        3: {1: 3, 2: 2, 3: 1, 4: 5, 5: 4}, 
        4: {1: 2, 2: 1, 3: 5, 4: 4, 5: 3}, 
        5: {1: 1, 2: 5, 3: 4, 4: 3, 5: 2}, 
}

def leech_1in(in1):
    if type(in1) == float:
        output = in1 + 5.00
    else:
        output = dict_1in[in1] * 50
    return output

def leech_2in(in1, in2):
    if type(in1) == float:
        output = in1 + in2 + 5.00
    else:
        output = dict_2in[in1][in2] * 50
    return output

def call_pe(in1, in2, op):
    PE = processingElement('PE', op, False, 1, 4, 1, False, False, False)
    compute, tempTicks = PE.operate([in1, in2], ticks)
    return compute[0] 

if __name__ == "__main__":
    # execute only if run as a script
    # output = leech_2in(5,4);
    # output = leech_2in(5.00,4.00);
    output = call_pe(5.00, 4.00, 'add');
    print('Output: {}'.format(output))
