#include "common/common.hpp"

#include <limits.h> /* INT_MAX */
#include <time.h> /* struct tm, localtime */
#include <string.h> // memcpy
#include <math.h>

namespace {

void draw_menu(nk::window& win, bool& show_menu, bool& show_app_about)
{
	auto menubar = win.menubar_scoped();
	auto row = win.layout_row_static_scoped(25, 5);

	/* menu #1 */
	row.push(45);
	if (auto menu = win.menu_label_scoped("MENU", NK_TEXT_LEFT, {120, 200}))
	{
		static size_t prog = 40;
		static int slider = 10;
		static bool check = true;
		auto drow = win.layout_row_dynamic(25, 1);
		if (menu.item_label("Hide", NK_TEXT_LEFT))
			show_menu = false;
		if (menu.item_label("About", NK_TEXT_LEFT))
			show_app_about = true;
		(void) win.progress_bar_in_place(prog, 100, NK_MODIFIABLE);
		(void) win.slider_in_place(0, slider, 16, 1);
		(void) win.checkbox_label_in_place("check", check);
	}

	/* menu #2 */
	row.push(60);
	if (auto menu = win.menu_label_scoped("ADVANCED", NK_TEXT_LEFT, {200, 600}))
	{
		enum menu_state {MENU_NONE, MENU_FILE, MENU_EDIT, MENU_VIEW, MENU_CHART};
		static menu_state menu_state = MENU_NONE;

		nk_collapse_states state = (menu_state == MENU_FILE) ? NK_MAXIMIZED : NK_MINIMIZED;
		if (auto tree = win.tree_state_scoped(NK_TREE_TAB, "FILE", state)) {
			menu_state = MENU_FILE;
			(void) menu.item_label("New", NK_TEXT_LEFT);
			(void) menu.item_label("Open", NK_TEXT_LEFT);
			(void) menu.item_label("Save", NK_TEXT_LEFT);
			(void) menu.item_label("Close", NK_TEXT_LEFT);
			(void) menu.item_label("Exit", NK_TEXT_LEFT);
		} else menu_state = (menu_state == MENU_FILE) ? MENU_NONE : menu_state;

		state = (menu_state == MENU_EDIT) ? NK_MAXIMIZED : NK_MINIMIZED;
		if (auto tree = win.tree_state_scoped(NK_TREE_TAB, "EDIT", state)) {
			menu_state = MENU_EDIT;
			(void) menu.item_label("Copy", NK_TEXT_LEFT);
			(void) menu.item_label("Delete", NK_TEXT_LEFT);
			(void) menu.item_label("Cut", NK_TEXT_LEFT);
			(void) menu.item_label("Paste", NK_TEXT_LEFT);
		} else menu_state = (menu_state == MENU_EDIT) ? MENU_NONE : menu_state;

		state = (menu_state == MENU_VIEW) ? NK_MAXIMIZED : NK_MINIMIZED;
		if (auto tree = win.tree_state_scoped(NK_TREE_TAB, "VIEW", state)) {
			menu_state = MENU_VIEW;
			(void) menu.item_label("About", NK_TEXT_LEFT);
			(void) menu.item_label("Options", NK_TEXT_LEFT);
			(void) menu.item_label("Customize", NK_TEXT_LEFT);
		} else menu_state = (menu_state == MENU_VIEW) ? MENU_NONE : menu_state;

		state = (menu_state == MENU_CHART) ? NK_MAXIMIZED : NK_MINIMIZED;
		if (auto tree = win.tree_state_scoped(NK_TREE_TAB, "CHART", state)) {
			const float values[] = {26.0f,13.0f,30.0f,15.0f,25.0f,10.0f,20.0f,40.0f,12.0f,8.0f,22.0f,28.0f};
			menu_state = MENU_CHART;
			win.layout_row_dynamic(150, 1);

			auto chart = win.chart_scoped(NK_CHART_COLUMN, nk::ssize(values), 0, 50);
			for (int i = 0; i < nk::ssize(values); ++i)
				(void) chart.push(values[i]);
		} else menu_state = (menu_state == MENU_CHART) ? MENU_NONE : menu_state;
	}

	/* menu widgets */
	row.push(70);
	static nk_size mprog = 60;
	static int mslider = 10;
	static bool mcheck = true;
	(void) win.progress_bar_in_place(mprog, 100, NK_MODIFIABLE);
	(void) win.slider_in_place(0, mslider, 16, 1);
	(void) win.checkbox_label_in_place("check", mcheck);
}

void overview_widgets(nk::window& win)
{
	auto parent_tree = NUKLEUS_TREE_SCOPED(win, NK_TREE_TAB, "Widgets", NK_MINIMIZED);
	if (!parent_tree)
		return;

	enum options {A,B,C};
	static bool checkbox_left_text_left;
	static bool checkbox_centered_text_right;
	static bool checkbox_right_text_right;
	static bool checkbox_right_text_left;
	static int option_left;
	static int option_right;

	if (auto tree = NUKLEUS_TREE_SCOPED(win, NK_TREE_NODE, "Text", NK_MINIMIZED))
	{
		/* Text Widgets */
		win.layout_row_dynamic(20, 1);
		win.label("Label aligned left", NK_TEXT_LEFT);
		win.label("Label aligned centered", NK_TEXT_CENTERED);
		win.label("Label aligned right", NK_TEXT_RIGHT);
		win.label_colored("Blue text", NK_TEXT_LEFT, nk::color(0, 0, 255));
		win.label_colored("Yellow text", NK_TEXT_LEFT, nk::color(255, 255, 0));
		win.text("Text without /0", 15, NK_TEXT_RIGHT);

		win.layout_row_static(100, 200, 1);
		win.label_wrap("This is a very long line to hopefully get this text to be wrapped into multiple lines to show line wrapping");
		win.layout_row_dynamic(100, 1);
		win.label_wrap("This is another long text to show dynamic window changes on multiline text");
	}

	if (auto tree = NUKLEUS_TREE_SCOPED(win, NK_TREE_NODE, "Button", NK_MINIMIZED))
	{
		/* Buttons Widgets */
		win.layout_row_static(30, 100, 3);
		if (win.button_label("Button"))
			fprintf(stdout, "Button pressed!\n");
		win.button_set_behavior(NK_BUTTON_REPEATER);
		if (win.button_label("Repeater"))
			fprintf(stdout, "Repeater is being pressed!\n");
		win.button_reset_behavior();
		(void) win.button_color(nk::color(0, 0, 255));

		win.layout_row_static(25, 25, 8);
		(void) win.button_symbol(NK_SYMBOL_CIRCLE_SOLID);
		(void) win.button_symbol(NK_SYMBOL_CIRCLE_OUTLINE);
		(void) win.button_symbol(NK_SYMBOL_RECT_SOLID);
		(void) win.button_symbol(NK_SYMBOL_RECT_OUTLINE);
		(void) win.button_symbol(NK_SYMBOL_TRIANGLE_UP);
		(void) win.button_symbol(NK_SYMBOL_TRIANGLE_UP_OUTLINE);
		(void) win.button_symbol(NK_SYMBOL_TRIANGLE_DOWN);
		(void) win.button_symbol(NK_SYMBOL_TRIANGLE_DOWN_OUTLINE);
		(void) win.button_symbol(NK_SYMBOL_TRIANGLE_LEFT);
		(void) win.button_symbol(NK_SYMBOL_TRIANGLE_LEFT_OUTLINE);
		(void) win.button_symbol(NK_SYMBOL_TRIANGLE_RIGHT);
		(void) win.button_symbol(NK_SYMBOL_TRIANGLE_RIGHT_OUTLINE);

		win.layout_row_static(30, 100, 2);
		(void) win.button_symbol_label(NK_SYMBOL_TRIANGLE_LEFT, "prev", NK_TEXT_RIGHT);
		(void) win.button_symbol_label(NK_SYMBOL_TRIANGLE_RIGHT, "next", NK_TEXT_LEFT);
	}

	if (auto tree = NUKLEUS_TREE_SCOPED(win, NK_TREE_NODE, "Basic", NK_MINIMIZED))
	{
		/* Basic widgets */
		static int int_slider = 5;
		static float float_slider = 2.5f;
		static int int_knob = 5;
		static float float_knob = 2.5f;
		static nk_size prog_value = 40;
		static float property_float = 2;
		static int property_int = 10;
		static int property_neg = 10;

		static float range_float_min = 0;
		static float range_float_max = 100;
		static float range_float_value = 50;
		static int range_int_min = 0;
		static int range_int_value = 2048;
		static int range_int_max = 4096;
		static const float ratio[] = {120, 150};
		static int range_int_value_hidden = 2048;

		win.layout_row_dynamic(0, 1);
		(void) win.checkbox_label_in_place("CheckLeft TextLeft", checkbox_left_text_left);
		(void) win.checkbox_label_align_in_place("CheckCenter TextRight", checkbox_centered_text_right, NK_WIDGET_ALIGN_CENTERED | NK_WIDGET_ALIGN_MIDDLE, NK_TEXT_RIGHT);
		(void) win.checkbox_label_align_in_place("CheckRight TextRight", checkbox_right_text_right, NK_WIDGET_LEFT, NK_TEXT_RIGHT);
		(void) win.checkbox_label_align_in_place("CheckRight TextLeft", checkbox_right_text_left, NK_WIDGET_RIGHT, NK_TEXT_LEFT);

		win.layout_row_static(30, 80, 3);
		option_left = win.option_label("optionA", option_left == A) ? A : option_left;
		option_left = win.option_label("optionB", option_left == B) ? B : option_left;
		option_left = win.option_label("optionC", option_left == C) ? C : option_left;

		win.layout_row_static(30, 80, 3);
		option_right = win.option_label_align("optionA", option_right == A, NK_WIDGET_RIGHT, NK_TEXT_RIGHT) ? A : option_right;
		option_right = win.option_label_align("optionB", option_right == B, NK_WIDGET_RIGHT, NK_TEXT_RIGHT) ? B : option_right;
		option_right = win.option_label_align("optionC", option_right == C, NK_WIDGET_RIGHT, NK_TEXT_RIGHT) ? C : option_right;

		win.layout_row_static(30, ratio);
		win.labelf(NK_TEXT_LEFT, "Slider int");
		(void) win.slider_in_place(0, int_slider, 10, 1);

		win.label("Slider float", NK_TEXT_LEFT);
		(void) win.slider_in_place(0, float_slider, 5.0, 0.5f);
		win.labelf(NK_TEXT_LEFT, "Progressbar: %u" , static_cast<int>(prog_value));
		(void) win.progress_bar_in_place(prog_value, 100, NK_MODIFIABLE);

		win.layout_row_static(40, ratio);
		win.labelf(NK_TEXT_LEFT, "Knob int: %d", int_knob);
		(void) win.knob_in_place(0, int_knob, 10, 1, NK_DOWN, 60.0f);
		win.labelf(NK_TEXT_LEFT, "Knob float: %.2f", float_knob);
		(void) win.knob_in_place(0, float_knob, 5.0, 0.5f, NK_DOWN, 60.0f);

		win.layout_row_static(25, ratio);
		win.label("Property float:", NK_TEXT_LEFT);
		win.property_in_place("Float:", 0, property_float, 64.0f, 0.1f, 0.2f);
		win.label("Property int:", NK_TEXT_LEFT);
		win.property_in_place("Int:", 0, property_int, 100, 1, 1);
		win.label("Property neg:", NK_TEXT_LEFT);
		win.property_in_place("Neg:", -10, property_neg, 10, 1, 1);

		win.layout_row_dynamic(25, 1);
		win.label("Range:", NK_TEXT_LEFT);
		win.layout_row_dynamic(25, 3);
		win.property_in_place("#min:", 0, range_float_min, range_float_max, 1.0f, 0.2f);
		win.property_in_place("#float:", range_float_min, range_float_value, range_float_max, 1.0f, 0.2f);
		win.property_in_place("#max:", range_float_min, range_float_max, 100, 1.0f, 0.2f);

		win.property_in_place("#min:", INT_MIN, range_int_min, range_int_max, 1, 10);
		win.property_in_place("#neg:", range_int_min, range_int_value, range_int_max, 1, 10);
		win.property_in_place("#max:", range_int_min, range_int_max, INT_MAX, 1, 10);

		win.layout_row_dynamic(0, 2);
		win.label("Hidden Label:", NK_TEXT_LEFT);
		win.property_in_place("##Hidden Label", range_int_min, range_int_value_hidden, INT_MAX, 1, 10);
	}

	if (auto tree = NUKLEUS_TREE_SCOPED(win, NK_TREE_NODE, "Inactive", NK_MINIMIZED))
	{
		static bool inactive = 1;
		win.layout_row_dynamic(30, 1);
		(void) win.checkbox_label_in_place("Inactive", inactive);

		win.layout_row_static(30, 80, 1);
		auto enabledness = win.widget_enabledness_scoped(!inactive);

		if (win.button_label("button"))
			fprintf(stdout, "button pressed\n");
	}

	if (auto tree = NUKLEUS_TREE_SCOPED(win, NK_TREE_NODE, "Selectable", NK_MINIMIZED))
	{
		if (auto t = NUKLEUS_TREE_SCOPED(win, NK_TREE_NODE, "List", NK_MINIMIZED))
		{
			static bool selected[4] = {false, false, true, false};
			win.layout_row_static(18, 100, 1);
			(void) win.selectable_label_in_place("Selectable", NK_TEXT_LEFT, selected[0]);
			(void) win.selectable_label_in_place("Selectable", NK_TEXT_LEFT, selected[1]);
			win.label("Not Selectable", NK_TEXT_LEFT);
			(void) win.selectable_label_in_place("Selectable", NK_TEXT_LEFT, selected[2]);
			(void) win.selectable_label_in_place("Selectable", NK_TEXT_LEFT, selected[3]);
		}

		if (auto t = NUKLEUS_TREE_SCOPED(win, NK_TREE_NODE, "Grid", NK_MINIMIZED))
		{
			static bool selected[16] = {1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1};
			win.layout_row_static(50, 50, 4);
			for (int i = 0; i < 16; ++i) {
				if (win.selectable_label_in_place("Z", NK_TEXT_CENTERED, selected[i])) {
					int x = (i % 4), y = i / 4;
					if (x > 0) selected[i - 1] ^= 1;
					if (x < 3) selected[i + 1] ^= 1;
					if (y > 0) selected[i - 4] ^= 1;
					if (y < 3) selected[i + 4] ^= 1;
				}
			}
		}
	}

	if (auto tree = NUKLEUS_TREE_SCOPED(win, NK_TREE_NODE, "Combo", NK_MINIMIZED))
	{
		/* Combobox Widgets
		 * In this library comboboxes are not limited to being a popup
		 * list of selectable text. Instead it is a abstract concept of
		 * having something that is *selected* or displayed, a popup window
		 * which opens if something needs to be modified and the content
		 * of the popup which causes the *selected* or displayed value to
		 * change or if wanted close the combobox.
		 *
		 * While strange at first handling comboboxes in a abstract way
		 * solves the problem of overloaded window content. For example
		 * changing a color value requires 4 value modifier (slider, property,...)
		 * for RGBA then you need a label and ways to display the current color.
		 * If you want to go fancy you even add rgb and hsv ratio boxes.
		 * While fine for one color if you have a lot of them it because
		 * tedious to look at and quite wasteful in space. You could add
		 * a popup which modifies the color but this does not solve the
		 * fact that it still requires a lot of cluttered space to do.
		 *
		 * In these kind of instance abstract comboboxes are quite handy. All
		 * value modifiers are hidden inside the combobox popup and only
		 * the color is shown if not open. This combines the clarity of the
		 * popup with the ease of use of just using the space for modifiers.
		 *
		 * Other instances are for example time and especially date picker,
		 * which only show the currently activated time/data and hide the
		 * selection logic inside the combobox popup.
		 */
		static float chart_selection = 8.0f;
		static int current_weapon = 0;
		static bool check_values[5];
		static float position[3];
		static nk::color combo_color(130, 50, 50, 255);
		static struct nk::colorf combo_color2(0.509f, 0.705f, 0.2f, 1.0f);
		static size_t prog_a = 20, prog_b = 40, prog_c = 10, prog_d = 90;
		static const char* weapons[] = {"Fist", "Pistol", "Shotgun", "Plasma", "BFG"};

		/* default combobox */
		win.layout_row_static(25, 200, 1);
		current_weapon = win.combobox(weapons, nk::ssize(weapons), current_weapon, 25, {200, 200});

		/* slider color combobox */
		if (auto combo = win.combo_color_scoped(combo_color, {200, 200})) {
			win.layout_row_dynamic(30, {0.15f, 0.85f});
			win.label("R:", NK_TEXT_LEFT);
			combo_color.r = static_cast<nk::byte>(win.slider(0, combo_color.r, 255, 5));
			win.label("G:", NK_TEXT_LEFT);
			combo_color.g = static_cast<nk::byte>(win.slider(0, combo_color.g, 255, 5));
			win.label("B:", NK_TEXT_LEFT);
			combo_color.b = static_cast<nk::byte>(win.slider(0, combo_color.b, 255, 5));
			win.label("A:", NK_TEXT_LEFT);
			combo_color.a = static_cast<nk::byte>(win.slider(0, combo_color.a, 255, 5));
		}
		/* complex color combobox */
		if (auto combo = win.combo_color_scoped(nk::color(combo_color2), {200, 400})) {
			enum color_mode {COL_RGB, COL_HSV};
			static int col_mode = COL_RGB;
			win.layout_row_dynamic(120, 1);
			combo_color2 = win.color_picker(combo_color2, NK_RGBA);

			win.layout_row_dynamic(25, 2);
			col_mode = win.option_label("RGB", col_mode == COL_RGB) ? COL_RGB : col_mode;
			col_mode = win.option_label("HSV", col_mode == COL_HSV) ? COL_HSV : col_mode;

			win.layout_row_dynamic(25, 1);
			if (col_mode == COL_RGB) {
				combo_color2.r = win.property("#R:", 0.0f, combo_color2.r, 1.0f, 0.01f, 0.005f);
				combo_color2.g = win.property("#G:", 0.0f, combo_color2.g, 1.0f, 0.01f, 0.005f);
				combo_color2.b = win.property("#B:", 0.0f, combo_color2.b, 1.0f, 0.01f, 0.005f);
				combo_color2.a = win.property("#A:", 0.0f, combo_color2.a, 1.0f, 0.01f, 0.005f);
			}
			else {
				float hsva[4];
				nk_colorf_hsva_fv(hsva, combo_color2); // TODO complete color API
				hsva[0] = win.property("#H:", 0.0f, hsva[0], 1.0f, 0.01f, 0.05f);
				hsva[1] = win.property("#S:", 0.0f, hsva[1], 1.0f, 0.01f, 0.05f);
				hsva[2] = win.property("#V:", 0.0f, hsva[2], 1.0f, 0.01f, 0.05f);
				hsva[3] = win.property("#A:", 0.0f, hsva[3], 1.0f, 0.01f, 0.05f);
				combo_color2 = nk_hsva_colorfv(hsva);
			}
		}

		/* progressbar combobox */
		size_t sum = prog_a + prog_b + prog_c + prog_d;
		char buffer[64];
		sprintf(buffer, "%lu", sum);
		if (auto combo = win.combo_label_scoped(buffer, {200, 200})) {
			win.layout_row_dynamic(30, 1);
			(void) win.progress_bar_in_place(prog_a, 100, NK_MODIFIABLE);
			(void) win.progress_bar_in_place(prog_b, 100, NK_MODIFIABLE);
			(void) win.progress_bar_in_place(prog_c, 100, NK_MODIFIABLE);
			(void) win.progress_bar_in_place(prog_d, 100, NK_MODIFIABLE);
		}

		/* checkbox combobox */
		sum = static_cast<size_t>(check_values[0] + check_values[1] + check_values[2] + check_values[3] + check_values[4]);
		sprintf(buffer, "%lu", sum);
		if (auto combo = win.combo_label_scoped(buffer, {200, 200})) {
			win.layout_row_dynamic(30, 1);
			(void) win.checkbox_label_in_place(weapons[0], check_values[0]);
			(void) win.checkbox_label_in_place(weapons[1], check_values[1]);
			(void) win.checkbox_label_in_place(weapons[2], check_values[2]);
			(void) win.checkbox_label_in_place(weapons[3], check_values[3]);
		}

		/* complex text combobox */
		sprintf(buffer, "%.2f, %.2f, %.2f", position[0], position[1],position[2]);
		if (auto combo = win.combo_label_scoped(buffer, {200, 200})) {
			win.layout_row_dynamic(25, 1);
			win.property_in_place("#X:", -1024.0f, position[0], 1024.0f, 1, 0.5f);
			win.property_in_place("#Y:", -1024.0f, position[1], 1024.0f, 1, 0.5f);
			win.property_in_place("#Z:", -1024.0f, position[2], 1024.0f, 1, 0.5f);
		}

		/* chart combobox */
		sprintf(buffer, "%.1f", chart_selection);
		if (auto combo = win.combo_label_scoped(buffer, {200, 250})) {
			static const float values[] = {26.0f, 13.0f, 30.0f, 15.0f, 25.0f, 10.0f, 20.0f, 40.0f, 12.0f, 8.0f, 22.0f, 28.0f, 5.0f};
			win.layout_row_dynamic(150, 1);
			if (auto chart = win.chart_scoped(NK_CHART_COLUMN, nk::ssize(values), 0, 50))
				for (size_t i = 0; i < nk::size(values); ++i) {
					nk_flags res = chart.push(values[i]);
					if (res & NK_CHART_CLICKED) {
						chart_selection = values[i];
						combo.close();
					}
				}
		}

		{
			static int time_selected = 0;
			static int date_selected = 0;
			static struct tm sel_time;
			static struct tm sel_date;
			if (!time_selected || !date_selected) {
				/* keep time and date updated if nothing is selected */
				time_t cur_time = time(nullptr);
				struct tm* n = localtime(&cur_time);
				if (!time_selected)
					memcpy(&sel_time, n, sizeof(struct tm));
				if (!date_selected)
					memcpy(&sel_date, n, sizeof(struct tm));
			}

			/* time combobox */
			sprintf(buffer, "%02d:%02d:%02d", sel_time.tm_hour, sel_time.tm_min, sel_time.tm_sec);
			if (auto combo = win.combo_label_scoped(buffer, {200, 250})) {
				time_selected = 1;
				win.layout_row_dynamic(25, 1);
				sel_time.tm_sec  = win.property("#S:", 0, sel_time.tm_sec,  60, 1, 1);
				sel_time.tm_min  = win.property("#M:", 0, sel_time.tm_min,  60, 1, 1);
				sel_time.tm_hour = win.property("#H:", 0, sel_time.tm_hour, 23, 1, 1);
			}

			/* date combobox */
			sprintf(buffer, "%02d-%02d-%02d", sel_date.tm_mday, sel_date.tm_mon+1, sel_date.tm_year+1900);
			if (auto combo = win.combo_label_scoped(buffer, {350, 400}))
			{
				const char* month[] = {
					"January", "February", "March", "April", "May", "June",
					"July", "August", "September", "October", "November", "December"
				};
				const char* week_days[] = {"SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"};
				const int month_days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
				int year = sel_date.tm_year + 1900;
				int leap_year = (!(year % 4) && (year % 100)) || !(year % 400);
				int days = (sel_date.tm_mon == 1) ?
					month_days[sel_date.tm_mon] + leap_year :
					month_days[sel_date.tm_mon];

				/* header with month and year */
				date_selected = 1;
				{
					auto row = win.layout_row_dynamic_scoped(20, 3);
					row.push(0.05f);
					if (win.button_symbol(NK_SYMBOL_TRIANGLE_LEFT)) {
						if (sel_date.tm_mon == 0) {
							sel_date.tm_mon = 11;
							sel_date.tm_year = NK_MAX(0, sel_date.tm_year - 1);
						} else sel_date.tm_mon--;
					}
					row.push(0.9f);
					sprintf(buffer, "%s %d", month[sel_date.tm_mon], year);
					win.label(buffer, NK_TEXT_CENTERED);
					row.push(0.05f);
					if (win.button_symbol(NK_SYMBOL_TRIANGLE_RIGHT)) {
						if (sel_date.tm_mon == 11) {
							sel_date.tm_mon = 0;
							sel_date.tm_year++;
						} else sel_date.tm_mon++;
					}
				}

				/* good old week day formula (double because precision) */
				{
					int year_n = (sel_date.tm_mon < 2) ? year - 1: year;
					int y = year_n % 100;
					int c = year_n / 100;
					int y4 = y / 4;
					int c4 = c / 4;
					int m = static_cast<int>(2.6 * static_cast<double>(((sel_date.tm_mon + 10) % 12) + 1) - 0.2);
					int week_day = (((1 + m + y + y4 + c4 - 2 * c) % 7) + 7) % 7;

					/* weekdays */
					win.layout_row_dynamic(35, 7);
					for (int i = 0; i < nk::ssize(week_days); ++i)
						win.label(week_days[i], NK_TEXT_CENTERED);

					/* days */
					if (week_day > 0) win.spacing(week_day);
					for (int i = 1; i <= days; ++i) {
						sprintf(buffer, "%d", i);
						if (win.button_label(buffer)) {
							sel_date.tm_mday = i;
							combo.close();
						}
					}
				}
			}
		}
	}

	if (auto tree = NUKLEUS_TREE_SCOPED(win, NK_TREE_NODE, "Input", NK_MINIMIZED))
	{
		static const float ratio[] = {120, 150};
		static char field_buffer[64];
		static char text[9][64];
		static int text_len[9];
		static char box_buffer[512];
		static int field_len;
		static int box_len;

		win.layout_row_static(25, ratio);
		win.label("Default:", NK_TEXT_LEFT);

		(void) win.edit_string(NK_EDIT_SIMPLE, text[0], text_len[0], 64, nk_filter_default);
		win.label("Int:", NK_TEXT_LEFT);
		(void) win.edit_string(NK_EDIT_SIMPLE, text[1], text_len[1], 64, nk_filter_decimal);
		win.label("Float:", NK_TEXT_LEFT);
		(void) win.edit_string(NK_EDIT_SIMPLE, text[2], text_len[2], 64, nk_filter_float);
		win.label("Hex:", NK_TEXT_LEFT);
		(void) win.edit_string(NK_EDIT_SIMPLE, text[4], text_len[4], 64, nk_filter_hex);
		win.label("Octal:", NK_TEXT_LEFT);
		(void) win.edit_string(NK_EDIT_SIMPLE, text[5], text_len[5], 64, nk_filter_oct);
		win.label("Binary:", NK_TEXT_LEFT);
		(void) win.edit_string(NK_EDIT_SIMPLE, text[6], text_len[6], 64, nk_filter_binary);

		win.label("Password:", NK_TEXT_LEFT);
		{
			int old_len = text_len[8];
			char buffer[64];
			for (int i = 0; i < text_len[8]; ++i) buffer[i] = '*';
			(void) win.edit_string(NK_EDIT_FIELD, buffer, text_len[8], 64, nk_filter_default);
			if (old_len < text_len[8])
				memcpy(&text[8][old_len], &buffer[old_len], static_cast<nk_size>(text_len[8] - old_len));
		}

		win.label("Field:", NK_TEXT_LEFT);
		(void) win.edit_string(NK_EDIT_FIELD, field_buffer, field_len, 64, nk_filter_default);

		win.label("Box:", NK_TEXT_LEFT);
		win.layout_row_static(180, 278, 1);
		(void) win.edit_string(NK_EDIT_BOX, box_buffer, box_len, 512, nk_filter_default);

		win.layout_row_static(25, ratio);
		nk_flags active = win.edit_string(NK_EDIT_FIELD|NK_EDIT_SIG_ENTER, text[7], text_len[7], 64, nk_filter_ascii);
		if (win.button_label("Submit") || (active & NK_EDIT_COMMITED))
		{
			text[7][text_len[7]] = '\n';
			text_len[7]++;
			memcpy(&box_buffer[box_len], &text[7], static_cast<nk_size>(text_len[7]));
			box_len += text_len[7];
			text_len[7] = 0;
		}
	}

	if (auto tree = NUKLEUS_TREE_SCOPED(win, NK_TREE_NODE, "Horizontal Rule", NK_MINIMIZED))
	{
		win.layout_row_dynamic(12, 1);
		win.label("Use this to subdivide spaces visually", NK_TEXT_LEFT);
		win.layout_row_dynamic(4, 1);
		win.rule_horizontal(nk::color(255, 255, 255), true);
		win.layout_row_dynamic(75, 1);
		win.label_wrap("Best used in 'Card'-like layouts, with a bigger title font on top. Takes on the size of the previous layout definition. Rounding optional.");
	}
}

void overview_chart(nk::window& win)
{
	auto tree = NUKLEUS_TREE_SCOPED(win, NK_TREE_TAB, "Chart", NK_MINIMIZED);
	if (!tree)
		return;

	/* Chart Widgets
	 * This library has two different rather simple charts. The line and the
	 * column chart. Both provide a simple way of visualizing values and
	 * have a retained mode and immediate mode API version. For the retain
	 * mode version `nk_plot` and `nk_plot_function` you either provide
	 * an array or a callback to call to handle drawing the graph.
	 * For the immediate mode version you start by calling `nk_chart_begin`
	 * and need to provide min and max values for scaling on the Y-axis.
	 * and then call `nk_chart_push` to push values into the chart.
	 * Finally `nk_chart_end` needs to be called to end the process.
	 */
	float id = 0;
	static int col_index = -1;
	static int line_index = -1;
	static bool show_markers = true;
	float step = (2 * 3.141592654f) / 32;

	/* line chart */
	int index = -1;
	win.layout_row_dynamic(15, 1);
	(void) win.checkbox_label_in_place("Show markers", show_markers);
	win.layout_row_dynamic(100, 1);
	win.get_context().style.chart.show_markers = show_markers;

	if (auto chart = win.chart_scoped(NK_CHART_LINES, 32, -1.0f, 1.0f)) {
		for (int i = 0; i < 32; ++i) {
			nk_flags res = chart.push(cosf(id));
			if (res & NK_CHART_HOVERING)
				index = i;
			if (res & NK_CHART_CLICKED)
				line_index = i;
			id += step;
		}
	}

	if (index != -1)
		win.tooltipf("Value: %.2f", cosf(index * step));

	if (line_index != -1) {
		win.layout_row_dynamic(20, 1);
		win.labelf(NK_TEXT_LEFT, "Selected value: %.2f", cosf(index * step));
	}

	/* column chart */
	win.layout_row_dynamic(100, 1);
	if (auto chart = win.chart_scoped(NK_CHART_COLUMN, 32, 0.0f, 1.0f)) {
		for (int i = 0; i < 32; ++i) {
			nk_flags res = chart.push(fabs(sinf(id)));
			if (res & NK_CHART_HOVERING)
				index = i;
			if (res & NK_CHART_CLICKED)
				col_index = i;
			id += step;
		}
	}

	if (index != -1)
		win.tooltipf("Value: %.2f", fabs(sinf(step * index)));
	if (col_index != -1) {
		win.layout_row_dynamic(20, 1);
		win.labelf(NK_TEXT_LEFT, "Selected value: %.2f", fabs(sinf(step * col_index)));
	}

	/* mixed chart */
	win.layout_row_dynamic(100, 1);
	if (auto chart = win.chart_scoped(NK_CHART_COLUMN, 32, 0.0f, 1.0f)) {
		chart.add_slot(NK_CHART_LINES, 32, -1.0f, 1.0f);
		chart.add_slot(NK_CHART_LINES, 32, -1.0f, 1.0f);
		id = 0;
		for (int i = 0; i < 32; ++i) {
			(void) chart.push_slot(fabs(sinf(id)), 0);
			(void) chart.push_slot(cosf(id), 1);
			(void) chart.push_slot(sinf(id), 2);
			id += step;
		}
	}

	/* mixed colored chart */
	win.layout_row_dynamic(100, 1);
	if (auto chart = win.chart_colored_scoped(NK_CHART_LINES, nk::color(255, 0, 0), nk::color(150, 0, 0), 32, 0.0f, 1.0f)) {
		chart.add_slot_colored(NK_CHART_LINES, nk::color(0, 0, 255), nk::color(0, 0, 150), 32, -1.0f, 1.0f);
		chart.add_slot_colored(NK_CHART_LINES, nk::color(0, 255, 0), nk::color(0, 150, 0), 32, -1.0f, 1.0f);
		id = 0;
		for (int i = 0; i < 32; ++i) {
			(void) chart.push_slot(fabs(sinf(id)), 0);
			(void) chart.push_slot(cosf(id), 1);
			(void) chart.push_slot(sinf(id), 2);
			id += step;
		}
	}
}

void overview_popup(nk::window& win, bool& show_menu, bool& show_app_about)
{
	auto tree = NUKLEUS_TREE_SCOPED(win, NK_TREE_TAB, "Popup", NK_MINIMIZED);
	if (!tree)
		return;

	static nk::color color(255, 0, 0, 255);
	static bool select[4];
	static bool popup_active;

	/* menu contextual */
	win.layout_row_static(30, 160, 1);
	auto bounds = win.widget_bounds();
	win.label("Right click me for menu", NK_TEXT_LEFT);

	if (auto contextual = win.contextual_scoped(0, {100, 300}, bounds)) {
		static size_t prog = 40;
		static int slider = 10;

		win.layout_row_dynamic(25, 1);
		(void) win.checkbox_label_in_place("Menu", show_menu);
		(void) win.progress_bar_in_place(prog, 100, NK_MODIFIABLE);
		(void) win.slider_in_place(0, slider, 16, 1);
		if (contextual.item_label("About", NK_TEXT_CENTERED))
			show_app_about = true;
		(void) win.selectable_label_in_place(select[0] ? "Unselect" : "Select", NK_TEXT_LEFT, select[0]);
		(void) win.selectable_label_in_place(select[1] ? "Unselect" : "Select", NK_TEXT_LEFT, select[1]);
		(void) win.selectable_label_in_place(select[2] ? "Unselect" : "Select", NK_TEXT_LEFT, select[2]);
		(void) win.selectable_label_in_place(select[3] ? "Unselect" : "Select", NK_TEXT_LEFT, select[3]);
	}

	/* color contextual */
	{
		auto layout_row = win.layout_row_static_scoped(30, 2);
		layout_row.push(120);
		win.label("Right Click here:", NK_TEXT_LEFT);
		layout_row.push(50);
		bounds = win.widget_bounds();
		(void) win.button_color(color);
	}

	if (auto contextual = win.contextual_scoped(0, {350, 60}, bounds)) {
		win.layout_row_dynamic(30, 4);
		color.r = static_cast<nk_byte>(win.property("#r", 0, color.r, 255, 1, 1));
		color.g = static_cast<nk_byte>(win.property("#g", 0, color.g, 255, 1, 1));
		color.b = static_cast<nk_byte>(win.property("#b", 0, color.b, 255, 1, 1));
		color.a = static_cast<nk_byte>(win.property("#a", 0, color.a, 255, 1, 1));
	}

	/* popup */
	{
		auto layout_row = win.layout_row_static_scoped(30, 2);
		layout_row.push(120);
		win.label("Popup:", NK_TEXT_LEFT);
		layout_row.push(50);
		if (win.button_label("Popup"))
			popup_active = 1;
	}

	if (popup_active)
	{
		if (auto popup = win.popup_scoped(NK_POPUP_STATIC, "Error", 0, {20, 100, 220, 90}))
		{
			win.layout_row_dynamic(25, 1);
			win.label("A terrible error has occurred", NK_TEXT_LEFT);
			win.layout_row_dynamic(25, 2);
			if (win.button_label("OK")) {
				popup_active = 0;
				popup.close();
			}
			if (win.button_label("Cancel")) {
				popup_active = 0;
				popup.close();
			}
		} else popup_active = false;
	}

	/* tooltip */
	win.layout_row_static(30, 150, 1);
	bounds = win.widget_bounds();
	win.label("Hover me for tooltip", NK_TEXT_LEFT);
	// this could also be done as ctx.input_is_mouse_hovering_rect(bounds)
	if (nk_input_is_mouse_hovering_rect(&win.get_context().input, bounds))
		win.tooltip("This is a tooltip");
}

void overview_layout(nk::context& ctx, nk::window& win)
{
	auto parent_tree = NUKLEUS_TREE_SCOPED(win, NK_TREE_TAB, "Layout", NK_MINIMIZED);
	if (!parent_tree)
		return;

	if (auto tree = NUKLEUS_TREE_SCOPED(win, NK_TREE_NODE, "Widget", NK_MINIMIZED))
	{
		win.layout_row_dynamic(30, 1);
		win.label("Dynamic fixed column layout with generated position and size:", NK_TEXT_LEFT);
		win.layout_row_dynamic(30, 3);
		(void) win.button_label("button");
		(void) win.button_label("button");
		(void) win.button_label("button");

		win.layout_row_dynamic(30, 1);
		win.label("static fixed column layout with generated position and size:", NK_TEXT_LEFT);
		win.layout_row_static(30, 100, 3);
		(void) win.button_label("button");
		(void) win.button_label("button");
		(void) win.button_label("button");

		win.layout_row_dynamic(30, 1);
		win.label("Dynamic array-based custom column layout with generated position and custom size:",NK_TEXT_LEFT);
		win.layout_row_dynamic(30, {0.2f, 0.6f, 0.2f});
		(void) win.button_label("button");
		(void) win.button_label("button");
		(void) win.button_label("button");

		win.layout_row_dynamic(30, 1);
		win.label("Static array-based custom column layout with generated position and custom size:",NK_TEXT_LEFT );
		win.layout_row_static(30, {100, 200, 50});
		(void) win.button_label("button");
		(void) win.button_label("button");
		(void) win.button_label("button");

		win.layout_row_dynamic(30, 1);
		win.label("Dynamic immediate mode custom column layout with generated position and custom size:",NK_TEXT_LEFT);
		{
			auto layout_row = win.layout_row_dynamic_scoped(30, 3);
			layout_row.push(0.2f);
			(void) win.button_label("button");
			layout_row.push(0.6f);
			(void) win.button_label("button");
			layout_row.push(0.2f);
			(void) win.button_label("button");
		}

		win.layout_row_dynamic(30, 1);
		win.label("Static immediate mode custom column layout with generated position and custom size:", NK_TEXT_LEFT);
		{
			auto layout_row = win.layout_row_static_scoped(30, 3);
			layout_row.push(100);
			(void) win.button_label("button");
			layout_row.push(200);
			(void) win.button_label("button");
			layout_row.push(50);
			(void) win.button_label("button");
		}

		win.layout_row_dynamic(30, 1);
		win.label("Static free space with custom position and custom size:", NK_TEXT_LEFT);
		{
			auto layout_space = win.layout_space_static_scoped(60, 4);
			layout_space.push({100, 0, 100, 30});
			(void) win.button_label("button");
			layout_space.push({0, 15, 100, 30});
			(void) win.button_label("button");
			layout_space.push({200, 15, 100, 30});
			(void) win.button_label("button");
			layout_space.push({100, 30, 100, 30});
			(void) win.button_label("button");
		}

		win.layout_row_dynamic(30, 1);
		win.label("Row template:", NK_TEXT_LEFT);
		{
			auto layout_row_template = win.layout_row_template_scoped(30);
			layout_row_template.push_dynamic();
			layout_row_template.push_variable(80);
			layout_row_template.push_static(80);
		}
		(void) win.button_label("button");
		(void) win.button_label("button");
		(void) win.button_label("button");
	}

	if (auto tree = NUKLEUS_TREE_SCOPED(win, NK_TREE_NODE, "Group", NK_MINIMIZED))
	{
		static bool group_titlebar = false;
		static bool group_border = true;
		static bool group_no_scrollbar = false;
		static int group_width = 320;
		static int group_height = 200;

		nk_flags group_flags = 0;
		if (group_border) group_flags |= NK_WINDOW_BORDER;
		if (group_no_scrollbar) group_flags |= NK_WINDOW_NO_SCROLLBAR;
		if (group_titlebar) group_flags |= NK_WINDOW_TITLE;

		win.layout_row_dynamic(30, 3);
		(void) win.checkbox_label_in_place("Titlebar", group_titlebar);
		(void) win.checkbox_label_in_place("Border", group_border);
		(void) win.checkbox_label_in_place("No Scrollbar", group_no_scrollbar);

		{
			auto layout_row = win.layout_row_static_scoped(22, 3);
			layout_row.push(50);
			win.label("size:", NK_TEXT_LEFT);
			layout_row.push(130);
			win.property_in_place("#Width:", 100, group_width, 500, 10, 1);
			layout_row.push(130);
			win.property_in_place("#Height:", 100, group_height, 500, 10, 1);
		}

		auto layout = win.layout_row_static(group_height, group_width, 2);
		if (auto group = layout.group_scoped("Group", group_flags)) {
			static bool selected[16];
			win.layout_row_static(18, 100, 1);
			for (int i = 0; i < 16; ++i)
				(void) win.selectable_label_in_place(selected[i] ? "Selected": "Unselected", NK_TEXT_CENTERED, selected[i]);
		}
	}

	if (auto tree = NUKLEUS_TREE_SCOPED(win, NK_TREE_NODE, "Tree", NK_MINIMIZED))
	{
		static bool root_selected = false;
		bool sel = root_selected;
		if (auto t1 = NUKLEUS_TREE_ELEMENT_SCOPED(win, NK_TREE_NODE, "Root", NK_MINIMIZED, sel)) {
			static bool selected[8];
			bool node_select = selected[0];
			if (sel != root_selected) {
				root_selected = sel;
				for (int i = 0; i < 8; ++i)
					selected[i] = sel;
			}
			if (auto t2 = NUKLEUS_TREE_ELEMENT_SCOPED(win, NK_TREE_NODE, "Node", NK_MINIMIZED, node_select)) {
				static bool sel_nodes[4];
				if (node_select != selected[0]) {
					selected[0] = node_select;
					for (int i = 0; i < 4; ++i)
						sel_nodes[i] = node_select;
				}
				win.layout_row_static(18, 100, 1);
				for (int j = 0; j < 4; ++j)
					(void) win.selectable_symbol_label_in_place(
						NK_SYMBOL_CIRCLE_SOLID, sel_nodes[j] ? "Selected" : "Unselected", NK_TEXT_RIGHT, sel_nodes[j]);
			}
			win.layout_row_static(18, 100, 1);
			for (int i = 1; i < 8; ++i)
				(void) win.selectable_symbol_label_in_place(
					NK_SYMBOL_CIRCLE_SOLID, selected[i] ? "Selected" : "Unselected", NK_TEXT_RIGHT, selected[i]);
		}
	}

	if (auto tree = NUKLEUS_TREE_SCOPED(win, NK_TREE_NODE, "Notebook", NK_MINIMIZED))
	{
		static int current_tab = 0;
		float step = (2 * 3.141592654f) / 32;
		enum chart_type {CHART_LINE, CHART_HISTO, CHART_MIXED};
		const char* names[] = {"Lines", "Columns", "Mixed"};

		/* Header */
		// there are 2 ways to override a style
		// - first option (the classic way, commented) is to provide T* and T
		// - second option (fancy way, uncommented) is to provide U nk_style::*, T U::* and T

		// auto style_vec2 = ctx.style_scoped_override(&ctx.get_style().window.spacing, {0, 0});
		auto style_vec2 = ctx.style_override_scoped(&nk_style::window, &nk_style_window::spacing, {0, 0});
		{
			// auto style_float = ctx.style_scoped_override(&ctx.get_style().button.rounding, 0);
			auto style_float = ctx.style_override_scoped(&nk_style::button, &nk_style_button::rounding, 0);
			// NOTE: this was nk_layout_row_begin in original example and had NO end call
			// I assumed the nk_layout_row_end call was just missing and thus used scoped layout
			auto layout_row = win.layout_row_static_scoped(20, 3);
			for (int i = 0; i < 3; ++i) {
				/* make sure button perfectly fits text */
				const nk_user_font& f = *ctx.get_style().font;
				float text_width = f.width(f.userdata, f.height, names[i], nk_strlen(names[i]));
				float widget_width = text_width + 3 * ctx.get_style().button.padding.x;
				layout_row.push(widget_width);
				if (current_tab == i) {
					/* active tab gets highlighted */
					struct nk_style_item button_color = ctx.get_style().button.normal;
					ctx.get_style().button.normal = ctx.get_style().button.active;
					current_tab = win.button_label(names[i]) ? i : current_tab;
					ctx.get_style().button.normal = button_color;
				} else current_tab = win.button_label(names[i]) ? i : current_tab;
			}
		}

		/* Body */
		auto layout = win.layout_row_dynamic(140, 1);
		if (auto group = layout.group_scoped("Notebook", NK_WINDOW_BORDER))
		{
			style_vec2.reset(); // intentionally reset an override before its destruction
			switch (current_tab) {
			default:
				break;
			case CHART_LINE:
				win.layout_row_dynamic(100, 1);
				if (auto chart = win.chart_colored_scoped(NK_CHART_LINES, nk::color(255, 0, 0), nk::color(150, 0, 0), 32, 0.0f, 1.0f)) {
					chart.add_slot_colored(NK_CHART_LINES, nk::color(0, 0, 255), nk::color(0, 0, 150), 32, -1.0f, 1.0f);
					float id = 0;
					for (int i = 0; i < 32; ++i) {
						(void) chart.push_slot(fabs(sinf(id)), 0);
						(void) chart.push_slot(cosf(id), 1);
						id += step;
					}
				}
				break;
			case CHART_HISTO:
				win.layout_row_dynamic(100, 1);
				if (auto chart = win.chart_colored_scoped(NK_CHART_COLUMN, nk::color(255, 0, 0), nk::color(150, 0, 0), 32, 0.0f, 1.0f)) {
					float id = 0;
					for (int i = 0; i < 32; ++i) {
						(void) chart.push_slot(fabs(sinf(id)), 0);
						id += step;
					}
				}
				break;
			case CHART_MIXED:
				win.layout_row_dynamic(100, 1);
				if (auto chart = win.chart_colored_scoped(NK_CHART_LINES, nk::color(255, 0, 0), nk::color(150, 0, 0), 32, 0.0f, 1.0f)) {
					chart.add_slot_colored(NK_CHART_LINES, nk::color(0, 0, 255), nk::color(0, 0, 150), 32, -1.0f, 1.0f);
					chart.add_slot_colored(NK_CHART_COLUMN, nk::color(0, 255, 0), nk::color(0, 150, 0), 32, 0.0f, 1.0f);
					float id = 0;
					for (int i = 0; i < 32; ++i) {
						(void) chart.push_slot(fabs(sinf(id)), 0);
						(void) chart.push_slot(fabs(cosf(id)), 1);
						(void) chart.push_slot(fabs(sinf(id)), 2);
						id += step;
					}
				}
				break;
			}
		}
	}

	if (auto tree = NUKLEUS_TREE_SCOPED(win, NK_TREE_NODE, "Simple", NK_MINIMIZED))
	{
		auto layout = win.layout_row_dynamic(300, 2);
		if (auto group = layout.group_scoped("Group_Without_Border", 0)) {
			win.layout_row_static(18, 150, 1);
			for (int i = 0; i < 64; ++i) {
				char buffer[64];
				sprintf(buffer, "0x%02x", i);
				win.labelf(NK_TEXT_LEFT, "%s: scrollable region", buffer);
			}
		}
		if (auto group = layout.group_scoped("Group_With_Border", NK_WINDOW_BORDER)) {
			win.layout_row_dynamic(25, 2);
			for (int i = 0; i < 64; ++i) {
				char buffer[64];
				sprintf(buffer, "%08d", ((((i%7)*10)^32))+(64+(i%2)*2));
				(void) win.button_label(buffer);
			}
		}
	}

	if (auto tree = NUKLEUS_TREE_SCOPED(win, NK_TREE_NODE, "Complex", NK_MINIMIZED))
	{
		auto layout_space = win.layout_space_static_scoped(500, 64);
		layout_space.push({0, 0, 150, 500});
		if (auto group = layout_space.group_scoped("Group_left", NK_WINDOW_BORDER)) {
			static bool selected[32];
			win.layout_row_static(18, 100, 1);
			for (int i = 0; i < 32; ++i)
				(void) win.selectable_label_in_place(selected[i] ? "Selected" : "Unselected", NK_TEXT_CENTERED, selected[i]);
		}

		layout_space.push({160, 0, 150, 240});
		if (auto group = layout_space.group_scoped("Group_top", NK_WINDOW_BORDER)) {
			win.layout_row_dynamic(25, 1);
			(void) win.button_label("#FFAA");
			(void) win.button_label("#FFBB");
			(void) win.button_label("#FFCC");
			(void) win.button_label("#FFDD");
			(void) win.button_label("#FFEE");
			(void) win.button_label("#FFFF");
		}

		layout_space.push({160, 250, 150, 250});
		if (auto group = layout_space.group_scoped("Group_buttom", NK_WINDOW_BORDER)) {
			win.layout_row_dynamic(25, 1);
			(void) win.button_label("#FFAA");
			(void) win.button_label("#FFBB");
			(void) win.button_label("#FFCC");
			(void) win.button_label("#FFDD");
			(void) win.button_label("#FFEE");
			(void) win.button_label("#FFFF");
		}

		layout_space.push({320, 0, 150, 150});
		if (auto group = layout_space.group_scoped("Group_right_top", NK_WINDOW_BORDER)) {
			static bool selected[4];
			win.layout_row_static(18, 100, 1);
			for (int i = 0; i < 4; ++i)
				(void) win.selectable_label_in_place(selected[i] ? "Selected" : "Unselected", NK_TEXT_CENTERED, selected[i]);
		}

		layout_space.push({320, 160, 150, 150});
		if (auto group = layout_space.group_scoped("Group_right_center", NK_WINDOW_BORDER)) {
			static bool selected[4];
			win.layout_row_static(18, 100, 1);
			for (int i = 0; i < 4; ++i)
				(void) win.selectable_label_in_place(selected[i] ? "Selected" : "Unselected", NK_TEXT_CENTERED, selected[i]);
		}

		layout_space.push({320, 320, 150, 150});
		if (auto group = layout_space.group_scoped("Group_right_bottom", NK_WINDOW_BORDER)) {
			static bool selected[4];
			win.layout_row_static(18, 100, 1);
			for (int i = 0; i < 4; ++i)
				(void) win.selectable_label_in_place(selected[i] ? "Selected" : "Unselected", NK_TEXT_CENTERED, selected[i]);
		}
	}

	if (auto tree = NUKLEUS_TREE_SCOPED(win, NK_TREE_NODE, "Splitter", NK_MINIMIZED))
	{
		win.layout_row_static(20, 320, 1);
		win.label("Use slider and spinner to change tile size", NK_TEXT_LEFT);
		win.label("Drag the space between tiles to change tile ratio", NK_TEXT_LEFT);

		if (auto tr = NUKLEUS_TREE_SCOPED(win, NK_TREE_NODE, "Vertical", NK_MINIMIZED))
		{
			static float a = 100, b = 100, c = 100;
			float row_layout[5];
			row_layout[0] = a;
			row_layout[1] = 8;
			row_layout[2] = b;
			row_layout[3] = 8;
			row_layout[4] = c;

			/* header */
			win.layout_row_static(30, 100, 2);
			win.label("left:", NK_TEXT_LEFT);
			(void) win.slider_in_place(10.0f, a, 200.0f, 10.0f);

			win.label("middle:", NK_TEXT_LEFT);
			(void) win.slider_in_place(10.0f, b, 200.0f, 10.0f);

			win.label("right:", NK_TEXT_LEFT);
			(void) win.slider_in_place(10.0f, c, 200.0f, 10.0f);

			/* tiles */
			auto layout_row = win.layout_row_static(200, row_layout);

			/* left space */
			if (auto group = layout_row.group_scoped("left", NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_BORDER|NK_WINDOW_NO_SCROLLBAR)) {
				win.layout_row_dynamic(25, 1);
				(void) win.button_label("#FFAA");
				(void) win.button_label("#FFBB");
				(void) win.button_label("#FFCC");
				(void) win.button_label("#FFDD");
				(void) win.button_label("#FFEE");
				(void) win.button_label("#FFFF");
			}

			/* scaler */
			auto bounds = win.widget_bounds();
			win.spacing(1);
			if ((ctx.input_is_mouse_hovering_rect(bounds) ||
				ctx.input_is_mouse_prev_hovering_rect(bounds)) &&
				ctx.input_is_mouse_down(NK_BUTTON_LEFT))
			{
				a = row_layout[0] + ctx.get_input().mouse.delta.x;
				b = row_layout[2] - ctx.get_input().mouse.delta.x;
			}

			/* middle space */
			if (auto group = layout_row.group_scoped("center", NK_WINDOW_BORDER|NK_WINDOW_NO_SCROLLBAR)) {
				win.layout_row_dynamic(25, 1);
				(void) win.button_label("#FFAA");
				(void) win.button_label("#FFBB");
				(void) win.button_label("#FFCC");
				(void) win.button_label("#FFDD");
				(void) win.button_label("#FFEE");
				(void) win.button_label("#FFFF");
			}

			/* scaler */
			bounds = win.widget_bounds();
			win.spacing(1);
			if ((ctx.input_is_mouse_hovering_rect(bounds) ||
				ctx.input_is_mouse_prev_hovering_rect(bounds)) &&
				ctx.input_is_mouse_down(NK_BUTTON_LEFT))
			{
				b = (row_layout[2] + ctx.get_input().mouse.delta.x);
				c = (row_layout[4] - ctx.get_input().mouse.delta.x);
			}

			/* right space */
			if (auto group = layout_row.group_scoped("right", NK_WINDOW_BORDER|NK_WINDOW_NO_SCROLLBAR)) {
				win.layout_row_dynamic(25, 1);
				(void) win.button_label("#FFAA");
				(void) win.button_label("#FFBB");
				(void) win.button_label("#FFCC");
				(void) win.button_label("#FFDD");
				(void) win.button_label("#FFEE");
				(void) win.button_label("#FFFF");
			}
		}

		if (auto tr = NUKLEUS_TREE_SCOPED(win, NK_TREE_NODE, "Horizontal", NK_MINIMIZED))
		{
			static float a = 100, b = 100, c = 100;

			/* header */
			win.layout_row_static(30, 100, 2);
			win.label("top:", NK_TEXT_LEFT);
			(void) win.slider_in_place(10.0f, a, 200.0f, 10.0f);

			win.label("middle:", NK_TEXT_LEFT);
			(void) win.slider_in_place(10.0f, b, 200.0f, 10.0f);

			win.label("bottom:", NK_TEXT_LEFT);
			(void) win.slider_in_place(10.0f, c, 200.0f, 10.0f);

			/* top space */
			auto layout_row = win.layout_row_dynamic(a, 1);
			if (auto group = layout_row.group_scoped("top", NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_BORDER)) {
				win.layout_row_dynamic(25, 3);
				(void) win.button_label("#FFAA");
				(void) win.button_label("#FFBB");
				(void) win.button_label("#FFCC");
				(void) win.button_label("#FFDD");
				(void) win.button_label("#FFEE");
				(void) win.button_label("#FFFF");
			}

			/* scaler */
			win.layout_row_dynamic(8, 1);
			auto bounds = win.widget_bounds();
			win.spacing(1);
			if ((ctx.input_is_mouse_hovering_rect(bounds) ||
				ctx.input_is_mouse_prev_hovering_rect(bounds)) &&
				ctx.input_is_mouse_down(NK_BUTTON_LEFT))
			{
				a = a + ctx.get_input().mouse.delta.y;
				b = b - ctx.get_input().mouse.delta.y;
			}

			/* middle space */
			win.layout_row_dynamic(b, 1);
			if (auto group = layout_row.group_scoped("middle", NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_BORDER)) {
				win.layout_row_dynamic(25, 3);
				(void) win.button_label("#FFAA");
				(void) win.button_label("#FFBB");
				(void) win.button_label("#FFCC");
				(void) win.button_label("#FFDD");
				(void) win.button_label("#FFEE");
				(void) win.button_label("#FFFF");
			}

			{
				/* scaler */
				win.layout_row_dynamic(8, 1);
				bounds = win.widget_bounds();
				if ((ctx.input_is_mouse_hovering_rect(bounds) ||
					ctx.input_is_mouse_prev_hovering_rect(bounds)) &&
					ctx.input_is_mouse_down(NK_BUTTON_LEFT))
				{
					b = b + ctx.get_input().mouse.delta.y;
					c = c - ctx.get_input().mouse.delta.y;
				}
			}

			/* bottom space */
			win.layout_row_dynamic(c, 1);
			if (auto group = layout_row.group_scoped("bottom", NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_BORDER)) {
				win.layout_row_dynamic(25, 3);
				(void) win.button_label("#FFAA");
				(void) win.button_label("#FFBB");
				(void) win.button_label("#FFCC");
				(void) win.button_label("#FFDD");
				(void) win.button_label("#FFEE");
				(void) win.button_label("#FFFF");
			}
		}
	}
}

}

bool overview(nk::context& ctx)
{
	/* window flags */
	static bool show_menu = true;
	static nk_flags window_flags = NK_WINDOW_TITLE|NK_WINDOW_BORDER|NK_WINDOW_SCALABLE|NK_WINDOW_MOVABLE|NK_WINDOW_MINIMIZABLE|NK_WINDOW_SCROLL_AUTO_HIDE;
	nk_flags actual_window_flags = 0;

	/* widget flags */
	static bool disable_widgets = false;

	/* popups */
	static enum nk_style_header_align header_align = NK_HEADER_RIGHT;
	static bool show_app_about = false;

	/* styles */
	static const char* themes[] = {
		"Black", "White", "Red", "Blue", "Dark", "Dracula",
		"Catppucin Latte", "Catppucin Frappe", "Catppucin Macchiato", "Catppucin Mocha"
	};
	static int current_theme = 0;

	/* window flags */
	ctx.get_style().window.header.align = header_align;

	actual_window_flags = window_flags;
	if (!(actual_window_flags & NK_WINDOW_TITLE))
		actual_window_flags &= static_cast<nk_flags>(~(NK_WINDOW_MINIMIZABLE|NK_WINDOW_CLOSABLE));

	if (auto win = ctx.window_scoped("Overview", {10, 10, 400, 600}, actual_window_flags)) {
		if (show_menu)
			draw_menu(win, show_menu, show_app_about);

		if (show_app_about)
		{
			/* about popup */
			if (auto popup = win.popup_scoped(NK_POPUP_STATIC, "About", NK_WINDOW_CLOSABLE, {20, 100, 300, 190}))
			{
				win.layout_row_dynamic(20, 1);
				win.label("Nuklear", NK_TEXT_LEFT);
				win.label("By Micha Mettke", NK_TEXT_LEFT);
				win.label("Nuklear is licensed under the public domain License.", NK_TEXT_LEFT);
				win.label("Nukleus is licensed under the Boost Software License.", NK_TEXT_LEFT);
			} else show_app_about = false;
		}

		/* style selector */
		win.layout_row_dynamic(0, 2);
		{
			win.label("Style:", NK_TEXT_LEFT);
			const int new_theme = win.combobox(themes, nk::ssize(themes), current_theme, 25, {200, 200});
			if (new_theme != current_theme) {
				current_theme = new_theme;
				set_style(ctx, static_cast<theme>(current_theme));
			}
		}

		/* window flags */
		if (auto tree = NUKLEUS_TREE_SCOPED(win, NK_TREE_TAB, "Window", NK_MINIMIZED)) {
			win.layout_row_dynamic(30, 2);
			(void) win.checkbox_label_in_place("Menu", show_menu);
			(void) win.checkbox_flags_label_in_place("Titlebar", window_flags, NK_WINDOW_TITLE);
			(void) win.checkbox_flags_label_in_place("Border", window_flags, NK_WINDOW_BORDER);
			(void) win.checkbox_flags_label_in_place("Resizable", window_flags, NK_WINDOW_SCALABLE);
			(void) win.checkbox_flags_label_in_place("Movable", window_flags, NK_WINDOW_MOVABLE);
			(void) win.checkbox_flags_label_in_place("No Scrollbar", window_flags, NK_WINDOW_NO_SCROLLBAR);
			(void) win.checkbox_flags_label_in_place("Minimizable", window_flags, NK_WINDOW_MINIMIZABLE);
			(void) win.checkbox_flags_label_in_place("Scale Left", window_flags, NK_WINDOW_SCALE_LEFT);
			(void) win.checkbox_label_in_place("Disable widgets", disable_widgets);
		}

		auto enabledness = win.widget_enabledness_scoped(!disable_widgets);

		overview_widgets(win);
		overview_chart(win);
		overview_popup(win, show_menu, show_app_about);
		overview_layout(ctx, win);
	}
	return !ctx.window_is_closed("Overview");
}
