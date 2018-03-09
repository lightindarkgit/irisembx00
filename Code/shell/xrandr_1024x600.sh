#!/bin/bash
xrandr --newmode "1024x600_60.00"   49.00  1024 1072 1168 1312  600 603 613 624 -hsync +vsync
xrandr --addmode LVDS1 "1024x600_60.00"
xrandr --output LVDS1 --mode "1024x600_60.00"

