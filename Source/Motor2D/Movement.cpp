#include "Log.h"

#include "Pathfinding.h"
#include "EntityManager.h"
#include "Entity.h"
#include "DynamicObject.h"
#include "Pathfinding.h"
#include "Application.h"
#include "Map.h"

#include "Movement.h"

Movement::Movement()
{
	name = ("movement");
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

void Movement::OrderUnitsToMove(iPoint tile, std::vector<DynamicObject*> units_selected)
{
	if (App->pathfinding->IsWalkable(tile))
	{
		std::vector<DynamicObject*>::iterator item = units_selected.begin();

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

void Movement::OrderUnitsToAttack(iPoint tile, std::vector<DynamicObject*> units_selected)
{
	if (App->map->CheckMapBoundaries(tile))														// Checks that the current mouse_tile is within the map's boundaries.
	{
		Entity* target = App->entity_manager->GetEntityAt(tile);

		if (target != nullptr)
		{
			if (!App->pathfinding->IsWalkable(tile))													// TMP CHECK. Avoids getting corrupted entity pointers from the entity map.
			{
				if (App->entity_manager->IsEnemyEntity(target))
				{
					std::vector<DynamicObject*> ally_units;												//Temporal fix. For now we only have infantries as combat units.

					for (int i = 0; i < (int)units_selected.size(); ++i)
					{
						if (App->entity_manager->IsScout(units_selected[i])
							|| App->entity_manager->IsInfantry(units_selected[i])
							|| App->entity_manager->IsHeavy(units_selected[i]))
						{
							ally_units.push_back(units_selected[i]);
						}
					}

					for (int i = 0; i < (int)ally_units.size(); ++i)
					{
						ally_units[i]->target = target;
						App->pathfinding->ChangeWalkability(ally_units[i]->occupied_tile, ally_units[i], WALKABLE);
					}

					if (ally_units.size() != 0)
					{
						App->pathfinding->AttackOrder(target->tile_position, ally_units);
					}

					ally_units.clear();
				}
			}

			if (App->pathfinding->IsNonWalkable(tile))
			{
				if (App->entity_manager->IsResource(target))
				{
					std::vector<DynamicObject*> gatherers;

					for (int i = 0; i < (int)units_selected.size(); ++i)
					{
						if (App->entity_manager->IsGatherer(units_selected[i]))
						{
							gatherers.push_back(units_selected[i]);

							units_selected[i]->target = target;
							App->pathfinding->ChangeWalkability(units_selected[i]->occupied_tile, units_selected[i], WALKABLE);
						}
					}

					if ( gatherers.size()  !=0)
					{
						App->pathfinding->AttackOrder(target->tile_position, gatherers);							//Gives units targets around main target
					}

					gatherers.clear();
				}
				else
				{
					LOG("Target entity is not an enemy entity.");
				}
			}
		}
	}
	else
	{
		LOG("Target tile (%d, %d) is not within the map's bounds", tile.x, tile.y);
	}
}