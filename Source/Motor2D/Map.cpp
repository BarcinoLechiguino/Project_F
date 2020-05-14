#include "Brofiler\Brofiler.h"
//#include "mmgr/mmgr.h"

#include <math.h>
#include "Definitions.h"
#include "Log.h"

#include "Application.h"
#include "Window.h"
#include "Render.h"
#include "Textures.h"
#include "Input.h"
#include "Audio.h"
#include "Pathfinding.h"
#include "EntityManager.h"
#include "Entity.h"
#include "GuiManager.h"
#include "Console.h"
#include "SceneManager.h"
#include "Scene.h"
#include "FowManager.h"

#include "Map.h"



Map::Map() : Module(), map_loaded(false), pathfinding_meta_debug(false), walkability_debug(false), entity_map_debug(false),smaller_camera(false)
{
	name = ("map");
	
}

// Destructor
Map::~Map()
{}

// Called before render is available
bool Map::Awake(pugi::xml_node& config)
{
	LOG("Loading Map Parser");
	bool ret = true;

	folder = (config.child("folder").child_value());

	return ret;
}

void Map::Draw()
{
	BROFILER_CATEGORY("Draw Map", Profiler::Color::Khaki);

	if (map_loaded == false)																						//If no map was loaded, return.
	{
		return;
	}

	//CAMERA CULLING

	int tiles_drawn = 0;

	App->win->GetWindowSize(winWidth, winHeight);																	//Gets the window size so it can be added to the camera collider as parameters.

	std::vector<MapLayer*>::iterator layer = data.layers.begin();

	for (; layer != data.layers.end(); layer++)																	
	{
		if (smaller_camera)
		{
			camera_pos_in_pixels.x = -App->render->camera.x + winWidth / 4;
			camera_pos_in_pixels.y = -App->render->camera.y + winHeight / 4;

			bottom_right_x = -App->render->camera.x + winWidth / 4 * 3;
			bottom_right_y = -App->render->camera.y + winHeight / 4 * 3;
		}
		else
		{
			camera_pos_in_pixels.x = -App->render->camera.x;
			camera_pos_in_pixels.y = -App->render->camera.y;

			bottom_right_x = -App->render->camera.x + winWidth;
			bottom_right_y = -App->render->camera.y + winHeight;
		}

		min_x_row = WorldToMap(camera_pos_in_pixels.x, camera_pos_in_pixels.y).x;						//Top Left Corner row
		max_x_row = WorldToMap(bottom_right_x + data.tile_width , bottom_right_y ).x + 1;				//Down Right Corner row

		min_y_row = WorldToMap(bottom_right_x, camera_pos_in_pixels.y).y;								//Top-Right Corner
		max_y_row = WorldToMap(camera_pos_in_pixels.x, bottom_right_y + data.tile_height).y + 1 ;		//Bottom-Left Corner

		if (min_x_row < 0)
		{
			min_x_row = 0;
		}
		if (min_y_row < 0)
		{
			min_y_row = 0;
		}

		for (int x = min_x_row ; x < max_x_row && x < data.width; x++)
		{
			for (int y = min_y_row ; y < max_y_row && y < data.height && MapToWorld(x, y).y < bottom_right_y && MapToWorld(x, y).x > camera_pos_in_pixels.x - data.tile_width; y++)
			{
				if (MapToWorld(x, y).y > camera_pos_in_pixels.y - data.tile_height  && MapToWorld(x, y).x < bottom_right_x)
				{
					int tile_id = (*layer)->Get(x, y);																	//Gets the tile id from the tile index. Gets the tile index for a given tile. x + y * data.tile_width;

					if (tile_id > 0)																				
					{
						TileSet* tileset = GetTilesetFromTileId(tile_id);												//Gets the tileset corresponding with the tile_id. If tile id is 34 and the current tileset first gid is 35, that means that the current  tile belongs to the previous tileset. 
						
						if (tileset != nullptr)																			
						{
							SDL_Rect tile_rect		= tileset->GetTileRect(tile_id);									//Gets the position on the world and the dimensions of the rect of the given tile_id 
							iPoint tile_position	= { x, y };
							iPoint world_position	= MapToWorld(x, y);													//Gets the position on the world (in pixels) of a specific point (in tiles). In the case of orthogonal maps the x and y are multiplied by the tile's width  or height. If 32x32, Map world_position: x = 1 --> World world_position: x = 32...

							App->render->Blit(tileset->texture, world_position.x + tileset->offset_x, world_position.y + tileset->offset_y, &tile_rect); //, false, (*layer)->speed)

							tiles_drawn++;

							// DRAWING THE FOG OF WAR							
							DrawFowTile(tile_position, world_position);
						}
					}
				}
			}
		}
	}
	//(*data.layers.begin())->tiles_tree->DrawQuadtree();
	//LOG("Tiles drawn: %d", tiles_drawn);
}

void Map::DrawFowTile(const iPoint& tile_position, const iPoint& world_position)
{
	uchar fow_state			= App->fow_manager->GetVisibilityAt(tile_position);																// Can be either UNEXPLORED, FOGGED or VISIBLE.
	uchar smoothing_state	= 0;																											// Smoothing state of the tile (if it is the case).
	SDL_Rect fow_tile_rect	= { 0, 0, 0, 0 };																								// Fow Tile Sprite Section.

	// NO FOW TILE SMOOTHING
	fow_tile_rect = App->fow_manager->GetFowTileRect(fow_state);
	App->render->Blit(App->fow_manager->fow_tex, world_position.x - 5, world_position.y - 19, &fow_tile_rect);
	
	// --- First tile out of line_of_sight is FOGGED
	/*if (fow_state == UNEXPLORED && !App->fow_manager->CheckNeighbourTilesVisibility(tile_position))
	{
		fow_tile_rect = App->fow_manager->GetFowTileRect(FOGGED);
		App->render->Blit(App->fow_manager->fow_tex, world_position.x - 5, world_position.y - 19, &fow_tile_rect);
	}
	else
	{
		fow_tile_rect = App->fow_manager->GetFowTileRect(fow_state);
		App->render->Blit(App->fow_manager->fow_tex, world_position.x - 5, world_position.y - 19, &fow_tile_rect);
	}*/

	// FOW TILE SMOOTHING
	/*if (fow_state == FOGGED)
	{
		smoothing_state = (uchar)App->fow_manager->GetSmoothingStateAt(tile_position);

		if (smoothing_state >= (uchar)FOW_SMOOTHING_STATE::UNXTOFOG_TOP)																	// UNEXPLORED TO FOGGED CASE
		{
			fow_tile_rect = App->fow_manager->GetFowTileRect(FOGGED);																		// First a FOGGED tile is printed.
			App->render->Blit(App->fow_manager->fow_tex, world_position.x - 5, world_position.y - 19, &fow_tile_rect);

			smoothing_state = smoothing_state - ((uchar)FOW_SMOOTHING_STATE::UNXTOFOG_TOP - (uchar)FOW_SMOOTHING_STATE::UNEXPLORED_TOP);	// Gets the corresponding UNEXPLORED smoothing state.

			fow_tile_rect = App->fow_manager->GetFowTileRect(smoothing_state);																// Gets the correct UNEXPLORED sprite section.
			App->render->Blit(App->fow_manager->fow_tex, world_position.x - 5, world_position.y - 19, &fow_tile_rect);

			return;
		}
		else
		{
			if (smoothing_state == (uchar)FOW_SMOOTHING_STATE::NONE)																		// If this is not a smoothing case.
			{
				fow_tile_rect = App->fow_manager->GetFowTileRect(FOGGED);
				App->render->Blit(App->fow_manager->fow_tex, world_position.x - 5, world_position.y - 19, &fow_tile_rect);
				return;
			}
			else
			{
				fow_tile_rect = App->fow_manager->GetFowTileRect(smoothing_state);															// Gets the correct FOGGED smoothing sprite section.
				App->render->Blit(App->fow_manager->fow_tex, world_position.x - 5, world_position.y - 19, &fow_tile_rect);
				return;
			}
		}	
	}
	else if (fow_state == UNEXPLORED)																										// If this is an UNEXPLORED case.
	{
		fow_tile_rect = App->fow_manager->GetFowTileRect(UNEXPLORED);
		App->render->Blit(App->fow_manager->fow_tex, world_position.x - 5, world_position.y - 19, &fow_tile_rect);
		return;
	}
	else
	{
		return;
	}*/
}

void Map::DataMapDebug()
{
	if (walkability_debug || entity_map_debug)
	{
		for (int x = min_x_row; x < max_x_row && x < data.width; x++)
		{
			for (int y = min_y_row; y < max_y_row && y < data.height && MapToWorld(x, y).y < bottom_right_y && MapToWorld(x, y).x > camera_pos_in_pixels.x - data.tile_width; y++)
			{
				if (MapToWorld(x, y).y > camera_pos_in_pixels.y - data.tile_height * 2 && MapToWorld(x, y).x < bottom_right_x)
				{
					if (walkability_debug)
					{
						if (App->pathfinding->IsOccupied(iPoint(x, y)))
						{
							iPoint draw_position = App->map->MapToWorld(x, y);
							App->render->Blit(App->scene_manager->current_scene->occupied_debug, draw_position.x, draw_position.y, nullptr);
						}
					}

					if (entity_map_debug)
					{
						if (App->entity_manager->entity_map[(y * App->map->data.width) + x] != nullptr)
						{
							iPoint draw_position = App->map->MapToWorld(x, y);
							App->render->Blit(App->scene_manager->current_scene->occupied_by_entity_debug, draw_position.x, draw_position.y, nullptr);
						}
					}
				}
			}
		}
	}
}


iPoint Map::MapToWorld(int x, int y) const 
{
	iPoint ret(0, 0);																//Position of the tile on the world.

	if (data.type == MAP_TYPE::ORTHOGONAL)											//Position calculus for orhogonal maps
	{
		ret.x = x * data.tile_width;												//Position in the X axis of the tile on the world in pixels. For tile_width = 32 --> Tile 1: x = 0, Tile 2: x = 32.
		ret.y = y * data.tile_height;												//Position in the Y axis of the tile on the world in pixels. For tile_height = 32 --> Tile 1: y = 0, Tile 2: y = 32.
	}
	else if (data.type == MAP_TYPE::ISOMETRIC)										//Position calculus for isometric maps
	{
		ret.x = (int)((x - y) * (data.tile_width * 0.5f));
		ret.y = (int)((x + y) * (data.tile_height * 0.5f));
	}
	else
	{
		//LOG("Unknown map type");
		ret.x = x; ret.y = y;
	}

	return ret;
}

iPoint Map::WorldToMap(int x, int y) const
{
	iPoint ret(0, 0);

	if (data.type == MAP_TYPE::ORTHOGONAL)
	{
		ret.x = x / data.tile_width;
		ret.y = y / data.tile_height;
	}
	else if (data.type == MAP_TYPE::ISOMETRIC)
	{
		float half_width = data.tile_width * 0.5f;
		float half_height = data.tile_height * 0.5f;
		ret.x = int((x / half_width + y / half_height) * 0.5f) - 1;				//THIS
		ret.y = int((y / half_height - (x / half_width)) *0.5f);
	}
	else
	{
		//LOG("Unknown map type");
		ret.x = x; ret.y = y;
	}

	return ret;
}

iPoint Map::TiledIsoCoordsToMap(int x, int y) const 
{
	iPoint ret(0, 0);

	//Yup. It is that dumb. Tiled Iso maps have their coords along the tile's axis, not aligned to the screen.
	ret.x = x / 40;
	ret.y = y / 40;

	return ret;
}

SDL_Rect TileSet::GetTileRect(uint tile_id) const
{
	SDL_Rect tile_rect;						

	int relative_id = tile_id - firstgid;	//Calculates the relative position of the tile_id respect the  first initial global id.

	tile_rect.w = tile_width;				
	tile_rect.h = tile_height;				
	tile_rect.x = margin + ((tile_rect.w + spacing) * (relative_id % num_tiles_width));
	tile_rect.y = margin + ((tile_rect.h + spacing) * (relative_id / num_tiles_width));

	return tile_rect;
}

TileSet* Map::GetTilesetFromTileId(int id) 				
{
	std::vector<TileSet*>::iterator tileset_item = data.tilesets.begin();

	TileSet* ret = (*tileset_item);

	for(tileset_item; tileset_item != data.tilesets.end(); ++tileset_item)
	{
		if (id >=  (*tileset_item)->firstgid)
		{
			ret = (*tileset_item);
		}
	}
	return ret;
}

// Called before quitting
bool Map::CleanUp()
{
	LOG("Unloading map");

	// Remove all tilesets from memory
	std::vector<TileSet*>::iterator item = data.tilesets.begin();

	for (; item != data.tilesets.end(); ++item)
	{	
		SDL_DestroyTexture((*item)->texture);
		RELEASE((*item));
	}
	data.tilesets.clear();

	// Remove all layers from memory
	std::vector<MapLayer*>::iterator map_layer_item = data.layers.begin();

	for (; map_layer_item != data.layers.end(); ++map_layer_item)
	{
		RELEASE((*map_layer_item));
	}
	data.layers.clear();

	//Removing all Objects from memory
	std::vector<ObjectGroup*>::iterator object_layer_item = data.objectGroups.begin();

	for (; object_layer_item != data.objectGroups.end() ; ++object_layer_item)
	{	
		delete[] (*object_layer_item)->object;		//Frees the memory allocated to the object array. LoadObjectLayers() line 544.

		RELEASE((*object_layer_item));				//RELEASE frees all memory allocated for a list tileset_item. All declared news that were added to the list will be deleted here.
	}
	data.objectGroups.clear();
	
	////Removing all data of all layers from memory.
	//data.layers.clear();		//Deletes from memory all the items inside the layers list.
	//data = { 0 };				//Frees all memory allocated to the data struct.

	// Clean up the pugui tree
	map_file.reset();

	return true;
}

// Load new map
bool Map::Load(std::string file_name)
{
	BROFILER_CATEGORY("Load Map", Profiler::Color::Khaki);
	bool ret = true;
	std::string tmp = folder + file_name;

	pugi::xml_parse_result result = map_file.load_file(tmp.c_str());

	if(result == NULL)
	{
		LOG("Could not load map xml file %s. pugi error: %s", file_name, result.description());
		ret = false;
	}

	// Load general info ----------------------------------------------
	if(ret == true)
	{
		ret = LoadMap();
	}

	// Load all tilesets info ----------------------------------------------
	pugi::xml_node tileset;
	int i = 0;
	for(tileset = map_file.child("map").child("tileset"); tileset && ret; tileset = tileset.next_sibling("tileset"))
	{
		
		TileSet* set = new TileSet();

		if(ret == true)
		{
			ret = LoadTilesetDetails(tileset, set);
		}

		if(ret == true)
		{
			ret = LoadTilesetImage(tileset, set);
		}

		data.tilesets.push_back(set);
		i++;
		LOG("LOADING TILESET %d", i);
	}

	// Load layer info ----------------------------------------------
	pugi::xml_node layer;
	for (layer = map_file.child("map").child("layer"); layer; layer = layer.next_sibling("layer"))
	{
		
		MapLayer* set_layer = new MapLayer();

		if (ret == true)
		{
			ret = LoadLayer(layer, set_layer);
		}

		data.layers.push_back(set_layer);
	}
	//Entity and Walkability maps are loaded here. Previously in GameScene.
	LoadMetaDataMaps();

	//Load Object / ObjectGroup / Collider Info ------------------------------------------
	pugi::xml_node objectgroup;
	int k = 0;
	for (objectgroup = map_file.child("map").child("objectgroup"); objectgroup && ret; objectgroup = objectgroup.next_sibling("objectgroup"))
	{
		ObjectGroup* new_objectgroup = new ObjectGroup();									

		if (ret == true)
		{
			ret = LoadObjectLayers(objectgroup, new_objectgroup);
		}
		data.objectGroups.push_back(new_objectgroup);		
		k++;
		LOG("LOADED OBJECTS %d", k);
	}

	//--------------------------- LOGS ----------------------------
	if(ret == true)
	{
		LOG("Successfully parsed map XML file: %s", file_name.c_str());
		LOG("width: %d height: %d", data.width, data.height);
		LOG("tile_width: %d tile_height: %d", data.tile_width, data.tile_height);


		for(std::vector<TileSet*>::iterator tileset_item = data.tilesets.begin(); tileset_item != data.tilesets.end(); ++tileset_item)
		{
			TileSet* tileset = (*tileset_item);
			LOG("Tileset ----");
			LOG("name: %s firstgid: %d", tileset->name.c_str(), tileset->firstgid);
			LOG("tile width: %d tile height: %d", tileset->tile_width, tileset->tile_height);
			LOG("spacing: %d margin: %d", tileset->spacing, tileset->margin);
		}

		for (std::vector<MapLayer*>::iterator map_layer_item = data.layers.begin(); map_layer_item != data.layers.end(); map_layer_item++)
		{
			MapLayer* layer = (*map_layer_item);
			LOG("Layer ----");
			LOG("name: %s", layer->name.c_str());
			LOG("layer width: %d layer height: %d", layer->width, layer->height);
			LOG("parallax speed: %f", layer->speed);
		}

		for (std::vector<ObjectGroup*>::iterator object_layer_item = data.objectGroups.begin(); object_layer_item != data.objectGroups.end(); object_layer_item++)
		{
			ObjectGroup* object = (*object_layer_item);
			LOG("Group ----");
			LOG("Gname: %s", object->name.c_str());
		}
	}
	map_loaded = ret;

	return ret;
}

// Load map general properties
bool Map::LoadMap()
{
	bool ret = true;
	pugi::xml_node map = map_file.child("map");

	if(map == NULL)
	{
		LOG("Error parsing map xml file: Cannot find 'map' tag.");
		ret = false;
	}
	else
	{
		data.width			= map.attribute("width").as_int();
		data.height			= map.attribute("height").as_int();
		data.tile_width		= map.attribute("tilewidth").as_int();
		data.tile_height	= map.attribute("tileheight").as_int();
		data.music_File		= map.child("properties").child("property").attribute("value").as_string();				//THIS HERE

		std::string bg_color = (map.attribute("backgroundcolor").as_string());

		data.background_color.r = 126;
		data.background_color.g = 0;
		data.background_color.b = 126;
		data.background_color.a = 255;

		if (bg_color.length() > 0)
		{
			std::string red, green, blue;
			red = bg_color.substr(1, 2);
			green = bg_color.substr(3, 4);
			blue = bg_color.substr(5, 6);

			int v = 0;

			sscanf_s(red.c_str(), "%x", &v);
			if (v >= 0 && v <= 255) data.background_color.r = v;

			sscanf_s(green.c_str(), "%x", &v);
			if (v >= 0 && v <= 255) data.background_color.g = v;

			sscanf_s(blue.c_str(), "%x", &v);
			if (v >= 0 && v <= 255) data.background_color.b = v;
		}

		std::string orientation = (map.attribute("orientation").as_string());

		if(orientation == "orthogonal")
		{
			data.type = MAP_TYPE::ORTHOGONAL;
		}
		else if(orientation == "isometric")
		{
			data.type = MAP_TYPE::ISOMETRIC;
		}
		else if(orientation == "staggered")
		{
			data.type = MAP_TYPE::STAGGERED;
		}
		else
		{
			data.type = MAP_TYPE::UNKNOWN;
		}
	}

	return ret;
}

bool Map::LoadTilesetDetails(pugi::xml_node& tileset_node, TileSet* set)
{
	bool ret = true;
	set->name = (tileset_node.attribute("name").as_string());
	set->firstgid		= tileset_node.attribute("firstgid").as_int();
	set->tile_width		= tileset_node.attribute("tilewidth").as_int();
	set->tile_height	= tileset_node.attribute("tileheight").as_int();
	set->margin			= tileset_node.attribute("margin").as_int();
	set->spacing		= tileset_node.attribute("spacing").as_int();

	pugi::xml_node offset = tileset_node.child("tileoffset");

	if(offset != nullptr)
	{
		set->offset_x = offset.attribute("x").as_int();
		set->offset_y = offset.attribute("y").as_int();
	}
	else
	{
		set->offset_x = 0;
		set->offset_y = 0;
	}

	return ret;
}

bool Map::LoadTilesetImage(pugi::xml_node& tileset_node, TileSet* set)
{
	bool ret = true;
	pugi::xml_node image = tileset_node.child("image");

	if(image == NULL)
	{
		LOG("Error parsing tileset xml file: Cannot find 'image' tag.");
		ret = false;
	}
	else
	{
		set->texture = App->tex->Load(PATH(folder.c_str(), image.attribute("source").as_string()));
		int w, h;
		SDL_QueryTexture(set->texture, NULL, NULL, &w, &h);
		set->tex_width = image.attribute("width").as_int();

		if(set->tex_width <= 0)
		{
			set->tex_width = w;
		}

		set->tex_height = image.attribute("height").as_int();

		if(set->tex_height <= 0)
		{
			set->tex_height = h;
		}

		set->num_tiles_width = set->tex_width / set->tile_width;
		set->num_tiles_height = set->tex_height / set->tile_height;
	}

	return ret;
}

bool Map::LoadLayer(pugi::xml_node& node, MapLayer* layer)
{
	bool ret = true;

	layer->name			= node.attribute("name").as_string();
	layer->width		= node.attribute("width").as_int();
	layer->height		= node.attribute("height").as_int();
	layer->speed		= node.child("properties").child("property").attribute("value").as_float();		
	LoadProperties(node, layer->properties);															
	pugi::xml_node layer_data = node.child("data");

	SDL_Rect quad_tile_rect;

	switch (data.type)
	{
	case(MAP_TYPE::ORTHOGONAL):

		break;

	case(MAP_TYPE::ISOMETRIC):

		quad_tile_rect.x = MapToWorld(0, data.height).x;
		quad_tile_rect.y = 0;

		quad_tile_rect.w = MapToWorld(data.width, 0).x - MapToWorld(0, data.height).x;
		quad_tile_rect.h = MapToWorld(data.width, data.height).y;

		break;
	}

	if (layer_data == NULL)
	{
		LOG("Error parsing map xml file: Cannot find 'layer/data' tag.");
		ret = false;
		RELEASE(layer);
	}
	else
	{
		layer->gid = new uchar[layer->width * layer->height];
		memset(layer->gid, 0, layer->width * layer->height);

		int i = 0;
		int j = 0;
		
		for (pugi::xml_node tile = layer_data.child("tile"); tile; tile = tile.next_sibling("tile"), i++)
		{
			layer->gid[i] = tile.attribute("gid").as_int(0);
		}
	}

	return ret;
}

void Map::LoadMetaDataMaps() 
{
	//Walkability map here.
	int w, h;
	uchar* walkability_data = NULL;
	if (App->map->CreateWalkabilityMap(w, h, &walkability_data))
	{
		App->pathfinding->SetMap(w, h, walkability_data);						//Sets a new walkability map with the map passed by CreateWalkabilityMap().
	}

	RELEASE_ARRAY(walkability_data);											//Frees all memory allocated to the data array.

	App->entity_manager->SetEntityMap(data.width, data.height);

	App->fow_manager->SetVisibilityMap(data.width, data.height);				//
}

//Loads the object layers (colliders) from the xml map. It iterates through  a specific object layer (in the load() it is iterated through to get all the object info).
bool Map::LoadObjectLayers(pugi::xml_node& node, ObjectGroup * objectgroup)
{
	bool ret = true;

	objectgroup->id		= node.attribute("id").as_int();							
	objectgroup->name	= node.attribute("name").as_string();						

	int object_count = 0;
	//This loop iterates all the childs with the object tag.
	//With each iteration of the loop one object is added to the count. 
	//Used to reduce memory space waste.
	for (pugi::xml_node object_iterator = node.child("object"); object_iterator; object_iterator = object_iterator.next_sibling("object"))	
	{
		object_count++;
	}

	objectgroup->num_objects = object_count;										
	objectgroup->object = new ObjectData[object_count];								
	memset(objectgroup->object, 0, object_count * sizeof(ObjectData));				//Set allocated memory to 0;

	int index = 0;
	for (pugi::xml_node object_iterator = node.child("object"); object_iterator; object_iterator = object_iterator.next_sibling("object"))
	{
		objectgroup->object[index].id		= object_iterator.attribute("id").as_int();			
		objectgroup->object[index].name		= object_iterator.attribute("name").as_string();	

		SDL_Rect* collider = new SDL_Rect();

		collider->x = object_iterator.attribute("x").as_int();						
		collider->y = object_iterator.attribute("y").as_int();						
		collider->w = object_iterator.attribute("width").as_int();					
		collider->h = object_iterator.attribute("height").as_int();					

		objectgroup->object[index].collider = collider;								

		std::string object_type = (object_iterator.attribute("type").as_string());	


		iPoint tile_coords = TiledIsoCoordsToMap(collider->x = object_iterator.attribute("x").as_int(), collider->x = object_iterator.attribute("y").as_int());

		//Checking the object type string being loaded from the tmx file. It's a string that's abitrarily set on Tiled
		if (object_type == "enemy")
		{
			objectgroup->object[index].type = ENEMY;
			App->entity_manager->CreateEntity(ENTITY_TYPE::ENEMY_INFANTRY, tile_coords.x, tile_coords.y);
		}
		else if (object_type == "enemy_barracks")
		{
			objectgroup->object[index].type = ENEMY_BARRACKS;
			App->entity_manager->CreateEntity(ENTITY_TYPE::ENEMY_BARRACKS, tile_coords.x, tile_coords.y);
		}
		else if (object_type == "enemy_hall")
		{
			objectgroup->object[index].type = ENEMY_HALL;
			App->entity_manager->CreateEntity(ENTITY_TYPE::ENEMY_TOWNHALL, tile_coords.x, tile_coords.y);
		}
		else if (object_type == "infantry")
		{
			objectgroup->object[index].type = INFANTRY;
			App->entity_manager->CreateEntity(ENTITY_TYPE::INFANTRY, tile_coords.x, tile_coords.y);
		}
		else if (object_type == "gatherer")
		{
			objectgroup->object[index].type = GATHERER;
			App->entity_manager->CreateEntity(ENTITY_TYPE::GATHERER, tile_coords.x, tile_coords.y);
		}
		else if (object_type == "hall")
		{
			objectgroup->object[index].type = HALL;
			App->entity_manager->CreateEntity(ENTITY_TYPE::TOWNHALL, tile_coords.x, tile_coords.y);
		}
		else if (object_type == "barracks")
		{
			objectgroup->object[index].type = BARRACKS;
			App->entity_manager->CreateEntity(ENTITY_TYPE::BARRACKS, tile_coords.x, tile_coords.y);
		}
		else if (object_type == "rock")
		{
			objectgroup->object[index].type = ROCK;
			App->entity_manager->CreateEntity(ENTITY_TYPE::ROCK, tile_coords.x, tile_coords.y);
		}
		else if (object_type == "tree")
		{
			objectgroup->object[index].type = TREE;
			App->entity_manager->CreateEntity(ENTITY_TYPE::TREE, tile_coords.x, tile_coords.y);
		}
		else if (object_type == "bit")
		{
			objectgroup->object[index].type = BITS;
			App->entity_manager->CreateEntity(ENTITY_TYPE::BITS, tile_coords.x, tile_coords.y);
		}

		index++;
	}

	return ret;
}

bool Map::LoadProperties(pugi::xml_node& node, Properties& properties)							//REVISE THIS HERE. Check why it crashes the game at exit time.
{
	bool ret = false;

	pugi::xml_node data = node.child("properties");					

	if (data != nullptr)
	{
		pugi::xml_node prop;
		
		for (prop = data.child("property"); prop; prop = prop.next_sibling("property"))		
		{
			Properties::Property* p = new Properties::Property();									

			p->name				= prop.attribute("name").as_string();
			p->intValue			= prop.attribute("value").as_int();	

			properties.property_list.push_back(p);
		}
	}

	return ret;
}

bool Map::CheckMapBoundaries(const iPoint& tile_position)
{
	return (tile_position.x >= 0 && tile_position.x < (int)data.width &&
			tile_position.y >= 0 && tile_position.y < (int)data.height);
}

bool Map::CreateWalkabilityMap(int& width, int& height, uchar** buffer)
{
	bool ret = false;

	std::vector<MapLayer*>::iterator item = data.layers.begin();

	for (; item != data.layers.end() ; ++item)
	{
		MapLayer* layer = (*item);

		//if (layer->properties.Get("walkability", 0) == 0)							
		//{
		//	continue;																//If the value mentioned above is 0, jump to the next iteration (of layers list).
		//}
		
		if ( layer->name != "walkability")
		{
			continue;
		}

		uchar* map = new uchar[layer->width * layer->height];						//Allocates memory for all the tiles in the map.
		memset(map, 1, layer->width*layer->height);									//Sets all memory allocated for map to 1. 

		for (int y = 0; y < data.height; ++y)										
		{
			for (int x = 0; x < data.width; ++x)									
			{
				int index = (y * layer->width) + x;									

				int tile_id = layer->Get(x, y);	

				if (tile_id > 0)													//skips empty tiles
				{
					TileSet* tileset = GetTilesetFromTileId(tile_id);

					if (tileset != nullptr)
					{
						//map[index] = (tile_id - tileset->firstgid);							//get the id of the tile we assigned: 0 = walkable, 1 = non walkable...

						if ((tile_id - tileset->firstgid) > 0)
						{
							map[index] = 0;
						}
						else
						{
							map[index] = 1;
						}
					}
				}
			}
		}

		*buffer		= map;															//Sets buffer as the map array.												
		width		= data.width;													//Sets width to the width of the map in tiles.
		height		= data.height;													//Sets height to the height of the map in tiles. 
		ret			= true;															//Sets the ret bool to true as the walkability map could be created
	}

	return ret;
}

bool Map::CreateEntityMap(int& width, int& height)
{
	bool ret = false;
	
	Entity** map = new Entity*[data.width * data.height];
	//memset(map, 1, data.width * data.height);							//THIS HERE

	for (int y = 0; y < data.height; ++y)
	{
		for (int x = 0; x < data.width; ++x)
		{
			int index = (y * data.width) + x;

			map[index] = nullptr;
		}
	}

	//RELEASE_ARRAY(map);												//THIS HERE

	width	= data.width;
	height	= data.height;
	ret		= true;

	return ret;
}

bool Map::CreateVisibilityMap(int& width, int& height, uchar** buffer)
{
	bool ret = false;

	uchar* map = new uchar[data.width * data.height];
	memset(map, 0, data.width * data.height);								// The base state of the FOW tile is set to UNEXPLORED. (See FowManager's FOW_TILE_STATE enum class)

	/*for (int y = 0; y < data.height; ++y)
	{
		for (int x = 0; x < data.width; ++x)
		{
			int index = (y * data.width) + x;

			map[index] = 0;
		}
	}*/

	width		= data.width;
	height		= data.height;
	*buffer		= map;
	ret			= true;

	return ret;
}

void Map::Restart_Cam() // function that resets the camera
{
	App->render->camera.x = (int)spawn_position_cam.x;
	App->render->camera.y = (int)spawn_position_cam.y;
}

void Map::GetMapSize(int& w, int& h) const
{
	w = App->map->data.width * App->map->data.tile_width;
	h = App->map->data.height * App->map->data.tile_height;
}

void Map::DrawMapGrid()
{
	// TMP. Change Later to fit the tiles.
	
	int tile_offset_x = 2;
	int tile_offset_y = 19;
	
	// --- RIGHT TO LEFT ---
	iPoint origin;																									// Origin point of the grid's lines.
	origin.x = (int)(data.tile_width * 0.5f) - tile_offset_x;
	origin.y = (int)(data.tile_height * 0.5f) - tile_offset_y;

	/*origin.x = 0;
	origin.y = 0;*/

	iPoint end;																										//End point of the grid's lines
	end.x = -((int)(data.width * data.tile_width * 0.5f) + (int)(data.tile_width * 0.5f)) + data.tile_width - tile_offset_x;
	end.y = (int)(data.height * data.tile_height * 0.5f) + (int)(data.tile_height * 0.5f) - tile_offset_y;

	for (int i = 0; i <= data.width; ++i)
	{
		App->render->DrawLine(origin.x, origin.y, end.x, end.y, 255, 255, 255, 100);

		origin.x += (int)(data.tile_width * 0.5f);
		origin.y += (int)(data.tile_height * 0.5f);

		end.x += (int)(data.tile_width * 0.5f);
		end.y += (int)(data.tile_height * 0.5f);
	}

	// --- LEFT TO RIGHT ---
	origin.x = (int)(data.tile_width * 0.5f);
	origin.y = (int)(data.tile_height * 0.5f) - tile_offset_y;
	
	end.x = (int)(data.width * data.tile_width * 0.5f) + (int)(data.tile_width * 0.5f);
	end.y = (int)(data.height * data.tile_height * 0.5f) + (int)(data.tile_height * 0.5f) - tile_offset_y;

	for (int i = 0; i <= data.height; ++i)
	{
		App->render->DrawLine(origin.x, origin.y, end.x, end.y, 255, 255, 255, 100);

		origin.x -= (int)(data.tile_width * 0.5f);
		origin.y += (int)(data.tile_height * 0.5f);

		end.x -= (int)(data.tile_width * 0.5f);
		end.y += (int)(data.tile_height * 0.5f);
	}
}

int Properties::Get(std::string name, int default_value)							//Revise how to be able to not have a property without default value being nullptr.
{
	std::vector<Property*>::iterator item = property_list.begin();

	for (; item != property_list.end(); item++)
	{
		if ((*item)->name == name)
		{
			return (*item)->intValue;
		}
	}
	return default_value;															//Default value is 0
}

MapLayer::MapLayer() : gid(NULL)
{
	height = 0;
	size = 0;
	speed = 0;
	width = 0;
}

MapLayer::~MapLayer()
{
	RELEASE(gid);		//Breaks with mmgr.
}