# Generates colors for the demo
# usage: 'python mkcolors.py > demo_colors.h'

from colorsys import hsv_to_rgb

linspace = [(x * 0.0005) for x in range(0, 2000)]

print('#include "color.h"')
print()
print(f'#define NUM_COLORS {len(linspace)}')
print()

print("static const led_color_t color_buffer[NUM_COLORS] = {")

for h in linspace:
    (r, g, b) = hsv_to_rgb(h, 1., 1.)
    r = round(r * 255)
    g = round(g * 255)
    b = round(b * 255)
    print("    {" + f'{g}, {r}, {b}' + "}, ")

print("};")
