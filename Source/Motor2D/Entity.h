#ifndef __ENTITY_H__
#define __ENTITY_H__

#include "Dependencies/SDL/include/SDL_rect.h"
#include "Point.h"						//Required. For some reason the iPoint namespace does not work without it, even with a forward declaration like class iPoint.

class Animation;
class GuiImage;
class GuiHealthbar;
class Emitter;

struct SDL_Texture;
struct SDL_Rect;
struct FowEntity;


enum class ENTITY_TYPE
{
	UNKNOWN,
	GATHERER,
	SCOUT,
	INFANTRY,
	HEAVY,
	ENEMY_GATHERER,
	ENEMY_SCOUT,
	ENEMY_INFANTRY,
	ENEMY_HEAVY,
	TOWNHALL,
	BARRACKS,
	WALL,
	ENEMY_TOWNHALL,
	ENEMY_BARRACKS,
	ENEMY_WALL,
	ROCK,
	TREE,
	OBELISK,
	BOULDER
};

struct EntityData
{
	ENTITY_TYPE		type;
	iPoint			position;
};

struct EntityCost
{
	uint			data;
	uint			electricity;
	uint			chips;
};

class Entity
{
public:
	Entity();
	Entity(int x, int y, ENTITY_TYPE type, int level);	// here we will set initial position
	//virtual ~Entity()

	//Must be virtual so when an entity is executed the compiler goes to the entity's methods instead of these
	virtual bool Awake(pugi::xml_node&);

	virtual bool Start();

	virtual bool PreUpdate();

	virtual bool Update(float dt, bool do_logic);

	virtual bool PostUpdate();

	virtual bool CleanUp();

public:
	//Entity Methods
	/*virtual bool Save(pugi::xml_node&) const;
	virtual bool Load(pugi::xml_node&);
	virtual void Restart();*/							//Maybe not needed THIS HERE REVISE, CHANGE
	
	virtual void InitEntity();

	virtual void AttachHealthbarToEntity();
	
	virtual void Draw();

	void ApplyDamage(Entity* target);											//Applies the current unit damage to the referenced target, also handles the target health bar.
	void RecieveDamage(int damage);

public:																			// --- ENTITY VARIABLES ---
	std::string		name_tag;													//Name of the entity in the xml file.
	ENTITY_TYPE		type;

	bool			is_selected;

	int				max_health;
	int				current_health;
	int				attack_damage;
	int				level;
	int				max_level;
	
	fPoint			pixel_position;												// The entity's world position (pixels).
	fPoint			center_point;												// Used for sprite ordering
	iPoint			tile_position;												// The entity's map position (tiles).
	iPoint			tiles_occupied;												// Amount of tiles that a given entity occupies in the x and y axis.

	SDL_Rect		healthbar_background_rect;									// The background sprite of an entity's healthbar.
	SDL_Rect		healthbar_rect;												// The foremost sprite of an entity's healthbar.
	iPoint			healthbar_position_offset;									// Value that will affect where around the entity the healthbar will appear.

	GuiHealthbar*	healthbar;

	SDL_Texture*	entity_sprite;

	Animation*		animation;
																				// --- ENTITY FOW RELATED VARIABLES ---
	bool			is_visible;													// Will determine whether or not an entity is currently visible. (See FowManager module)
	bool			is_neutral;													// Will determine whether or not an entity will be seen through FOGGED tiles.
	bool			provides_visibility;										// Will determine whether or not an entity will grant the player visibility over the Fog of War.
	uint			range_of_vision;											// Radius in which a given entity will grant vision. In tiles.

	FowEntity*		fow_entity;

};

#endif // __ENTITY_H__