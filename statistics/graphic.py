import matplotlib.pyplot as plt
import numpy as np
import datetime

# Здесь должны быть данные или код для их генерации.
# Пример данных для демонстрации:

a, b, n = [], [], []

filename = "statistics.txt"
# Чтение данных из файла
with open(filename, 'r') as file:
    for line in file:
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

for i in range(len(old_len)):
    if (old_len[i] == 0):
        old_len[i] = 1
        new_len[i] = 1

# Настройка размера фигуры для трех графиков
plt.figure(figsize=(15, 5))

max_size = max(size)
sizes = np.array([0] * (int(max_size) + 1))

for sz in size:
    sizes[int(sz)] += 1

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
plt.bar(size, old_len - new_len, color='blue')
plt.xlabel('Количество заказов в батче')
plt.ylabel('Метрика (уменьшенеие длины батча в км)')

# Третий график: Зависимость a/b от b (обычный график)
plt.subplot(1, 3, 3)  # 1 строка, 3 колонки, 3-й график
plt.bar(size, old_len / new_len, color='green')
plt.xlabel('Длина до (км)')
plt.ylabel('Относительная длина батча после применения алгоритма')

# Отображаем всю картинку с графиками
plt.tight_layout()  # автоматически подгоняет графики, чтобы они не перекрывали друг друга

current_datetime = datetime.datetime.now()
current_datetime_str = current_datetime.strftime("%Y-%m-%d_%H-%M-%S")

# Показываем гистограмму
plt.savefig('images/histogram.png')
