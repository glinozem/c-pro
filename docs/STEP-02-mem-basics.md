# STEP-02 — Мини-библиотека памяти (обёртки + арена)

> Цель шага: ввести единые **безопасные обёртки** над `malloc/calloc/realloc/free` и реализовать **простую арену** (bump allocator) для временных выделений. Подключить модуль в сборку, написать мини‑тест и прогнать в двух режимах диагностики: **ASan/UBSan** и **Valgrind**.

---

## 1) Что реализовано
- Обёртки:
    - `void* xmalloc(size_t n)` — OOM ⇒ аварийное завершение (fail‑fast). Обрабатывает `malloc(0)`.
    - `void* xcalloc(size_t n, size_t sz)` — нулирует; проверка переполнения `n * sz`.
    - `void* xrealloc(void* p, size_t n)` — проверка ошибок; `n==0` ⇒ 1 байт.
    - `void  xfree(void* p)` — для единообразия интерфейса.
- Арена (bump allocator):
    - `int arena_init(mem_arena* A, size_t capacity)` — выделяет буфер.
    - `void* arena_alloc(mem_arena* A, size_t size, size_t align)` — выдаёт место с выравниванием (по умолчанию `alignof(max_align_t)`).
    - `void arena_reset(mem_arena* A)` — сбрасывает оффсет в 0 (мгновенно «освобождает» всё).
    - `void arena_free(mem_arena* A)` — освобождает буфер.
    - `char* arena_strdup(mem_arena* A, const char* s)` — удобство для строк.

---

## 2) Структура репозитория (фрагмент)
```
c-pro/
  src/
    mem/
      inc/            # на STEP-03 переносим в include/cpro/
        mem.h
      mem.c
      test_mem.c
```

---

## 3) Публичное API (кратко)
```c
/* Обёртки */
void* xmalloc(size_t n);
void* xcalloc(size_t n, size_t sz);
void* xrealloc(void* p, size_t n);
void  xfree(void* p);

/* Арена */
typedef struct {
    unsigned char* base;
    size_t cap;
    size_t off;
} mem_arena;

int   arena_init(mem_arena* A, size_t capacity);  /* 0=ok, -1=oom */
void  arena_reset(mem_arena* A);
void  arena_free (mem_arena* A);
void* arena_alloc(mem_arena* A, size_t size, size_t align); /* align=2^k */
char* arena_strdup(mem_arena* A, const char* s);
```

---

## 4) Сборка и запуск

### Debug (ASan/UBSan включены)
```bash
cmake -S . -B cmake-build-debug-wsl -G Ninja -DENABLE_SANITIZERS=ON -DBUILD_TESTING=ON
cmake --build cmake-build-debug-wsl -j
./cmake-build-debug-wsl/test_mem
```
**Ожидаемый вывод:**
```
[YYYY-MM-DD HH:MM:SS] INFO: heap wrappers ok
[YYYY-MM-DD HH:MM:SS] INFO: arena ok
```

### Memcheck (Valgrind, санитайзеры выключены)
```bash
cmake -S . -B cmake-build-memcheck-wsl -G Ninja -DENABLE_SANITIZERS=OFF -DBUILD_TESTING=ON
cmake --build cmake-build-memcheck-wsl -j
valgrind --leak-check=full ./cmake-build-memcheck-wsl/test_mem
```
**Ожидаемый хвост отчёта:**
```
All heap blocks were freed -- no leaks are possible
ERROR SUMMARY: 0 errors
```

> Примечание: **не** совмещайте ASan и Valgrind одновременно — используйте разные профили сборки.

---

## 5) Флаги качества
- Компилятор: `-std=c11 -Wall -Wextra -Wpedantic -Werror=return-type`.
- В Debug-профиле для тестов добавлены: `-fsanitize=address,undefined -fno-omit-frame-pointer`.
- CTest (если активирован `BUILD_TESTING=ON`):
  ```bash
  ctest --test-dir cmake-build-debug-wsl -V
  ```

---

## 6) Ключевые решения
- **Fail‑fast**: при OOM — немедленное завершение процесса (`abort()`), чтобы рано выявить ошибки.
- **Безопасность умножения**: перед `calloc(n, sz)` проверяем `n > SIZE_MAX / sz`.
- **Портируемость**: `malloc(0)` приводит к выделению 1 байта.
- **Арена**: выравнивание до `alignof(max_align_t)`, `reset()` — быстрый способ «освободить всё» пакетно.
- **Чистые интерфейсы**: всё API в `.h`, реализация — в `.c`, внутренние помощники — `static`.

---

## 7) Частые ошибки и быстрые фиксы
| Симптом | Причина | Решение |
|---|---|---|
| `fatal error: inc/logger.h: No such file or directory` | Тест использует относительный путь к логгеру | Подключай `#include "logger.h"` (на STEP-03 будет `<cpro/logger.h>`), а в CMake у `logger` должен быть PUBLIC include-dir. |
| Предупреждение Valgrind про ASan runtime | Запустили бинарь, собранный с санитайзерами, под Valgrind | Используй отдельный профиль `-DENABLE_SANITIZERS=OFF`. |
| UB/утечки в Debug | Ошибки в коде | Смотри отчёт ASan/UBSan, исправляй, потом прогоняй Valgrind. |

---

## 8) Критерии приёмки STEP-02
- [ ] `test_mem` успешно выполняется в Debug (ASan/UBSan без отчётов).
- [ ] Valgrind (Memcheck) показывает: **0 утечек / 0 ошибок**.
- [ ] Компиляция без предупреждений на `-Wall -Wextra -Wpedantic`.
- [ ] Этот документ добавлен в `docs/` и запушен в GitHub.

---

## 9) Что дальше
**STEP-03 — include/**: перенос публичных заголовков в `include/cpro/`, переход на инклюды `<cpro/...>`, обновлённый `CMakeLists.txt`, `ctest` в CI.
