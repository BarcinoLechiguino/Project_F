# Art Bible

The game is set in a simulation, and inside the aesthetics drink from the 80s' and vaporwave movement.

The game will follow a minimalistic approach to art, using low-poly, texture-less 3D assets. These assets are then rendered in isometric view to create a tileset compatible with our current pipeline.


## References

![Astroneer 1](https://github.com/BarcinoLechiguino/Project-RTS/blob/master/Docs/Images/ArtBible/astroneer-ref1.jpg?raw=true)
![Astroneer 2](https://github.com/BarcinoLechiguino/Project-RTS/blob/master/Docs/Images/ArtBible/astroneer-ref2.jpg?raw=true)
![Kenney](https://github.com/BarcinoLechiguino/Project-RTS/blob/master/Docs/Images/ArtBible/kenney_ref.png?raw=true)
![Crossy Road](https://github.com/BarcinoLechiguino/Project-RTS/blob/master/Docs/Images/ArtBible/crossy-road_ref.jpg?raw=true)
![Goose game](https://github.com/BarcinoLechiguino/Project-RTS/blob/master/Docs/Images/ArtBible/untitled_goose_game_ref.jpg?raw=true)
![Vaporwave](https://github.com/BarcinoLechiguino/Project-RTS/blob/master/Docs/Images/ArtBible/vaporwave.jpg?raw=true)


## Art Style
Our core for art assets is [kenney.nl](https://kenney.nl/assets?q=3d). You should always try to follow its style when editiong any asset, to avoid stand outs.
- **Avoid Small Details.** They won't be resolvable at a zoom, are a big time investment and won't look good surrounded by a different detail level. 
- **Respect Lighting.** All tiles should have the light applied in the same way. We do not have any shading techniques at the moment, so this is the only way for us to make it seem lit. The light will always come from the South-East, making the darkest side the left side, then right side, and the lightest side is the top of the tile. This can be seen at the palette below.  

### Characters

We will be using the different aircrafts and vehicles available at [kenney.nl](https://kenney.nl/assets?q=3d), recolored accordingly to the game's palette.

- Scout:
![scout](https://github.com/BarcinoLechiguino/Project-RTS/blob/master/Docs/Images/ArtBible/Units/scout.png?raw=true)

- Collector:
![collector](https://github.com/BarcinoLechiguino/Project-RTS/blob/master/Docs/Images/ArtBible/Units/collector.png?raw=true)

- Infantry:
![infantry](https://github.com/BarcinoLechiguino/Project-RTS/blob/master/Docs/Images/ArtBible/Units/infantry.png?raw=true)

- Heavy:
![heavy](https://github.com/BarcinoLechiguino/Project-RTS/blob/master/Docs/Images/ArtBible/Units/heavy.png?raw=true)



### Scenarios
![Example 1](https://github.com/BarcinoLechiguino/Project-RTS/blob/master/Docs/Images/ArtBible/Sample_HD_1.png?raw=true)

![Example 2](https://github.com/BarcinoLechiguino/Project-RTS/blob/master/Docs/Images/ArtBible/Sample_HD_2.png?raw=true)


- Level of Detail
As stated before, we will use low polygon models as our source for assets. This assets don't use texture, only solid color. They are rendered with a hard light that creates three separate colours, for shadow, natural and highlighted sides. High level of detail is discouraged, since the game should be resolvable whilst zooming out, so assets should be clear based in shape and color only.

- Camera
The isometric tiles we will be using follows a width-height ratio of sqrt(2) (or 1.414:1)
- Scale
A tile should measure 151x107 px, and should still be resolvable viewed at 48px wide.

## Color Palettes
When it comes to colour we will stay within a vaporwave pallete.

- **Saturation**: Saturation will be generally high, so avoid grays, browns, and other desaturated colours. The darker a colour is the more saturation you can give to it. Lighter colours, specially if in large areas, will tolerate less saturation before turning excessively bright. 

- **Brightness**: We will use darker colours as a background, with lighter colours for units and buildings. "White" will help entities stand out against the background. That being said, DO NOT use pure black nor white. You have availible the palette and the references to see how a dark colour should look. Most of the time the color we will be adding to white and black will be blue or purple.

- **Hue**: Blue, pink and purple are kings. When it comes to hue we want to avoid an excessivly large palette, so whenever using a color outside these colours it should be done sparingly. An example would be the green "data" crystals that you can see in the recolored assetpack sample. We also want to avoid excessive tones, so instead of hand-picking the hue we should copy the ones in the palette. We want one single pink (along with shade and light for it). Think of it as working with a 16-color palette.  

![Our colour palette](https://github.com/BarcinoLechiguino/Project-RTS/blob/master/Docs/Images/ArtBible/palette_3.png?raw=true)

## Technical Guidelines
- How to Export
  All images will be exported as .png files. Tiles will not have a margin, both against edges and other tiles. The tile will be exported at 151px wide. When possible, lowest file size will be prefered. For example, if exporting with any Adobe software, in the export settings should be set as 'best compresion'. Avoid excessive manipulation in 2D software, when possible we will be creating the model in 3D over cropping and bashing in Photoshop (or equivalent).
  
- Tools
  - Adobe Photoshop: Our go to tool for 2D bitmap editing. It will be used for all task related to recolour and generating Tilesets from individual images. Since most assets will be retrieved from [Kenney](https://kenney.nl/) 
  - [Asset Forge](https://kenney.nl/tools/assetforge): We have available this tool which will allow us to create more specific assets we might need. We do not expect to get most assets through this, but might come handy.


## Art Bible References:
- [Final Art Bible Reference](https://github.com/DevCrumbs/Warcraft-II/wiki/6.-Art-Bible)
- [Alex's Art Bible Research](https://github.com/AlexLA99/Art-Bible/blob/master/docs/Art%20Bible.pdf)
