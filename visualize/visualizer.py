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

for i in range(int(len(coords) / 2)):
    folium.Marker(
        location=coords[i],
        icon=folium.DivIcon(
            icon_size=(10, 10),
            icon_anchor=(0, 0),
            html=f'<div style="font-size: 15pt; color : cadeblue">{i + 1}</div>'
        )
    ).add_to(my_map)

for i in range(int(len(coords) / 2)):
    folium.Marker(
        location=coords[int(len(coords) / 2) + i],
        icon=folium.DivIcon(
            icon_size=(10, 10),
            icon_anchor=(0, 0),
            html=f'<div style="font-size: 15pt; color : black">{i + 1}</div>'
        )
    ).add_to(my_map)

# Сохранение карты в HTML-файл

current_datetime = datetime.now()
current_datetime_str = current_datetime.strftime("%Y-%m-%d_%H:%M:%S")
my_map.save(f"maps/map_{current_datetime_str}.html")

