#define _LCDML_DISP_w                 128            // LCD width
#define _LCDML_DISP_h                 64             // LCD height
#define _LCDML_DISP_font              u8g_font_6x13  // u8glib font (more fonts under u8g.h line 1520 ...)
#define _LCDML_DISP_font_w            6              // font width
#define _LCDML_DISP_font_h            13             // font height
#define _LCDML_DISP_cursor_char       ">"            // cursor char
#define _LCDML_DISP_cur_space_before  2              // cursor space between
#define _LCDML_DISP_cur_space_behind  4              // cursor space between
#define _LCDML_DISP_box_x0            0              // start point (x0, y0)
#define _LCDML_DISP_box_y0            0              // start point (x0, y0)
#define _LCDML_DISP_box_x1            128            // width x  (x0 + width)
#define _LCDML_DISP_box_y1            64             // hight y  (y0 + height)
#define _LCDML_DISP_scrollbar_w       6  // scrollbar width (if this value is < 3, the scrollbar is disabled)
#define _LCDML_DISP_cols_max          ((_LCDML_DISP_box_x1-_LCDML_DISP_box_x0)/_LCDML_DISP_font_w)
#define _LCDML_DISP_rows_max          ((_LCDML_DISP_box_y1-_LCDML_DISP_box_y0-((_LCDML_DISP_box_y1-_LCDML_DISP_box_y0)/_LCDML_DISP_font_h))/_LCDML_DISP_font_h)
#define _LCDML_DISP_rows              _LCDML_DISP_rows_max  // max rows
#define _LCDML_DISP_cols              20                   // max cols
