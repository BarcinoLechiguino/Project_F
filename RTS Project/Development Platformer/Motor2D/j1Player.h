#ifndef __j1PLAYER_H__
#define __j1PLAYER_H__

#include "j1Module.h"
#include "p2Point.h"
#include "Animation.h"
#include "j1Entity.h"

class Animation;
class j1Render;
class j1Input;

struct Collider;
struct SDL_Texture;


enum class Player_State
{
	Idle,
	Going_Right,
	Going_Left,
	Crouching,
	Jumping,
	Falling,
	Boost_jump,
	Attacking,
	Teleporting,
	Dying
};

struct Player
{
	fPoint			spawn_position;			//Keeps record of the first position(x, y) of a player in a map (at spawn time).
	fPoint			max_speed;				//Player's Cruiser Speed for both axis.
	fPoint			acceleration;			//Sets how much time it takes a player to reach Cruiser Speed horizontally and/or vertically.
	fPoint			boost_jump;				//Sets how much vertical or horizontal impulse will a player get.
	float			gravity;				//Acceleration variable for jumps. Gravitational Pull.
	float			tpCdCount;				//Keeps track of how much time has passed since tp was last used.
	float			tpCdTime;				//Amount of cooldown time the tp skill has.
	float			godModeSpeed;			//Player movement speed while in God Mode.
	float			frontflipStart;			//At which point of the boost jump the frontflip animation should start.
	float			frontflipEnd;			//At which point of the boost jump the frontflip animation should end.
	int				lives;					//Number of lives a player has. It just defines how many times a player can get impaled with spikes before having to restart the whole level over.
	int				max_lives;				//Maximum number of lives a player can have.
	int				coins;					//Amount of coins that a player currently has.
	int				score;

	bool			grounded;				//Keeps track of whether a player is jumping, falling or is on the ground.
	bool			airborne;				//Keeps track of whether a player is on the air ir not.
	bool			flip;					//Keeps track of which direction a player is looking at. Changes the sprite orientation when returns true (flip).
	bool			isCrouching;			//Keeps track of whether a player is crouching or not.
	bool			isJumping;				//Keeps track of whether a player is jumping or not.
	bool			isBoostJumping;			//Keeps track of whether a player is boost jumping or not
	bool			isGoingRight;			//Keeps track of whether a player is going to the right or not.
	bool			isGoingLeft;			//Keeps track of whether a player is going to the left or not.
	bool			item_activated;			//Keeps track of whether a player has interacted with an item or not.
	bool			platformDrop;			//Keeps track of whether a player wants to drop from a platform or not.
	bool			tpInCd;					//Keeps track of whether a player's tp skill is on cooldown or not.
	bool			isAttacking;			//Keeps track of whether a player is attacking or not.
	bool			isAlive;				//If a player runs out of lives, then this returns false.
	bool			isDying;				//If a player runs out of lives, then this returns true.
	bool			againstRightWall;		//If a player is against a right wall then this returns true.
	bool			againstLeftWall;		//If a player is against a left wall then this returns true.
	bool			againstCeiling;			//If a player is against a ceiling then this is true.
	bool			checkpointReached;		//Keeps track whether a player has reacked a checkpoint or not.
	bool			fading;					//Fade character when changing scenes.
	bool			GodMode;				//Defines whether GodMode is active or not.
	bool			extraLife;				//Bool that keep track of whether or not a player has been awarded an extra life.

	int				boostThreshold;			//Threshold from within which the boost jump is activated. Temporal
	p2SString		mapTag;					//Depending on the mapTag, the initial position loaded from the xml file of the player will be one or another. mapTag = "factoryMap" --> Origin Pos in the factory map.

	SDL_Rect		HitBox;					//Rectangle that represents a player.
	SDL_Rect		atkHitBox;				//Rectangle that represents a player's attack reach / range.
	Player_State	state;					//Adds the state enum to a player's variables.
	Collider*		collider;				//Collider that will be assigned to a player. 
	Collider*		atkCollider;			//Collider that  will be assigned to a player as it's attack reach.
};

class j1Player : public j1Entity
{
public://methods

	j1Player(int x, int y, ENTITY_TYPE type);
	// Destructor
	virtual ~j1Player();

	bool Awake(pugi::xml_node&);

	bool Start();

	bool PreUpdate();

	bool Update(float dt, bool doLogic);

	bool PostUpdate();

	bool CleanUp();

public: //P1 Variables

	Player player;

	//void TeleportP2ToP1();													//Moves P2 directly in front of P1. It takes into account where P1 is looking at.
	//void RespawnP1ToP2();														//Moves P1 directly behind P2 on death.
	
	virtual void OnCollision(Collider* C1, Collider* C2);						//Collision Logic Handling.										//Saving to xml file.

	void LoadAnimationPushbacks();												//Loads the player's animation pushbacks from the xml file.
	void LoadEntityProperties();												//Loads the player's properties.
	void LoadEntityAudio();														//Loads a player's audio.

	virtual bool InitPlayer();													//Initializes all variables and colliders of the players (Position, Colliders...) and loads them on screen. 
	virtual bool LoadPlayerPosition(p2SString playerPosition, p2SString map);	//Loads the player's position from the config file. Takes into account which map the player is at.
	virtual void GodModeInput();												//Enables / Disables the God Mode.

	virtual void SetPlayerState(Player_State& player_state);					//Depending on the input, a player will be set in a state or another. Without "&" the state would not be chenged.
	virtual void PlayerMovement(Player_State player_state, float dt);			//Holds all states of a player. Depending on which state a player is set, it will move, jump, teleport...
	virtual void LivesCheck(int lives);											//Checks if the player has any lives left.
	virtual void SkillCooldown(bool& inCd, float& cdCounter, float& cdTime);	//Keeps track of any skill's cooldown. Revise --> Pass dt as an argument?

	void ApplyGravity();														//If a player is airborne then this function brings her/him down to the ground.
	fPoint SpeedFactor();														//Returns the current speed factor. Its a completely arbitrary calculation (speed * 0.1f). Simplifies calculus.
	void LoadNextMap();															//When a player reaches the goal, the next map will be loaded. (firstMap & secondMap).

	//bool LoadPlayer1Textures();												//Loads P1's textures on screen.
	//void Restart();															//Resets P1's position to where P1 started the level. 

public:
	pugi::xml_document	config_file;
	pugi::xml_node		player_entity;

	//Animation Variables
	Animation		idle;			//Idle animation.
	Animation		running;		//Running animation.
	Animation		crouching;		//Crouching animation.
	Animation		jumping;		//Jumping animation.
	Animation		frontflip;		//Frontflip  animation.
	Animation		falling;		//Falling animation.
	Animation		boosted_jump;	//Boosted Jump Animation.
	Animation		attacking;		//Attack animation.
	Animation		wallslide;		//Wallslide animation.
	Animation		death;			//Death animation.

	//Sound Variables
	uint			jumpFX;			//Jump sfx.
	uint			deathFX;		//Death sfx.
	uint			duoFX;			//Boost Jump sfx.
	uint			goalFX;			//Goal Reached sfx.
	uint			tpFX;			//Teleport sfx.
	uint			activateFX;		//Item Activation sfx.
	uint			mecha_die;
	uint			alien_die;
	uint			mecha_walking;
	uint			saver;
	uint			no_teleport;
	uint			ready_tp;
	uint			alien_walking;
	uint			attack;
	uint			one_up_fx;


	bool			audioIsLoaded;	//Keeps track whether the audio has already been loaded.
};

#endif // __j1Player_H__