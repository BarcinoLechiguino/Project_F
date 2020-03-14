# Art Bible

The game will follow a minimalistic approach to art, using low-poly, texture-less 3D assets. These assets are then rendered in isometric view to create a tileset compatible with our current pipeline.

## References
- {All art from where we drew our inspiration}

## Art Style
- We will be using the different aircrafts and vehicles available at [kenney.nl](https://kenney.nl/assets?q=3d), recolored accordingly to the game's palette.


- {Scenarios}


- {Level of Detail}
As stated before, we will use low polygon models as our source for assets. This assets don't use texture, only solid color. They are rendered with a hard light that creates three separate colours, for shadow, natural and highlighted sides. High level of detail is discouraged, since the game should be resolvable whilst zooming out, so assets should be clear based in shape and color only.

- {Camera}
The isometric tiles we will be using follows a width-height ratio of sqrt(2) (or 1.414:1)
- {Scale}
A tile should measure 151x107 px, and should still be resolvable viewed at 48px wide.

## Color Palettes
- {Visual Hierarchy}

## UI
- {Main Menu}
- {Font}

## Technical Guidelines
- How to Export
  All images will be exported as .png files. Tiles will not have a margin, both against edges and other tiles. The tile will be exported at 151px wide. When possible, lowest file size will be prefered. For example, if exporting with any Adobe software, in the export settings should be set as 'best compresion'. Avoid excessive manipulation in 2D software, when possible we will be creating the model in 3D over cropping and bashing in Photoshop (or equivalent).
  
- Tools
  - Adobe Photoshop: Our go to tool for 2D bitmap editing. It will be used for all task related to recolour and generating Tilesets from individual images. Since most assets will be retrieved from [Kenney](https://kenney.nl/) 
  - [Asset Forge](https://kenney.nl/tools/assetforge): We have available this tool which will allow us to create more specific assets we might need. We do not expect to get most assets through this, but might come handy.


## Art Bible References:
- [Final Art Bible Reference](https://github.com/DevCrumbs/Warcraft-II/wiki/6.-Art-Bible)
- [Alex's Art Bible Research](https://github.com/AlexLA99/Art-Bible/blob/master/docs/Art%20Bible.pdf)
