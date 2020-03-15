# Audio Bible
The vast majority of the audio, both music and FX, that will be implemented will be copyright free. It should be noted that this is an educational project where no economical income will be involved.

## Audio Style
The overall style of all the audio implemented in the game will have a heavy retro Sci-Fi / Retrowave (Outrun) influence. The game's main theme will be virtual reality envisioned from the 80's, and the audio will work synergistically around that.

### Sound/FX
- **Ambience:** {The sounds that are being constantly reproduced during the gameplay}. There will always be a theme background music playing softly (so as not to disturb the player). There will be differnt FXs being triggered by the player actions, such as the units' animations for instance.  

- **Characters:** {Sound related with the in-game entities}. Each of the units will have their corresponding FX, such as the unit spawning/dying, unit movement, combat... 

- **Impact:** {Sounds that give feedback to the player}. Almost all the player's possible actions will have their own feedback FX. The FX reproduced when the mouse hovers over a building, a unit's audio response to being given an order ("Roger that" i.e)...

- **UI:** {FX Feedback given by the UI}. All the UI's interactible elements will play a FX when the player interacts with them. (Hovering/Clicking a button...) 

### Music
- **Genre:** All the game's music tracks will have a heavy retro Sci-Fi influence along with influences from derived genres such as Retrowave or Vaporwave. As mentioned before, there will always be background music playing through all the phases of the game. Each stage of the gameplay will have it's own theme (Main Menu, Pause Menu, Campaign...). 

- **Style:** {Is the track epic? Relaxed? what do we want to achieve with it?}. The overall aesthetic that the soundtrack intends to achieve is the feeling of nostalgia that can be felt by listening to old records or watching old movies, in this case from the 80's. Moreover, the music will pick up the pace when there is an in-game event (combat...) to convey the player the distinct change of pace, as well as influence the player to be aggressive instead of passive.

- **Implementation:** {How will all the music be layered?}

- **Recording:** {How will the music be recorded?} All the tracks that are to be implemented have already been recorded, as they have been acquired from ready to use sources.

- **Flow:** {How will the music change according to in-game events?} The music flow will change according to whatever happens to the player. During gameplay the main theme song will be playing softly in the background. However, if the player is engaged in combat or triggers an important event (a quest has been cleared i.e), the music will change accordingly. (Louder/Faster in combat, etc).

### Dialogue
- **Style:** {What will the voices sound like? Distorted? Mechanized?}

- **Type:** {What will be tone of the voices? Which emotion will they convey} The units will have strict and disciplined voices that take after military trained soldiers.

- **Visualization:** {When will the dialog appear and what will it convey?}

- **Entities with dialog:** {Which entities will have a voice/dialog? Will their voices be distored?}

### Animations
- **On Event:** {Unit attack, unit movement, building creation...}

### References
{All the references that were used to reach the current audio library}

## Implementation
{How will the audio be organized in-game}

## Samples
{Links to all implemented audios. Organize according to menu.}

## Audio Technology
### File Format
- **Music:** All music related files will use the .ogg format.

- **FX:** All FX related files will use the .wav format.

### Tools
- **Audacity:** Used to generate any audio that might be needed during development. It will also be used to modify any already existing audio.

## Audio Organization
### File Naming
**Naming conventions:**
- No capital letters

- All files will have their words separated by an underscore, never by a space.

- All audio files will follow the folowing naming structure:
```
The name will be composed of 3 elements and circumstancially by a number:
- First - Category: The object/space of the audio. Ex: Player...
- Second - Subcategory: The action of the audio. Could also be a more specific location/type/emotion. Ex: Shoot...
- Third - Description: Word adding detail to what the audio is about. Ex: Enemy...
- Fourth - Number: Only applyed if there is more than one sound for the same action / trigger.

Ex: infantry_walk_ground_2.wav
``` 

### Folder Organization
The folder organization and distribution will be as shown below:
{Link images of how our audio file directory is organized}

## Music and Fx

- *Menus:* When the game starts and goes to the main menu,there will be a voice that says the name of the game and then in menu there will be a retro calm track.

- *HUD:* We will have FX for the HUD from "Kenney.nl". Those FX will have a Sci-Fi thematic, like laser clicks. 

- *Gameplay:* When there are enemies in the screen, the music becomes stronger to make the action part more stressful and inmersive.

- *Loops & Transitions:* For each theme, we have a part of calm and a part when the song breaks, so, each part is looped and when the enemies appears it makes a transition to the huge part and while enemies still be in screen, the huge part will be in loop. Once the wave is overcome, the song ends and a new song sound with the same structure.


The game will have a song for each level and each song has a slow part and a fast one that will corresponds to each part of the level. There will be effects for almost every action the player does to get feedback and enemies too. 

Most of the FX that will be in the game are from "Kenney.nl" that are completely free and with no copyright. But, in some cases there will be modified with Audacity to suit to the type of game having a Sci-Fi thematic as lasers, robotic voices, etc.

In the case of music, there will be some tracks that will feel like in 80's. It will be inspired in some movies as "Kung Fury" or some games like "Far cry 3: Blood dragon". Every song is copyright free.

# {The file's structures can be modified should it be needed}
# {Audio Bible References:}
- [Final Audio Bible Example](https://github.com/DevCrumbs/Warcraft-II/wiki/5.-Audio-Bible)
- [Adrian's Audio Bible Research](https://github.com/M1R4B3L/Research-Audio-Bible/tree/master/docs)
- [Adrian's Audio Bible Template](https://github.com/M1R4B3L/Research-Audio-Bible/tree/master/templates)
