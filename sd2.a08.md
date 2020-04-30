C29.SD2.A08 Mesh Loading
======

## Tasks
- [x] Be able to load `OBJ` Files
    - [x] Load OBJ as a single vertex array
    - [x] At end of each group (or at very end), apply post steps..
        - [x] Invert-V
        - [x] Calculate Normals
        - [x] Calculate Tangents
    - [x] At end of import...
        - [x] Apply transform
        - [ ] *Clean* (is an extra) 
        - [x] Invert Winding
- [x] Post-Import Options supported
    - [x] Invert-V
        - Some programs us UV with V at the top, and some bottom.  On import
          be able to swap it out to the correct version for your engine.
    - [x] Invert Winding Order
        - Some programs use clockwise by default.  If you notice your mesh is 
          inside out (you can see it from the inside, but not outside), you will
          want to invert the winding order of faces.
    - [x] Generate Normals (Flat only required)
        - Some OBJ files do not include normals.  If this import option is set, 
          you will want to generate normals if the file didn't include them.
          For **Flat Normals**, just calculate the normal for each face and assign it to
          each version that defines the face. 
    - [x] Generate Tangents
        - Use MikkT to generate tangents.  This requires the mesh have normals, so generate
          normals should also be true. 
          Use [./mikktspace.h](./mikktspace.h) and [./mikktspace.c](./mikktspace.c) to
          do this step.  
    - [x] Post import transform from the authoring engines space to our space.
        - [x] Transform Positions (full)
        - [x] Normals (just direction, not scaled or translated)
        - [x] Tangents & Bitangents (just direction, not scaled or translated)
- [x] Have a lit model loaded in your scene to show it is working
- [x] Support a `Shader` or `ShaderState` class that is data driven
    - [x] Define a data format you like
    - [x] Define a class you like
    - [x] Implement `RenderContext::BindShader` or `BindShaderState` that
          binds the program and the additional state for you.
    - [x] Add `RenderContext::GetOrCreateShader(...)` to support a database of shaders
    - [x] Implement `RenderContext::BindShaderByName` or `RenderContext::BindShaderStateByName`
- [x] Support a `Material` class to make managing material data easier.
    - [x] Should encapsulate a `ShaderState` and relevant data needed for the material
    - [x] Define a data format you like.
    - [x] Define a `Material` class you can use that works with our data
        - [x] Must be able to support owning at least one UBO for material specific data?
        - [x] Can set material specific textures
        - [x] Can set material specific samplers
        - [x] Can set your engine specific constants (spec/tint)
    - [x] Can make a material from file
    - [x] Can call `RenderContext::BindMaterial` to bind the shader and all data assocated with the material



If you do this extra, print out the memory size needed before and after 
clean (and the percent memory saved/lost from the process).  