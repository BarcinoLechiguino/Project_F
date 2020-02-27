#ifndef __j1MAP_H__
#define __j1MAP_H__

#include "PugiXml/src/pugixml.hpp"
#include "p2List.h"
#include "p2Point.h"
#include "p2Log.h"
#include "j1Module.h"
#include "j1Collisions.h"
#include "SDL/include/SDL.h"

class j1Collisions;
struct Collider;

// Collider Types -------------------------
enum Object_Type					//Care with enum class. If enum class-> Object_Type::HAZARD and not just HAZARD (i.e)
{
	PLAYER,
	PLAYER2,
	ATTACK,
	ENEMY,
	SOLID,
	PLATFORM,
	HAZARD,
	SWITCH,
	DEACTIVABLE,
	ITEM,
	RESPAWN,
	CHECKPOINT,
	GOAL,
	NONE,
	UNKNOWN = -1
};

// Properties and Properties methods of a layer -------------------------
union values																//A union makes the elements of a struct share the same memory space (int = 1 0 0 0 0 0 0 0, float = 1 0 0 0 0 0 0 0 ...). It is really efficient.
{
	const char* un_string;													//Union string.
	int			un_int;														//Union int.
	float		un_float;													//Union float.
};

//Information / Data members of the properties of a layer.
struct Properties
{
	struct Property															//Property struct that will hold the data members of the properties of a layer.
	{
		p2SString	name;													//Name of the property in a layer.
		values		value;													//Value of the property in a layer.
		int			intValue;												//Int value of the property in a layer. Used mainly for pathfinding
	};
	
	~Properties()															//Deletes every property and frees all allocated memory.
	{
		LOG("The Properties' destructor has been called");
		LOG("property_list has %d elements", property_list.count());

		p2List_item<Property*>* prop_iterator = property_list.start;		//Item that will iterate all the items in the property_list.
		 
		int i = 0;
		while (prop_iterator != NULL)
		{
			++i;
			RELEASE(prop_iterator->data);									//Deletes all data members of a property and frees all allocated memory.
			prop_iterator = prop_iterator->next;
		}

		property_list.clear();												//Clears poperty_list by deleting all items in the list and freeing all allocated memory.
	}

	//values default_values = { 0 };

	//values Get(p2SString name, values* default_value = nullptr) const;			//Will get a specified property's data members. //Revise string type (p2SString, const char*...)
	
	int Get(p2SString name, int default_value = 0) const;			//Will get a specified property's data members. //This version will be used exclusively for pathfinding. (Draw / Nodraw)

	p2List<Property*>	property_list;
};

//Information of a specific object in the map.
struct ObjectData
{
	uint				id;				//Object's id.
	p2SString			name;			//Object's name.
	Object_Type			type;			//Type of collider associated with the object.
	SDL_Rect*			collider;		//Rectangle that represents the object. As x, y, w and h are object properties, they can be grouped in a SDL_Rect.
	float				rotation;		//Rotation of the object in degrees clockwise.
	SDL_Texture*		texture;		//Visible image of the object.
};

//Object layer that has all the objects in the same "plane".
struct ObjectGroup
{
	uint				id;				//ObjectGroup layer id.
	p2SString			name;			//ObjectGroup's name.
	ObjectData*			object;			//Individual info of each object in the ObjectGroup layer.
	p2SString			type;			//ObjectGroup's type. It's an arbitrary string added in Tiled to the Object/ObjectGroup.
	uint				num_objects;	//Quantity of objects. Treure per obj def.
};

// Map Layer data members --------------------------------
struct MapLayer
{
	p2SString			name;			//Map name.
	uint*				gid;			//Tile Id.
	uint				width;			//Layer width in tiles.
	uint				height;			//Layer height in tiles.
	uint				size;			//width * height.
	float				speed;			//Parallax speed.
	Properties			properties;		//Properties of a layer.

	MapLayer() : gid(NULL) {} //New Comment

	~MapLayer()
	{
		RELEASE(gid);		//Breaks with mmgr.
	}

	inline uint Get(uint x, uint y) const
	{
		return gid[(y * width) + x];
		//return x + y * width;
	}
};

// Tileset Data Members and methods ----------------------------------------------------
struct TileSet
{	
	//This method calculates the position of each tile when given a tile id. 
	SDL_Rect GetTileRect(uint tile_id) const;

	p2SString			name;					//Tileset name.
	int					firstgid;				//First global tile id. Maps to the first id in the tileset.
	int					tile_width;				//Maximum width of tiles in a given tileset.
	int					tile_height;			//Maximum height of tiles in a given tilesset.
	int					spacing;				//Space in pixels between the tiles in a given tileset.
	int					margin;					//Margin around the tiles in a given tileset.
	SDL_Texture*		texture = nullptr;		//Image that will be embedded on the tileset.
	int					tex_width;				//Image width in pixels.
	int					tex_height;				//Image height in pixels.
	int					num_tiles_width;		//Number of tiles at the X axis that will have a given texture. Ex: num_tiles_width = tile_width / tex_width; 
	int					num_tiles_height;		//Number of tiles at the Y axis that will have a given texture. Ex: num_tiles_height = tile_height / tex_height;
	int					offset_x;				//Horizontal offset in pixels.
	int					offset_y;				//Vertical offset in pixels.

};

// Different supported MapTypes ----------------------------------------------------
enum MapTypes
{
	MAPTYPE_UNKNOWN = 0,
	MAPTYPE_ORTHOGONAL,
	MAPTYPE_ISOMETRIC,
	MAPTYPE_STAGGERED,
	MAPTYPE_HEXAGONAL
};



// ----------------------------------------------------
struct MapData
{
	int						width;				//Map width in tiles.
	int						height;				//Map height in tiles.
	int						tile_width;			//Tile width in pixels.
	int						tile_height;		//Tile height in pixels.
	SDL_Color				background_color;	//Background colours.
	MapTypes				type;				//Type of map (Orthogonal, Isometric, Staggered or Hexagonal)
	p2List<TileSet*>		tilesets;			//List that accesses all tilesets and their data members/properties.
	p2List<MapLayer*>		layers;				//List that accesses all layers and their data members/properties.
	p2List<ObjectGroup*>	objectGroups;		//List that accesses all object groups and their data members/properties.

	p2SString music_File;
};

// ----------------------------------------------------
class j1Map : public j1Module
{
public:

	j1Map();
	// Destructor
	virtual ~j1Map();
	// Called before render is available
	bool Awake(pugi::xml_node& conf);
	// Called each loop iteration
	void Draw();
	// Called before quitting
	bool CleanUp();
	// Load new map
	bool Load(const char* path);

	iPoint MapToWorld(int x, int y) const;	//This method translates the position of the tile on the map to its equivalent position on screen.
	iPoint WorldToMap(int x, int y) const;	//This method translates the position of the tile on the screen to its equivalent position on the map.
	bool CreateWalkabilityMap(int& width, int& height, uchar** buffer) const;
	
	bool SwitchMaps(p2SString* new_map);	//Unloads the map and changes by another one. 
	bool ChangeMap(const char* newMap);		//Change map with a fade time
	void Restart_Cam();

private:

	bool LoadMap();
	bool LoadTilesetDetails(pugi::xml_node& tileset_node, TileSet* set);
	bool LoadTilesetImage(pugi::xml_node& tileset_node, TileSet* set);
	bool LoadLayer(pugi::xml_node& node, MapLayer* layer);
	bool LoadObjectLayers(pugi::xml_node& node, ObjectGroup* group);
	bool LoadProperties(pugi::xml_node& node, Properties& properties);

	TileSet* GetTilesetFromTileId(int id) const;

public:

	MapData			data;
	p2Point<float>	spawn_position_cam;
	Collider		camera_collider;				//Collider that will have the same position and dimensions as the camera. Will be used for camera culling.
	uint			winWidth;						//Declared to store the window's width.
	uint			winHeight;						//Declared to store the window's height.
	
	iPoint			cam_tilePos;					//Position of the camera in the X and Y axis in tiles.
	int				tile_index;						//Will store the tile's index number so the correct tile is loaded.
	
	bool			pathfindingMetaDebug = false;	//Keeps track of whether to load the PathfindingCollisions layer. Temporally declared here.

private:
	pugi::xml_document	map_file;
	p2SString			folder;
	bool				map_loaded;
};

#endif // __j1MAP_H__