# c-pro — путь к профессиональному C

Учебно-практический репозиторий: маленькие целевые модули, строгие проверки качества и капстоун‑проект (мини‑компилятор). Работаем под **Windows 11 + WSL2 (Ubuntu)** и **CLion** с WSL‑toolchain.

---

## Статус
- ✅ **STEP‑01 — Logger:** модуль логирования, сборка в WSL/CLion, санитайзеры, Valgrind.
- ✅ **STEP‑02 — Memory:** безопасные обёртки `xmalloc/xcalloc/xrealloc/xfree` + арена (bump allocator).
- 🔜 **STEP‑03 — include/**: перенос публичных хедеров в `include/cpro/`, чистые инклюды `<cpro/...>`.
- 🔜 **STEP‑04 — CI:** GitHub Actions (Ubuntu + Ninja + `ctest`, ASan/UBSan).
- 🔜 **STEP‑10..13 — Компилятор:** driver → lexer → parser/AST → codegen.

Документация по шагам — в папке [`docs/`](docs/):
- [STEP‑01 — Logger](docs/STEP-01-logger.md)
- [STEP‑02 — Memory](docs/STEP-02-mem-basics.md)

---

## Быстрый старт (WSL + CMake + Ninja)

### Зависимости (в WSL/Ubuntu)
```bash
sudo apt update
sudo apt install -y build-essential cmake ninja-build gdb clang lldb valgrind git
```

### Сборка и запуск (Debug, ASan/UBSan включены)
```bash
cmake -S . -B cmake-build-debug-wsl -G Ninja -DENABLE_SANITIZERS=ON -DBUILD_TESTING=ON
cmake --build cmake-build-debug-wsl -j
./cmake-build-debug-wsl/test_logger
./cmake-build-debug-wsl/test_mem
ctest --test-dir cmake-build-debug-wsl -V
```

### Проверка утечек (Memcheck, санитайзеры выключены)
```bash
cmake -S . -B cmake-build-memcheck-wsl -G Ninja -DENABLE_SANITIZERS=OFF -DBUILD_TESTING=ON
cmake --build cmake-build-memcheck-wsl -j
valgrind --leak-check=full ./cmake-build-memcheck-wsl/test_logger
valgrind --leak-check=full ./cmake-build-memcheck-wsl/test_mem
```

> Примечание: ASan/UBSan и Valgrind не совмещаем — используем разные профили.

---

## Структура проекта (текущая)
```
c-pro/
  src/
    logger/
      inc/            # на STEP‑03 → include/cpro/
        logger.h
      logger.c
      test_logger.c
    mem/
      inc/            # на STEP‑03 → include/cpro/
        mem.h
      mem.c
      test_mem.c
  docs/
    STEP-01-logger.md
    STEP-02-mem-basics.md
  CMakeLists.txt
  .gitignore
  .gitattributes
  README.md
```
На **STEP‑03** публичные заголовки переедут в `include/cpro/`, а исходники будут подключать их как `<cpro/logger.h>` и `<cpro/mem.h>`.

---

## Политика качества
- Стандарт: **C11**.
- Флаги: `-Wall -Wextra -Wpedantic -Werror=return-type`.
- Диагностика: **ASan/UBSan** (Debug) и **Valgrind Memcheck** (отдельный профиль).
- Тесты: **CTest** (`logger.smoke`, `mem.smoke` при `BUILD_TESTING=ON`).

Запуск тестов:
```bash
ctest --test-dir cmake-build-debug-wsl -V
```

---

## Дорожная карта
1. **include/**: общий публичный каталог `include/cpro/`, зонтичная цель `cpro` в CMake.
2. **CI**: GitHub Actions — сборка Debug (ASan) + `ctest`, артефакты логов.
3. **Compiler (capstone)**:
    - Driver с CLI (`--lex/--parse/--codegen`),
    - Лексер,
    - Парсер + AST,
    - Генерация ассемблера x86‑64.

---

## Лицензия
MIT
