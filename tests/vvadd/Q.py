import time

class processingElement:
    def __init__(self, name, op, flex, regs, ports, bdwth, route, ctrl, branch):
        self.name  = name
        self.op    = op
        self.tick  = 1
        self.ins   = [0, 0]
        self.outs  = [None, None]
        self.flex  = flex
        self.regs  = regs
        self.ports = ports
        self.bdwth = bdwth
        self.route = route
        self.ctrl  = ctrl
        self.branch= branch
        self.validate()
    
    def validate(self):
        if (len(self.ins) + len(self.outs) > self.ports):
            print(self.name + " needs more ports than provided!")

    def update_ticks(self, ticks):
        return ticks + self.tick

    def operate(self, ins, ticks):
        self.ins =  ins
        if (self.ins[0] is None or self.ins[1] is None) :
            return None, self.update_ticks(ticks)
        switcher = {
                    'add': self.ins[0] + self.ins[1],
                    'mul': self.ins[0] * self.ins[1],
                    'div': self.ins[0] / self.ins[1] if self.ins[1] is not 0 else 0,
                    'sub': self.ins[0] - self.ins[1],
                    'gt' : self.ins[0] > self.ins[1],
                    'lt' : self.ins[0] < self.ins[1],
                    'ge' : self.ins[0] >= self.ins[1],
                    'eq' : self.ins[0] is self.ins[1],
                    'and': self.ins[0] and self.ins[1],
                    'or' : self.ins[0] or self.ins[1]
                   }
        self.outs = switcher.get(self.op, None)
        return self.outs, self.update_ticks(ticks)


class unitQ:
    def __init__(self, name, regT, regIdx, inNodeT, inWire, outT, outIdx, ports):
        self.name  = name
        self.tick  = 1
        self.regT  = regT
        self.inNT  = inNodeT
        self.outT  = outT
        #self.outNT = outNodeT
        self.ridx  = regIdx
        self.inW   = inWire
        self.oidx  = outIdx
        #self.outW  = outWire
        self.ports = ports
        self.validate()
    
    def validate(self):
        if (self.inW is not None and self.inW > self.ports):
            print(self.name + " accessing non existent port!")
        if (self.regT and self.ridx is None):
            print(self.name + " register is not set!")
        if (self.outT and self.oidx is None):
            print(self.name + " out memory is not updated!")
        if (not self.regT and self.inNT and self.inW > 3):
            print(self.name + " input is not a node!")
        if ((not self.regT and self.inW is None)):
            print(self.name + " input not set!")

    def update_ticks(self, ticks):
        return ticks + self.tick

    def route(self, ins, inputq, outputq, readyq, ticks):
        if (self.regT):
            out = inputq[self.ridx]
        elif self.inW is not None:
            out = ins[self.inW]
        else:
            out = None
        if (self.outT):
            outputq[self.oidx] = out
            readyq[self.oidx] = True if (out is not None) else False
        self.outs = out
        return self.outs, outputq, readyq, self.update_ticks(ticks)


class sliceQ:
    def __init__(self, name):
        self.name  = name
        self.validate()
        self.route()

    def validate(self):
        pass

    def route(self):
        pass
