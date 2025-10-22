# STEP-01 — Модуль логирования

> Цель шага: завести минимальный модуль логирования **logger**, собрать проект в **WSL2 (Ubuntu) + CLion**, включить строгие предупреждения и санитайзеры (ASan/UBSan), проверить отсутствие утечек под Valgrind. Итог — две строки в консоли и чистые отчёты инструментов.

---

## 1) TL;DR
- Папка: `src/logger/`.
- Публичный заголовок (пока локально): `src/logger/inc/logger.h` *(на STEP‑03 переедет в `include/cpro/logger.h`)*.
- API: `void log_info(const char*); void log_error(const char*);`
- Формат вывода: `[YYYY-MM-DD HH:MM:SS] LEVEL: message` в `stderr`.
- Сборка: CMake + WSL Toolchain из CLion.
- Проверки: `-Wall -Wextra -Wpedantic`, ASan/UBSan (Debug), Valgrind (Memcheck).

---

## 2) Окружение
- **Windows 11 + WSL2 (Ubuntu 24.04)**  
  Утилиты: `build-essential cmake ninja-build gdb clang lldb valgrind git`
- **CLion** с **WSL Toolchain** (не MinGW).

Полезные проверки:
```bash
cc --version
cmake --version
valgrind --version
```

---

## 3) Структура проекта (фрагмент)
```
c-pro/
  src/
    logger/
      inc/
        logger.h
      logger.c
      test_logger.c
  CMakeLists.txt
  docs/
    STEP-01-logger.md   # ← этот файл
```

---

## 4) Публичное API
```c
/* logger.h */
void log_info (const char* msg);
void log_error(const char* msg);
```
- Модуль **без состояния**: нет глобальных переменных, только функции.
- Заголовок защищён include‑guard’ом.
- Печать идёт в `stderr` (лог — это диагностический поток).

---

## 5) Ключевые моменты реализации
- Метка времени через `time()` + `localtime_r/_s` + `strftime("%Y-%m-%d %H:%M:%S")`.
- Внутренние помощники объявлены `static` в `logger.c`:
    - `stamp_now(char* buf, size_t n)` — формирует метку времени;
    - `log_with(const char* level, const char* msg, FILE* out)` — единый форматтер.
- Заголовки подключаются как `#include "logger.h"` (на STEP‑03 станет `<cpro/logger.h>`).

---

## 6) Сборка и запуск

### 6.1 Debug (ASan/UBSan включены)
```bash
cmake -S . -B cmake-build-debug-wsl -G Ninja -DENABLE_SANITIZERS=ON -DBUILD_TESTING=ON
cmake --build cmake-build-debug-wsl -j
./cmake-build-debug-wsl/test_logger
```
**Ожидаемый вывод:**
```
[YYYY-MM-DD HH:MM:SS] INFO: hello
[YYYY-MM-DD HH:MM:SS] ERROR: oops
```

### 6.2 Memcheck (Valgrind, санитайзеры выключены)
```bash
cmake -S . -B cmake-build-memcheck-wsl -G Ninja -DENABLE_SANITIZERS=OFF -DBUILD_TESTING=ON
cmake --build cmake-build-memcheck-wsl -j
valgrind --leak-check=full ./cmake-build-memcheck-wsl/test_logger
```
**Ожидаемый хвост отчёта:**
```
All heap blocks were freed -- no leaks are possible
ERROR SUMMARY: 0 errors
```

> Замечание: не совмещайте одновременно ASan и Valgrind — это разные режимы диагностики.

---

## 7) Флаги качества и тесты
- Компилятор: `-std=c11 -Wall -Wextra -Wpedantic -Werror=return-type`.
- Тест (через CTest), если `BUILD_TESTING=ON`:
  ```cmake
  include(CTest)
  if (BUILD_TESTING)
    add_test(NAME logger.smoke COMMAND test_logger)
  endif()
  ```
Запуск: `ctest --test-dir cmake-build-debug-wsl -V`.

---

## 8) Частые ошибки и решения
| Симптом | Причина | Решение |
|---|---|---|
| `inc/logger.h: No such file or directory` | Не экспортирован include‑путь или используешь MinGW‑toolchain | В `CMakeLists.txt`: `target_include_directories(logger PUBLIC src/logger/inc)`. Собирай через **WSL**. |
| CMake тянет `D:\...\mingw\gcc.exe` | Активен MinGW профиль | В CLion выбери **Toolchain: WSL**, профиль CMake привяжи к WSL, сделай Reset Cache + Reload. |
| Valgrind пишет про ASan runtime | Запущен бинарь, собранный с санитайзером | Используй отдельный профиль `-DENABLE_SANITIZERS=OFF` для Memcheck. |

---

## 9) Критерии приёмки STEP‑01
- [ ] Сборка в WSL проходит, предупреждений нет.
- [ ] `test_logger` печатает две строки с метками времени.
- [ ] Debug‑сборка (ASan/UBSan) — без репортов.
- [ ] Memcheck — `0` утечек / `0` ошибок.
- [ ] Этот файл добавлен в `docs/` и запушен в GitHub.

---

## 10) Что дальше
**STEP‑02 — Memory:** обёртки `xmalloc/xcalloc/xrealloc/xfree` и арена (bump allocator), мини‑тест `test_mem`, проверки ASan/UBSan и Valgrind.

