# Project F

## Description
This project was developed from scratch in C++ by 7 [Bachelor's degree in Video Game Design and Development](<https://www.citm.upc.edu/ing/estudis/graus-videojocs/>) students at [UPC CITM](<https://www.citm.upc.edu/>) [Tech Talent Center](<https://www.talent.upc.edu/cat/school/ttc/>) in Barcelona.

The following software and libraries were used:

* [Standard Template Library](<https://en.wikipedia.org/wiki/Standard_Template_Library>)
* [Simple DirectMedia Layer](<https://libsdl.org/>)
* [pugixml](<https://pugixml.org/>)
* [TIled Map Editor](<https://pugixml.org/>)



**Project F** is a Real Time Strategy game for PC in which you will have to defend your Base from enemy attacks as well as to search and destroy the enemy's Base.

The game is set in a fictional virtual world simulation from which you will need to get out of to survive. However, to be able to get out of it you will need to destroy each enemy base.

Feel free to visit our [Repository](<https://github.com/BarcinoLechiguino/Project-RTS>)

## Key Features

* Gameplay Features:
  * First version of the map.

  * Group selection and movement.

  * 2 types of Playable Units:

    * **Gatherer:** Gathers resources. Can be selected and moved around.
    * **Infantry:** Attacks enemy entities. Can be selected and moved around.

    

  * 1 type of Non-Playable Unit:

    * **Enemy:** Attacks allied entities. Cannot be selected and will attack any ally entities on-sight.

  * Ally and enemy combat units will automatically attack if one enters within attack range of the other and will chase them if they get out of it.
    

  * 2 types of Buildings:

    * **Town-hall:** Can generate Gatherer units. Can be upgraded. Can be an enemy entity.
    * **Barracks:** Can generate Infantry units. Can be upgraded. Can be an enemy entity.

  

  * 2 types of Resources:
    * **Data Rocks**: Can be mined from certain spots all over the map. Used to create units and upgrade buildings.
    * **Data Trees:** Can be harvested from certain spots all over the map. Used to create units and upgrade buildings.

  * Victory can be achieved by destroying the enemy's Town-hall. Conversely, if your Town-hall is destroyed it is Game Over.



* HUD Features:

  * **Minimap:** Shows the whole map as well as ally and enemy entities. Moreover the camera can be manipulated by clicking on it.

  * **Pause Menu:** Pauses the game and allows the player to access the options menu, get to the main menu, exit the application or resume the game.

  * **Building Menu:** Menu that is displayed when a building is selected. It has a text explaining what it does and allows to make use of the building's functionalities (Town-hall menu will have a button to create Gatherers, etc.)

  * **Resource Menu:** Menu that will display the amount of resources that the player has at any given moment.

  * **Entity Healthbars:** Each entity will have their own healthbars. Said healthbars will only show when an entity has been selected or has been hurt.

    * Ally entity healthbars will be GREEN.

    * Enemy entity healthbars will be RED.

    * Resource entity healthbars will be BLUE.

      

  * **Creation Bars:** When a unit entity is created, a bar displaying the progress will appear below the building that is creating the unit.

    

* Key Technical Features:

  * **Camera Culling:** All tiles outside the camera's bounds will neither be iterated or drawn.
  * **Sprite Rendering Order:** All entity sprites will be rendered ordered by their Y axis position in the world. The unit with the lower Y value will be rendered first.



## About Use

### Installation

No previous installation is required to play the game, just download the latest [RELEASE](<https://github.com/BarcinoLechiguino/Project-RTS/releases>) from our repository, open the file and execute the Project_F.exe executable.


### Controls

#### User Controls

* **Left Mouse Button:** 
  * **Drag:** Will generate a selection rectangle. All units inside the rectangle will be selected.
  * **Click on empty tile:** All entities selected will be unselected.
  * **Click on ally or neutral entity:**
    * **Unit Entity:**  The clicked unit will be and all other entities will be unselected.
    * **Building Entity:** The clicked building will be selected and all other entities will be unselected.
    * **Resource Entity:** The clicked resource will be selected and all other entities will be unselected.


* **Right Mouse Button:**

  * **Click on empty tile:** If there are any units selected,  they will path towards whichever tile that was clicked.

  * **Click on enemy entity:** If there are any Infantries selected, they will be given the clicked entity as a target and will path to get within attack range to attack it.

  * **Click on resource entity:** If there are any Gatherers selected, they will be given the clicked entity as a target and will path to get within gathering range to gather it.

    

* **Z Key:** Select all ally units.
* **X Key:** Select all ally gatherers.
* **C Key:** Select all ally infantries.
* **ESCAPE Key:** Will open the Pause Menu.


* **Camera Movement:**
  * **Cursor:** If the cursor is brought to any of the edges of the screen, the camera will move in the direction the cursor is at.
  * **UP Arrow Key:** Move camera upwards.
  * **DOWN Arrow Key:** Move camera downwards.
  * **RIGHT Arrow Key:**  Move the camera to the right.
  * **LEFT Arrow Key:** Move the camera to the left.
  * **Space Key:** The camera will be centered around your Town-hall.


#### Debug Keys

- **F1 Key:** Enable/Disable the God Mode
- **F2 Key:** Set the frame cap at 30 or 60 FPS depending on which cap is currently active.
- **F3 Key:** Enable/Disable the Pause Mode.
- **F4 Key:** Enable/Disable the GUI Debug Mode. (Shows all hitboxes, invisible elements...)
- **F7 Key:** Enable/Disable the Pathfinding Meta Debug Mode. (Shows the current path)
- **F8 Key:** Enable/Disable the Walkability Map Debug Mode. (Shows non-walkable tiles, etc)
- **F9 Key:** Enable/Disable the Entity Map Debug Mode. (Shows tiles occupied by entities.)



#### God Mode Keys 

* **G Key:** Spawn **Gatherer** at mouse position.
* **I Key:** Spawn **Infantries** at mouse position.
* **E Key:** Spawn **Enemy** at mouse position.
* **H Key:** Spawn **Town-hall** at mouse position.
* **J Key:** Spawn **Enemy Town-hall** at mouse position.
* **B Key:** Spawn **Barracks** at mouse position.
* **N Key:**  Spawn **Enemy Barracks** at mouse position.
* **R Key:** Spawn **Data Rock Resource Point** at mouse position.
* **T Key:** Spawn **Data Tree Resource Point** at mouse position.
* **V Key:** Select all enemy units. Can also be given a path.
* **U Key:** Upgrade building at mouse position (if there is any).
* **D Key:** Delete entity at mouse position (if there is any).
* **F Key:** Delete all entities. 
  * Be careful with this feature, if all entities are deleted and then God Mode is deactivated then you will be sent to either the Win or Lose Scene. To avoid this remember to spawn both a Town-hall and an Enemy Town-hall before deactivating the God Mode.



## Changelog

### v0.1

* Base code was stabilized.

* Code adapted to STL.



### v0.2

* Basic entity system.
* Basic unit control (Arrow Keys).
* Scenes and scene transitions were implemented.



### v0.3

* Basic Group Movement implementation.

* Basic Main Menu implementation.
* Some UI FX were implemented.
* Basic Unit Targeting System was implemented.



### v0.5

* Basic Combat (Target, Path, Attack, Chase) was implemented.
* Basic Gathering (Target, Path, Gather) was implemented.
* Basic Building Functionality was implemented.
* The first version of the map was implemented.
* Minimap was implemented.
* Logo, Win and Lose Scene were implemented.
* Main Menu was implemented.
* FX and Music was implemented in each scene.
* Pause Menu, Resource menu and Building Menu were implemented.
* All basic UI FX was implemented.
* Sprite rendering order was implemented.



## Developer Team: missing.md

 - {Ángel González} - {Project Lead, Management, Programmer} - GitHub: [@BarcinoLechiguino](https://github.com/BarcinoLechiguino)
 - {Pau Pedra} - {Lead Programmer} - GitHub: [@paupedra](https://github.com/paupedra)
 - {Pau Fiol} - {Lead Artist} - GitHub: [@paufiol](https://github.com/paufiol)
 - {Aitor Luque} - {Lead Designer} - GitHub: [@Aitorlb7](https://github.com/Aitorlb7)
 - {Francisco Guerrero} - {UI Artist} - GitHub: [@FranGV98](https://github.com/FranGV98)
 - {Gerard Romeu} - {QA, Designer} - GitHub: [@Gromeu2000](https://github.com/Gromeu2000)
 - {Guillem Turmo} - {Management} - GitHub: [@Turmo11](https://github.com/Turmo11)



### Where to find us

Mail: [Gmail](p2.missing.md@gmail.com) 

Twitter: [@missingmd2](<https://twitter.com/missingmd2>)



## Disclosure

***missing.md*** does not own any of the sprites, tilesets, music or sounds used in the development of this game.

* The sprites and tilesets belong to [Kenney](<https://www.kenney.nl/>). Note that they were modified to better fit the game's art style and mood.

* The FX were generated by [rFXGen](<https://raylibtech.itch.io/rfxgen>) from [raylib technologies](<https://www.raylibtech.com/>).

* All songs used were copyright free.



## License

MIT License

Copyright (c) 2020 [missing.md]

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.