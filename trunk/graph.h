/*  Grafx2 - The Ultimate 256-color bitmap paint program

    Copyright 2007-2008 Adrien Destugues
    Copyright 1996-2001 Sunset Design (Guillaume Dorme & Karl Maritaud)

    Grafx2 is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; version 2
    of the License.

    Grafx2 is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Grafx2; if not, see <http://www.gnu.org/licenses/> or
    write to the Free Software Foundation, Inc.,
    59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

//////////////////////////////////////////////////////////////////////////////
///@file graph.h
/// Graphic functions that target the screen and/or image.
//////////////////////////////////////////////////////////////////////////////

void Shade_list_to_lookup_tables(word * list, short step, byte mode, byte * table_inc,
        byte * table_dec
);

void Transform_point(short x, short y,
                       float cos_a, float sin_a, short * rx, short * ry);

int Init_mode_video(int width, int height, int fullscreen,int pixel_ratio);

byte No_effect(word x,word y,byte color);
byte Effect_shade(word x,word y,byte color);
byte Effect_quick_shade(word x,word y,byte color);
byte Effect_tiling(word x,word y,byte color);
byte Effect_smooth(word x,word y,byte color);

void Display_foreback(void);


void Display_pixel(word x,word y,byte color);

void Display_paintbrush(short x,short y,byte color,byte is_preview);
void Hide_paintbrush(short x,short y);

void Resize_image(word chosen_width,word chosen_height);

void Fill_general(byte fill_color);
void Replace(byte New_color);

void Pixel_figure_preview    (word x_pos,word y_pos,byte color);
void Pixel_figure_preview_auto(word x_pos,word y_pos);
void Pixel_figure_preview_xor(word x_pos,word y_pos,byte color);
void Pixel_figure_preview_xorback(word x_pos,word y_pos,byte color);
void Pixel_figure_in_brush(word x_pos,word y_pos,byte color);

void Draw_empty_circle_permanent(short center_x,short center_y,short radius,byte color);
void Draw_empty_circle_preview  (short center_x,short center_y,short radius,byte color);
void Hide_empty_circle_preview (short center_x,short center_y,short radius);
void Draw_filled_circle         (short center_x,short center_y,short radius,byte color);

void Draw_empty_ellipse_permanent(short center_x,short center_y,short horizontal_radius,short vertical_radius,byte color);
void Draw_empty_ellipse_preview  (short center_x,short center_y,short horizontal_radius,short vertical_radius,byte color);
void Hide_empty_ellipse_preview (short center_x,short center_y,short horizontal_radius,short vertical_radius);
void Draw_filled_ellipse        (short center_x,short center_y,short horizontal_radius,short vertical_radius,byte color);

void Clamp_coordinates_45_degrees(short ax, short ay, short* bx, short* by);
void Draw_line_general(short start_x,short start_y,short end_x,short end_y, byte color);
void Draw_line_permanet  (short start_x,short start_y,short end_x,short end_y,byte color);
void Draw_line_preview    (short start_x,short start_y,short end_x,short end_y,byte color);
void Draw_line_preview_xor(short start_x,short start_y,short end_x,short end_y,byte color);
void Draw_line_preview_xorback(short start_x,short start_y,short end_x,short end_y,byte color);
void Hide_line_preview   (short start_x,short start_y,short end_x,short end_y);

void Draw_empty_rectangle(short start_x,short start_y,short end_x,short end_y,byte color);
void Draw_filled_rectangle(short start_x,short start_y,short end_x,short end_y,byte color);

void Draw_curve_permanent(short x1, short y1, short x2, short y2, short x3, short y3, short x4, short y4, byte color);
void Draw_curve_preview  (short x1, short y1, short x2, short y2, short x3, short y3, short x4, short y4, byte color);
void Hide_curve_preview (short x1, short y1, short x2, short y2, short x3, short y3, short x4, short y4, byte color);

void Airbrush(short clicked_button);

void Gradient_basic           (long index,short x_pos,short y_pos);
void Gradient_dithered (long index,short x_pos,short y_pos);
void Gradient_extra_dithered(long index,short x_pos,short y_pos);
void Degrade_aleatoire         (long index,short x_pos,short y_pos);

void Draw_grad_circle  (short center_x,short center_y,short radius,short spot_x,short spot_y);
void Draw_grad_ellipse(short center_x,short center_y,short horizontal_radius,short vertical_radius,short spot_x,short spot_y);
void Draw_grad_rectangle(short rax,short ray,short rbx,short rby,short vax,short vay, short vbx, short vby);

void Polyfill_general(int vertices, short * points, int color);
void Polyfill(int vertices, short * points, int color);

// Gestion des backups:
void Download_infos_page_main(T_Page * page);
void Download_infos_page_spare(T_Page * page);
void Download_infos_backup(T_List_of_pages * list);
void Free_all_backup_lists(void);
int  Backup_with_new_dimensions(int upload,int width,int height);
int  Backup_and_resize_the_spare(int width,int height);
void Undo(void);
void Redo(void);
void Free_current_page(void);
void Exchange_main_and_spare(void);

void Change_magnifier_factor(byte factor_index);

void Remap_picture(void);

// D�finition d'une fonction g�n�rique de tra�age de figures:
Func_pixel Pixel_figure;
void Update_part_of_screen(short x, short y, short width, short height);
