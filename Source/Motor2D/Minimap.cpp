#include "Dependencies/Brofiler/Brofiler.h"
#include "Log.h"

#include <list>

#include "Application.h"
#include "Window.h"
#include "Render.h"
#include "Textures.h"
#include "Input.h"
#include "Map.h"
#include "EntityManager.h"
#include "Entity.h"
#include "Player.h"
#include "FowManager.h"

#include "Minimap.h"

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

	player_is_moving_camera = false;

	return true;
}

bool Minimap::Start()
{
	minimap_tex			= nullptr;
	minimap_fow_tex		= nullptr;
	tex					= nullptr;
	fow_tex				= nullptr;
	minimap_bg			= nullptr;
	map_surface			= nullptr;
	map_renderer		= nullptr;

	return true;
}

void Minimap::CreateTexture() 
{
	BROFILER_CATEGORY("Load MiniMap", Profiler::Color::Khaki);

	map_width = (App->map->data.width * App->map->data.tile_width);
	minimap_scale = minimap_width / map_width;

	x_offset = App->map->data.tile_width * 0.5f * minimap_scale;
	y_offset = App->map->data.tile_height * 0.5f * minimap_scale;

	map_surface = SDL_CreateRGBSurface(0, minimap_width + x_offset, minimap_height + y_offset, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);

	map_renderer = SDL_CreateSoftwareRenderer(map_surface);

	tex			= App->tex->Load("Assets/maps/Prototileset.png", map_renderer);
	fow_tex		= App->tex->Load(App->config_file.child("config").child("fog_of_war").child("fow_tex").attribute("path").as_string(), map_renderer);
	minimap_bg	= App->tex->Load(App->config_file.child("config").child("gui").child("backgrounds").child("minimap").attribute("path").as_string());

	DrawMinimap();

	minimap_tex = SDL_CreateTextureFromSurface(App->render->renderer, map_surface);
}



bool Minimap::Update(float dt)
{	
	BROFILER_CATEGORY("Update Minimap", Profiler::Color::Indigo);
	
	if (App->map->map_loaded == true && App->minimap->minimap_loaded == false) 
	{
		App->minimap->minimap_loaded = true;
		CreateTexture();
	}

	if (App->map->map_loaded == true) 
	{
		if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_STATE::KEY_DOWN)
		{
			int map_x, map_y;

			if (MinimapCoords(map_x, map_y))
			{
				player_is_moving_camera = true;
			}
		}
		
		if (player_is_moving_camera)
		{
			if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_STATE::KEY_REPEAT)
			{
				int map_x, map_y;

				if (MinimapCoords(map_x, map_y))
				{
					// Assign to the center of the camera, the coordinates "map_x" and "map_y"
					App->render->camera.x = -map_x + App->win->width * 0.5f;
					App->render->camera.y = -map_y + App->win->height * 0.5f;
				}
			}
		}

		if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_STATE::KEY_UP)
		{
			player_is_moving_camera = false;
		}
	}

	// FOG OF WAR
	if (App->player->CurrentlyInGameplayScene())
	{
		DrawFogOfWar();
	}

	return true;
}

bool Minimap::PostUpdate()
{	


	return true;
}

void Minimap::BlitMinimap() 
{
	BROFILER_CATEGORY("Mini Map", Profiler::Color::LavenderBlush);

	App->render->Blit(minimap_bg, minimap_position.x + 2, minimap_position.y + 2, NULL, false, 0.0f);

	App->render->Blit(minimap_tex, minimap_position.x, minimap_position.y, NULL, false, 0.0f);

	App->render->Blit(minimap_fow_tex, minimap_position.x, minimap_position.y, NULL, false, 0.0f);

	MinimapBorders();
	DrawEntities();
	DrawCamera();
}


bool Minimap::CleanUp()
{
	App->tex->UnLoad(tex);
	App->tex->UnLoad(minimap_tex);
	App->tex->UnLoad(minimap_fow_tex);

	if (SDL_RenderClear(map_renderer) == 0)
	{
		map_renderer = nullptr;
	}

	SDL_DestroyTexture(minimap_tex);
	SDL_FreeSurface(map_surface);

	return true;
}

bool Minimap::InteractingWithMinimap()
{	
	int x, y;

	return (MinimapCoords(x, y) || player_is_moving_camera);
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
		map_x = (mouse_x - minimap_position.x - (minimap_width * 0.5f)) / minimap_scale;
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
					App->render->Blit(tex, (pos.x + minimap_width * 0.5f), pos.y, &r, false, 0.0f, minimap_scale, 0.0f, 0, 0, map_renderer);
				}
			}
		}
	}
}

void Minimap::DrawFogOfWar()
{
	BROFILER_CATEGORY("Draw Fog Of War on Minimap", Profiler::Color::Black);
	
	SDL_RenderClear(map_renderer);
	
	if (minimap_fow_tex != nullptr)
	{
		App->tex->UnLoad(minimap_fow_tex);
		SDL_DestroyTexture(minimap_fow_tex);													// The texture needs to be destroyed as we use SDL_CreateTextureFromSurface().
	}
	
	for (int y = 0; y < App->map->data.height; ++y)
	{
		for (int x = 0; x < App->map->data.width; ++x)
		{
			iPoint tile_position = { x, y };
			iPoint world_position = App->map->MapToWorld(x, y);

			world_position.x *= minimap_scale;
			world_position.y *= minimap_scale;
			
			// FOG OF WAR
			uchar fow_state = App->fow_manager->GetVisibilityAt({ x, y });

			SDL_Rect fow_tile_rect = App->fow_manager->GetFowTileRect(fow_state);
			App->render->Blit(fow_tex, (world_position.x + minimap_width * 0.5f), world_position.y - 2, &fow_tile_rect, false, 0.0f, minimap_scale, 0.0f, 0, 0, map_renderer);
		}
	}

	minimap_fow_tex = SDL_CreateTextureFromSurface(App->render->renderer, map_surface);
}

void Minimap::DrawCamera()
{
	SDL_Rect map_camera = { ((-App->render->camera.x)) * minimap_scale + minimap_width * 0.5f + minimap_position.x, -App->render->camera.y * minimap_scale + minimap_position.y,
							App->render->camera.w * minimap_scale, App->render->camera.h * minimap_scale };

	App->render->DrawQuad(map_camera, 255, 255, 0, 255, false, false);
}

void Minimap::MinimapBorders()
{
	App->render->DrawLine(x_offset + minimap_position.x													// Origin X
						, (minimap_height * 0.5f) + y_offset + minimap_position.y						// Origin Y
						, (minimap_width * 0.5f) + x_offset + minimap_position.x						// End X
						, y_offset + minimap_position.y													// End Y
						, 255, 255, 255, 255, false);													// Colour, opacity and camera use.
	
	App->render->DrawLine(minimap_width + x_offset + minimap_position.x									// Origin X
						, (minimap_height * 0.5f) + y_offset + minimap_position.y						// Origin Y
						, (minimap_width * 0.5f) + x_offset + minimap_position.x						// End X
						, y_offset + minimap_position.y													// End Y
						, 255, 255, 255, 255, false);													// Colour, opacity and camera use.
	
	App->render->DrawLine(minimap_width + x_offset + minimap_position.x									// Origin X
						, (minimap_height * 0.5f) + y_offset + minimap_position.y						// Origin Y
						, (minimap_width * 0.5f) + x_offset + minimap_position.x						// End X
						, minimap_height + y_offset + minimap_position.y								// End Y
						, 255, 255, 255, 255, false);													// Colour, opacity and camera use.
	
	App->render->DrawLine(x_offset + minimap_position.x													// Origin X
						, (minimap_height * 0.5f) + y_offset + minimap_position.y						// Origin Y
						, (minimap_width * 0.5f) + x_offset + minimap_position.x						// End X
						, minimap_height + y_offset + minimap_position.y								// End Y
						, 255, 255, 255, 255, false);													// Colour, opacity and camera use.
}

void Minimap::DrawEntities()
{
	int pos_x, pos_y;

	int minimap_x_offset = (int)(minimap_width * 0.5f) + minimap_position.x;
	int minimap_y_offset = y_offset + minimap_position.y;

	std::vector<Entity*>::iterator entity = App->entity_manager->entities.begin();

	for (; entity != App->entity_manager->entities.end(); ++entity)
	{
		if ((*entity)->is_visible)
		{
			pos_x = (*entity)->pixel_position.x * minimap_scale;
			pos_y = (*entity)->pixel_position.y * minimap_scale;

			switch ((*entity)->type) 
			{
			case ENTITY_TYPE::BARRACKS:
				App->render->DrawQuad({ pos_x + (int)(minimap_width * 0.5f) + minimap_position.x, pos_y - y_offset + minimap_position.y, 4, 4 }, 204, 0, 204, 255, true, false);
				break;
			case ENTITY_TYPE::TOWNHALL:
				App->render->DrawQuad({ pos_x + (int)(minimap_width * 0.5f) + minimap_position.x, pos_y - y_offset + minimap_position.y, 4, 4 }, 204, 0, 204, 255, true, false);
				App->render->DrawQuad({ (pos_x + (int)(minimap_width * 0.5f)) + 1 + minimap_position.x, pos_y - y_offset + minimap_position.y + 1, 2, 2 }, 204, 153, 204, 255, true, false);
				break;
			case ENTITY_TYPE::INFANTRY:
				App->render->DrawQuad({ pos_x + (int)(minimap_width * 0.5f) + minimap_position.x, pos_y - y_offset + minimap_position.y, 2, 2 }, 204, 0, 204, 255, true, false);
				break;
			case ENTITY_TYPE::GATHERER:
				App->render->DrawQuad({ pos_x + (int)(minimap_width * 0.5f) + minimap_position.x, pos_y - y_offset + minimap_position.y, 2, 2 }, 204, 0, 204, 255, true, false);
				break;
			case ENTITY_TYPE::ENEMY_BARRACKS:
				App->render->DrawQuad({ pos_x + (int)(minimap_width * 0.5f) + minimap_position.x, pos_y - y_offset + minimap_position.y, 4, 4 }, 0, 204, 204, 255, true, false);
				break;
			case ENTITY_TYPE::ENEMY_TOWNHALL:
				App->render->DrawQuad({ pos_x + (int)(minimap_width * 0.5f) + minimap_position.x, pos_y - y_offset + minimap_position.y, 4, 4 }, 0, 204, 204, 255, true, false);
				App->render->DrawQuad({ (pos_x + (int)(minimap_width * 0.5f)) + 1 + minimap_position.x, pos_y - y_offset + minimap_position.y + 1, 2, 2 }, 153, 204, 204, 255, true, false);
				break;
			case ENTITY_TYPE::ENEMY_INFANTRY:
				App->render->DrawQuad({ pos_x + (int)(minimap_width * 0.5f) + minimap_position.x, pos_y - y_offset + minimap_position.y, 2, 2 }, 0, 204, 204, 255, true, false);
				break;
			case ENTITY_TYPE::ROCK:
				App->render->DrawQuad({ pos_x + (int)(minimap_width * 0.5f) + minimap_position.x, pos_y - y_offset + minimap_position.y, 2, 2 }, 255, 255, 255, 255, true, false);
				break;
			}
		}
	}
}