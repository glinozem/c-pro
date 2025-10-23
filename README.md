# c-pro — путь к профессиональному C
![CI](https://github.com/glinozem/c-pro/actions/workflows/ci.yml/badge.svg)

Учебно-практический репозиторий: целевые модули + строгие проверки (ASan/UBSan, Valgrind) + CI. Работаем под **Windows 11 + WSL2 (Ubuntu)** и **CLion** (WSL toolchain).

## Статус
- ✅ **STEP-01 — Logger** — модуль логирования.
- ✅ **STEP-02 — Memory** — обёртки и арена.
- ✅ **STEP-03 — include/** — публичные заголовки в `include/cpro/`, инклюды `<cpro/...>`.
- ✅ **STEP-04 — CI** — Actions: ASan/UBSan (gcc/clang) + Valgrind.
- 🔜 **STEP-05 — CLI** — утилита `cpro_cli` и тесты.

## Документация
- [STEP-01 — Logger](docs/STEP-01-logger.md)
- [STEP-02 — Memory](docs/STEP-02-mem-basics.md)
- [STEP-03 — File Organization](docs/STEP-03-file-organization.md)
- [STEP-04 — CI](docs/STEP-04-ci.md)

## Быстрый старт (WSL)
```bash
sudo apt update
sudo apt install -y build-essential cmake ninja-build gdb valgrind
# Debug (ASan/UBSan)
cmake -S . -B cmake-build-debug-wsl -G Ninja -DENABLE_SANИТIZERS=ON -DBUILD_TESTING=ON
cmake --build cmake-build-debug-wsl -j
ctest --test-dir cmake-build-debug-wsl -V
# Memcheck
cmake -S . -B cmake-build-memcheck-wsl -G Ninja -DENABLE_SANИТIZERS=OFF -DBUILD_TESTING=ON
cmake --build cmake-build-memcheck-wsl -j
valgrind --leak-check=full ./cmake-build-memcheck-wsl/test_logger
valgrind --leak-check=full ./cmake-build-memcheck-wsl/test_mem
```

## Структура (после STEP-03)
```
include/cpro/    logger.h, mem.h
src/logger/      logger.c, test_logger.c
src/mem/         mem.c, test_mem.c
```

## Политика качества
- Стандарт: C11
- Флаги: `-Wall -Wextra -Wpedantic -Werror=return-type`
- Диагностика: ASan/UBSan в Debug, Valgrind в отдельном профиле
- Тесты: CTest (logger.smoke, mem.smoke)
