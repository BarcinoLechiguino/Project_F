# Art Bible
The game will be set in a virtual simulation, and the aesthetics will be heavily influenced by the aesthetics of the 80's and the vaporwave movement.

The game will take a minimalistic approach to the artstyle, using low-poly, texture-less 3D assets rendered in a 2D isometric view. This process will generate a tileset compatible with the project's current pipeline.


## References
Here can be seen how this reliance on shape only is something present in other succesful games such as Astroneer, Crossy Road or Untitled Gooose Game. In astroneer you can see how a spacial theme can blend with a minimalistic approach to modelling. 

![Astroneer 1](https://github.com/BarcinoLechiguino/Project-RTS/blob/master/Docs/Images/ArtBible/astroneer-ref1.jpg?raw=true)
![Astroneer 2](https://github.com/BarcinoLechiguino/Project-RTS/blob/master/Docs/Images/ArtBible/astroneer-ref2.jpg?raw=true)
![Kenney](https://github.com/BarcinoLechiguino/Project-RTS/blob/master/Docs/Images/ArtBible/kenney_ref.png?raw=true)
![Crossy Road](https://github.com/BarcinoLechiguino/Project-RTS/blob/master/Docs/Images/ArtBible/crossy-road_ref.jpg?raw=true)
![Goose game](https://github.com/BarcinoLechiguino/Project-RTS/blob/master/Docs/Images/ArtBible/untitled_goose_game_ref.jpg?raw=true)



Vaporwave will have its influence specially in palettes. In addition, Vaporwave also features heavy use of technology which fits into our theme. Another key is retrofuturism: We will be using togheter high tech elements and out of fashion gadgets. This allows having a wider range of assets and helps us evoque nostalgia on the player. 

![Vaporwave](https://github.com/BarcinoLechiguino/Project-RTS/blob/master/Docs/Images/ArtBible/vaporwave.jpg?raw=true)

Kung Fury
![[Kung Fury](https://www.youtube.com/watch?v=sUwny2hyHCE)](https://github.com/BarcinoLechiguino/Project-RTS/blob/master/Docs/Images/ArtBible/kung_fury_ref.jpg?raw=true)

Hotline Miami
![Hotline Miami](https://github.com/BarcinoLechiguino/Project-RTS/blob/master/Docs/Images/ArtBible/hotline_miami_ref.png?raw=true)

Far Cry 3: Blood Dragon
![Blood Dragon](https://github.com/BarcinoLechiguino/Project-RTS/blob/master/Docs/Images/ArtBible/blood_dragon_ref.jpg?raw=true)

## Art Style
The core of the project's art assets will be [kenney.nl](https://kenney.nl/assets?q=3d). Following its style when editing any asset will be key in avoiding stand outs.

### General Conventions {Temporal}
- **Avoid Small Details.** They won't be resolvable at a zoom, are a big time investment and won't look good surrounded by a different detail level. 

- **Regarding Lighting.** All tiles should have the light applied in the same way. We do not have any shading techniques at the moment, so this is the only way for us to make it seem lit. The light will always come from the South-East, making the darkest side the left side, then right side, and the lightest side is the top of the tile. This can be seen at the palette below.  

### Characters

We will be using the different aircrafts and vehicles available at [kenney.nl](https://kenney.nl/assets?q=3d), recolored accordingly to the game's palette. {Maybe later add a brief summary to each of the units with a bit of lore and the reason behind their design}

- Scout:

![scout](https://github.com/BarcinoLechiguino/Project-RTS/blob/master/Docs/Images/ArtBible/Units/scout.png?raw=true)

- Collector:

![collector](https://github.com/BarcinoLechiguino/Project-RTS/blob/master/Docs/Images/ArtBible/Units/collector.png?raw=true)

- Infantry:

![infantry](https://github.com/BarcinoLechiguino/Project-RTS/blob/master/Docs/Images/ArtBible/Units/infantry.png?raw=true)

- Heavy:

![heavy](https://github.com/BarcinoLechiguino/Project-RTS/blob/master/Docs/Images/ArtBible/Units/heavy.png?raw=true)


### Scenarios
We will be using a tileset 
{Later on, the map should be added with an explanation on why it was designed that way artwise}

![Example 1](https://github.com/BarcinoLechiguino/Project-RTS/blob/master/Docs/Images/ArtBible/Sample_HD_1.png?raw=true)

![Example 2](https://github.com/BarcinoLechiguino/Project-RTS/blob/master/Docs/Images/ArtBible/Sample_HD_2.png?raw=true)


### Level of Detail
As stated before, low polygon 3D models rendered to 2D will be used as assets. This assets do not have any textures, only solid colours. They are rendered with a hard light that creates three distinctly separate colours, which represent the natural, highlighted and darkened sides. High level of detail is discouraged, since the game should be resolvable whilst zooming out, so assets should be clear based in shape and color only.

### Camera
The isometric tiles that will be used follow a width-height ratio of sqrt(2) (or 1.414:1).  It will not be able to rotate, but it will be able to move freely. It will also feature zoom, where the closest will be at true tile size (151px) and will be furthest with a tile size of about 48px.


### Scale
A tile should measure 151x107 px, and should still be resolvable viewed at 48px wide.

Units will have a rough correspondance with tiles, but they can exceed the size of the tile. It will depend on the size of the unit. For example, Heavy units will be larger than a tile whereas scouts, for example, won't.


## Color Palettes
Regarding colour, the pallette will take after the vaporwave aesthetic:

- **Saturation**: Saturation will be generally high, so grays, browns, and other desaturated colours should be avoided. The darker the colour the more saturation that can be given to it. On the other hand, lighter colours, specially within large areas, will tolerate less saturation before turning excessively bright. 

- **Brightness**: Darker colours will be used on the background, with lighter colours being applied to units and buildings. "White" will help entities stand out against the background. That being said, DO NOT use pure black nor white. Most of the time, the color that will be added to white and black will be blue or purple. The pallette, as well as references on how a dark colour should look, are available down below. 

- **Hue**: Blue, pink and purple will be the kings. When it comes to hue an excessively large pallette should be avoided, so whenever using a color outside these colours it should be done sparingly. An example would be the green "data" crystals that you can see in the recolored assetpack sample. Excessive tones should also be avoided, so instead of hand-picking the hue the ones already in the palette should be used. One single pink (along with shade and light for it) will also be integrated. Think of it as working with a 16-color palette.  

![Our colour palette](https://github.com/BarcinoLechiguino/Project-RTS/blob/master/Docs/Images/ArtBible/palette_3.png?raw=true)

## Technical Guidelines
### How to Export
  All images will be exported as .png files. Tiles will not have a margin, as all will be placed edge against edge. Each tile will be exported at 151px wide. When possible, the lowest file size will be prefered. For instance, when exporting with any Adobe software, the export settings should be set at 'best compresion'. Excessive manipulation in 2D software should be avoided and, when possible, modeling a new 3D model is preferabble over cropping and bashing in Photoshop (or any other equivalent software).
  
### Tools
  - Adobe Photoshop: The go to tool for 2D bitmap editing. It will be used for all tasks related with recolouring and generating Tilesets from individual images. Since most assets will be retrieved from [Kenney](https://kenney.nl/), this tool will be mostly employed for tile editing purposes.
  
  - [Asset Forge](https://kenney.nl/tools/assetforge): This tool which will allow the team to create more specific assets that might be needed. The intended use that will be given to the tool is a supporting one, the majority of the project's assets will already be available for implementation.
