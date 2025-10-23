# STEP-03 — Глобальный `include/cpro/` и чистые инклюды

## Зачем
- Единый публичный API-пакет (`include/cpro`) без относительных путей вроде `"inc/..."`.
- Потребители пишут `#include <cpro/logger.h>` и `#include <cpro/mem.h>`.
- В CMake PUBLIC-include решает подключение автоматически при линковке на таргет.

## Итоговая структура
```
include/
  cpro/
    logger.h
    mem.h
src/
  logger/ logger.c  test_logger.c
  mem/    mem.c     test_mem.c
```
> На следующих шагах в `include/` также будут появляться заголовки других модулей.

## CMake (ключевые строки)
```cmake
set(CPRO_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/include")

add_library(logger src/logger/logger.c)
target_include_directories(logger PUBLIC "${CPRO_INCLUDE_DIR}")

add_library(mem src/mem/mem.c)
target_include_directories(mem PUBLIC "${CPRO_INCLUDE_DIR}")
target_link_libraries(mem PRIVATE logger)

add_library(cpro INTERFACE)
target_include_directories(cpro INTERFACE "${CPRO_INCLUDE_DIR}")
target_link_libraries(cpro INTERFACE logger mem)
```

## Сборка/проверка
```bash
# Debug (ASan/UBSan)
cmake -S . -B cmake-build-debug-wsl -G Ninja -DENABLE_SANITIZERS=ON -DBUILD_TESTING=ON
cmake --build cmake-build-debug-wsl -j
ctest --test-dir cmake-build-debug-wsl -V

# Memcheck (Valgrind)
cmake -S . -B cmake-build-memcheck-wsl -G Ninja -DENABLE_SANИТИZERS=OFF -DBUILD_TESTING=ON
cmake --build cmake-build-memcheck-wsl -j
valgrind --leak-check=full ./cmake-build-memcheck-wsl/test_logger
valgrind --leak-check=full ./cmake-build-memcheck-wsl/test_mem
```

## Типичные ошибки
- `fatal error: logger.h: No such file or directory` — проверь, что **везде** `<cpro/logger.h>`, а не `"logger.h"`.
- В логе компилятора нет `-I …/include` — проверь `target_include_directories(... PUBLIC "${CPRO_INCLUDE_DIR}")`.

## Критерии приёмки
- Инклюды только через `<cpro/...>`.
- Debug + ctest ок, Valgrind: 0/0.
