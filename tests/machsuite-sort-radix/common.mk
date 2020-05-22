SOURCES   := $(HW_SRCS) $(HOST_SRCS) $(RT_SRCS)
LLVMS     := $(SOURCES:%.c=%.ll)
RTPASS    := $(HW_SRCS:%.c=%-rt.ll) $(RT_SRCS:%.c=%.ll)
PROFPASS  := $(SOURCES:%.c=%-prof.ll)
ALLCPASS  := $(SOURCES:%.c=%-alloc.ll)
DEPSPASS  := $(SOURCES:%.c=%-dep.ll)
PLCEPASS  := $(SOURCES:%.c=%-plc.ll)
PASSES    := $(PROFPASS) $(ALLCPASS) $(DEPSPASS) $(PLCEPASS) $(RTPASS)
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
ALLCFLAGS := -load ../../build/skeleton/libStiltonPass.so --stilton
DEPSFLAGS := -load ../../build/skeleton/libSheltonPass.so --shelton
PLCEFLAGS := -load ../../build/skeleton/libSingletonPass.so --singleton
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

# Run allocate pass on kernel.
$(KERNEL)-alloc.ll: $(KERNEL).ll
	$(OPT) $(ALLCFLAGS) $(PASSFLAGS) $(ASMFLAG) $^ -o $@

# Run dependence pass on kernel.
$(KERNEL)-dep.ll: $(KERNEL).ll
	$(OPT) $(DEPSFLAGS) $(ASMFLAG) $^ -o $@

# Run placement pass on kernel.
$(KERNEL)-plc.ll: $(KERNEL).ll
	$(OPT) $(PLCEFLAGS) $(PASSFLAGS) $(ASMFLAG) $^ -o $@

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

# Run linking pass.
.PHONY: runtime
runtime: $(TARGET)
	./$(TARGET)

# Run profiler.
.PHONY: profile
profile: $(TARGET)
	./$(TARGET)

# Run profiler with timer.
.PHONY: time
time: $(TARGET)
	time ./$(TARGET)

# Run allocator.
.PHONY: allocate
allocate: $(KERNEL)-alloc.ll
	continue

# Run dependece.
.PHONY: depgen
depgen: $(KERNEL)-dep.ll
	continue

# Run placement.
.PHONY: place
place: $(KERNEL)-plc.ll
	continue

.PHONY: clean
clean:
	rm -f $(LLVMS) $(PASSES) $(TARGET) $(SIMPLE) $(OUTPUT)

