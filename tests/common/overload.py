import os,sys,inspect
currentdir = os.path.dirname(os.path.abspath(inspect.getfile(inspect.currentframe())))
parentdir = os.path.dirname(currentdir)
sys.path.insert(0,parentdir) 
from itertools import chain
from collections import deque
from Q import processingElement, unitQ

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

dict_ops = {
        1:  'neg', 
        2:  'add',
        3:  'sub',
        4:  'mul',
        5:  'div',
        6:  'rem',
        7:  'neg', 
        8:  'add',
        9:  'sub',
        10: 'mul',
        11: 'div',
        12: 'rem',
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
    ticks = 0
    PE = processingElement('PE', dict_ops[op], False, 1, 4, 1, False, False, False)
    compute, tempTicks = PE.operate([in1, in2], ticks)
    return compute 

def call_chain(in1, in2, in3, in4):
    im1 = in1 + in2
    im2 = im1 * in3
    im3 = im1 * in4
    output = im2 + im3
    return output

def call_func(in1, in2, out):
    input1 = in1[0]
    input2 = in2[0]
    output = out[0]
    for i, (i1, i2) in enumerate(zip(input1, input2)):
        im1 = i1 + i2
        im2 = im1 * 10
        im3 = im1 * 0.1
        output[i] = im2 + im3
    return output

#def call_func(in1, in2, out):
#    input1 = in1[0]
#    input2 = in2[0]
#    output = out[0]
#    f = open("check.data", "r")
#    current = []
#    for x in f:
#        if ('%%' in x):
#            current = []
#            continue
#        current.append(float(x.strip('\n')))
#    return current

if __name__ == "__main__":
    # execute only if run as a script
    # output = leech_2in(5,4);
    # output = leech_2in(5.00,4.00);
    #output = call_pe(5.00, 4.00, 8);
    output = call_func(([5.32, 4.35],), ([3.65, 7.74],),([0.00, 0.00],));
    print('Output: {}'.format(output))

