#include "common/common.hpp"

void canvas(nk::context& ctx, nk_user_font& font)
{
	/* use the complete window space and set background */
	auto over1 = ctx.style_override_scoped(&nk_style::window, &nk_style_window::padding, {0, 0});
	auto over2 = ctx.style_override_scoped(&nk_style::window, &nk_style_window::spacing, {0, 0});
	auto over3 = ctx.style_override_scoped(&nk_style::window, &nk_style_window::fixed_background, nk::style_item_color({250, 250, 250}));

	/* create/update window and set position + size */
	auto win = ctx.window_scoped("Canvas", {10, 10, 500, 550},
		NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_SCALABLE|
		NK_WINDOW_CLOSABLE|NK_WINDOW_MINIMIZABLE|NK_WINDOW_TITLE);
	if (!win)
		return;

	/* allocate the complete window space for drawing */
	auto total_space = win.get_content_region();
	win.layout_row_dynamic(total_space.h, 1);
	win.widget(total_space);
	auto canvas = win.get_canvas();

	float x = canvas.get().clip.x;
	float y = canvas.get().clip.y;

	canvas.fill_rect({x + 15, y + 15, 210, 210}, 5, nk::color(247, 230, 154));
	canvas.fill_rect({x + 20, y + 20, 200, 200}, 5, nk::color(188, 174, 118));
	// draw_text was originally commented out in the example
	canvas.draw_text({x + 30, y + 30, 150, 20}, "Text to draw", 12, font, nk::color(188, 174, 118), nk::color(0, 0, 0));
	canvas.fill_rect({x + 250, y + 20, 100, 100}, 0, nk::color(0, 0, 255));
	canvas.fill_circle({x + 20, y + 250, 100, 100}, nk::color(255, 0, 0));
	canvas.fill_triangle(x + 250, y + 250, x + 350, y + 250, x + 300, y + 350, nk::color(0, 255, 0));
	canvas.fill_arc(x + 300, y + 420, 50, 0, 3.141592654f * 3.0f / 4.0f, nk::color(255, 255, 0));

	{
		float points[12];
		points[0]  = x + 200; points[1]  = y + 250;
		points[2]  = x + 250; points[3]  = y + 350;
		points[4]  = x + 225; points[5]  = y + 350;
		points[6]  = x + 200; points[7]  = y + 300;
		points[8]  = x + 175; points[9]  = y + 350;
		points[10] = x + 150; points[11] = y + 350;
		canvas.fill_polygon(points, 6, nk::color(0, 0, 0));
	}

	{
		float points[12];
		points[0]  = x + 200; points[1]  = y + 370;
		points[2]  = x + 250; points[3]  = y + 470;
		points[4]  = x + 225; points[5]  = y + 470;
		points[6]  = x + 200; points[7]  = y + 420;
		points[8]  = x + 175; points[9]  = y + 470;
		points[10] = x + 150; points[11] = y + 470;
		canvas.stroke_polygon(points, 6, 4, nk::color(0, 0, 0));
	}

	{
		float points[8];
		points[0]  = x + 250; points[1]  = y + 200;
		points[2]  = x + 275; points[3]  = y + 220;
		points[4]  = x + 325; points[5]  = y + 170;
		points[6]  = x + 350; points[7]  = y + 200;
		canvas.stroke_polyline(points, 4, 2, nk::color(255, 128, 0));
	}

	canvas.stroke_line(x + 15, y + 10, x + 200, y + 10, 2.0f, nk::color(189, 45, 75));
	canvas.stroke_rect({x + 370, y + 20, 100, 100}, 10, 3, nk::color(0, 0, 255));
	canvas.stroke_curve(x + 380, y + 200, x + 405, y + 270, x + 455, y + 120, x + 480, y + 200, 2, nk::color(0, 150, 220));
	canvas.stroke_circle({x + 20, y + 370, 100, 100}, 5, nk::color(0, 255, 120));
	canvas.stroke_triangle(x + 370, y + 250, x + 470, y + 250, x + 420, y + 350, 6, nk::color(255, 0, 143));
	canvas.stroke_arc(x + 420, y + 420, 50, 0, 3.141592654f * 3.0f / 4.0f, 5, nk::color(0, 255, 255));
}
