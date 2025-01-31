// comment from Nuklear:
/* This is a simple node editor just to show a simple implementation and that
 * it is possible to achieve it with this library. While all nodes inside this
 * example use a simple color modifier as content you could change them
 * to have your custom content depending on the node time.
 * Biggest difference to most usual implementation is that this example does
 * not have connectors on the right position of the property that it links.
 * This is mainly done out of laziness and could be implemented as well but
 * requires calculating the position of all rows and add connectors.
 * In addition adding and removing nodes is quite limited at the
 * moment since it is based on a simple fixed array. If this is to be converted
 * into something more serious it is probably best to extend it.
 */

#include "common/common.hpp"

#include <list>
#include <vector>
#include <string>
#include <math.h>

struct node {
	int id;
	std::string name;
	nk::rect<float> bounds;
	float value;
	nk::color color;
	int input_count;
	int output_count;
};

struct node_link {
	int input_id;
	int input_slot;
	int output_id;
	int output_slot;
	nk::vec2<float> in;
	nk::vec2<float> out;
};

struct node_linking {
	int active = 0;
	node* nd = nullptr;
	int input_id = 0;
	int input_slot = 0;
};

struct node_editor {
	std::list<node> nodes;
	std::vector<node_link> links;

	nk::rect<float> bounds;
	node* selected = nullptr; // set but not used for anything
	bool show_grid = true;
	nk::vec2<float> scrolling;
	node_linking linking{};

	node_editor()
	{
		add("Source", {40, 10, 180, 220}, nk::color(255, 0, 0), 0, 1);
		add("Source", {40, 260, 180, 220}, nk::color(0, 255, 0), 0, 1);
		add("Combine", {400, 100, 180, 220}, nk::color(0,0,255), 2, 2);
		link(0, 0, 2, 0);
		link(1, 0, 2, 1);
	}

	node* find(int id)
	{
		for (auto& node : nodes)
			if (node.id == id)
				return &node;

		return nullptr;
	}

	void push(node n)
	{
		nodes.push_back(nk::move(n));
	}

	void pop()
	{
		NUKLEUS_ASSERT(!nodes.empty());
		nodes.pop_back();
	}

	void add(const char* name, nk::rect<float> bounds, nk::color col, int in_count, int out_count)
	{
		static int ids = 0;
		node nd;
		nd.id = ids++;
		nd.value = 0;
		nd.color = nk::color(255, 0, 0);
		nd.input_count = in_count;
		nd.output_count = out_count;
		nd.color = col;
		nd.bounds = bounds;
		nd.name = name;
		push(nk::move(nd));
	}

	void link(int in_id, int in_slot, int out_id, int out_slot)
	{
		node_link link;
		link.input_id = in_id;
		link.input_slot = in_slot;
		link.output_id = out_id;
		link.output_slot = out_slot;
		links.push_back(link);
	}

};
static struct node_editor editor;

bool node_editor(nk::context& ctx)
{
	if (auto win = ctx.window_scoped("NodeEdit", {0, 0, 800, 600},
		NK_WINDOW_BORDER|NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_MOVABLE|NK_WINDOW_CLOSABLE))
	{
		/* allocate complete window space */
		auto canvas = win.get_canvas();
		auto total_space = win.get_content_region();

		{
			auto layout_space = win.layout_space_static_scoped(total_space.h, editor.nodes.size());
			auto size = layout_space.bounds();

			// I could not understand this part from original demo. It's not about C89 start-of-scope variables.
			// The panel object is used in multiple API calls in a quite large scope but initialized later.
			// Initially I just initialized it earlier, in a larger scope but then I was getting a different object.
			// Seems like this demo implementation relies on reuse of some internal state that is accessible
			// only at particular moment in time. There is no documentation or any comment how it works. Very ugly.
			nk_panel* node = nullptr;

			if (editor.show_grid) {
				/* display grid */
				const float grid_size = 32.0f;
				const nk::color grid_color(50, 50, 50);
				for (float x = fmod(size.x - editor.scrolling.x, grid_size); x < size.w; x += grid_size)
					canvas.stroke_line(x+size.x, size.y, x+size.x, size.y+size.h, 1.0f, grid_color);
				for (float y = fmod(size.y - editor.scrolling.y, grid_size); y < size.h; y += grid_size)
					canvas.stroke_line(size.x, y+size.y, size.x+size.w, y+size.y, 1.0f, grid_color);
			}

			/* execute each node as a movable group */
			auto updated = editor.nodes.end();
			for (auto it = editor.nodes.begin(); it != editor.nodes.end(); ++it) {
				/* calculate scrolled node window position and size */
				layout_space.push({it->bounds.x - editor.scrolling.x, it->bounds.y - editor.scrolling.y, it->bounds.w, it->bounds.h});

				/* execute node window */
				if (auto group = layout_space.group_scoped(it->name.c_str(), NK_WINDOW_MOVABLE|NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_BORDER|NK_WINDOW_TITLE))
				{
					// I replicated the same point where the panel object is obtained.
					// If this call is moved elsewhere, a different object is obtained.
					node = &win.get_panel();

					/* always have last selected node on top */
					if (ctx.input_mouse_clicked(NK_BUTTON_LEFT, node->bounds) &&
						(!(it != editor.nodes.begin() && ctx.input_mouse_clicked(NK_BUTTON_LEFT,
						layout_space.rect_to_screen(node->bounds)))) &&
						editor.nodes.end() != std::next(it))
					{
						updated = it;
					}

					/* ================= NODE CONTENT =====================*/
					win.layout_row_dynamic(25, 1);
					(void) win.button_color(it->color);
					it->color.r = static_cast<nk_byte>(win.property("#R:", 0, it->color.r, 255, 1, 1));
					it->color.g = static_cast<nk_byte>(win.property("#G:", 0, it->color.g, 255, 1, 1));
					it->color.b = static_cast<nk_byte>(win.property("#B:", 0, it->color.b, 255, 1, 1));
					it->color.a = static_cast<nk_byte>(win.property("#A:", 0, it->color.a, 255, 1, 1));
					/* ====================================================*/
				}
				{
					/* node connector and linking */
					auto bounds = layout_space.rect_to_local(node->bounds);
					bounds.x += editor.scrolling.x;
					bounds.y += editor.scrolling.y;
					it->bounds = bounds;

					/* output connector */
					float space = node->bounds.h / static_cast<float>(it->output_count + 1);
					for (int n = 0; n < it->output_count; ++n) {
						nk::rect<float> circle(
							node->bounds.x + node->bounds.w - 4,
							node->bounds.y + space * static_cast<float>(n + 1),
							8,
							8
						);
						canvas.fill_circle(circle, {100, 100, 100});

						/* start linking process */
						if (ctx.input_has_mouse_click_down_in_rect(NK_BUTTON_LEFT, circle, nk_true)) {
							editor.linking.active = nk_true;
							editor.linking.nd = &*it;
							editor.linking.input_id = it->id;
							editor.linking.input_slot = n;
						}

						/* draw curve from linked node slot to mouse position */
						if (editor.linking.active && editor.linking.nd == &*it && editor.linking.input_slot == n) {
							nk::vec2<float> l0(circle.x + 3, circle.y + 3);
							nk::vec2<float> l1 = ctx.get_input().mouse.pos;
							canvas.stroke_curve(l0.x, l0.y, l0.x + 50.0f, l0.y, l1.x - 50.0f, l1.y, l1.x, l1.y, 1.0f, nk::color(100, 100, 100));
						}
					}

					/* input connector */
					space = node->bounds.h / static_cast<float>(it->input_count + 1);
					for (int n = 0; n < it->input_count; ++n) {
						nk::rect<float> circle(
							node->bounds.x - 4,
							node->bounds.y + space * static_cast<float>(n + 1),
							8,
							8
						);
						canvas.fill_circle(circle, nk::color(100, 100, 100));
						if (ctx.input_is_mouse_released(NK_BUTTON_LEFT) &&
							ctx.input_is_mouse_hovering_rect(circle) &&
							editor.linking.active && editor.linking.nd != &*it) {
							editor.linking.active = nk_false;
							editor.link(editor.linking.input_id, editor.linking.input_slot, it->id, n);
						}
					}
				}
			}

			/* reset linking connection */
			if (editor.linking.active && ctx.input_is_mouse_released(NK_BUTTON_LEFT)) {
				editor.linking.active = false;
				editor.linking.nd = nullptr;
				fprintf(stdout, "linking failed\n");
			}

			/* draw each link */
			for (const node_link& link : editor.links) {
				struct node* ni = editor.find(link.input_id);
				struct node* no = editor.find(link.output_id);
				float spacei = node->bounds.h / static_cast<float>(ni->output_count + 1);
				float spaceo = node->bounds.h / static_cast<float>(no->input_count + 1);
				auto l0 = layout_space.to_screen(
					{ni->bounds.x + ni->bounds.w, 3.0f + ni->bounds.y + spacei * static_cast<float>(link.input_slot + 1)});
				auto l1 = layout_space.to_screen(
					{no->bounds.x, 3.0f + no->bounds.y + spaceo * static_cast<float>(link.output_slot + 1)});

				l0.x -= editor.scrolling.x;
				l0.y -= editor.scrolling.y;
				l1.x -= editor.scrolling.x;
				l1.y -= editor.scrolling.y;
				canvas.stroke_curve(l0.x, l0.y, l0.x + 50.0f, l0.y, l1.x - 50.0f, l1.y, l1.x, l1.y, 1.0f, nk::color(100, 100, 100));
			}

			if (updated != editor.nodes.end()) {
				/* reshuffle nodes to have least recently selected node on top */
				editor.nodes.splice(editor.nodes.end(), editor.nodes, updated);
			}

			/* node selection */
			if (ctx.input_mouse_clicked(NK_BUTTON_LEFT, layout_space.bounds())) {
				editor.selected = nullptr;
				editor.bounds = nk::rect<float>(ctx.get_input().mouse.pos.x, ctx.get_input().mouse.pos.y, 100, 200);
				for (auto it = editor.nodes.begin(); it != editor.nodes.end(); ++it) {
					struct nk_rect b = layout_space.rect_to_screen(it->bounds);
					b.x -= editor.scrolling.x;
					b.y -= editor.scrolling.y;
					if (ctx.input_is_mouse_hovering_rect(b))
						editor.selected = &*it;
				}
			}

			/* contextual menu */
			if (auto contextual = win.contextual_scoped(0, {100, 220}, win.get_bounds())) {
				const char* grid_option[] = {"Show Grid", "Hide Grid"};
				win.layout_row_dynamic(25, 1);
				if (contextual.item_label("New", NK_TEXT_CENTERED))
					editor.add("New", {400, 260, 180, 220}, nk::color(255, 255, 255), 1, 2);
				if (contextual.item_label(grid_option[editor.show_grid], NK_TEXT_CENTERED))
					editor.show_grid = !editor.show_grid;
			}
		}

		/* window content scrolling */
		if (ctx.input_is_mouse_hovering_rect(win.get_bounds()) &&
			ctx.input_is_mouse_down(NK_BUTTON_MIDDLE)) {
			editor.scrolling.x += ctx.get_input().mouse.delta.x;
			editor.scrolling.y += ctx.get_input().mouse.delta.y;
		}
	}

	return !ctx.window_is_closed("NodeEdit");
}
