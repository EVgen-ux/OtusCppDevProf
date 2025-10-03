# Курс "C++ Developer. Professional" Домашнее задание №11 “Outer join”

Асинхронный сервер сервер для выполнения операций (INTERSECTION, SYMMETRIC DIFFERENCE) над множествами.

## Возможности

- **Сетевой интерфейс** - асинхронная обработка TCP-подключений с использованием Boost.Asio
- **Управление таблицами** - поддержка двух таблиц (A и B) с операциями INSERT и TRUNCATE
- **Реляционные операции** - вычисление пересечения и симметрической разности таблиц
- **In-memory база данных** - хранение данных в SQLite в оперативной памяти
- **Многопользовательский режим** - одновременная работа нескольких клиентов

## Установка и запуск

### Установка зависимостей (Ubuntu/Debian)

```
sudo apt update
sudo apt install build-essential cmake libboost-system-dev libsqlite3-dev
```

### Установка из пакета

Скачайте .deb пакет, соответствующий релизу. Введите следующие команды:

```
sudo dpkg -i  HW011-0.0.X-Linux.deb 
```
(где Х - номер релиза)

### Запуск сервера
```
join_server <port> 
```

### Запуск клиента
```
join_client <host> <port>
```

### Базовые операции:
```
INSERT <table> <id> <name>    # Добавить запись в таблицу
TRUNCATE <table>              # Очистить таблицу
```

## Пример использования

В терминал запустите сервер:

```
join_server 9000
```
В другом терминал запустите клиент:

```
join_client localhost 9000
```

Построчно добавьте данные:

```
INSERT A 0 lean
INSERT A 1 sweater
INSERT A 2 frank
INSERT A 3 violation
INSERT A 4 quality
INSERT A 5 precision
INSERT B 3 proposal
INSERT B 4 axample
INSERT B 5 lake
INSERT B 6 flour
INSERT B 7 wonder
INSERT B 8 selection
```

```
INTERSECTION                  # Пересечение таблиц A и B
SYMMETRIC_DIFFERENCE          # Симметрическая разность таблиц
TRUNCATE A                    # Очистить таблицу A

```

### Завершение работы
Для завершения работы сервера или клиента нажмите Ctrl+C в терминале, где он запущен.
