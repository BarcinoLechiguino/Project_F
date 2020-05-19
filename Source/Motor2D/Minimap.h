#include "Dependencies\SDL\include\SDL.h"
#include "Application.h"
#include "Module.h"
#include "Point.h"

struct SDL_Texture;
struct SDL_Rect;

class Minimap : public Module
{
public:
	Minimap();
	virtual ~Minimap();

	bool Awake(pugi::xml_node& config);

	bool Start();
	bool Update(float dt);
	bool PostUpdate();
	bool CleanUp();

	void BlitMinimap(); //Prints all elements of the minimap; Analogous to map->Draw() in practical uses

public:
	bool InteractingWithMinimap();						// Will return true if the cursor is over the minimap or the player is moving the minimap camera.

private:

	void CreateTexture(); 
	bool MinimapCoords(int& map_x, int& map_y);
	void DrawCamera();
	void MinimapBorders();
	void DrawEntities();
	void DrawMinimap();									//Blits onto the alternate renderer, not on screen
	void DrawFogOfWar();

public:
	bool			player_is_moving_camera;

private:
	SDL_Texture*	minimap_tex;
	SDL_Texture*	minimap_fow_tex;
	SDL_Texture*	tex;
	SDL_Texture*	fow_tex;
	SDL_Texture*	minimap_bg; 


	SDL_Surface*	map_surface;

	SDL_Renderer*	map_renderer;

	float minimap_scale; //value from 

	int x_offset;
	int y_offset;

	float map_width;
	
	int minimap_width;
	int minimap_height;

	iPoint minimap_position;

	bool minimap_loaded;

	std::vector<iPoint> minimap_tiles;
};

