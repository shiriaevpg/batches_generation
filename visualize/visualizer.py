import folium
from datetime import datetime

# Список координат в формате (широта, долгота)
coords = []

with open('batch.txt') as f:
    for line in f:
        lat, lon = map(float, line.strip().split())
        coords.append((lat, lon))

# Создание объекта карты с центром на первых координатах и установкой начального уровня масштабирования
my_map = folium.Map(location=coords[0], zoom_start=14)

# Добавление линии на карту
folium.PolyLine(coords, color='blue', weight=2.5, opacity=1).add_to(my_map)

# Сохранение карты в HTML-файл

current_datetime = datetime.now()
current_datetime_str = current_datetime.strftime("%Y-%m-%d_%H:%M:%S")
my_map.save(f"maps/map_{current_datetime_str}.html")

