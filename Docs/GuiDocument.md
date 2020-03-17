# GUI Document
This document has been redacted to showcase and keep track of all the UI's functionalities along all the design related decisions made regarding it. In this document, and with the purpose of being as clear and informative as possible, multiple flow diagrams and UI Art will be displayed.

## GUI Style
The GUI will combine both minimalistic and neon visual styles with the intention to bring to life a clean and clear interface that does not saturate the player with information. The GUI will be focused on aiding the player in focusing on the truly relevant and useful information, gameplay wise.

## Main Menu
The Main Menu will present the player with three different options:

- **New Game:** Starts a new game. In case there was an already existing saved game, it will be overwritten.

- **Continue:** Allows the player to continue the game from wherever he/she left it last time.

- **Options:** Sends the player to the *Options Menu*.

- **Exit:** Exits the application and sends the player back to the Desktop.

![](https://github.com/BarcinoLechiguino/Project-RTS/blob/master/Docs/UI/MainMenuF.png?raw=true)

### Options Menu:
This menu will have the following elements:
- **Music Audio Volume Slider:** Modifies the music audio volume.

- **FX Audio Volume Slider:** Modifies the FX audio volume.

- **Control Remapping Window:** Allows to remap both keyboard and controller controls. There will be a tab on the window that, on click, will switch between keyboard and controller control schemes.

- **Screen Size Selector:** Switches the screen size between three pre-stablished sizes with the same aspect ratio (Fullscreen, windowed, window). The selector will have the common (<-- **Fullscreen** -->) design.

Moreover, there will be two additional buttons:
- **Back Button:** The current configuration will be saved and the player will be sent back to the **Main Menu**.

- **Default Configuration Button:** The current configuration will be reset to the original pre-established parameters.

![](https://github.com/BarcinoLechiguino/Project-RTS/blob/master/Docs/UI/OptionsMenu.jpg?raw=true)

{Add a workflow diagram of the Main Menu}

## In-Game
### HUD
- **Player Mouse Cursor:** The Cursor will take after the standard mouse cursor that can be found in Windows OS. Art wise, the cursor will be designed according to the game's aesthetics (Vaporwave / Outrun). {Link the cursor image / sprite)

- **Minimap:** The minimap will be placed at the top right corner of the screen, and it will have a romboidal design. It's size can be modified in-game by clicking on the "+" and "-" tabs on the minimap's window. There will be 3 standard sizes, all of which will have been tested not to disturb the gameplay in any way.

- **Unit Management:** To upgrade or modify the behaviour of the units, the units themselves need to be clicked on and a little window will appear with the different options of each of them. An information window will appear if the mouse is hovered over a specific unit.

- **Building Management:** To upgrade a building, the player will need to click on top of each individual building. The building menu that will appear will allow the player to upgrade the building if the required resources are at hand. 

- **Resources:** The current amount of the different resources will be displayed in little windows at the top left corner of the screen along with their corresponding sprite. If the mouse is hovered over any of the resource windows, a text pop-up will appear giving a brief summary of the possible uses of that specific resource.

{Show the concept art of the different HUD elements, just to see how will the elements be placed}
{Add a the workflow diagram of the HUD}

### Pause Menu
While in-game the player can access the *Pause Menu* (by pressing the ESCAPE Key or the PAUSE Button), which offers the following options:
- **Music Volume Slider:** Modifies the music audio volume.

- **FX Volume Slider:** Modifies the FX audio volume.

- **Save Game:** Saves the game at the current checkpoint.

- **Exit to Main Menu:** Exits the current game and sends the player back to the **Main Menu**.

{Show the concept art of the Pause Menu, just to see how will the elements be placed}
{Add a Pause Menu workflow diagram}

### Menu Diagram
![](https://github.com/BarcinoLechiguino/Project-RTS/blob/master/Docs/UI/UML_Menus.png?raw=true)

## Victory/Defeat Screen
{Will our game have a victory/defeat screen? What will they show?}

## Sprites
### UI Spritesheet (v0.2)
![](https://github.com/BarcinoLechiguino/Project-RTS/blob/master/Docs/UI/HUD_RETRO.png?raw=true)

### Font samples
![](https://github.com/BarcinoLechiguino/Project-RTS/blob/master/Docs/UI/preview_Fonts.png?raw=true)


## {Use this as reference} {See the GUI Document from the first reference and see how many workflow diagrams are needed}
- [GUI Document Reference](https://github.com/DevCrumbs/Warcraft-II/wiki/4.-User-Interface-Document)
- [GUI Document Reference II](https://github.com/CheckTheDog/Fantasy-Brawl/wiki/3.-User-Interface-Document)
