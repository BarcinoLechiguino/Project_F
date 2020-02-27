#ifndef __j1PLAYER_2_H__
#define __j1PLAYER_2_H__

#include "j1Module.h"
#include "p2Point.h"
#include "j1Player.h"

class Animation;
class j1Render;
class j1Input;

struct Collider;
struct SDL_Texture;

class j1Player2 : public j1Player
{
public://methods

	j1Player2(int x, int y, ENTITY_TYPE type);
	// Destructor
	virtual ~j1Player2();

	bool Awake(pugi::xml_node&);

	bool Start();

	bool PreUpdate();

	bool Update(float dt, bool doLogic);

	bool PostUpdate();

	bool CleanUp();

public: //P2 Variables
	void SetPlayerState(Player_State& player_state);			//Depending on the input, P2 will be set in a state or another. 
	void PlayerMovement(Player_State player_state, float dt);	//Holds all states of P2. Depending on which state P2 is set, it will move, jump, teleport...
	
	void TeleportP1ToP2();										//Moves P1 directly in front of P2. It takes into account where P2 is looking at.
	void RespawnP2ToP1();										//Moves P2 directly behind P1 on death.
	void SetPlayer2Position();									//Sets P2's origin position in a map. Takes into account which map is being loaded (firstMap and secondMap).
	void LivesCheck(int lives);									//Checks if the player has any lives left.
	void OnCollision(Collider* C1, Collider* C2);				//Collision Logic Handling.

	// Load / Save
	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&) const;

	void Restart();									//Resets P2's position to where P2 started the level. 
	void GodModeInput();							//Enables / Disables the God Mode.
};

#endif // __j1Player_2_H__