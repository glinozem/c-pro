# STEP-04 — CI (GitHub Actions)

## Что делает workflow
- **Матрица ASan/UBSan** (gcc/clang): сборка Debug с санитайзерами + `ctest -V`.
- **Valgrind (gcc)**: сборка без санитайзеров и прогон `test_logger`/`test_mem` под Memcheck.
- Используем Ninja, отдельные билды, таймауты; при падении — артефакты логов.

## Файл
`.github/workflows/ci.yml` (см. репозиторий).

### Бейдж для README
```markdown
![CI](https://github.com/glinozem/c-pro/actions/workflows/ci.yml/badge.svg)
```

## Как читать логи
- Падение тестов: `Testing/Temporary/LastTest.log`.
- Проблемы конфигурации: `CMakeFiles/CMakeError.log`.
- Компоновка/компилятор: шаг **Build** в Actions.

## Частые вопросы
- **apt зависает** на ubuntu-24.04 → в ASan-джобах apt не используем; для Valgrind ставим только `valgrind` с ретраями.
- **ASan и Valgrind одновременно?** Нет: разные профили сборки.

## Критерии приёмки
- Все 3 job зелёные.
- В логах `ctest`: `100% tests passed`.
- Валгринд: `All heap blocks were freed` / `ERROR SUMMARY: 0 errors`.
