# Project F

## Description
This project was developed from scratch in C++ by 7 [Bachelor's degree in Video Game Design and Development](<https://www.citm.upc.edu/ing/estudis/graus-videojocs/>) students at [UPC CITM](<https://www.citm.upc.edu/>) [Tech Talent Center](<https://www.talent.upc.edu/cat/school/ttc/>) in Barcelona.

The following software and libraries were used:

- [Standard Template Library](<https://en.wikipedia.org/wiki/Standard_Template_Library>)
- [Simple DirectMedia Layer](<https://libsdl.org/>)
- [pugixml](<https://pugixml.org/>)
- [TIled Map Editor](<https://pugixml.org/>)


**Project F** is a Real Time Strategy game for PC in which you will have to defend your Base from enemy attacks as well as to search and destroy the enemy's Base.

The game is set in a fictional virtual world simulation from which you will need to get out of to survive. However, to be able to get out of it you will need to destroy each enemy base.

## Key Features

- Gameplay Features:
  - 4 types of Playable Units:

   - **Gatherer:** Gathers resources. Can be selected and moved around.
   - **Scout:** Has high mobility but low attack prowess. Can be selected and moved around.
   - **Infantry:** Attacks enemy entities. Standard combat unit. Can be selected and moved around.
   - **Heavy:** Has low mobility but high attack prowess. Can be seleted and moved around.

  - 4 type of Non-Playable Unit:

   - **Enemy Gatherer:** Gathers resources. Cannot be selected.
   - **Enemy Scout:** Attacks allied entities. Cannot be selected and will attack any ally entities on-sight.
   - **Enemy Infantry:** Attacks allied entities. Cannot be selected and will attack any ally entities on-sight.
   - **Enemy Heavy:** Attacks allied entities. Cannot be selected and will attack any ally entities on-sight.

  - Ally and enemy combat units will automatically attack if one enters within attack range of the other and will chase them if they get out of it.

- 4 types of Buildings:

  - **Town-hall:** Can generate Gatherer units. Can be upgraded.
  - **Barracks:** Can generate Scout, Infantry and Heavy units. Can be upgraded.
  
  - **Enemy Town-hall:** Can generate Enemy Gatherer units. Can be upgraded by the enemy.
  - **Enemy Barracks:** Can generate Enemy Scout, Infantry and Heavy units. Can be upgraded by the enemy.

- 3 types of Resources:

  - **Data Rocks:** Can be mined from certain spots all over the map. Used to create units and upgrade buildings.
  - **Data Trees:** Can be harvested from certain spots all over the map. Used to create units and upgrade buildings.
  - **Bits:** Can be collected from certain spots all over the map. Used to upgrade buildings.

- Victory can be achieved by destroying the enemy's Town-hall. Conversely, if your Town-hall is destroyed it is Game Over.

## Controls
### Debug Keys

#### Debug Keys

- **F1 Key:** Enable/Disable the God Mode
- **F2 Key:** Set the frame cap at 30 or 60 FPS depending on which cap is currently active.
- **F3 Key:** Enable/Disable the Pause Mode.
- **F4 Key:** Enable/Disable the GUI Debug Mode. (Shows all hitboxes, invisible elements...)
- **F7 Key:** Enable/Disable the Pathfinding Meta Debug Mode. (Shows the current path)
- **F8 Key:** Enable/Disable the Walkability Map Debug Mode. (Shows non-walkable tiles, etc)
- **F9 Key:** Enable/Disable the Entity Map Debug Mode. (Shows tiles occupied by entities.)



#### God Mode Keys 

- **G Key:** Spawn **Gatherer** at mouse position.
- **I Key:** Spawn **Infantries** at mouse position.
- **E Key:** Spawn **Enemy** at mouse position.
- **H Key:** Spawn **Town-hall** at mouse position.
- **J Key:** Spawn **Enemy Town-hall** at mouse position.
- **B Key:** Spawn **Barracks** at mouse position.
- **N Key:**  Spawn **Enemy Barracks** at mouse position.
- **R Key:** Spawn **Data Rock Resource Point** at mouse position.
- **T Key:** Spawn **Data Tree Resource Point** at mouse position.
- **K Key:** Increase each type of resource stored by 300.
- **V Key:** Select all enemy units. Can also be given a path.
- **U Key:** Upgrade building at mouse position (if there is any).
- **D Key:** Delete entity at mouse position (if there is any).
- **F Key:** Delete all entities. 
  - Be careful with this feature, if all entities are deleted and then God Mode is deactivated then you will be sent to either the Win or Lose Scene. To avoid this remember to spawn both a Town-hall and an Enemy Town-hall before deactivating the God Mode.

## Developers

 - Angel Gonzalez - Project Lead, Management, Programmer - GitHub: [BarcinoLechiguino](https://github.com/BarcinoLechiguino)
 - Aitor Luque - Lead Designer, QA - GitHub: [Aitorlb7](https://github.com/Aitorlb7)
 - Pau Fiol - Lead Artist - GitHub: [paufiol](https://github.com/paufiol)
 - Pau Pedra - Lead Programmer - GitHub: [paupedra](https://github.com/paupedra)
 - Francisco Guerrero - UI Artist, Designer - GitHub: [FranGV98](https://github.com/FranGV98)
 - Gerard Romeu - Designer - GitHub: [Gromeu2000](https://github.com/Gromeu2000)
 - Guillem Turmo - Management - GitHub: [Turmo11](https://github.com/Turmo11)

## License

MIT License

Copyright (c) 2020 [missing.md]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
