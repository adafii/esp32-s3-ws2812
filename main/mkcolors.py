# Generates colors for the demo
# usage: 'python mkcolors.py > demo_colors.h'

from math import sin, pi

radians = [round(x * 0.001 * pi, 3) for x in range(0, 12000)]

print('#include "color.h"')
print()
print(f'#define NUM_COLORS {len(radians)}')
print()

print("static const led_color_t color_buffer[NUM_COLORS] = {")

for rad in radians:
    r = round((sin(rad * 1.0 + 0.0 * pi) + 1) * 127.5)
    g = round((sin(rad * 0.5 + 0.5 * pi) + 1) * 127.5)
    b = round((sin(rad * 0.2 + 1.0 * pi) + 1) * 127.5)

    print("    {" + f'{r}, {g}, {b}' + "}, ")

print("};")
