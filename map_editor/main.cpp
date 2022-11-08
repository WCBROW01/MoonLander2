#include <stdio.h>
#include <string.h>
#include <limits.h>

#include <SDL.h>

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_sdlrenderer.h"

#include "tilesheet.h"
#include "tiles.h"
#include "map.h"

#if !SDL_VERSION_ATLEAST(2,0,17)
#error This backend requires SDL 2.0.17+ because of SDL_RenderGeometry() function
#endif

static char file_path[PATH_MAX] = ""; // File path for the currently open map

void new_window(bool *open, ML2_Map **map, SDL_Renderer *renderer, SDL_Point *camera_pos) {
	if (!ImGui::Begin("Create New Map", open)) {
		ImGui::End();
		return;
	}

	static int w, h, start_fuel;
	ImGui::InputInt("Width", &w);
	ImGui::InputInt("Height", &h);
	ImGui::InputInt("Starting Fuel", &start_fuel);
	
	static float col[3];
	ImGui::ColorEdit3("Background Color", col, 0);
	
	static enum builtin_tilesheets ts = TILESHEET_MOON;
	ImGui::Text("Tilesheet");
	if (ImGui::RadioButton("Moon", ts == TILESHEET_MOON)) ts = TILESHEET_MOON;
	ImGui::SameLine();
	if (ImGui::RadioButton("Custom", ts == TILESHEET_CUSTOM)) ts = TILESHEET_CUSTOM;
	
	static char ts_path[PATH_MAX];
	if (ts != TILESHEET_CUSTOM) ImGui::BeginDisabled();
	ImGui::InputText("Path", ts_path, PATH_MAX);
	if (ts != TILESHEET_CUSTOM) ImGui::EndDisabled();
	
	if (ImGui::Button("Create")) {
		ML2_Map params = {
			.width = w,
			.height = h,
			.start_x = 5,
			.start_y = 6,
			.start_fuel = start_fuel,
			.bgcolor = {col[0] * 255, col[1] * 255, col[2] * 255, 255},
			.tiles = ts ? nullptr : TileSheet_create(ts_path, renderer, 16, 16, TILESHEET_CREATESURFACE),
			.tilesheet_enum = ts
		};
		
		ML2_Map_free(*map);
		*map = ML2_Map_create(params, renderer);
		if (!*map) fprintf(stderr, "SDL_Map_create: %s\n", SDL_GetError());
		*camera_pos = {0, 0};
		*open = false;
	}
	ImGui::SameLine();
	if (ImGui::Button("Cancel")) {
		*open = false;
	}
	ImGui::End();
}

void open_window(bool *open, ML2_Map **map, SDL_Renderer *renderer) {
	if (!ImGui::Begin("Open Map")) {
		ImGui::End();
		return;
	}
	
	static char working_path[PATH_MAX];
	static bool refresh_text = true;
	
	if (refresh_text) {
		strncpy(working_path, file_path, PATH_MAX);
		refresh_text = false;
	}
	
	ImGui::InputText("Path", working_path, PATH_MAX);
	if (ImGui::Button("Open")) {
		ML2_Map_free(*map);
		strncpy(file_path, working_path, PATH_MAX);
		refresh_text = true;
		*map = ML2_Map_loadFromFile(file_path, renderer);
		*open = false;
	}
	ImGui::SameLine();
	if (ImGui::Button("Cancel")) {
		refresh_text = true;
		*open = false;
	}
	ImGui::End();
}

void save_as_window(bool *open, ML2_Map *map) {
	if (!ImGui::Begin("Save As")) {
		ImGui::End();
		return;
	}
	
	static char working_path[PATH_MAX];
	static bool refresh_text = true;
	
	if (refresh_text) {
		strncpy(working_path, file_path, PATH_MAX);
		refresh_text = false;
	}
	
	ImGui::InputText("Path", working_path, PATH_MAX);
	if (ImGui::Button("Save")) {
		strncpy(file_path, working_path, PATH_MAX);
		refresh_text = true;
		ML2_Map_save(map, file_path);
		*open = false;
	}
	ImGui::SameLine();
	if (ImGui::Button("Cancel")) {
		refresh_text = true;
		*open = false;
	}
	ImGui::End();
}

void tiles_window(bool *open, ML2_Map *map, int *selected_tile) {
	if (!ImGui::Begin("Tiles", open)) {
		ImGui::End();
		return;
	}
	
	if (map) {
		int ts_w = map->tiles->tile_width * map->tiles->sheet_width;
		int ts_h = map->tiles->tile_height * map->tiles->sheet_height;
		for (size_t i = 0; i < TILE_COUNT; ++i) {
			SDL_Rect clip = TileSheet_getTileRect(map->tiles, i);
			ImVec2 uv0 = ImVec2((float) clip.x / ts_w, (float) clip.y / ts_h);
			ImVec2 uv1 = ImVec2((float) (clip.x + clip.w) / ts_w, (float) (clip.y + clip.h) / ts_h);
			
			ImGui::PushID(i);
			bool selected = *selected_tile == i;
			if (selected) ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyle().Colors[ImGuiCol_ButtonActive]);
			if (ImGui::ImageButton("##tile", map->tiles->texture, ImVec2(map->tiles->tile_width, map->tiles->tile_height), uv0, uv1)) {
				*selected_tile = i;
			}
			if (selected) ImGui::PopStyleColor();
			ImGui::PopID();
			ImGui::SameLine();
		}
	}
	
	ImGui::End();
}

void about_window(bool *open) {
	if (!ImGui::Begin("About ML2 Editor", open)) {
		// Don't render window if collapsed
		ImGui::End();
		return;
	}
	
	ImGui::Text("Map editor for Moon Lander 2.");
	ImGui::Text("Licensed under the GNU General Public License v3");
	ImGui::Text("See LICENSE or https://www.gnu.org/licenses/");
	ImGui::Text("(c) Will Brown");
	ImGui::End();
}	

int main(int argc, char *argv[]) {
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) < 0) {
		fprintf(stderr, "SDL_Init: %s\n", SDL_GetError());
		return 1;
	}
	
	// Setup window
	SDL_WindowFlags window_flags = (SDL_WindowFlags) (SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
	SDL_Window *window = SDL_CreateWindow("ML2 Editor", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1024, 768, window_flags);
	if (!window) {
		fprintf(stderr, "SDL_CreateWindow: %s\n", SDL_GetError());
		return 1;
	}
	
	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
	if (!renderer) {
		fprintf(stderr, "SDL_CreateRenderer: %s\n", SDL_GetError());
		return 1;
	}
	
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO();
	
	// Set style
	ImGui::StyleColorsLight();
	
	// Init backends
	ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
	ImGui_ImplSDLRenderer_Init(renderer);
	
	// Map state
	ML2_Map *map = nullptr;
	int selected_tile = 0;
	SDL_Point camera_pos = {0, 0};
	SDL_Point tile_pos = {0, 0};
	
	// Open window state
	bool show_new_window = false;
	bool show_open_window = false;
	bool show_save_as_window = false;
	bool show_tiles_window = false;
	bool show_demo_window = false;
	bool show_about_window = false;
	bool dark_theme = false;
	
	bool done = false;
	while (!done) {
		SDL_Event e;
		while (SDL_PollEvent(&e)) {
			ImGui_ImplSDL2_ProcessEvent(&e);
			if (e.type == SDL_QUIT) {
				done = true;
			} else if (e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_CLOSE && e.window.windowID == SDL_GetWindowID(window)) {
				done = true;
			}
		}
		
		// Start Dear ImGui frame
		ImGui_ImplSDLRenderer_NewFrame();
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();
		
		if (ImGui::BeginMainMenuBar()) {
			if (ImGui::BeginMenu("File")) {
				if (ImGui::MenuItem("New")) {
					show_new_window = true;
				}
				if (ImGui::MenuItem("Open")) {
					show_open_window = true;
				}
				if (ImGui::MenuItem("Save")) {
					if (map) ML2_Map_save(map, file_path);
					else show_save_as_window = true;
				}
				if (ImGui::MenuItem("Save As")) {
					show_save_as_window = true;
				}
				ImGui::Separator();
				if (ImGui::MenuItem("Quit")) {
					done = true;
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("View")) {
				if (ImGui::MenuItem("Toggle dark theme")) {
					if ((dark_theme = !dark_theme)) {
						ImGui::StyleColorsDark();
					} else {
						ImGui::StyleColorsLight();
					}
				}
				if (ImGui::MenuItem("Tiles")) {
					show_tiles_window = true;
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Help")) {
				if (ImGui::MenuItem("Demo Window")) {
					show_demo_window = true;
				}
				if (ImGui::MenuItem("About")) {
					show_about_window = true;
				}
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}
		
		if (show_new_window) new_window(&show_new_window, &map, renderer, &camera_pos);
		if (show_open_window) open_window(&show_open_window, &map, renderer);
		if (show_save_as_window) save_as_window(&show_save_as_window, map);
		if (show_tiles_window) tiles_window(&show_tiles_window, map, &selected_tile);
		if (show_demo_window) ImGui::ShowDemoWindow(&show_demo_window);
		if (show_about_window) about_window(&show_about_window);
		
#define UNPACK_COLOR(color) (color).r, (color).g, (color).b, (color).a
		
		// Render
		ImGui::Render();
		if (map) SDL_SetRenderDrawColor(renderer, UNPACK_COLOR(map->bgcolor));
		else SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);
		
#define MAP_RENDER_SCALE 2

		if (map) {
			ML2_Map_renderScaled(map, renderer, &camera_pos, MAP_RENDER_SCALE);
			
			// Create green highlight for tile being hovered over
			if (!io.WantCaptureMouse) {
				int win_w, win_h, render_w, render_h, mouse_x, mouse_y, mouse_rel_x, mouse_rel_y;
				SDL_GetWindowSize(window, &win_w, &win_h);
				SDL_GetRendererOutputSize(renderer, &render_w, &render_h);
				Uint32 mouse_state = SDL_GetMouseState(&mouse_x, &mouse_y);
				SDL_GetRelativeMouseState(&mouse_rel_x, &mouse_rel_y);
				mouse_x = mouse_x * render_w / win_w;
				mouse_y = mouse_y * render_h / win_h;
				mouse_rel_x = mouse_rel_x * render_w / win_w;
				mouse_rel_y = mouse_rel_y * render_h / win_h;
				
				tile_pos = {
					.x = (camera_pos.x + mouse_x) / map->tiles->tile_width / MAP_RENDER_SCALE,
					.y = (camera_pos.y + render_h - mouse_y) / map->tiles->tile_height / MAP_RENDER_SCALE
				};
				
				if (mouse_state & SDL_BUTTON_LMASK) {
					ML2_Map_setTile(map, tile_pos.x, tile_pos.y, selected_tile, 0);
				} else if (mouse_state & SDL_BUTTON_RMASK) {
					camera_pos.x -= mouse_rel_x;
					if (camera_pos.x < 0) {
						camera_pos.x = 0;
					} else if ((unsigned) camera_pos.x > map->width * map->tiles->tile_width * MAP_RENDER_SCALE - render_w) {
						camera_pos.x = map->width * map->tiles->tile_width * MAP_RENDER_SCALE - render_w;
					}
					
					camera_pos.y += mouse_rel_y;
					if (camera_pos.y < 0) {
						camera_pos.y = 0;
					} else if ((unsigned) camera_pos.y > map->height * map->tiles->tile_height * MAP_RENDER_SCALE - render_h) {
						camera_pos.y = map->height * map->tiles->tile_height * MAP_RENDER_SCALE - render_h;
					}
				}
				
				SDL_Rect highlight_rect = {
					.x = tile_pos.x * map->tiles->tile_width * MAP_RENDER_SCALE - camera_pos.x,
					.y = render_h - map->tiles->tile_height * MAP_RENDER_SCALE - (tile_pos.y * map->tiles->tile_height * MAP_RENDER_SCALE - camera_pos.y),
					.w = map->tiles->tile_width * MAP_RENDER_SCALE,
					.h = map->tiles->tile_height * MAP_RENDER_SCALE
				};
				
				SDL_SetRenderDrawColor(renderer, 0, 127, 0, 200);
				SDL_RenderFillRect(renderer, &highlight_rect);
			}
		}
		
		ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
		SDL_RenderPresent(renderer);
	}
	
	// Cleanup
	ML2_Map_free(map);
	
	ImGui_ImplSDLRenderer_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
	
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	
	return 0;
}
