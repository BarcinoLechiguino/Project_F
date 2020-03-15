# Technical Design Document
{Brief Description}

## Target Hardware
- PC windows
- Works in the university computers:
  - CPU: Intel Core i7-9750H 2.6Hz
  - RAM: 16Gb
  - GPU: NVIDIA GeForce RTX 2060 6Gb

**Target Performance:**
- 60fps
- Resolution: 1280 x 720 pixels


## Performance Budgets
Hardware Used:
- PC windows

Programs Used:
- Github
- Visual Studio 2017
- Visual Studio 2019
- Google Drive
- Adobe Photoshop CC
- Adobe Illustrator CC
- Brofiler

## Branching Policy
There will be 2 main branches, development branch and master branch. Development will be used for developing the program while master will only contain stable versions with various milestones accomplished.

## UML


## Code Style
General:

-Curly braces 

-Always put first { in next line of function:
if()
{

-Enters:

-Only use double enter when separating includes/defines and code or if it dramatically improves readability in code.

-Separate functions with one single enter. 
DoLogic()
{

};

DoRoutine()

-Spaces

- Leave spaces between +, -, >, ()... to improve readability
- Leave space between ( and first item in parenthesis.
- Don’t leave spaces between () inside bigger () so that are seen as items in the statement.
if( a > b && (b + a) == 10 )

- Comment Policy:

Only comment on variables on functions which seem unclear at first sight.
Avoid commenting non complexe functions.
Code should not need comments to be understood.

Naming conventions:

Variables: 
- Always use lower case and underscore between words. 
- Words should be full so that they are more easily understood (position over pos). 

-Compound words should use uppercase on second word. 

Functions:
- Always use uppercase for every word and nothing between them. DoLogic()

Loops:
- Use for() over while()


Class:

Example class structure:

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


Bool:


If statements:
- if(bool) instead of if(bool == true)
- nullptr over NULL
- if()
{

}


- enum class


## Version List
- {List containing all the current and previous versions and should detail what was implemented in each one of them}
- {v0.1}
- {v0.2}

## External Libraries
- SDL
- Brofiler
- STL
- mmgr

## Build Delivery Method
- {See Enric's Research}

## {The file's structures can be modified should it be needed}

## TDD References:
- [Final TDD Reference](https://github.com/DevCrumbs/Warcraft-II/wiki/7.-Tech-Design-Document)
- [Enric's TDD Research](https://github.com/EnricGDV/Research-TDD)

