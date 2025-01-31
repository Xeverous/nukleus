#pragma once

#include <nukleus.hpp>

#include <iostream>

// Based on code from Nuklear commit 171090c8e21584330207544dbde5e7c1ec581135 (tag: 4.12.7).
// Most of the code is very simple translation of C to C++, however some examples may deviate
// more if there is a particular feature in C++ than be taken advantage of.
// These should be good examples how to use Nukleus in C++ and how to do particular things.

// from style.c
enum theme {
	THEME_BLACK,
	THEME_WHITE,
	THEME_RED,
	THEME_BLUE,
	THEME_DARK,
	THEME_DRACULA,
	THEME_CATPPUCCIN_LATTE,
	THEME_CATPPUCCIN_FRAPPE,
	THEME_CATPPUCCIN_MACCHIATO,
	THEME_CATPPUCCIN_MOCHA
};
void set_style(nk::context& ctx, enum theme theme);

// from style_configurator.c
bool style_configurator(nk::context& ctx, const nk::color_table& default_color_table, nk::color_table& color_table);

// from overview.c
bool overview(nk::context& ctx);

// from canvas.c
void canvas(nk::context& ctx, nk_user_font& font);

// from node_editor.c
bool node_editor(nk::context& ctx);

// from main function of demos
inline void demo_window(nk::context& ctx, nk::colorf& bg)
{
	auto w = ctx.window_scoped(
		"Demo",
		{50, 50, 230, 250},
		NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_SCALABLE | NK_WINDOW_MINIMIZABLE | NK_WINDOW_TITLE);

	if (!w)
		return;

	enum {EASY, HARD};
	static int op = EASY;
	static int property = 20;

	w.layout_row_static(30, 80, 1);
	if (w.button_label("button"))
		std::cout << "button pressed\n";

	w.layout_row_dynamic(30, 2);
	if (w.option_label("easy", op == EASY)) op = EASY;
	if (w.option_label("hard", op == HARD)) op = HARD;

	w.layout_row_dynamic(25, 1);
	w.property_in_place("Compression:", 0, property, 100, 10, 1);

	w.layout_row_dynamic(20, 1);
	w.label("background:", NK_TEXT_LEFT);

	w.layout_row_dynamic(25, 1);
	if (auto combo = w.combo_color_scoped(nk::color(bg), {w.widget_width(), 400})) {
		w.layout_row_dynamic(120, 1);
		bg = w.color_picker(bg, NK_RGBA);

		w.layout_row_dynamic(25, 1);
		bg.r = w.property("#R:", 0.0f, bg.r, 1.0f, 0.01f, 0.005f);
		bg.g = w.property("#G:", 0.0f, bg.g, 1.0f, 0.01f, 0.005f);
		bg.b = w.property("#B:", 0.0f, bg.b, 1.0f, 0.01f, 0.005f);
		bg.a = w.property("#A:", 0.0f, bg.a, 1.0f, 0.01f, 0.005f);
	}
}

// from calculator.c
inline void calculator(nk::context& ctx)
{
	if (auto win = ctx.window_scoped("Calculator", {10, 10, 180, 250},
		NK_WINDOW_BORDER|NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_MOVABLE))
	{
		static int set = 0, prev = 0, op = 0;
		static const char numbers[] = "789456123";
		static const char ops[] = "+-*/";
		static double a = 0, b = 0;
		static double *current = &a;

		int solve = 0;
		{
			int len;
			char buffer[256];
			win.layout_row_dynamic(35, 1);
			len = snprintf(buffer, 256, "%.2f", *current);
			(void) win.edit_string(NK_EDIT_SIMPLE, buffer, len, 255, nk_filter_float);
			buffer[len] = 0;
			*current = atof(buffer);
		}

		win.layout_row_dynamic(35, 4);
		for (int i = 0; i < 16; ++i) {
			if (i >= 12 && i < 15) {
				if (i > 12)
					continue;
				if (win.button_label("C")) {
					a = b = op = 0; current = &a; set = 0;
				}
				if (win.button_label("0")) {
					*current = *current * 10.0f; set = 0;
				}
				if (win.button_label("=")) {
					solve = 1; prev = op; op = 0;
				}
			}
			else if (((i+1) % 4)) {
				if (win.button_text(&numbers[(i / 4) * 3 + i % 4], 1)) {
					*current = *current * 10.0f + numbers[(i / 4) * 3 + i % 4] - '0';
					set = 0;
				}
			}
			else if (win.button_text(&ops[i/4], 1)) {
				if (!set) {
					if (current != &b) {
						current = &b;
					}
					else {
						prev = op;
						solve = 1;
					}
				}
				op = ops[i/4];
				set = 1;
			}
		}
		if (solve) {
			if (prev == '+') a = a + b;
			if (prev == '-') a = a - b;
			if (prev == '*') a = a * b;
			if (prev == '/') a = a / b;
			current = &a;
			if (set) current = &b;
			b = 0; set = 0;
		}
	}
}
