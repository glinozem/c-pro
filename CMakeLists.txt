cmake_minimum_required(VERSION 3.16)
project(c_pro C)
set(CMAKE_C_STANDARD 11)

option(ENABLE_SANITIZERS "Enable Address/UB sanitizers for test targets" ON)

# единый публичный include
set(CPRO_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/include")

# ---------- logger ----------
add_library(logger src/logger/logger.c)
target_include_directories(logger PUBLIC "${CPRO_INCLUDE_DIR}")

add_executable(test_logger src/logger/test_logger.c)
target_link_libraries(test_logger PRIVATE logger)

# ---------- mem ----------
add_library(mem src/mem/mem.c)
target_include_directories(mem PUBLIC "${CPRO_INCLUDE_DIR}")
target_link_libraries(mem PRIVATE logger)  # если mem будет логировать ошибки

add_executable(test_mem src/mem/test_mem.c)
target_link_libraries(test_mem PRIVATE mem logger)

# ---------- umbrella interface (для будущих клиентов) ----------
add_library(cpro INTERFACE)
target_include_directories(cpro INTERFACE "${CPRO_INCLUDE_DIR}")
target_link_libraries(cpro INTERFACE logger mem)

# ---------- warnings ----------
function(enable_warnings tgt)
    if (CMAKE_C_COMPILER_ID MATCHES "Clang|GNU")
        target_compile_options(${tgt} PRIVATE -Wall -Wextra -Wpedantic -Werror=return-type)
    endif()
endfunction()

foreach(t IN ITEMS logger mem test_logger test_mem)
    if (TARGET ${t})
        enable_warnings(${t})
    endif()
endforeach()

# ---------- sanitizers (только для тестов) ----------
if (ENABLE_SANITIZERS AND CMAKE_C_COMPILER_ID MATCHES "Clang|GNU")
    foreach(t IN ITEMS test_logger test_mem)
        if (TARGET ${t})
            target_compile_options(${t} PRIVATE -fsanitize=address,undefined -fno-omit-frame-pointer)
            target_link_options(${t} PRIVATE   -fsanitize=address,undefined)
        endif()
    endforeach()
endif()

# ---------- CTest ----------
include(CTest)
if (BUILD_TESTING)
    add_test(NAME logger.smoke COMMAND test_logger)
    add_test(NAME mem.smoke    COMMAND test_mem)
endif()
