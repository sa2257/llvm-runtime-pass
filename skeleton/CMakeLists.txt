add_library(LeechPass MODULE
    # List your source files here.
    Leech.cpp
)
add_library(PragmaPass MODULE
    # List your source files here.
    Pragma.cpp
)

# Use C++11 to compile our pass (i.e., supply -std=c++11).
target_compile_features(LeechPass PRIVATE cxx_range_for cxx_auto_type)
target_compile_features(PragmaPass PRIVATE cxx_range_for cxx_auto_type)

# LLVM is (typically) built with no C++ RTTI. We need to match that;
# otherwise, we'll get linker errors about missing RTTI data.
set_target_properties(LeechPass PROPERTIES
    COMPILE_FLAGS "-fno-rtti"
)
set_target_properties(PragmaPass PROPERTIES
    COMPILE_FLAGS "-fno-rtti"
)

# Get proper shared-library behavior (where symbols are not necessarily
# resolved when the shared library is linked) on OS X.
if(APPLE)
    set_target_properties(LeechPass PROPERTIES
        LINK_FLAGS "-undefined dynamic_lookup"
    )
    set_target_properties(PragmaPass PROPERTIES
        LINK_FLAGS "-undefined dynamic_lookup"
    )
endif(APPLE)
