# Technical Design Document
In this document all the technical specifications related with the project will be laid out. This project will be developed in C++.


## Technical Goals
All the main technical goals that have been set for this project will be stated below:

- 2D Isometric World
- Unit Logic and Management
- Battle System
- Resource Management
- Fog of War

### Target Platform
The target platform that has been set for this project will have the following specifications:

|     Hardware    |         Requirements        |  Minimum Requirements  |
| --------------- | --------------------------- | ---------------------- |
|       OS        | Windows 10 *version*        | Windows 7 Professional |
|       CPU       | Intel Core i7-9750H 2.6Hz   | Intel Core i3          |
|       GPU       | NVIDIA GeForce RTX 2060 6GB | Integrated             |
|       RAM       | 16GB                        | 100 MB                 |
| Free Disk Space | ???                         | 200 MB                 |

- Must correctly run in the university computers.

### Performance Budget
The final build must run at 60fps in the target platform described above.
The base screen resolution will be 1280x720px. Moreover Fullscreen mode, as well as Windowed mode, must be supported.


## Development Hardware
{Minimum requirements of the hardware used to develop the project}
Hardware Used:
- PC windows


## Development Software
The following programs will be employed in order to correctly develop the project:

**Code related software:**
- Github
- Visual Studio 2017 v15.9.21
- Visual Studio 2019

**Asset related software:**
- Adobe Photoshop CC
- Adobe Illustrator CC

**Profiling related software:**
- Brofiler

**Map related software:**
- Tiled

### External Libraries
All the external libraries that will be employed during the project's development will be shown down below:
- SDL
- Brofiler
- STL
- mmgr


## Branching Policy
There will be 2 main branches, development branch and master branch. Development will be used for developing the program while master will only contain stable versions with various milestones accomplished.


## Game Mechanics
{Each of the items to be implemented in the game codewise}

### Technical Requirements
{A list of every important feature that the team needs to develop and implement into the game and what planning has been made around them. (Entity System, Minimap, Fog of War, Group Movement...)}

### Game Architecture
{Diagram or written explanation on the game loop and how the data is managed through playtime}

### Graphics and Audio
{Specifications for the graphics (screen resolution, tile dimensions...) and audio (format, channels, layers...) accompanied by a short explanation of their implementation.}

### Artificial Intelligence
{Diagram of the AI's behaviour, which role it plays in the gameplay as well as which entities does it control}

### Logic
{Diagram/UML or written explanation of the Entity System and Game Object Hierarchy}

### Physics and Collisions
{Will the game have any physics/collisions?}


## Code Style
{Separate the code  guidelines in different subsections}

### 1. General

- Curly braces 
Always put first { in next line of function:
```
if()
{
```
- Enters:
Only use double enter when separating includes/defines and code or if it dramatically improves readability in code.

Separate functions with one single enter. 
```
DoLogic()
{

};

DoRoutine()
```
- Spaces

-Leave spaces between +, -, >, ()... to improve readability
-Leave space between ( and first item in parenthesis.
-Space each item inside statement.
```
if(a > b && (b + a) == 10)
{
```
- Comment Policy:

-Only comment on variables on functions which seem unclear at first sight.
-Avoid commenting non complexe functions.
-Code should not need comments to be understood.

### 2. Naming Conventions

- Variables:
-Always use lower case and underscore between words. 
-Words should be full so that they are more easily understood (position over pos).
-Compound words should use uppercase on second word. 

- Functions:
-Always use uppercase for every word and nothing between them. DoLogic()
### 3. Loops

- Use for() over while()

### 4. Condition Statements

- If statements:

-if(bool) instead of if(bool == true)
-nullptr prefered over NULL

### 5. Classes & Structs

Example class structure:
```
class Entity
{
public:
	constructor/destructor
	public app functions (order updates)
	public useful functions (Jump(), LoadMap()...)
private:
	private functions

public:
	public variables (order by types, then alphabetically)
private:
	private variables (order by type, then alphabetically)
```

### 6. Indentation

### 7. XML Conventions



## UML
{When the Gold Version is delivered, the UML will be updated and uploaded}.


## Data Layout
{How are the project's folders organized and a brief statement on the logic behind it. Should have either a flow diagram or screenshots}


## Build Delivery Method
- {See Enric's Research}


## Version List
- {List containing all the current and previous versions and should detail what was implemented in each one of them}
- {v0.1}
- {v0.2}


## Bibliography:
- [Final TDD Reference](https://github.com/DevCrumbs/Warcraft-II/wiki/7.-Tech-Design-Document)
- [Enric's TDD Research](https://github.com/EnricGDV/Research-TDD)

