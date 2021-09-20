# Тестовое задание для осеннего проекта от JetBrains "Визуализация указателей на локальные переменные в отладчике"

1. [Сборка](#Сборка)
1. [Реализация](#Реализация)
1. [Гарантии-исключений](#Гарантии-исключений)


## Сборка

```
git clone https://github.com/fleex-x/JB-pointer-visualizer-test-task.git
cd JB-pointer-visualizer-test-task
cmake .
make
./random-container-tests
```

## Реализация
Учитывая, что в реализации я использую хэш-таблицы, сложность операций `insert`,`erase` и
`random_element` составляет `O(1)`. `insert` просто складывает элемент в конец вектора и запоминает в хеш-таблице, что среди его индексов теперь последний индекс вектора. `erase` меняет текущий элемент в таблице с последним, и перестраивает хэш-табицу с индексами за `O(1)`.

## Гарантии-исключений
Так как `std::vector` и `std::unordered_map` дают только базовую гарантию исключений, то мое решение тоже дает только базовую гарантию исключений.