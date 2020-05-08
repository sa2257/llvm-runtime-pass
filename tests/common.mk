SOURCES   := $(HW_SRCS) $(HOST_SRCS) $(RT_SRCS)
LLVMS     := $(SOURCES:%.c=%.ll)
RTPASS    := $(HW_SRCS:%.c=%-rt.ll) $(RT_SRCS:%.c=%.ll)
PROFPASS  := $(SOURCES:%.c=%-prof.ll)
PASSES    := $(PROFPASS)  $(RTPASS)
TARGET    := $(KERNEL)
SIMPLE    := exe
OUTPUT	  := output.txt

# Ordinary Clang options.
CXX+      := /usr/local/opt/llvm/bin/clang
CXX       := clang
OPT       := /usr/local/opt/llvm/bin/opt
ASMFLAG   := -S
LLVMFLAG  := -emit-llvm
CXXFLAGS  := 
RTFLAGS   := -load ../../build/skeleton/libSkeletonPass.so --skeleton
PROFFLAGS := -load ../../build/skeleton/libShackletonPass.so --shackleton
PASSFLAGS := -select

# Create assembly.
%.ll: %.c
	$(CXX) $(CXXFLAGS) $^ $(ASMFLAG) $(LLVMFLAG) -o $@

# Add runtime library pass on kernel.
$(KERNEL)-rt.ll: $(KERNEL).ll
	$(OPT) $(RTFLAGS) $(ASMFLAG) $^ -o $@

# Run profile pass on kernel.
$(KERNEL)-prof.ll: $(KERNEL).ll
	$(OPT) $(PROFFLAGS) $(PASSFLAGS) $(ASMFLAG) $^ -o $@

# Run profile pass on main.
%-prof.ll: %.ll
	$(OPT) $(PROFFLAGS) $(ASMFLAG) $^ -o $@

# Link the program.
$(TARGET): $(RTPASS)
	$(CXX+) $^ --output $@

# Link the program.
$(SIMPLE): $(LLVMS)
	$(CXX+) $^ --output $@

# Run without pass.
.PHONY: simple
simple: $(SIMPLE)
	./$(SIMPLE)

# Run profiler.
.PHONY: profile
profile: $(TARGET)
	./$(TARGET)

.PHONY: clean
clean:
	rm -f $(LLVMS) $(PASSES) $(TARGET) $(SIMPLE) $(OUTPUT)

