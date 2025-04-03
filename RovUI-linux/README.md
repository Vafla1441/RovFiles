## RovUI - пульт управления для murROV
> Данное программа позиционируется как отправная точка для написания своего ПО управления murROV.

## Сборка
Данное ПО зависит от:
- Qt 5.6+
- C++11
- SFML 2.4.1 (должен находиться в одной директории с src/) [скачать](https://www.sfml-dev.org/download/sfml/2.4.1/)

После установки перечисленных библиотек, убедитесь, что пути до SFML в src/src.pro указаны корректно, пример:
```
// конец src.pro
INCLUDEPATH += ../SFML-2.4.1/include/

LIBS += -L../SFML-2.4.1/lib/ \
        -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio -lsfml-network
```
`INCLUDEPATH` - путь до заголовков .h / .hpp
`LIBS` - путь до библиотек
Для компиляции используйте скрипт `sh run.txt`

## Запуск
Для быстрого запуска используйте скрипт `sh RovUI.sh`