import matplotlib.pyplot as plt
import numpy as np
import sys

a, b, n = [], [], []

in_file = sys.argv[1]
# Чтение данных из файла
with open(in_file, 'r') as file:
    flag = True
    for line in file:
        if flag:
            benchmark = float(line)
            flag = False
            continue
        # Разделяем строку по пробелам и удаляем лишние пробелы
        row = line.strip().split()
        # Преобразуем строки в вещественные числа и добавляем их в соответствующие списки
        n_value, a_value, b_value = map(float, row)
        n.append(n_value)
        a.append(a_value)
        b.append(b_value)

size = np.array(n)
old_len = np.array(a)
new_len = np.array(b)

diff = old_len - new_len
av_percent = 0

av_unit_gain = 0

for i in range(len(old_len)):
    if old_len[i] == 0:
        av_percent += 100
        old_len[i] = 1
        new_len[i] = 1
    else:
        av_percent += (diff[i] / old_len[i]) * 100
        av_unit_gain += diff[i] / size[i]

av_percent /= len(old_len)
av_unit_gain /= len(old_len)

# Настройка размера фигуры для трех графиков
fig = plt.figure(figsize=(15, 7))
fig.suptitle(f"{sys.argv[3]}, Время генерации батча: {benchmark} сек, средний процент экономии: {av_percent}, "
             f"удельная выгода {av_unit_gain} км/размер батча")
max_size = max(size)
sizes = np.array([0] * (int(max_size) + 1))
sums = np.array([0] * (int(max_size) + 1))


for i in range(len(old_len)):
    sizes[int(size[i])] += 1
    sums[int(size[i])] += old_len[i] - new_len[i]

for i in range(len(sizes)):
    if sizes[i] != 0:
        sums[i] /= sizes[i]

other_sums = [sums[i] / i for i in range(len(sums)) if sums[i] != 0]
sums = [sums[i] for i in range(len(sums)) if sums[i] != 0]

# Исправленный Второй график: Распределение количества заказов в батче
plt.subplot(1, 3, 1)  # 1 строка, 3 колонки, 2-й график
# Создаем список всех возможных размеров батчей от 0 до max_size
possible_sizes = np.arange(int(max_size) + 1)
plt.bar(possible_sizes, sizes, color='red')
plt.xlabel('Количество заказов в батче')
plt.ylabel('Количество батчей')
plt.yscale('log')

# Первый график: Гистограмма зависимости a/b от n
plt.subplot(1, 3, 2)  # 1 строка, 3 колонки, 1-й график
plt.plot(sums, color='blue')
plt.xlabel('Количество заказов в батче')
plt.ylabel('Метрика (уменьшенеие длины батча в км)')

# Третий график: Зависимость a/b от b (обычный график)
plt.subplot(1, 3, 3)  # 1 строка, 3 колонки, 3-й график
plt.plot(other_sums, color='green')
plt.xlabel('Размер батча')
plt.ylabel('Удельная метрика')

# Отображаем всю картинку с графиками
plt.tight_layout()  # автоматически подгоняет графики, чтобы они не перекрывали друг друга

# Показываем гистограмму
plt.savefig(sys.argv[2])
