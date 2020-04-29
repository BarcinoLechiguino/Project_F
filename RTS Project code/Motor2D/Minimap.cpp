#include "Minimap.h"

#include "Application.h"
#include "Window.h"
#include "Render.h"
#include "Textures.h"
#include "Input.h"
#include "EntityManager.h"
#include "Entity.h"

#include "Brofiler/Brofiler.h"

#include "Map.h"

#include "p2Log.h"

#include <list>

Minimap::Minimap() : Module(), minimap_loaded(false)
{
	name = "minimap";
}

Minimap::~Minimap() {}

bool Minimap::Awake(pugi::xml_node & config)
{
	minimap_width = 300;
	minimap_height = 220;

	minimap_position.x = 10;
	minimap_position.y = 490;

	return true;
}

bool Minimap::Start()
{
	return true;
}

void Minimap::CreateTexture() 
{

	BROFILER_CATEGORY("Load MiniMap", Profiler::Color::Khaki);

	// Initialize the variable "map_width" to obtain the width of the map in pixels
	// Initialize the variable "minimap_scale" to get the relation between the map width and
	// the minimap width (defined at config.xml and initialized in Awake())
	map_width = (App->map->data.width * App->map->data.tile_width);
	minimap_scale = minimap_width / map_width;

	x_offset = App->map->data.tile_width / 2 * minimap_scale;
	y_offset = App->map->data.tile_height / 2 * minimap_scale;

	// Use the function SDL_CreateRGBSurface() to allocate a RGB surface to the variable "map_surface"
	// The last four parameters should be: 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000 in order to be totally transparent
	// You have to add the x & y offsets
	map_surface = SDL_CreateRGBSurface(0, minimap_width + x_offset, minimap_height + y_offset, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);

	// TODO 3: Use the function SDL_CreateSoftwareRenderer() to create a 2D software rendering context for a surface
	// Assign it to the variable "map_renderer"
	map_renderer = SDL_CreateSoftwareRenderer(map_surface);

	tex = App->tex->Load("maps/Prototileset.png", map_renderer);
	minimap_bg = App->tex->Load("textures/Spritesheets/UI/Minimap_bg.png");


	DrawMinimap();

	// Use the function SDL_CreateTextureFromSurface to create a texture from an existing surface
	// Assign it to the variable "map_tex". Use the renderer from render.h
	minimap_tex = SDL_CreateTextureFromSurface(App->render->renderer, map_surface);

}



bool Minimap::Update(float dt)
{	
	if (App->map->map_loaded == true && App->minimap->minimap_loaded == false) 
	{
		App->minimap->minimap_loaded = true;
		CreateTexture();
	}

	if (App->map->map_loaded == true) {
		if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_STATE::KEY_REPEAT)
		{
			int map_x, map_y;

			if (MinimapCoords(map_x, map_y))
			{
				// Assign to the center of the camera, the coordinates "map_x" and "map_y"
				App->render->camera.x = -map_x + App->win->width / 2;
				App->render->camera.y = -map_y + App->win->height / 2;
			}
		}
	}

	return true;
}

bool Minimap::PostUpdate()
{	


	return true;
}

void Minimap::BlitMinimap() {

	BROFILER_CATEGORY("Mini Map", Profiler::Color::LavenderBlush);

	App->render->Blit(minimap_bg, minimap_position.x, minimap_position.y, NULL, false, 0.0f);

	App->render->Blit(minimap_tex, minimap_position.x, minimap_position.y, NULL, false, 0.0f);
	MinimapBorders();
	DrawEntities();
	DrawCamera();

}


bool Minimap::CleanUp()
{
	App->tex->UnLoad(tex);
	App->tex->UnLoad(minimap_tex);

	if (SDL_RenderClear(map_renderer) == 0)
		map_renderer = nullptr;

	SDL_DestroyTexture(minimap_tex);
	SDL_FreeSurface(map_surface);

	return true;
}

bool Minimap::MinimapCoords(int& map_x, int& map_y)
{
	int mouse_x, mouse_y;
	App->input->GetMousePosition(mouse_x, mouse_y);

	if (mouse_x >= minimap_position.x && mouse_x <= (minimap_width + minimap_position.x) &&	mouse_y >= minimap_position.y && mouse_y <= (minimap_height + minimap_position.y))
	{
		// Assign to "map_x" and "map_y" the mouse position respect the minimap, to the
		// position that corresponds to the map
		// Take into account that it is an isometric map
		map_x = (mouse_x - minimap_position.x - minimap_width/2) / minimap_scale;
		map_y = (mouse_y - minimap_position.y) / minimap_scale;
	}
	else
		return false;

	return true;
}

void Minimap::DrawMinimap()
{
	std::vector<MapLayer*>::const_iterator item = App->map->data.layers.begin();

	for (; item != App->map->data.layers.end(); item = next(item))
	{
		MapLayer* layer = *item;

		for (int y = 0; y < App->map->data.height; ++y)
		{
			for (int x = 0; x < App->map->data.width; ++x)
			{
				int tile_id = layer->Get(x, y);
				if (tile_id > 0)
				{
					TileSet* tileset = App->map->GetTilesetFromTileId(tile_id);

					SDL_Rect r = tileset->GetTileRect(tile_id);
					iPoint pos = App->map->MapToWorld(x, y);
					
					pos.x *= minimap_scale;
					pos.y *= minimap_scale;

					// Blit the minimap. You need to pass all the parameters until renderer included.
					// As it is an isometric map, keep in mind that x == 0 is in the middle of the map.
					App->render->Blit(tex, (pos.x + minimap_width / 2), pos.y, &r, false, 0.0f, minimap_scale, 0.0f, 0, 0,map_renderer);
				}
			}
		}
	}
}

void Minimap::DrawCamera()
{
	SDL_Rect map_camera = { ((-App->render->camera.x)) * minimap_scale + minimap_width * 0.5f + minimap_position.x, -App->render->camera.y * minimap_scale + minimap_position.y,
							App->render->camera.w * minimap_scale, App->render->camera.h * minimap_scale };

	App->render->DrawQuad(map_camera, 255, 255, 0, 255, false, false);
}

void Minimap::MinimapBorders()
{
	App->render->DrawLine(x_offset + minimap_position.x, minimap_height / 2 + y_offset + minimap_position.y, minimap_width / 2 + x_offset + minimap_position.x, y_offset + minimap_position.y, 255, 255, 255, 255, false);
	App->render->DrawLine(minimap_width + x_offset + minimap_position.x, minimap_height / 2 + y_offset + minimap_position.y, minimap_width / 2 + x_offset + minimap_position.x, y_offset + minimap_position.y, 255, 255, 255, 255, false);
	App->render->DrawLine(minimap_width + x_offset + minimap_position.x, minimap_height / 2 + y_offset + minimap_position.y, minimap_width / 2 + x_offset + minimap_position.x, minimap_height + y_offset + minimap_position.y, 255, 255, 255, 255, false);
	App->render->DrawLine(x_offset + minimap_position.x, minimap_height / 2 + y_offset + minimap_position.y, minimap_width / 2 + x_offset + minimap_position.x, minimap_height + y_offset + minimap_position.y, 255, 255, 255, 255, false);
}

void Minimap::DrawEntities()
{
	int pos_x, pos_y;

	for (int i = 0; i < (int)App->entity_manager->entities.size(); ++i)
	{

		pos_x = App->entity_manager->entities[i]->pixel_position.x * minimap_scale;
		pos_y = App->entity_manager->entities[i]->pixel_position.y * minimap_scale;

		switch (App->entity_manager->entities[i]->type) {
			case ENTITY_TYPE::BARRACKS:
				App->render->DrawQuad({ pos_x + minimap_width / 2 + minimap_position.x, pos_y - y_offset + minimap_position.y, 4, 4 }, 204, 0, 204, 255, true, false);
				break;
			case ENTITY_TYPE::TOWNHALL:
				App->render->DrawQuad({ pos_x + minimap_width / 2 + minimap_position.x, pos_y - y_offset + minimap_position.y, 4, 4 }, 204, 0, 204, 255, true, false);
				App->render->DrawQuad({ (pos_x + minimap_width / 2) + 1 + minimap_position.x, pos_y - y_offset + minimap_position.y + 1, 2, 2 }, 204, 153, 204, 255, true, false);
				break;
			case ENTITY_TYPE::INFANTRY:
				App->render->DrawQuad({ pos_x + minimap_width / 2 + minimap_position.x, pos_y - y_offset + minimap_position.y, 2, 2 }, 204, 0, 204, 255, true, false);
				break;
			case ENTITY_TYPE::GATHERER:
				App->render->DrawQuad({ pos_x + minimap_width / 2 + minimap_position.x, pos_y - y_offset + minimap_position.y, 2, 2 }, 204, 0, 204, 255, true, false);
				break;
			case ENTITY_TYPE::ENEMY_BARRACKS:
				App->render->DrawQuad({ pos_x + minimap_width / 2 + minimap_position.x, pos_y - y_offset + minimap_position.y, 4, 4 }, 0, 204, 204, 255, true, false);
				break;
			case ENTITY_TYPE::ENEMY_TOWNHALL:
				App->render->DrawQuad({ pos_x + minimap_width / 2 + minimap_position.x, pos_y - y_offset + minimap_position.y, 4, 4 }, 0, 204, 204, 255, true, false);
				App->render->DrawQuad({(pos_x + minimap_width / 2) + 1 + minimap_position.x, pos_y - y_offset + minimap_position.y +1, 2, 2 }, 153, 204, 204, 255, true, false);
				break;
			case ENTITY_TYPE::ENEMY:
				App->render->DrawQuad({ pos_x + minimap_width / 2+ minimap_position.x, pos_y - y_offset + minimap_position.y, 2, 2 }, 0, 204, 204, 255, true, false);
				break;
			case ENTITY_TYPE::ROCK:
				App->render->DrawQuad({ pos_x + minimap_width / 2 + minimap_position.x, pos_y - y_offset + minimap_position.y, 2, 2 }, 255, 255, 255, 255, true, false);
				break;
		}
	}
}