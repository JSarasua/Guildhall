C29.SD2.A06 Blinn-Phong Lighting
======

## Overview
Implement Per-Pixel Lighting using a shader; 

**Turn-In Branch Name: `sd2/turnin/a06`**

**Due Date: Apr 09, 2020**

### Goal [100/100]
- [x] Have a Quad, Sphere, and Cube rendering in world with normals and tangents;
    - [x] Make a new `Vertex_PCUTBN` or `VertexLit`
    - [x] Update or create new methods for generating these shapes, with normals/tangents/bitangents computed.
    - [x] Create meshes using this new vertex format.
    - [x] Be sure to update places where D3D11 needs to know format...
        - Setting vertex buffer needs to know correct stride
        - Creating an input layout needs the correct layout to tie it to the shader
    - [x] Be sure your vertex buffer stores the correct format for the vertices stored in it
- [x] Be able to switch to cycle active shader to show the following...  Use keys `<` and `>` for this
    - [x] Current shader and hotkeys are shown on screen using debug screen text.
    - [x] Normal lighting shader - `Phong.hlsl`
    - [x] Diffuse color only (no lighting) - this is your `default` shader
    - [x] Vertex Normals (transformed by model) - `normals.hlsl`
    - [x] Vertex Tangents (transformed by model) - `tangents.hlsl`
    - [x] Vertex Bitangents (transformed by model) - `bitangents.hlsl`
    - [x] Surface Normals (uses TBN) - `surface_normals.hlsl`
- [x] Each object should be rotating around the `Y` and `X` axis at different speeds.  This is to correct a correct application to model matrices to TBN space.
- [x] `9,0` - Be able to adjust global ambient light
    - [x] Console command `light_set_ambient_color color=rgb` to set ambient color to whatever you want
- [x] There should be one point light in the scene that you can adjust.
    - [x] Be able to toggle attentuation using `T`
        - [x] Default to `linear attenuation`, or (0, 1, 0)
        - [x] Cycle from linear -> quadratic -> constant, ie (0,1,0) -> (0,0,1) -> (1,0,0)
        - *Note: Suggest adding a `light_set_attenuation` command that allows you to set it arbitrarily so you can see how it affects the light behaviour.*
    - [x] Point light color and position is represented in world using a `DebugDrawWorld...` call.  Point or Sphere work well. 
        - [x] Don't do this if the light is following the camera it is will just get in the way. 
    - [x] Keyboard `-` and `+` should change its intensity
    - [x] A console command `light_set_color color=rgb` to set the light color.  
    - [x] Be able to switch the light's positioning mode... Keys are just suggestions, feel free to use whatever as long as it is in your readme. 
        - [x] `F5` - Origin (light is positioned at (0,0,0))
        - [x] `F6` - Move to Camera (light is moved to where the camera currently is, and stays there)
        - [x] `F7` - Follow Camera (light is moved to camera each frame while in this mode)
        - [x] `F8` - Animated.  Light follows a fixed path through the enfironment (cirlce your objects, or figure 8 through the objects)
- [x] Be able to adjust object material properties...
    - [x] `[,]` keys should adjust specular factor between 0 and 1 (how much does specular light show up
    - [x] `',"` keys should adjust specular power (min of 1, but allow it to go as high as you want.