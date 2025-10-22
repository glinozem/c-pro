cmake_minimum_required(VERSION 3.16)
project(c_pro C)
set(CMAKE_C_STANDARD 11)

# ---- options ----
option(ENABLE_SANITIZERS "Enable Address/UB sanitizers for test targets" ON)

# ---- logger (step 1) ----
add_library(logger src/logger/logger.c)
target_include_directories(logger PUBLIC src/logger/inc)

add_executable(test_logger src/logger/test_logger.c)
target_link_libraries(test_logger PRIVATE logger)

# ---- mem (step 2, optional) ----
set(MEM_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/mem")
if (EXISTS "${MEM_DIR}/mem.c" AND EXISTS "${MEM_DIR}/inc/mem.h")
    add_library(mem "${MEM_DIR}/mem.c")
    target_include_directories(mem PUBLIC "${MEM_DIR}/inc")

    if (EXISTS "${MEM_DIR}/test_mem.c")
        add_executable(test_mem "${MEM_DIR}/test_mem.c")
        target_link_libraries(test_mem PRIVATE mem logger)
    endif()
endif()

# ---- warnings helper ----
function(enable_warnings tgt)
    if (CMAKE_C_COMPILER_ID MATCHES "Clang|GNU")
        target_compile_options(${tgt} PRIVATE -Wall -Wextra -Wpedantic -Werror=return-type)
    endif()
endfunction()

foreach(t IN ITEMS logger test_logger mem test_mem)
    if (TARGET ${t})
        enable_warnings(${t})
    endif()
endforeach()

# ---- sanitizers for tests only ----
if (ENABLE_SANITIZERS AND CMAKE_C_COMPILER_ID MATCHES "Clang|GNU")
    foreach(t IN ITEMS test_logger test_mem)
        if (TARGET ${t})
            target_compile_options(${t} PRIVATE -fsanitize=address,undefined -fno-omit-frame-pointer)
            target_link_options(${t} PRIVATE   -fsanitize=address,undefined)
        endif()
    endforeach()
endif()

# ---- CTest (single place; no duplicates) ----
include(CTest)
if (BUILD_TESTING)
    if (TARGET test_logger)
        add_test(NAME logger.smoke COMMAND test_logger)
    endif()
    if (TARGET test_mem)
        add_test(NAME mem.smoke COMMAND test_mem)
    endif()
endif()
