## Устройство диагностики редуктора
Предназначено для считывания показаний с вибродатчиков (максимум 8) и термодатчиков (максимум 8) и передачи на ПК. Управление устройством и считывание данных осуществляется по Ethernet по протоколу UDP.

### Установки устройства по умолчанию
- IP адрес устройства: 192.168.0.10;
- порт команд: 2020, порт данных термодатчиков: 2021, порт данных вибродатчиков: 2022
(порты на стороне ПК должны быть такими же);
- частота дискретизации АЦП вибродатчиков равна 97.65625&nbsp;кГц,
делитель равен 1;
- включены каналы АЦП вибродатчиков с 0 по 5, выключены 6 и 7.

### Команды управления
Команды состоят из заголовка (4 байта в виде строки в кодировке ASCII) и опционального аргумента (число uint32) (таблица ниже). После каждой принятой команды отсылается эхо самой команды.

| Наименование                                   | Заголовок (4&nbsp;байта) | Аргумент (uint32)          |
| --------------------------------------------- | ------------------- | -------------------------- |
| Подключить устройство                          | cmd0                | -                          |
| Отключить устройство                           | cmd1                | -                          |
| Включить сбор данных с вибродатчиков           | cmd2                | -                          |
| Выключить сбор данных                          | cmd3                | -                          |
| Включить сбор данных с термосопротивлений      | cmd4                | -                          |
| Задать делитель частоты дискретизации АЦП вибродатчиков | cmd5      | Значение делителя от 1 до 5 |
| Задать активные каналы АЦП вибродатчиков       | cmd6                | Маска для каналов от 0x01 (активен только нулевой канал) до 0xFF (активны все каналы) |
| Сменить IP адрес                               | cmd7                | 4 байта нового ip адреса преобразованные в uint32 |
| Эхо                                            | cmd9                | -                          |
| Перезагрузка устройства                        | cmdr                | -                          |

### Примеры команд управления
- Установка делителя частоты дискретизации АЦП вибродатчиков равным 3:  
`0x63 0x6D 0x64 0x35 0x03 0x00 0x00 0x00`.

- Включение только 0, 1, 3, 5 каналов АЦП вибродатчиков:  
`0x63 0x6D 0x64 0x36 0x2B 0x00 0x00 0x00`.

- Установка нового ip адреса 192.168.0.21:  
`0x63 0x6D 0x64 0x37 0xC0 0xA8 0x00 0x15`.

### Примечания к командам
- при выполнении команды по смене IP адреса сначала устройство перезагружается и затем начинает работать с новым адресом;
- при активации всех каналов АЦП вибродатчиков, делитель частоты АЦП устанавливается минимум на 2 (максимальная частота 48.828125 кГц), если делитель был меньше или равен 2, то он не меняется;
- при выполнении команд установки делителя и активных каналов АЦП вибродатчиков, сбор данных с вибродатчиков останавливается

### Формат пакета данных с датчиков вибрации
Размер  | Наименование          | Значение по умолчанию
---     | ---                   | ---
u32     | идентификатор         | 1
u32     | номер пакета          | 0
byte[]  | 24 набора данных АЦП  | -

Каждый набор данных содержит по одному 24-битному слову с активного канала АЦП, слова упорядочены по возрастанию номера канала. Например, если активны 0, 1, 3, 5 каналы АЦП, то в наборе данных АЦП будет следующая последовательность байт (12&nbsp;байт):  
 `CH0[2] CH0[1] CH0[0] CH1[2] CH1[1] CH1[0] CH3[2] CH3[1] CH3[0] CH5[2] CH5[1] CH5[0]`,  
 где `CHx[2] CHx[1] CHx[0]` – 24-битное слово с канала x АЦП.

Общее количество байт в пакете данных с датчиков вибрации вычисляется по формуле:
$$
size = 4 + 4 + 24 \cdot n \cdot 3,
$$
где $n$ - количество активных каналов.

### Формат пакета данных с датчиков температуры (40 байт):
Размер  | Наименование          | Значение по умолчанию
---     | ---                   | ---
u32     | идентификатор         | 2
u32     | номер пакета          | 0
byte[32]| набор данных АЦП      | -

Каждый набор данных содержит 8 32-битных слов данных с каждого АЦП, в каждом слове данных содержится 24-битное значение АЦП выравнено по левому краю, т.е. находятся в [31-8] битах слова. Биты [7-0] нулевые. Значение АЦП (*ADC_VAL*) является знаковым 24-битным числом.

Температура с датчика в ℃ находится по следующей формуле:

$$
t = {1 \over \alpha R_{0\ RTD}} \left({ADC\_VAL \over 2^{22}Gain}R_{ref} - R_{0\ RTD}\right),
$$

где $\alpha$ – температурный коэффициент термосопротивления (равен $0.00385\ ℃$);
$Gain$ – коэффициент усиления АЦП (равен 16);
$R_{ref}$ – значение опорного сопротивления (равен $1.62e3\ Ом$);
$R_{0\ RTD}$ – значение сопротивления датчика при температуре 0 ℃ (равно $100 \ Ом$).