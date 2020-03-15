# Game Design Document
## Overview
“Name” is a survival RTS game. The basic pillars that sustain the game are exploration, the system of hordes and the combat system. The player will have to create, control and sustain different troops and buildings. 

The main objective is to explore through the map, fortify your base and gathering resources across the map to beat the final boss that will be waiting for you at the end of the It.

The game creates unique and challenging experiences where the player is forced to use his experience and wisdom in order to success on his mission and doesn’t let the nerves to get over him.

You will have to explore all the surroundings, find resources, complete quests to gain more and more materials to build the strongest wing to defeat the final boss. Anyway, don’t leave alone your city, don’t go too far away as hordes of enemies will appear to besiege your base. Think strategies and make sure to cover your back. 


## Plot/Lore
We are in the 80s, a time where journalism is one of the main pillars of information. Our character, Vrainen Heber, is a journalist who works for the Austrian national newspaper and is on the verge of dismissal. His continuous failures in the company when publishing inconspicuous, ridiculous and even, in some cases, false articles, have led him to the serious situation of seeing himself as a homeless person in a matter of weeks.

Our character is on a critical situation but still has something to hold on to. The newspaper editor gives him one last chance and suggests to him to write an article about a possible case of corruption and data retention in the Austrian government that directly concerns the actual President: Rudolf Kirchschläger.

Motivated by this last opportunity, he is determined to uncover the government once and for all. But how would he do it? Who was him to deal with a subject of the first order when his most important article had been about a robbery in a greengrocer? he didn't have the means or the resources to get that type of information. At least not legally.

Vrainen took out the little money he had been saving during these bad months, he barely gave him enough to survive the day to day and that is why he was so appreciated about It. All his savings were 500 shillings. 
These past weeks, in the streets a name was being rumored, a certain scientist of bad reputation, with macabre ideas but with a magnificent brain, who had just moved to the city after being expelled from the engineering university in Berlin. In a few days he was able to find him, Dr. Luka Ackermann.
Vrainen brought him up to date and the Professor decided to help him. Since the doctor is a nanotechnology expert, he told him about a method that he had been working on for years, it had benn remained a secret and was the reason why he was expelled from the university. This does not give Vrainen peace of mind but it is the only way. Immediately afterwards, Dr. Ackermann explains how this method works.

Everything consists on doing a “nano-transformation”, a concept invented by the professor himself, where his molecules will be converted into nano-pixels and will be introduced into the power supply of the professor's computer. In this way, Vrainen will be able to be within the network and use everything at his hand such as electricity flows, data structures, servers, wi-fi network, among others, to get to the bottom of the government's data system and uncover them, and thus keep your job and enhance your name in the journalistic world.

3,2,1 ... Immersion.


## Game Pillars
**Exploration**: the dungeon pattern of the map. If you recruit an explorer, you can explore all the map to find resources as data, electricity and others. The more explored and more recollected, the better base and troops you’ll have.

**System of hordes**: Horde after horde enemies will keep coming with a break in between so you can prepare yourself against the next seizing or even explore and complete quests.The time set between hordes it is thought to give the player enough time to engage on a new quest, without taking an eye out of the timer and the base. The difficulty will grow within the hordes.

**Combat system**: By clicking and dragging you can select the amount of units you want to move wherever the player wants a long as is a walkable area for the units. When enemy units slip into the attack area of those units, they will begin to attack them. This system improves the strategy side of the game, placing your units to defend the weaker side of your base.


## Game Flow
- {Control mapping}

## Gameplay
### **Exploration and objectives** 
You can send your units through the fog of war deep into the unexplored area, a dangerous land but also very rewarding. In there new resources sources can be found, trapped allies and others. But even while exploring the player must keep an eye on the base having in mind the hordes never stop coming.
Some of the mandatory quests will require your units to enter the fog of war in order to perform the task requested in the unexplored area.

### **Resource recollecting**
Some buildings are specifically made for that purpose, taking into account the upgrade level of the building it will gather a certain amount of the specified resource in a defined time space. There are various resource collecting buildings for each available resource.
Natural source are another way to gather resources that can’t be obtained through an specific building, in order to achieve those the player will have to explore through the fog of war.

### **Buildings creation and upgrading**
The creation or upgrading of a building will consume an specific amount of resources and time, so the constant income of resources is vital in order to keep making the base grow. Even the training and creation of new units will require resources and time. Those factors are a very important mechanic to consider, the timing when it comes to create and upgrade a building or train a unit is essential, doing it while the enemy is attacking could result in a catastrophic outcome.
Buildings and units will have an specific resource to be upgraded so your base keeps growing up in terms of defensive and attacking stats, as the basic stats of buildings and units will improve at the same time the they get upgraded.

### **Unit management**
When you start the game, you start with a single explorer and a single collector. From here, your job is to recollect other resources to start building new structures and recruiting new troops. 

If you left click in a unit, a menu will display that will provide all the actions the unit can do, anyway, shortcuts will be provided to set your troops in a mode in an easier way.

All units will have shortcuts to do some actions and some of those mechanics will be specific for a few units, in other words not all units will have the same functionality or actions. Anyway, let’s start with general ones. 

All units can move, individually or in group if you right click the terrain. Also, all units, except for recollectors, can attack if you right click an enemy unit. Also, all the units that can attack will be able to set in defensive/guard mode. The unit will stop in the zone the player set before and if any enemy gets inside their attack radius, that unit will start attack until it dies or the enemy does so. Finally, the last action that only collectors can do is to repair. If your base and your buildings are damaged but not destroyed, all of them will recover their HP automatically with an amount of time. Otherwise and only if the building is destroyed, only collectors will be able to repair It.


## Elements
### **Buildings**
- **Wall**
- **Recollector**
- **Defensive**
- **Barracks**
- **Hall**


### **Units**
#### **Allies**
There are 4 types of units that you can recruit, all of them are different types of spaceships:

- **Scout**:  It is the fastest unit, but It has the lowest HP. It is useful to advance faster through the map. Its attack is also low.
- **Recollector**: Useful to collect resources as it is the only unit that can do so. It moves slower and has low HP. 
- **Infantry**: Combat unit. Attacks a single unit. 
- **Heavy**: It has high HP. Slower unit and consumes more resources than infantry.

#### **Enemies**
They will have the same structure, but the enemy won’t have collectors and explorers as their objective is just to destroy your base. 

- **Infantry**
- **Heavy**
- **Final Boss**

#### **Resources** 
- **For building**: chunks of data.
- **For recruiting**: electricity.
- **For upgrading**: byte.


#### **Balancing**
This will be a variant changing while we develop the game trying to adjust it to be as fair as possible with the player, without leaving aside the difficulty as a core aspect of it, taking into account our aim for the game and also our limitations.
At this current state of the development we can’t provide with any exact information of the exact balance numbers we will be working on, like amount of resources needed to build an structure, the time to train or create a unit, etc.


## Map
### **Exploration (Fog of War)**
One of the game pillars is the exploration, not only focusing on fortifying your base also finding out other features and benefits that you can obtain of the surroundings.
Although it won’t be easy, around the base, covering the unexplored area, there will be a fog keeping the player off seeing further from a small radius, so the enemies will come from any possible angle without any previous warning.

Exploration will be a fundamental feature, the core of most of the quests will require to explore and even fight outside the base, we will include an increase of 
the difficulty as you advance stages.

The real difficulty here is that the game forces the player to keep an eye on the base every time he goes to explore as without any warning and if you are far away from home and your base It is not well protected, the hordes will siege It.

### **Enemy Position (Hordes)**
Enemies won’t have an specific location as enemies will come and attack your with hordes that will come from any direction at any time. 

It is true that there will be other enemies spread around the map on the exploration zone, so we create a variety of enemies and dangers in the game.

You have some probabilities to find some enemies while you explore so It is important not to pick a single unit to explore and to accompany your recollectors and scouts with troops to defend them as well as to leave some units in base to defend It and gain time for the player to reach the base and beat the specific horde.

### **Minimap**
At the top-”..” on the screen we will see the minimap that shows a generic view of the map as quests, resources, etc. 

As the map may be too big, the minimap’s radius will be limited and the minimap will move within the map. Next to the minimap we have a button that by pressing it allows the player to change the minimap range. So if player wants a general or a focused view It can be changed with a single click.


## Learn To Play
At the start of every game, an icon with the face of the Dr. Ackermann will appear at the top-”...” corner of the screen. He will welcome the player to the new planet and the importance of the mission the player is taking part. He is the omniscient character who knows everything taking place in the game universe.
The General will show you the controls of the game as It advances and will introduce the quests every time the player accepts It. Guiding you to the challenges and warning you if a new horde appears.


## Rewards
The player will have to complete 4 different quests. Those quests will be different types:

- **2 kill quest**:
- **1 gather quest**:
- **1 delivery quests**: 


## Controls
We recommend to play the game with both mouse and keyboard, but it can be played also with gamepad. 

### **Mouse**: 
- Right click: Target an enemy/resource/building
- Left click: Select a building/unit
- Left drag: Select a group of units

### **Keyboard**:

### **Gamepad**: 

