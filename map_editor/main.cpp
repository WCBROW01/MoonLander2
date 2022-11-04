#include <stdio.h>
#include <limits.h>

#include <SDL.h>

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_sdlrenderer.h"

#include "tilesheet.h"
#include "map.h"

#if !SDL_VERSION_ATLEAST(2,0,17)
#error This backend requires SDL 2.0.17+ because of SDL_RenderGeometry() function
#endif

void new_window(bool *open) {
	if (!ImGui::Begin("Create New Map", open)) {
		ImGui::End();
		return;
	}

	static int w, h;
	ImGui::InputInt("Width", &w);
	ImGui::InputInt("Height", &h);
	if (ImGui::Button("Create")) {
		*open = false;
	}
	ImGui::SameLine();
	if (ImGui::Button("Cancel")) {
		*open = false;
	}
	ImGui::End();
}

void open_window(bool *open, ML2_Map **map, SDL_Renderer *renderer) {
	if (!ImGui::Begin("Open Map", open)) {
		ImGui::End();
		return;
	}
	
	static char path[PATH_MAX];
	ImGui::InputText("Path", path, PATH_MAX);
	if (ImGui::Button("Open")) {
		ML2_Map_free(*map);
		*map = ML2_Map_loadFromFile(path, renderer);
		*open = false;
	}
	ImGui::SameLine();
	if (ImGui::Button("Cancel")) {
		*open = false;
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
	
	// Load map
	ML2_Map *map = ML2_Map_loadFromFile("test4.ml2", renderer);
	SDL_Point camera_pos = {0, 0};
	SDL_Point tile_pos = {0, 0};
	
	bool show_new_window = false;
	bool show_open_window = false;
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
					ML2_Map_save(map, "test5.ml2");
				}
				if (ImGui::MenuItem("Save As")) {
					// file dialog of some sort
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
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Help")) {
				if (ImGui::MenuItem("About")) {
					show_about_window = true;
				}
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}
		
		
		if (show_new_window) new_window(&show_new_window);
		if (show_open_window) open_window(&show_open_window, &map, renderer);
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
					ML2_Map_setTile(map, tile_pos.x, tile_pos.y, 1, 0);
				} else if (mouse_state & SDL_BUTTON_MMASK) {
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
				} else if (mouse_state & SDL_BUTTON_RMASK) {
					ML2_Map_setTile(map, tile_pos.x, tile_pos.y, 0, 0);
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
