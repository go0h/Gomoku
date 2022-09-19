# Gomoku

## Basic code style rules

[Google C++ code style guide](https://google.github.io/styleguide/cppguide.html#Formatting) (only **Formatting** part)

## Dependencies

1. [ASIO BOOST](https://www.boost.org/doc/libs/1_75_0/doc/html/boost_asio.html)
2. [Nlohmann JSON parser](https://github.com/nlohmann/json)

### Install dependencies

#### 1 Linux

```bash
sudo apt-get install libboost-all-dev
sudo apt-get install nlohmann-json3-dev
```

#### 2 MacOS

```bash
brew install boost
brew install nlohmann-json
```

## TODO

- Корректное завершение клиента если сервер упал.
- Игнор некорректных данных клиент-сервер.
- Вычисление рабочего поля, т.е. контур вокруг существующих фишек.
- Вычисление ситуаций - [link](https://arxiv.org/pdf/1912.05407.pdf)

## Уточнение правил

- Если игрок собирает 5 ряд и его можно захватить, но при этом у можно самому собрать 5 в ряд,
нужно обязательно производить захват.
