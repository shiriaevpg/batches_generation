import folium
from datetime import datetime
import random

# Определим список из 12 основных цветов
basic_colors = [
    '#FF0000',  # красный
    '#FFA500',  # оранжевый
    '#FFFF00',  # желтый
    '#008000',  # зеленый
    '#00FFFF',  # голубой
    '#0000FF',  # синий
    '#800080',  # пурпурный
    '#FF00FF',  # фуксия
    '#A52A2A',  # коричневый
    '#FFC0CB',  # розовый
    '#808080',  # серый
    '#000000',  # черный
]

# Открываем файл для чтения
with open('batch.txt') as f:
    # Считываем количество маршрутов
    n = int(f.readline().strip())
    routes = []  # Список для хранения маршрутов

    # Читаем каждый маршрут
    for _ in range(n):
        m = int(f.readline().strip())  # количество координат для текущего маршрута
        coords = []  # Список координат текущего маршрута

        # Читаем координаты для текущего маршрута
        for _ in range(m):
            lat, lon = map(float, f.readline().strip().split())
            coords.append((lat, lon))

        # Сохранение карты в HTML-файл
        routes.append(coords)  # Добавляем маршрут в список маршрутов

# Создаем объект карты (используем первые координаты первого маршрута для центрирования карты)
if routes:
    my_map = folium.Map(location=routes[0][0], zoom_start=14)

    # Добавляем все маршруты на карту
    for route in routes:
        color = random.choice(basic_colors)  # Выбираем случайный цвет для маршрута
        folium.PolyLine(route, color=color, weight=2.5, opacity=1).add_to(my_map)

    # Сохраняем карту в HTML-файл
    current_datetime = datetime.now()
    current_datetime_str = current_datetime.strftime("%Y-%m-%d_%H-%M-%S")
    filename = f"maps/map_{current_datetime_str}.html"
    my_map.save(filename)
    print(f"Map saved to {filename}")
else:
    print("No routes to display.")
