What will you do?
I will make a template to replace instructions and basic blocks with a runtime function call.

How will you do it?
I will create two passes, one to replace an instruction, one to replace a basic block.
These passes will insert a runtime function call which will evoke a function in a runtime library in C.
These functions can also call Python functions.

How will you empirically measure success?
I will test couple of instruction replacements and a basic block replacement for the Machsuite and some
other benchmark suite.

