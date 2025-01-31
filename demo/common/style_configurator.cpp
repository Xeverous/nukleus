#include "common/common.hpp"

namespace {

static const char* symbols[NK_SYMBOL_MAX] =
{
	"NONE",
	"X",
	"UNDERSCORE",
	"CIRCLE_SOLID",
	"CIRCLE_OUTLINE",
	"RECT_SOLID",
	"RECT_OUTLINE",
	"TRIANGLE_UP",
	"TRIANGLE_DOWN",
	"TRIANGLE_LEFT",
	"TRIANGLE_RIGHT",
	"PLUS",
	"MINUS",
	"TRIANGLE_UP_OUTLINE",
	"TRIANGLE_DOWN_OUTLINE",
	"TRIANGLE_LEFT_OUTLINE",
	"TRIANGLE_RIGHT_OUTLINE"
};

bool style_rgb(nk::window& win, const char* name, nk_color& color)
{
	win.label(name, NK_TEXT_LEFT);
	if (auto combo = win.combo_color_scoped(color, {win.widget_width(), 400})) {
		win.layout_row_dynamic(120, 1);
		nk::colorf colorf = win.color_picker(nk::colorf(color), NK_RGB);
		win.layout_row_dynamic(25, 1);
		colorf.r = win.property("#R:", 0.0f, colorf.r, 1.0f, 0.01f, 0.005f);
		colorf.g = win.property("#G:", 0.0f, colorf.g, 1.0f, 0.01f, 0.005f);
		colorf.b = win.property("#B:", 0.0f, colorf.b, 1.0f, 0.01f, 0.005f);

		color = nk::color(colorf);
		return true;
	}

	return false;
}

/* TODO style_style_item?  how to handle images if at all? */
void style_item_color(nk::window& win, const char* name, nk_style_item& style_item)
{
	style_rgb(win, name, style_item.data.color);
}

void style_vec2(nk::window& win, const char* name, struct nk_vec2& vec)
{
	char buffer[64];
	win.label(name, NK_TEXT_LEFT);
	sprintf(buffer, "%.2f, %.2f", vec.x, vec.y);
	if (auto combo = win.combo_label_scoped(buffer, {200, 200})) {
		win.layout_row_dynamic(25, 1);
		win.property_in_place("#X:", -100.0f, vec.x, 100.0f, 1, 0.5f);
		win.property_in_place("#Y:", -100.0f, vec.y, 100.0f, 1, 0.5f);
	}
}

/* style_general? pass array in instead of static? */
void style_global_colors(nk::context& ctx, nk::window& win, nk::color_table& color_table)
{
	const char* color_labels[NK_COLOR_COUNT] =
	{
		"COLOR_TEXT:",
		"COLOR_WINDOW:",
		"COLOR_HEADER:",
		"COLOR_BORDER:",
		"COLOR_BUTTON:",
		"COLOR_BUTTON_HOVER:",
		"COLOR_BUTTON_ACTIVE:",
		"COLOR_TOGGLE:",
		"COLOR_TOGGLE_HOVER:",
		"COLOR_TOGGLE_CURSOR:",
		"COLOR_SELECT:",
		"COLOR_SELECT_ACTIVE:",
		"COLOR_SLIDER:",
		"COLOR_SLIDER_CURSOR:",
		"COLOR_SLIDER_CURSOR_HOVER:",
		"COLOR_SLIDER_CURSOR_ACTIVE:",
		"COLOR_PROPERTY:",
		"COLOR_EDIT:",
		"COLOR_EDIT_CURSOR:",
		"COLOR_COMBO:",
		"COLOR_CHART:",
		"COLOR_CHART_COLOR:",
		"COLOR_CHART_COLOR_HIGHLIGHT:",
		"COLOR_SCROLLBAR:",
		"COLOR_SCROLLBAR_CURSOR:",
		"COLOR_SCROLLBAR_CURSOR_HOVER:",
		"COLOR_SCROLLBAR_CURSOR_ACTIVE:",
		"COLOR_TAB_HEADER:",
		"COLOR_KNOB:",
		"COLOR_KNOB_CURSOR:",
		"COLOR_KNOB_CURSOR_HOVER:",
		"COLOR_KNOB_CURSOR_ACTIVE:"
	};

	int clicked = 0;

	win.layout_row_dynamic(30, 2);
	for (int i = 0; i < NK_COLOR_COUNT; ++i) {
		clicked |= style_rgb(win, color_labels[i], color_table.get()[i]);
	}

	if (clicked) {
		ctx.style_from_table(color_table);
	}
}

void style_text(nk::window& win, nk_style_text& style_text)
{
	win.layout_row_dynamic(30, 2);
	style_rgb(win, "Color:", style_text.color);
	style_vec2(win, "Padding:", style_text.padding);
}

void style_button(nk::window& win, nk_style_button& style_button)
{
	win.layout_row_dynamic(30, 2);
	style_item_color(win, "Normal:", style_button.normal);
	style_item_color(win, "Hover:", style_button.hover);
	style_item_color(win, "Active:", style_button.active);

	style_rgb(win, "Border:", style_button.border_color);
	style_rgb(win, "Text Background:", style_button.text_background);
	style_rgb(win, "Text Normal:", style_button.text_normal);
	style_rgb(win, "Text Hover:", style_button.text_hover);
	style_rgb(win, "Text Active:", style_button.text_active);

	style_vec2(win, "Padding:", style_button.padding);
	style_vec2(win, "Image Padding:", style_button.image_padding);
	style_vec2(win, "Touch Padding:", style_button.touch_padding);

	const char* alignments[] =
	{
		"LEFT",
		"CENTERED",
		"RIGHT",
		"TOP LEFT",
		"TOP CENTERED",
		"TOP RIGHT",
		"BOTTOM LEFT",
		"BOTTOM CENTERED",
		"BOTTOM RIGHT"
	};

#define TOP_LEFT       NK_TEXT_ALIGN_TOP|NK_TEXT_ALIGN_LEFT
#define TOP_CENTER     NK_TEXT_ALIGN_TOP|NK_TEXT_ALIGN_CENTERED
#define TOP_RIGHT      NK_TEXT_ALIGN_TOP|NK_TEXT_ALIGN_RIGHT
#define BOTTOM_LEFT    NK_TEXT_ALIGN_BOTTOM|NK_TEXT_ALIGN_LEFT
#define BOTTOM_CENTER  NK_TEXT_ALIGN_BOTTOM|NK_TEXT_ALIGN_CENTERED
#define BOTTOM_RIGHT   NK_TEXT_ALIGN_BOTTOM|NK_TEXT_ALIGN_RIGHT

	unsigned int aligns[] =
	{
		NK_TEXT_LEFT,
		NK_TEXT_CENTERED,
		NK_TEXT_RIGHT,
		TOP_LEFT,
		TOP_CENTER,
		TOP_RIGHT,
		BOTTOM_LEFT,
		BOTTOM_CENTER,
		BOTTOM_RIGHT
	};

	int cur_align = static_cast<int>(style_button.text_alignment) - NK_TEXT_LEFT;
	for (int i = 0; i < nk::ssize(aligns); ++i) {
		if (style_button.text_alignment == aligns[i]) {
			cur_align = i;
			break;
		}
	}

	win.label("Text Alignment:", NK_TEXT_LEFT);
	cur_align = win.combobox(alignments, nk::ssize(alignments), cur_align, 25, {200, 200});
	style_button.text_alignment = aligns[cur_align];

	win.property_in_place("#Border:", -100.0f, style_button.border, 100.0f, 1, 0.5f);
	win.property_in_place("#Rounding:", -100.0f, style_button.rounding, 100.0f, 1, 0.5f);
}

void style_toggle(nk::window& win, nk_style_toggle& style_toggle)
{
	win.layout_row_dynamic(30, 2);

	style_item_color(win, "Normal:", style_toggle.normal);
	style_item_color(win, "Hover:", style_toggle.hover);
	style_item_color(win, "Active:", style_toggle.active);
	style_item_color(win, "Cursor Normal:", style_toggle.cursor_normal);
	style_item_color(win, "Cursor Hover:", style_toggle.cursor_hover);

	style_rgb(win, "Border:", style_toggle.border_color);
	style_rgb(win, "Text Background:", style_toggle.text_background);
	style_rgb(win, "Text Normal:", style_toggle.text_normal);
	style_rgb(win, "Text Hover:", style_toggle.text_hover);
	style_rgb(win, "Text Active:", style_toggle.text_active);

	style_vec2(win, "Padding:", style_toggle.padding);
	style_vec2(win, "Touch Padding:", style_toggle.touch_padding);

	win.property_in_place("#Border:", -100.0f, style_toggle.border, 100.0f, 1, 0.5f);
	win.property_in_place("#Spacing:", -100.0f, style_toggle.spacing, 100.0f, 1, 0.5f);
}

void style_selectable(nk::window& win, nk_style_selectable& style_selectable)
{
	win.layout_row_dynamic(30, 2);

	style_item_color(win, "Normal:", style_selectable.normal);
	style_item_color(win, "Hover:", style_selectable.hover);
	style_item_color(win, "Pressed:", style_selectable.pressed);
	style_item_color(win, "Normal Active:", style_selectable.normal_active);
	style_item_color(win, "Hover Active:", style_selectable.hover_active);
	style_item_color(win, "Pressed Active:", style_selectable.pressed_active);

	style_rgb(win, "Text Normal:", style_selectable.text_normal);
	style_rgb(win, "Text Hover:", style_selectable.text_hover);
	style_rgb(win, "Text Pressed:", style_selectable.text_pressed);
	style_rgb(win, "Text Normal Active:", style_selectable.text_normal_active);
	style_rgb(win, "Text Hover Active:", style_selectable.text_hover_active);
	style_rgb(win, "Text Pressed Active:", style_selectable.text_pressed_active);

	style_vec2(win, "Padding:", style_selectable.padding);
	style_vec2(win, "Image Padding:", style_selectable.image_padding);
	style_vec2(win, "Touch Padding:", style_selectable.touch_padding);

	win.property_in_place("#Rounding:", -100.0f, style_selectable.rounding, 100.0f, 1, 0.5f);
}

void style_slider(nk::window& win, nk_style_slider& style_slider)
{
	win.layout_row_dynamic(30, 2);

	style_item_color(win, "Normal:", style_slider.normal);
	style_item_color(win, "Hover:", style_slider.hover);
	style_item_color(win, "Active:", style_slider.active);

	style_rgb(win, "Bar Normal:", style_slider.bar_normal);
	style_rgb(win, "Bar Hover:", style_slider.bar_hover);
	style_rgb(win, "Bar Active:", style_slider.bar_active);
	style_rgb(win, "Bar Filled:", style_slider.bar_filled);

	style_item_color(win, "Cursor Normal:", style_slider.cursor_normal);
	style_item_color(win, "Cursor Hover:", style_slider.cursor_hover);
	style_item_color(win, "Cursor Active:", style_slider.cursor_active);

	style_vec2(win, "Cursor Size:", style_slider.cursor_size);
	style_vec2(win, "Padding:", style_slider.padding);
	style_vec2(win, "Spacing:", style_slider.spacing);

	win.property_in_place("#Bar Height:", -100.0f, style_slider.bar_height, 100.0f, 1, 0.5f);
	win.property_in_place("#Rounding:", -100.0f, style_slider.rounding, 100.0f, 1, 0.5f);

	win.layout_row_dynamic(30, 1);
	bool show_buttons = style_slider.show_buttons != 0;
	(void) win.checkbox_label_in_place("Show Buttons", show_buttons);
	style_slider.show_buttons = show_buttons;

	if (style_slider.show_buttons) {
		win.layout_row_dynamic(30, 2);
		win.label("Inc Symbol:", NK_TEXT_LEFT);
		style_slider.inc_symbol = static_cast<nk_symbol_type>(win.combobox(symbols, NK_SYMBOL_MAX, style_slider.inc_symbol, 25, {200, 200}));
		win.label("Dec Symbol:", NK_TEXT_LEFT);
		style_slider.dec_symbol = static_cast<nk_symbol_type>(win.combobox(symbols, NK_SYMBOL_MAX, style_slider.dec_symbol, 25, {200, 200}));

		/* necessary or do tree's always take the whole width? */
		/* win.layout_row_dynamic(30, 1); */
		if (auto tree = NUKLEUS_TREE_SCOPED(win, NK_TREE_TAB, "Slider Buttons", NK_MINIMIZED)) {
			nk_style_button& inc_button = win.get_context().style.slider.inc_button;
			style_button(win, inc_button);
			win.get_context().style.slider.dec_button = inc_button;
		}

	}
}

void style_progress(nk::window& win, nk_style_progress& style_progress)
{
	win.layout_row_dynamic(30, 2);

	style_item_color(win, "Normal:", style_progress.normal);
	style_item_color(win, "Hover:", style_progress.hover);
	style_item_color(win, "Active:", style_progress.active);
	style_item_color(win, "Cursor Normal:", style_progress.cursor_normal);
	style_item_color(win, "Cursor Hover:", style_progress.cursor_hover);
	style_item_color(win, "Cursor Active:", style_progress.cursor_active);

	/* TODO rgba? */
	style_rgb(win, "Border Color:", style_progress.border_color);
	style_rgb(win, "Cursor Border Color:", style_progress.cursor_border_color);

	style_vec2(win, "Padding:", style_progress.padding);

	win.property_in_place("#Rounding:", -100.0f, style_progress.rounding, 100.0f, 1, 0.5f);
	win.property_in_place("#Border:", -100.0f, style_progress.border, 100.0f, 1, 0.5f);
	win.property_in_place("#Cursor Rounding:", -100.0f, style_progress.cursor_rounding, 100.0f, 1, 0.5f);
	win.property_in_place("#Cursor Border:", -100.0f, style_progress.cursor_border, 100.0f, 1, 0.5f);
}

void style_scrollbars(nk::window& win, nk_style_scrollbar& style_scrollbar)
{
	win.layout_row_dynamic(30, 2);

	style_item_color(win, "Normal:", style_scrollbar.normal);
	style_item_color(win, "Hover:", style_scrollbar.hover);
	style_item_color(win, "Active:", style_scrollbar.active);
	style_item_color(win, "Cursor Normal:", style_scrollbar.cursor_normal);
	style_item_color(win, "Cursor Hover:", style_scrollbar.cursor_hover);
	style_item_color(win, "Cursor Active:", style_scrollbar.cursor_active);

	/* TODO rgba? */
	style_rgb(win, "Border Color:", style_scrollbar.border_color);
	style_rgb(win, "Cursor Border Color:", style_scrollbar.cursor_border_color);

	style_vec2(win, "Padding:", style_scrollbar.padding);

	win.property_in_place("#Border:", -100.0f, style_scrollbar.border, 100.0f, 1, 0.5f);
	win.property_in_place("#Rounding:", -100.0f, style_scrollbar.rounding, 100.0f, 1, 0.5f);

	/* TODO naming inconsistency with style_scrollress? */
	win.property_in_place("#Cursor Border:", -100.0f, style_scrollbar.border_cursor, 100.0f, 1, 0.5f);
	win.property_in_place("#Cursor Rounding:", -100.0f, style_scrollbar.rounding_cursor, 100.0f, 1, 0.5f);

	/* TODO what is wrong with scrollbar buttons?  Also look into controlling the total width (and height) of scrollbars */
	win.layout_row_dynamic(30, 1);
	bool show_buttons = style_scrollbar.show_buttons != 0;
	(void) win.checkbox_label_in_place("Show Buttons", show_buttons);
	style_scrollbar.show_buttons = show_buttons;

	if (style_scrollbar.show_buttons) {
		win.layout_row_dynamic(30, 2);
		win.label("Inc Symbol:", NK_TEXT_LEFT);
		style_scrollbar.inc_symbol = static_cast<nk_symbol_type>(win.combobox(symbols, NK_SYMBOL_MAX, style_scrollbar.inc_symbol, 25, {200, 200}));
		win.label("Dec Symbol:", NK_TEXT_LEFT);
		style_scrollbar.dec_symbol = static_cast<nk_symbol_type>(win.combobox(symbols, NK_SYMBOL_MAX, style_scrollbar.dec_symbol, 25, {200, 200}));

		/* win.layout_row_dynamic(30, 1); */
		if (auto tree = NUKLEUS_TREE_SCOPED(win, NK_TREE_TAB, "Scrollbar Buttons", NK_MINIMIZED)) {
			nk_style_button& inc_button = win.get_context().style.scrollh.inc_button;
			style_button(win, inc_button);
			win.get_context().style.scrollh.dec_button = inc_button;
			win.get_context().style.scrollv.inc_button = inc_button;
			win.get_context().style.scrollv.dec_button = inc_button;
		}
	}
}

void style_edit(nk::window& win, nk_style_edit& style_edit)
{
	win.layout_row_dynamic(30, 2);

	style_item_color(win, "Normal:", style_edit.normal);
	style_item_color(win, "Hover:", style_edit.hover);
	style_item_color(win, "Active:", style_edit.active);

	style_rgb(win, "Cursor Normal:", style_edit.cursor_normal);
	style_rgb(win, "Cursor Hover:", style_edit.cursor_hover);
	style_rgb(win, "Cursor Text Normal:", style_edit.cursor_text_normal);
	style_rgb(win, "Cursor Text Hover:", style_edit.cursor_text_hover);
	style_rgb(win, "Border:", style_edit.border_color);
	style_rgb(win, "Text Normal:", style_edit.text_normal);
	style_rgb(win, "Text Hover:", style_edit.text_hover);
	style_rgb(win, "Text Active:", style_edit.text_active);
	style_rgb(win, "Selected Normal:", style_edit.selected_normal);
	style_rgb(win, "Selected Hover:", style_edit.selected_hover);
	style_rgb(win, "Selected Text Normal:", style_edit.selected_text_normal);
	style_rgb(win, "Selected Text Hover:", style_edit.selected_text_hover);

	style_vec2(win, "Scrollbar Size:", style_edit.scrollbar_size);
	style_vec2(win, "Padding:", style_edit.padding);

	win.property_in_place("#Row Padding:", -100.0f, style_edit.row_padding, 100.0f, 1, 0.5f);
	win.property_in_place("#Cursor Size:", -100.0f, style_edit.cursor_size, 100.0f, 1, 0.5f);
	win.property_in_place("#Border:", -100.0f, style_edit.border, 100.0f, 1, 0.5f);
	win.property_in_place("#Rounding:", -100.0f, style_edit.rounding, 100.0f, 1, 0.5f);
}

void style_property(nk::window& win, nk_style_property& style_property)
{
	win.layout_row_dynamic(30, 2);

	style_item_color(win, "Normal:", style_property.normal);
	style_item_color(win, "Hover:", style_property.hover);
	style_item_color(win, "Active:", style_property.active);

	style_rgb(win, "Border:", style_property.border_color);
	style_rgb(win, "Label Normal:", style_property.label_normal);
	style_rgb(win, "Label Hover:", style_property.label_hover);
	style_rgb(win, "Label Active:", style_property.label_active);

	style_vec2(win, "Padding:", style_property.padding);

	/* TODO check weird hover bug with properties, happens in overview basic section too */
	win.property_in_place("#Border:", -100.0f, style_property.border, 100.0f, 1, 0.5f);
	win.property_in_place("#Rounding:", -100.0f, style_property.rounding, 100.0f, 1, 0.5f);

	/* there is no style_property.show_buttons, they're always there */

	win.label("Left Symbol:", NK_TEXT_LEFT);
	style_property.sym_left = static_cast<nk_symbol_type>(win.combobox(symbols, NK_SYMBOL_MAX, style_property.sym_left, 25, {200, 200}));
	win.label("Right Symbol:", NK_TEXT_LEFT);
	style_property.sym_right = static_cast<nk_symbol_type>(win.combobox(symbols, NK_SYMBOL_MAX, style_property.sym_right, 25, {200, 200}));

	if (auto tree = NUKLEUS_TREE_SCOPED(win, NK_TREE_TAB, "Property Buttons", NK_MINIMIZED)) {
		style_button(win, style_property.inc_button);
		style_property.dec_button = style_property.inc_button;
	}

	if (auto tree = NUKLEUS_TREE_SCOPED(win, NK_TREE_TAB, "Property Edit", NK_MINIMIZED)) {
		style_edit(win, win.get_context().style.property.edit);
	}
}

void style_chart(nk::window& win, nk_style_chart& style_chart)
{
	win.layout_row_dynamic(30, 2);

	style_item_color(win, "Background:", style_chart.background);

	style_rgb(win, "Border:", style_chart.border_color);
	style_rgb(win, "Selected Color:", style_chart.selected_color);
	style_rgb(win, "Color:", style_chart.color);

	style_vec2(win, "Padding:", style_chart.padding);

	win.property_in_place("#Border:", -100.0f, style_chart.border, 100.0f, 1, 0.5f);
	win.property_in_place("#Rounding:", -100.0f, style_chart.rounding, 100.0f, 1, 0.5f);
}

void style_combo(nk::window& win, nk_style_combo& style_combo)
{
	win.layout_row_dynamic(30, 2);

	style_item_color(win, "Normal:", style_combo.normal);
	style_item_color(win, "Hover:", style_combo.hover);
	style_item_color(win, "Active:", style_combo.active);

	style_rgb(win, "Border:", style_combo.border_color);
	style_rgb(win, "Label Normal:", style_combo.label_normal);
	style_rgb(win, "Label Hover:", style_combo.label_hover);
	style_rgb(win, "Label Active:", style_combo.label_active);

	win.label("Normal Symbol:", NK_TEXT_LEFT);
	style_combo.sym_normal = static_cast<nk_symbol_type>(win.combobox(symbols, NK_SYMBOL_MAX, style_combo.sym_normal, 25, {200, 200}));
	win.label("Hover Symbol:", NK_TEXT_LEFT);
	style_combo.sym_hover = static_cast<nk_symbol_type>(win.combobox(symbols, NK_SYMBOL_MAX, style_combo.sym_hover, 25, {200, 200}));
	win.label("Active Symbol:", NK_TEXT_LEFT);
	style_combo.sym_active = static_cast<nk_symbol_type>(win.combobox(symbols, NK_SYMBOL_MAX, style_combo.sym_active, 25, {200, 200}));

	style_vec2(win, "Content Padding:", style_combo.content_padding);
	style_vec2(win, "Button Padding:", style_combo.button_padding);
	style_vec2(win, "Spacing:", style_combo.spacing);

	win.property_in_place("#Border:", -100.0f, style_combo.border, 100.0f, 1, 0.5f);
	win.property_in_place("#Rounding:", -100.0f, style_combo.rounding, 100.0f, 1, 0.5f);
}

void style_tab(nk::window& win, nk_style_tab& style_tab)
{
	win.layout_row_dynamic(30, 2);

	style_item_color(win, "Background:", style_tab.background);

	style_rgb(win, "Border:", style_tab.border_color);
	style_rgb(win, "Text:", style_tab.text);

	/*
	 * FTR, I feel these fields are misnamed and should be sym_minimized and sym_maximized since they are
	 * what show in that state, not the button to push to get to that state
	 */
	win.label("Minimized Symbol:", NK_TEXT_LEFT);
	style_tab.sym_minimize = static_cast<nk_symbol_type>(win.combobox(symbols, NK_SYMBOL_MAX, style_tab.sym_minimize, 25, {200, 200}));
	win.label("Maxmized Symbol:", NK_TEXT_LEFT);
	style_tab.sym_maximize = static_cast<nk_symbol_type>(win.combobox(symbols, NK_SYMBOL_MAX, style_tab.sym_maximize, 25, {200, 200}));

	style_vec2(win, "Padding:", style_tab.padding);
	style_vec2(win, "Spacing:", style_tab.spacing);

	win.property_in_place("#Indent:", -100.0f, style_tab.indent, 100.0f, 1, 0.5f);
	win.property_in_place("#Border:", -100.0f, style_tab.border, 100.0f, 1, 0.5f);
	win.property_in_place("#Rounding:", -100.0f, style_tab.rounding, 100.0f, 1, 0.5f);
}

void style_window_header(nk::window& win, nk_style_window_header& style_wh)
{
	win.layout_row_dynamic(30, 2);

	style_item_color(win, "Normal:", style_wh.normal);
	style_item_color(win, "Hover:", style_wh.hover);
	style_item_color(win, "Active:", style_wh.active);

	style_rgb(win, "Label Normal:", style_wh.label_normal);
	style_rgb(win, "Label Hover:", style_wh.label_hover);
	style_rgb(win, "Label Active:", style_wh.label_active);

	style_vec2(win, "Label Padding:", style_wh.label_padding);
	style_vec2(win, "Padding:", style_wh.padding);
	style_vec2(win, "Spacing:", style_wh.spacing);

	const char* alignments[] = { "LEFT", "RIGHT" };
	win.layout_row_dynamic(30, 2);
	win.label("Button Alignment:", NK_TEXT_LEFT);
	style_wh.align = static_cast<nk_style_header_align>(win.combobox(alignments, nk::ssize(alignments), style_wh.align, 25, {200, 200}));

	win.label("Close Symbol:", NK_TEXT_LEFT);
	win.label("Minimize Symbol:", NK_TEXT_LEFT);
	win.label("Maximize Symbol:", NK_TEXT_LEFT);
	style_wh.close_symbol = static_cast<nk_symbol_type>(win.combobox(symbols, NK_SYMBOL_MAX, style_wh.close_symbol, 25, {200, 200}));
	style_wh.minimize_symbol = static_cast<nk_symbol_type>(win.combobox(symbols, NK_SYMBOL_MAX, style_wh.minimize_symbol, 25, {200, 200}));
	style_wh.maximize_symbol = static_cast<nk_symbol_type>(win.combobox(symbols, NK_SYMBOL_MAX, style_wh.maximize_symbol, 25, {200, 200}));

	/* necessary or do tree's always take the whole width? */
	/* nk_layout_row_dynamic(win, 30, 1); */
	if (auto tree = NUKLEUS_TREE_SCOPED(win, NK_TREE_TAB, "Close and Minimize Button", NK_MINIMIZED)) {
		style_button(win, style_wh.close_button);
		style_wh.minimize_button = style_wh.close_button;
	}
}

void style_window(nk::window& win, nk_style_window& style_window)
{
	win.layout_row_dynamic(30, 2);

	style_rgb(win, "Background:", style_window.background);

	style_item_color(win, "Fixed Background:", style_window.fixed_background);

	style_rgb(win, "Border:", style_window.border_color);
	style_rgb(win, "Popup Border:", style_window.popup_border_color);
	style_rgb(win, "Combo Border:", style_window.combo_border_color);
	style_rgb(win, "Contextual Border:", style_window.contextual_border_color);
	style_rgb(win, "Menu Border:", style_window.menu_border_color);
	style_rgb(win, "Group Border:", style_window.group_border_color);
	style_rgb(win, "Tooltip Border:", style_window.tooltip_border_color);

	style_item_color(win, "Scaler:", style_window.scaler);

	style_vec2(win, "Spacing:", style_window.spacing);
	style_vec2(win, "Scrollbar Size:", style_window.scrollbar_size);
	style_vec2(win, "Min Size:", style_window.min_size);
	style_vec2(win, "Padding:", style_window.padding);
	style_vec2(win, "Group Padding:", style_window.group_padding);
	style_vec2(win, "Popup Padding:", style_window.popup_padding);
	style_vec2(win, "Combo Padding:", style_window.combo_padding);
	style_vec2(win, "Contextual Padding:", style_window.contextual_padding);
	style_vec2(win, "Menu Padding:", style_window.menu_padding);
	style_vec2(win, "Tooltip Padding:", style_window.tooltip_padding);

	win.property_in_place("#Rounding:", -100.0f, style_window.rounding, 100.0f, 1, 0.5f);
	win.property_in_place("#Combo Border:", -100.0f, style_window.combo_border, 100.0f, 1, 0.5f);
	win.property_in_place("#Contextual Border:", -100.0f, style_window.contextual_border, 100.0f, 1, 0.5f);
	win.property_in_place("#Menu Border:", -100.0f, style_window.menu_border, 100.0f, 1, 0.5f);
	win.property_in_place("#Group Border:", -100.0f, style_window.group_border, 100.0f, 1, 0.5f);
	win.property_in_place("#Tooltip Border:", -100.0f, style_window.tooltip_border, 100.0f, 1, 0.5f);
	win.property_in_place("#Popup Border:", -100.0f, style_window.popup_border, 100.0f, 1, 0.5f);
	win.property_in_place("#Border:", -100.0f, style_window.border, 100.0f, 1, 0.5f);

	win.layout_row_dynamic(30, 1);
	win.property_in_place("#Min Row Height Padding:", -100.0f, style_window.min_row_height_padding, 100.0f, 1, 0.5f);

	if (auto tree = NUKLEUS_TREE_SCOPED(win, NK_TREE_TAB, "Window Header", NK_MINIMIZED)) {
		style_window_header(win, style_window.header);
	}
}

} // namespace

bool style_configurator(nk::context& ctx, const nk::color_table& default_color_table, nk::color_table& color_table)
{
	/* window flags */
	bool border = true;
	bool resize = true;
	bool movable = true;
	bool no_scrollbar = false;
	bool scale_left = false;
	bool minimizable = true;

	/* window flags */
	nk_flags window_flags = 0;
	if (border) window_flags |= NK_WINDOW_BORDER;
	if (resize) window_flags |= NK_WINDOW_SCALABLE;
	if (movable) window_flags |= NK_WINDOW_MOVABLE;
	if (no_scrollbar) window_flags |= NK_WINDOW_NO_SCROLLBAR;
	if (scale_left) window_flags |= NK_WINDOW_SCALE_LEFT;
	if (minimizable) window_flags |= NK_WINDOW_MINIMIZABLE;

	nk_style& style = ctx.get_style();

	if (auto win = ctx.window_scoped("Configurator", {10, 10, 400, 600}, window_flags)) {
		if (auto tree = NUKLEUS_TREE_SCOPED(win, NK_TREE_TAB, "Global Colors", NK_MINIMIZED)) {
			style_global_colors(ctx, win, color_table);
		}

		if (auto tree = NUKLEUS_TREE_SCOPED(win, NK_TREE_TAB, "Text", NK_MINIMIZED)) {
			style_text(win, style.text);
		}

		if (auto tree = NUKLEUS_TREE_SCOPED(win, NK_TREE_TAB, "Button", NK_MINIMIZED)) {
			style_button(win, style.button);
		}

		if (auto tree = NUKLEUS_TREE_SCOPED(win, NK_TREE_TAB, "Contextual Button", NK_MINIMIZED)) {
			style_button(win, style.contextual_button);
		}

		if (auto tree = NUKLEUS_TREE_SCOPED(win, NK_TREE_TAB, "Menu Button", NK_MINIMIZED)) {
			style_button(win, style.menu_button);
		}

		if (auto tree = NUKLEUS_TREE_SCOPED(win, NK_TREE_TAB, "Combo Buttons", NK_MINIMIZED)) {
			style_button(win, style.combo.button);
		}

		if (auto tree = NUKLEUS_TREE_SCOPED(win, NK_TREE_TAB, "Tab Min/Max Buttons", NK_MINIMIZED)) {
			style_button(win, style.tab.tab_minimize_button);
			style.tab.tab_maximize_button = style.tab.tab_minimize_button;
		}
		if (auto tree = NUKLEUS_TREE_SCOPED(win, NK_TREE_TAB, "Node Min/Max Buttons", NK_MINIMIZED)) {
			style_button(win, style.tab.node_minimize_button);
			style.tab.node_maximize_button = style.tab.node_minimize_button;
		}

		if (auto tree = NUKLEUS_TREE_SCOPED(win, NK_TREE_TAB, "Window Header Close Buttons", NK_MINIMIZED)) {
			style_button(win, style.window.header.close_button);
		}

		if (auto tree = NUKLEUS_TREE_SCOPED(win, NK_TREE_TAB, "Window Header Minimize Buttons", NK_MINIMIZED)) {
			style_button(win, style.window.header.minimize_button);
		}

		if (auto tree = NUKLEUS_TREE_SCOPED(win, NK_TREE_TAB, "Checkbox", NK_MINIMIZED)) {
			style_toggle(win, style.checkbox);
		}

		if (auto tree = NUKLEUS_TREE_SCOPED(win, NK_TREE_TAB, "Option", NK_MINIMIZED)) {
			style_toggle(win, style.option);
		}

		if (auto tree = NUKLEUS_TREE_SCOPED(win, NK_TREE_TAB, "Selectable", NK_MINIMIZED)) {
			style_selectable(win, style.selectable);
		}

		if (auto tree = NUKLEUS_TREE_SCOPED(win, NK_TREE_TAB, "Slider", NK_MINIMIZED)) {
			style_slider(win, style.slider);
		}

		if (auto tree = NUKLEUS_TREE_SCOPED(win, NK_TREE_TAB, "Progress", NK_MINIMIZED)) {
			style_progress(win, style.progress);
		}

		if (auto tree = NUKLEUS_TREE_SCOPED(win, NK_TREE_TAB, "Scrollbars", NK_MINIMIZED)) {
			style_scrollbars(win, style.scrollh);
			style.scrollv = style.scrollh;
		}

		if (auto tree = NUKLEUS_TREE_SCOPED(win, NK_TREE_TAB, "Edit", NK_MINIMIZED)) {
			style_edit(win, style.edit);
		}

		if (auto tree = NUKLEUS_TREE_SCOPED(win, NK_TREE_TAB, "Property", NK_MINIMIZED)) {
			style_property(win, style.property);
		}

		if (auto tree = NUKLEUS_TREE_SCOPED(win, NK_TREE_TAB, "Chart", NK_MINIMIZED)) {
			style_chart(win, style.chart);
		}

		if (auto tree = NUKLEUS_TREE_SCOPED(win, NK_TREE_TAB, "Combo", NK_MINIMIZED)) {
			style_combo(win, style.combo);
		}

		if (auto tree = NUKLEUS_TREE_SCOPED(win, NK_TREE_TAB, "Tab", NK_MINIMIZED)) {
			style_tab(win, style.tab);
		}

		if (auto tree = NUKLEUS_TREE_SCOPED(win, NK_TREE_TAB, "Window", NK_MINIMIZED)) {
			style_window(win, style.window);
		}

		win.layout_row_dynamic(30, 1);
		if (win.button_label("Reset all styles to defaults")) {
			ctx.style_default();
			color_table = default_color_table;
		}
	}

	return !ctx.window_is_closed("Configurator");
}
