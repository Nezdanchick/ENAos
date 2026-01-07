#pragma once

extern void gcursor_show();
extern void gcursor_hide();
extern void gcursor_set(int x, int y);
extern void gcursor_move(int dx, int dy);
extern int gcursor_get_x();
extern int gcursor_get_y();
extern void gcursor_update();
