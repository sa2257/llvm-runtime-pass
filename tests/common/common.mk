SOURCES   := $(HW_SRCS) $(HOST_SRCS) $(RT_SRCS)
LLVMS     := $(SOURCES:%.c=%.ll)
HOST      := $(HOST_SRCS:%.c=%.ll)
RTPASS    := $(HW_SRCS:%.c=%-rt.ll)
RTOBJT    := $(RT_SRCS:%.c=%.o)
PROFPASS  := $(HW_SRCS:%.c=%-prof.ll)
PASSES    := $(PROFPASS) $(RTPASS) $(RTOBJT)
TARGET    := $(KERNEL)
SIMPLE    := exe

# Ordinary Clang options.
CXX+      := /usr/local/opt/llvm/bin/clang
CXX       := clang
CFLAGS    := python3.7-config --cflags
LDFLAGS   := python3.7-config --ldflags
OPT       := /usr/local/opt/llvm/bin/opt
ASMFLAG   := -S
LLVMFLAG  := -emit-llvm
CXXFLAGS  := 
RTFLAGS   := -load ../../build/skeleton/libLeechPass.so --leech
PROFFLAGS := -load ../../build/skeleton/libShackletonPass.so --shackleton
PASSFLAGS := -select

# Create assembly.
%.ll: %.c
	$(CXX) $(CXXFLAGS) $^ $(ASMFLAG) $(LLVMFLAG) -o $@

# Compile library.
%.o: %.c
	$(CXX+) `$(CFLAGS)` -c $^ -o $@

# Add runtime library pass on kernel.
$(KERNEL)-rt.ll: $(KERNEL).ll
	$(OPT) $(RTFLAGS) $(ASMFLAG) $^ -o $@

# Run profile pass on kernel.
$(KERNEL)-prof.ll: $(KERNEL).ll
	$(OPT) $(PROFFLAGS) $(ASMFLAG) $^ -o $@

# Link the program.
$(TARGET): $(PROFPASS) $(HOST) $(RTOBJT)
	$(CXX+) `$(LDFLAGS)` $^ --output $@

# Link the program.
$(SIMPLE): $(LLVMS)
	$(CXX+) $^ --output $@

# Run without pass.
.PHONY: simple
simple: $(SIMPLE)
	./$(SIMPLE)

# Run profiler.
.PHONY: runtime
runtime: $(TARGET)
	./$(TARGET)

.PHONY: clean
clean:
	rm -rf $(LLVMS) $(PASSES) $(TARGET) $(SIMPLE)  __pycache__
