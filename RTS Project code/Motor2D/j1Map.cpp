#include "p2Defs.h"
#include "p2Log.h"
#include "p2Point.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Map.h"
#include "j1Window.h"
#include "j1Collisions.h"
#include "j1FadeScene.h"
#include "j1Input.h"
#include "j1Audio.h"
#include "j1Pathfinding.h"
#include "j1Scene.h"
#include "j1EntityManager.h"
#include "j1Console.h"
#include "Brofiler\Brofiler.h"
#include <math.h>

//#include "mmgr/mmgr.h"

j1Map::j1Map() : j1Module(), map_loaded(false)
{
	name = ("map");
}

// Destructor
j1Map::~j1Map()
{}

// Called before render is available
bool j1Map::Awake(pugi::xml_node& config)
{
	LOG("Loading Map Parser");
	bool ret = true;

	folder = (config.child("folder").child_value());

	return ret;
}

void j1Map::Draw()
{
	BROFILER_CATEGORY("Draw Map", Profiler::Color::Khaki);
	if (map_loaded == false)																						//If no map was loaded, return.
	{
		return;
	}

	int tiles_drawn = 0;

	App->win->GetWindowSize(winWidth, winHeight);																	//Gets the window size so it can be added to the camera collider as parameters.

	for (std::list<MapLayer*>::iterator layer = data.layers.begin(); layer != data.layers.end(); layer++)																	
	{
		camera_pos_in_pixels_x = -App->render->camera.x ;
		camera_pos_in_pixels_y = -App->render->camera.y ;

		bottom_right_x = camera_pos_in_pixels_x + winWidth;
		bottom_right_y = camera_pos_in_pixels_y + winHeight;

		top_left_x_row = WorldToMap(camera_pos_in_pixels_x, camera_pos_in_pixels_y).x;

		bottom_right_x_row = WorldToMap(bottom_right_x + data.tile_width , bottom_right_y ).x;

		top_right_y_row = WorldToMap(bottom_right_x, camera_pos_in_pixels_y).y; //Esquina dereche arriba
		bottom_left_y_row = WorldToMap(camera_pos_in_pixels_x, bottom_right_y + data.tile_height).y; //Esquina izquierda abajo

		//LOG("min x row %d, max x row %d", top_left_x_row, bottom_right_x_row);
		//LOG("min y row %d, max y row %d", top_right_y_row, bottom_left_y_row);
		
		if (top_left_x_row < 0)
		{
			top_left_x_row = 0;
		}

		if (top_right_y_row < 0)
		{
			top_right_y_row = 0;
		}

		for (int x = top_left_x_row ; x < bottom_right_x_row && x < data.width; x++)
		{
			for (int y = top_right_y_row ; y < bottom_left_y_row && y < data.height && MapToWorld(x, y).y < bottom_right_y && MapToWorld(x, y).x > camera_pos_in_pixels_x - data.tile_width; y++)
			{
				if (MapToWorld(x, y).y > camera_pos_in_pixels_y - data.tile_height * 2 && MapToWorld(x, y).x < bottom_right_x)
				{
					int tile_id = (*layer)->Get(x, y);																//Gets the tile id from the tile index. Gets the tile index for a given tile. x + y * data.tile_width;

					if (tile_id > 0)																				
					{
						TileSet* tileset = GetTilesetFromTileId(tile_id);												//Gets the tileset corresponding with the tile_id. If tile id is 34 and the current tileset first gid is 35, that means that the current  tile belongs to the previous tileset. 
						if (tileset != nullptr)																			
						{
							SDL_Rect tile_rect = tileset->GetTileRect(tile_id);											//Gets the position on the world and the dimensions of the rect of the given tile_id 
							iPoint pos = MapToWorld(x, y);																//Gets the position on the world (in pixels) of a specific point (in tiles). In the case of orthogonal maps the x and y are multiplied by the tile's width  or height. If 32x32, Map pos: x = 1 --> World pos: x = 32...

							App->render->Blit(tileset->texture, pos.x + tileset->offset_x, pos.y + tileset->offset_y, &tile_rect); //, false, (*layer)->speed)

							tiles_drawn++;
						}
					}
				}
			}
		}
	}
	//LOG("Tiles drawn: %d", tiles_drawn);
}

iPoint j1Map::MapToWorld(int x, int y) const 
{
	iPoint ret(0, 0);																//Position of the tile on the world.

	if (data.type == MAPTYPE_ORTHOGONAL)											//Position calculus for orhogonal maps
	{
		ret.x = x * data.tile_width;												//Position in the X axis of the tile on the world in pixels. For tile_width = 32 --> Tile 1: x = 0, Tile 2: x = 32.
		ret.y = y * data.tile_height;												//Position in the Y axis of the tile on the world in pixels. For tile_height = 32 --> Tile 1: y = 0, Tile 2: y = 32.
	}
	else if (data.type == MAPTYPE_ISOMETRIC)										//Position calculus for isometric maps
	{
		ret.x = (x - y) * (data.tile_width * 0.5);
		ret.y = (x + y) * (data.tile_height * 0.5);
	}
	else
	{
		LOG("Unknown map type");
		ret.x = x; ret.y = y;
	}

	return ret;
}

iPoint j1Map::WorldToMap(int x, int y) const
{
	iPoint ret(0, 0);

	if (data.type == MAPTYPE_ORTHOGONAL)
	{
		ret.x = x / data.tile_width;
		ret.y = y / data.tile_height;
	}
	else if (data.type == MAPTYPE_ISOMETRIC)
	{
		float half_width = data.tile_width * 0.5f;
		float half_height = data.tile_height * 0.5f;
		ret.x = int((x / half_width + y / half_height) / 2) - 1;
		ret.y = int((y / half_height - (x / half_width)) / 2);
	}
	else
	{
		LOG("Unknown map type");
		ret.x = x; ret.y = y;
	}

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

TileSet* j1Map::GetTilesetFromTileId(int id) 				
{
	std::list<TileSet*>::iterator tilesetIter = data.tilesets.begin();

	TileSet* ret = (*tilesetIter);

	for(tilesetIter; tilesetIter != data.tilesets.end(); tilesetIter++)
	{
		if (id >=  (*tilesetIter)->firstgid)
		{
			ret = (*tilesetIter);
		}
	}
	return ret;
}

// Called before quitting
bool j1Map::CleanUp()
{
	LOG("Unloading map");

	// Remove all tilesets from memory
	for (std::list<TileSet*>::iterator item = data.tilesets.begin() ; item != data.tilesets.end(); item++)
	{	
		SDL_DestroyTexture((*item)->texture);
		RELEASE((*item));
	}
	data.tilesets.clear();

	// Remove all layers from memory
	for (std::list<MapLayer*>::iterator map_layer_item = data.layers.begin(); map_layer_item != data.layers.end(); map_layer_item++)
	{
		RELEASE((*map_layer_item));
	}
	data.layers.clear();

	//Removing all Objects from memory
	for (std::list<ObjectGroup*>::iterator object_iterator = data.objectGroups.begin() ; object_iterator != data.objectGroups.end() ; object_iterator++)
	{	
		delete[] (*object_iterator)->object;		//Frees the memory allocated to the object array. LoadObjectLayers() line 544.

		RELEASE((*object_iterator));				//RELEASE frees all memory allocated for a list item. All declared news that were added to the list will be deleted here.
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
bool j1Map::Load(std::string file_name)
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

	//Load Object / ObjectGroup / Collider Info ------------------------------------------
	pugi::xml_node objectgroup;
	for (objectgroup = map_file.child("map").child("objectgroup"); objectgroup && ret; objectgroup = objectgroup.next_sibling("objectgroup"))
	{
		ObjectGroup* new_objectgroup = new ObjectGroup();									//Allocates memory for the objectgroup being iterated.

		if (ret == true)
		{
			ret = LoadObjectLayers(objectgroup, new_objectgroup);							//Loads the data members of the objectgroup that is being iterated.
			
			if (new_objectgroup->name.c_str() == "Coins")
			{
				LOG("Creating coins from object layer %s", new_objectgroup->name.c_str());
			}
		}
		data.objectGroups.push_back(new_objectgroup);										//Adds the object group being iterated to the list.
	}

	//--------------------------- LOGS ----------------------------
	if(ret == true)
	{
		LOG("Successfully parsed map XML file: %s", file_name.c_str());
		LOG("width: %d height: %d", data.width, data.height);
		LOG("tile_width: %d tile_height: %d", data.tile_width, data.tile_height);

		
		for(std::list<TileSet*>::iterator item = data.tilesets.begin() ; item != data.tilesets.end() ; item++)
		{
			TileSet* tileset = (*item);
			LOG("Tileset ----");
			LOG("name: %s firstgid: %d", tileset->name.c_str(), tileset->firstgid);
			LOG("tile width: %d tile height: %d", tileset->tile_width, tileset->tile_height);
			LOG("spacing: %d margin: %d", tileset->spacing, tileset->margin);
		}

		for (std::list<MapLayer*>::iterator item_layer = data.layers.begin(); item_layer != data.layers.end(); item_layer++)
		{
			MapLayer* layer = (*item_layer);
			LOG("Layer ----");
			LOG("name: %s", layer->name.c_str());
			LOG("layer width: %d layer height: %d", layer->width, layer->height);
			LOG("parallax speed: %f", layer->speed);
		}

		for (std::list<ObjectGroup*>::iterator obj_layer = data.objectGroups.begin(); obj_layer != data.objectGroups.end(); obj_layer++)
		{
			ObjectGroup* object = (*obj_layer);
			LOG("Group ----");
			LOG("Gname: %s", object->name.c_str());
		}
	}
	map_loaded = ret;

	return ret;
}

// Load map general properties
bool j1Map::LoadMap()
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
		data.music_File		= map.child("properties").child("property").attribute("value").as_string();

		std::string bg_color = (map.attribute("backgroundcolor").as_string());

		data.background_color.r = 0;
		data.background_color.g = 0;
		data.background_color.b = 0;
		data.background_color.a = 0;

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
			data.type = MAPTYPE_ORTHOGONAL;
		}
		else if(orientation == "isometric")
		{
			data.type = MAPTYPE_ISOMETRIC;
		}
		else if(orientation == "staggered")
		{
			data.type = MAPTYPE_STAGGERED;
		}
		else
		{
			data.type = MAPTYPE_UNKNOWN;
		}
	}

	return ret;
}

bool j1Map::LoadTilesetDetails(pugi::xml_node& tileset_node, TileSet* set)
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

bool j1Map::LoadTilesetImage(pugi::xml_node& tileset_node, TileSet* set)
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

bool j1Map::LoadLayer(pugi::xml_node& node, MapLayer* layer)
{
	bool ret = true;

	layer->name			= node.attribute("name").as_string();
	layer->width		= node.attribute("width").as_int();
	layer->height		= node.attribute("height").as_int();
	layer->speed		= node.child("properties").child("property").attribute("value").as_float();		//Gets the speed property of a layer.
	LoadProperties(node, layer->properties);															//Loads the layer's properties and stores them in a list (property_list) //REVISE THIS HERE
	
	pugi::xml_node layer_data = node.child("data");

	if (layer_data == NULL)
	{
		LOG("Error parsing map xml file: Cannot find 'layer/data' tag.");
		ret = false;
		RELEASE(layer);
	}
	else
	{
		layer->gid = new uint[layer->width*layer->height];
		memset(layer->gid, 0, layer->width*layer->height);

		int i = 0;
		for (pugi::xml_node tile = layer_data.child("tile") ; tile; tile = tile.next_sibling("tile"), i++)
		{
			layer->gid[i] = tile.attribute("gid").as_int(0);
		}
	}

	return ret;
}

//Loads the object layers (colliders) from the xml map. It iterates through  a specific object layer (in the load() it is iterated through to get all the object info).
bool j1Map::LoadObjectLayers(pugi::xml_node& node, ObjectGroup * objectgroup)
{
	bool ret = true;

	objectgroup->id		= node.attribute("id").as_int();							//Sets the id of a given objectgroup to the id loaded from the tmx map.
	objectgroup->name	= node.attribute("name").as_string();						//Sets the name of a given objectgroup to the name loaded from the tmx map.

	int object_count = 0;
	//This loop iterates all the childs with the object tag.
	//With each iteration of the loop one object is added to the count. 
	//Used to reduce memory space waste.
	for (pugi::xml_node object_iterator = node.child("object"); object_iterator; object_iterator = object_iterator.next_sibling("object"))	//Iterates the data members inside the object tag and stops when the pointer is NULL(there are no objects left to iterate) or if ret == false.
	{
		object_count++;
	}

	objectgroup->num_objects = object_count;										//Sets the value of num_objects to the value gotten from iteration all the objectgroups.
	objectgroup->object = new ObjectData[object_count];								//Individually allocates memory for each object. Here object_count is the exact number of objects all objectgroups contain, so there is no memory waste.
	memset(objectgroup->object, 0, object_count * sizeof(ObjectData));				//Sets all allocated memory to 0;

	int index = 0;
	for (pugi::xml_node object_iterator = node.child("object"); object_iterator; object_iterator = object_iterator.next_sibling("object")) //Iterates again all objects passed in the tmx map.
	{
		objectgroup->object[index].id		= object_iterator.attribute("id").as_int();			//Gets the id of the object being loaded from tmx and sets it to the corresponding object in the world.
		objectgroup->object[index].name		= object_iterator.attribute("name").as_string();	//Gets the name of the object being loaded from tmx and sets it to the corresponding object in the world.

		SDL_Rect* collider = new SDL_Rect();

		collider->x = object_iterator.attribute("x").as_int();						//Sets the buffer rect's x position to the x position of the object given by the tmx map this iteration.
		collider->y = object_iterator.attribute("y").as_int();						//Sets the buffer rect's y position to the y position of the object given by the tmx map this iteration.
		collider->w = object_iterator.attribute("width").as_int();					//Sets the buffer rect's width to the width of the object given by the tmx map this iteration.
		collider->h = object_iterator.attribute("height").as_int();					//Sets the buffer rect's height to the height of the object given by the tmx map this iteration.

		objectgroup->object[index].collider = collider;								//Passes the buffer rect's data members to the object in this index position. Need to use a buffer due to objectgroup only accepting a class expression.

		std::string object_type = (object_iterator.attribute("type").as_string());	//Buffer string that improves readability of the code.

		//Checking the object type string being loaded from the tmx file. It's a string that's abitrarily set on Tiled, so it should be known exactly which type strings will be passed. 
		if (object_type == "solid")
		{
			objectgroup->object[index].type = SOLID;								//As the object type string matches "solid" the object's type will be set to SOLID.
		}
		else if (object_type == "platform")
		{
			objectgroup->object[index].type = PLATFORM;								//As the object type string matches "platform" the object's type will be set to PLATFORM.
		}
		else if (object_type == "hazard")
		{
			objectgroup->object[index].type = HAZARD;								//As the object type string matches "hazard" the object's type will be set to HAZARD.
		}
		else if (object_type == "item")
		{
			objectgroup->object[index].type = SWITCH;								//As the object type string matches "item" the object's type will be set to ITEM.
		}
		else if (object_type == "desactivable")
		{
			objectgroup->object[index].type = DEACTIVABLE;							//As the object type string matches "desactivable" the object's type will be set to DESACTIVABLE.
		}
		else if (object_type == "respawn")
		{
			objectgroup->object[index].type = RESPAWN;								//As the object type string matches "respawn" the object's type will be set to RESPAWN.
		}
		else if (object_type == "checkpoint")
		{
			objectgroup->object[index].type = CHECKPOINT;							//As the object type string matches "checkpoint", the object's type will be set to CHECKPOINT.
		}
		else if (object_type == "goal")
		{
			objectgroup->object[index].type = GOAL;									//As the object type string matches "goal" the object's type will be set to GOAL.
		}
		
		

		else
		{
			objectgroup->object[index].type = SOLID;					//If the object type string does not match any type, the object will be assigned the UKNOWN type.
		}

		index++;	//index is increased in one so the next object is iterated.
	}

	return ret;
}

bool j1Map::LoadProperties(pugi::xml_node& node, Properties& properties)							//REVISE THIS HERE. Check why it crashes the game at exit time.
{
	bool ret = false;

	pugi::xml_node data = node.child("properties");													//Sets an xml_node with the properties child of the layer node in the map tmx file.

	if (data != NULL)
	{
		pugi::xml_node prop;
		
		for (prop = data.child("property"); prop; prop = prop.next_sibling("property"))		//Iterates all property childs from the layer's properties child.
		{
			Properties::Property* p = new Properties::Property();									//Allocates memory for a new property.

			p->name				= prop.attribute("name").as_string();								//Sets the Property's name to the name of the property being iterated.
			p->intValue			= prop.attribute("value").as_int();								//Sets the Property's value to the value of the property being iterated.

			properties.property_list.push_back(p);														//Adds the property beint iterated to property_list.
		}
	}

	return ret;
}

bool j1Map::CreateWalkabilityMap(int& width, int& height, uchar** buffer)
{
	bool ret = false;

	for (std::list<MapLayer*>::iterator item = data.layers.begin(); item != data.layers.end() ; item++)					//While the item is not NULL, continue to iterate.
	{
		MapLayer* layer = (*item);												//Sets the item of the list as item->data. Done for readability.

		if (layer->properties.Get("Navigation", 0) == 0)							//If the value of the Navigation property of the mapLayer is 0.
		{
			continue;																//If the value mentioned above is 0, jump to the next iteration (of layers list).
		}

		uchar* map = new uchar[layer->width*layer->height];							//Allocates memory for all the tiles in the map.
		memset(map, 1, layer->width*layer->height);									//Sets all memory allocated for map to 1. 

		for (int y = 0; y < data.height; ++y)										//Iterate all the tiles in the y axis of the map.
		{
			for (int x = 0; x < data.width; ++x)									//Iterate all the tiles in the x axis of the map.
			{
				int index = (y * layer->width) + x;									//Tile index of a tile in the map: If 32x32, (2, 1) --> Index = (1 * 32) + 2 = 34; 

				int tile_id = layer->Get(x, y);										//Gets the tile id  for a given tile.
				if (tile_id > 0)
				{
					TileSet* tileset = GetTilesetFromTileId(tile_id);				//Gets the corresponding tileset for a tile by taking into account the tile_id.
					if (tileset != NULL)
					{
						map[index] = (tile_id - tileset->firstgid) > 0 ? 0 : 1;		//If (tile_id - firstgid) > 0, return 0. Else return 1.

						/*TileType* ts = tileset->GetTileType(tile_id);
						if (ts != NULL)
						{
							map[i] = ts->properties.Get("walkable", 1);
						}*/
					}
				}
			}
		}
		*buffer		= map;															//Sets buffer as the map array.
		width		= data.width;													//Sets width to the width of the map in tiles.
		height		= data.height;													//Sets height to the height of the map in tiles. 
		ret			= true;															//Sets the ret bool to true as the walkability map could be created.
	}

	return ret;
}

int Properties::Get(std::string name, int default_value)							//Revise how to be able to not have a property without default value being nullptr.
{
	for (std::list<Property*>::iterator prop_iterator = property_list.begin() ; prop_iterator != property_list.end() ; prop_iterator++)
	{
		if ((*prop_iterator)->name == name)											//If the string passed as argument matches the name of a property in the property_list.						
		{
			return (*prop_iterator)->intValue;										//Returns the value of the property.
		}
	}
	return default_value;															//If no property is found then the value returned is 0.
}

//values Properties::Get(p2SString name, values* default_value) const				//Revise how to be able to not have a property without default value being nullptr.
//{
//	p2List_item<Property*>* prop_iterator = property_list.start;
//
//	while (prop_iterator != NULL)
//	{
//		if (prop_iterator->data->name == name)										//If the string passed as argument matches the name of a property in the property_list.						
//		{
//			return prop_iterator->data->value;										//Returns the value of the property.
//		}
//
//		prop_iterator = prop_iterator->next;
//	}
//
//	return *default_value;															//If no property is found then the value returned is 0.
//}

bool j1Map::SwitchMaps(std::string new_map) // switch map function that passes the number of map defined in config.xml
{
	CleanUp();
	App->scene->to_end = false; // we put this in false so there are no repetitions
	Load( new_map.c_str() );
	//App->audio->PlayMusic(App->map->data.music_File.c_str());

	return true;
}

bool j1Map::ChangeMap(const char* newMap)
{
	bool ret = true;

	App->scene->CleanUp();

	App->map->Load(newMap);						//Loads a specified map
	App->collisions->LoadColliderFromMap();		//Load Collisions

	if (newMap == "Test_map.tmx")
	{
		App->scene->firstMap	= true;
		App->scene->secondMap	= false;

		//This needs to be changed somewhere else. Here it works but probably this is not it's place.
		int w, h;
		uchar* data = NULL;
		if (App->map->CreateWalkabilityMap(w, h, &data))				//If CreatewalkabilityMap() returns true. It means that the walkability map could be created.
		{
			App->pathfinding->SetMap(w, h, data);						//Sets a new walkability map with the map passed by CreateWalkabilityMap().
		}

		RELEASE_ARRAY(data);
	}
	if (newMap == "Test_map_2.tmx")
	{
		App->scene->secondMap	= true;
		App->scene->firstMap	= false;

		//This needs to be changed somewhere else. Here it works but probably this is not it's place.
		int w, h;
		uchar* data = NULL;
		if (App->map->CreateWalkabilityMap(w, h, &data))				//If CreatewalkabilityMap() returns true. It means that the walkability map could be created.
		{
			App->pathfinding->SetMap(w, h, data);						//Sets a new walkability map with the map passed by CreateWalkabilityMap().
		}

		RELEASE_ARRAY(data);
	}


	App->gui->Start();
	App->console->Start();
	App->scene->LoadGuiElements();
	//App->scene->Start();

	return ret;
}

void j1Map::Restart_Cam() // function that resets the camera
{
	App->render->camera.x = spawn_position_cam.x;
	App->render->camera.y = spawn_position_cam.y;
}