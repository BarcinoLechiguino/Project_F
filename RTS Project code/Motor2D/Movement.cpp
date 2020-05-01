#include "Pathfinding.h"
#include "EntityManager.h"
#include "Entity.h"
#include "Dynamic_Object.h"
#include "Pathfinding.h"
#include "Application.h"
#include "p2Log.h"

#include "Movement.h"

Movement::Movement()
{

}

Movement::~Movement()
{

}

bool Movement::Awake(pugi::xml_node& config)
{
	return true;
}

bool Movement::Start()
{
	return true;
}

bool Movement::PreUpdate()
{
	return true;
}

bool Movement::Update()
{
	return true;
}

bool Movement::PostUpdate()
{
	return true;
}

bool Movement::CleanUp()
{
	return true;
}

void Movement::OrderUnitsToMove(iPoint tile, std::vector<Dynamic_Object*> units_selected)
{
	if (App->pathfinding->IsWalkable(tile))
	{
		std::vector<Dynamic_Object*>::iterator item = units_selected.begin();

		for (; item != units_selected.end(); item++)
		{
			if ((*item)->target != nullptr)
			{
				(*item)->target = nullptr;
			}
			App->pathfinding->ChangeWalkability((*item)->occupied_tile, (*item), WALKABLE);
		}
		App->pathfinding->MoveOrder(tile, units_selected);							//Gives units targets around main target
	}
	else
	{
		LOG("Tile cannot be targeted");
	}
}

void Movement::OrderUnitsToAttack(iPoint tile, std::vector<Dynamic_Object*> units_selected)
{
	Entity* target = App->entity_manager->GetEntityAt(tile);

	if (target != nullptr)
	{
		if (App->entity_manager->IsEnemyEntity(target))
		{
			std::vector<Dynamic_Object*> infantries;															//Temporal fix. For now we only have infantries as combat units.

			for (int i = 0; i < (int)units_selected.size(); ++i)
			{
				if (App->entity_manager->IsInfantry(units_selected[i]))
				{
					units_selected[i]->target = target;
					App->pathfinding->ChangeWalkability(units_selected[i]->occupied_tile, units_selected[i], WALKABLE);
					infantries.push_back(units_selected[i]);
				}
			}

			App->pathfinding->MoveOrder(target->tile_position, infantries);

			//AttackOrder(target->tile_position, infantries);

			infantries.clear();
		}
		
		if (App->entity_manager->IsResource(target))
		{
			std::vector<Dynamic_Object*> gatherers;

			for (int i = 0; i < (int)units_selected.size(); ++i)
			{
				if (App->entity_manager->IsGatherer(units_selected[i]))
				{
					units_selected[i]->target = target;
					App->pathfinding->ChangeWalkability(units_selected[i]->occupied_tile, units_selected[i], WALKABLE);
					gatherers.push_back(units_selected[i]);
				}
			}

			App->pathfinding->MoveOrder(target->tile_position, gatherers);							//Gives units targets around main target

			gatherers.clear();
		}
		else
		{
			LOG("Target entity is not an enemy entity.");
		}
		
	}
}