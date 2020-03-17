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
All the code conventions that the development team will follow will be stated down below:

### 1. General Conventions
- **Language:** Every line of code and every comment must be written in **English**.

- **Coding Language:** C++ nomenclature will be used over C.


#### Spaces:

- Leave spaces between +, -, >, ()... to improve readability
- Leave space between ( and first item in parenthesis.
- Space each item inside statement.
```C++
if((a > b) && (b + a) == 10)
{

}
```

#### Comment Policy:
- All methods should have a brief comment on their functionality.
- Only comment variables on methods which would seem unclear at first sight.
- Avoid commenting non complex functions.
- In general, the code should not need any comments to be understood.

#### Getters & Setters:
- Always use Get()/Set() to get and modify variables.

```C++
const int Get() {}
int Set() {}
```

### 2. Naming Conventions
#### Variables:
- Words should be full so that they are more easily understood (position over pos).
- All variable names should be in lower case.
- Compound variable names will have each of their words separated by an underscore.
- Compound words should use uppercase on second word.  {?}
- Temporal variables should be avoided. Only in the case that they are absolutely necessary, their name should be one word only.

Example:
```C++
// Single & Compound Variable Names
int position;
int player_position;

// Long Term Variable
int map_width;

// Short Term Variable. Understandable due to context.
int i;
```

- **Bools:**
Bool Variables should generally be preceeded by `is`:

```C++
// General Rule
bool is_visible;
bool is_interactible;

// Exception example
bool has_child;
```

- **Constants:**
All constant names must be in uppercase and separated by an underscore should there be a compound name.

```C++
ALPHA 80
MAX_SIZE 1000
```

#### Enumerations
- All enumerations will be `enum class`
- All enumeration names must be in uppercase and separated by an underscore should there be a compound name.
- All enumeration item names must be in uppercase and separated by an underscore should there be a compound name.

```C++
enum class ENTITY_STATE
{
	IDLE
	PATHFINDING_NORTH
	PATHFINDING_SOUTH
	PATHFINDING_EAST
	PATFHINFING_WEST
};
```

#### Functions:
- All function names should follow the `FunctionName()` format and must be clearly descriptive of what they do.
  
  ```C++
  // Function examples extracted from j1Gui.h
  void CreateGuiCommands();
  bool ElementCanBeFocused();
  UI* FirstElementUnderMouse();
  ```
  
- The object to which the function belongs too should be taken into account when naming a function:
  
  ```C++
  // Not valid
  player.GetPlayerPostion();
  
  // Valid
  player.GetPosition();
  ```
  
- Function input variables should have names as similar as possible to their type:
  
  ```C++
  // Function example extracted from j1Text.h
  
  // Not valid
  void RefreshTextInput(const char* newInput);
  
  // Valid
  void RefreshTextInput(const char* string);
  ```

### 3. Variables
- Variable names should always start at the same column.
- Variables should never be initialized in their origin .h file.
- Variables should never be initialized with magic numbers. 
- Variables should be initialized with values loaded from an xml file, other vars or constants.
- Variables should be initialized in the same column. {?}

```C++
// Variable Declaration Examples
int 	health;
float 	speed;
bool 	is_running;
char* 	unit_name;

// Variable Initialization Examples
health 		= MAX_HEALTH;
speed 		= config.child("entities").child("units").child("scout").attribute("speed").as_float();
is_running 	= false;
unit_name 	= nullptr;
```

### 4. Condition Statements
- Operators should always be used, regardless of the lines of code that comprise the if() statement:

```C++
if (health == 0)
{
	DestroyUnit();
}
```

#### Booleans
- When inputted as conditions, **booleans** will be represented with `is_alive / !is_alive` instead of `is_alive == true / is_alive == false`.

```C++
if (is_alive)
{
	DoAction();
}

if (!is_alive)
{
	DestroyUnit();
}
```

- With multiple outcomes, the positive condition will always be placed in the if() statement. {?}

```C++
if (is_alive)
{
	DoLogic();
}
else
{
	DestroyUnit();
}
```

- If possible, sequences of if() else if() statements should be avoided. In such cases switch statements should be used instead, specially when enumerations are involved.

### 5. Loops
- `for()` over `while()` loops.
- Loop iterators can be temporal variables.

```C++
int i = 0;

for (; i < MAX; ++i)
{
	printf("This is the loop iteration n %d.", i);
}
```

- Always use `++var` over `var++`. It will be less resource consuming, specially with loops.

```C++
var++ returns the value of the variable BEFORE being incremented.
++var returns the value of the variable AFTER being incremented.
```

### 6. Classes & Structs
- All **class** elements will be organized following the below example:

```C++
class Entity
{
public:
	constructor/destructor
	public app functions (order updates)
	public utility functions (Jump(), LoadMap()...)
private:
	private functions

public:
	public variables (order by types, then alphabetically)
private:
	private variables (order by type, then alphabetically)
};
```

- All **struct** elements will be organized following the below example:

```C++
struct Collider
{
	variables
	
	functions
	
	constructor/destructor
};
```

### 7. Indentation
For brace indentation, the [Allman style](http://syque.com/cstyle/ch6.7.htm) will be followed.

- Braces will be placed in the same column where the control statement begins:

```C++
// Allman indentation example

for (int i = 0; i < MAX; ++i)
{
	if (!is_false)
	{
		DoRoutine();
	}
}
```

#### Whitespacing:
- Separate variables/functions that do not belong to the same data type:

```C++
int health;
int lives;

bool is_alive;
bool is_jumping;

void Move();
void Jump();

bool IsMoving() const;
bool IsJumping() const;
```

- Separate functions with one single enter:
```C++
DoLogic()
{

};

DoRoutine()
```

Only use double enter when separating includes/defines and code or if it dramatically improves readability in code.


### 8. XML Conventions
- Boolean variables must be 0 or 1.

- The structure example shown down below must be followed:

```XML
<animations>
	<scout>
		<idle speed="4.0f" loop="1">
			<pushback x="27" y="14" w="40" h="58"/>
      			<pushback x="126" y="12" w="40" h="58"/>
      			<pushback x="228" y="12" w="40" h="58"/>
      			<pushback x="326" y="14" w="40" h="58"/>
		</idle>
	</scout>
</animations>
```


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

