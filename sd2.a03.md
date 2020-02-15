## Core Task List [85%]

This is the first assignment where you must supplement your assignment with some extras to get full credit.  Core tasks
only cover 85 of the total 100 points.  Make a note of all extras you attempt in your assignment, 
and remember you can submit extras from previous assignments.

- [ ] *15pts*: Get a Working Orthographic Camera
    - [ ] *05pts*: Implement `MakeOrthographicProjectionMatrixD3D` in `Engine/Math/MatrixUtils.hpp`, `.cpp`.  
    - [x] *05pts*: Camera now owns a `UniformBuffer` for their matrices
    - [x] *05pts*: Camera updates and binds their uniform buffer in `BeginCamera`
- [ ] *18pts*: Get Working Textures
    - [x] *04pts*: Be able to create a `Texture` from an Image/File
    - [x] *04pts*: `TextureView` supports shader resource views (srv)
    - [x] *04pts*: Make a `Sampler` class
    - [ ] *03pts*: Default Samplers (Linear+Wrap, Point+Wrap)
    - [x] *03pts*: `RenderContext::GetOrCreateTexture` works again.
- [x] *08pts*: Blend States supported by shader.
    - [x] *04pts*: Blend state is created/recreated and bound when shader is bound.
    - [x] *04pts*: Only recreate blend state if changed since last binding
- [ ] *05pts*: Default Built-In Shader to use when `BindShader(nullptr)` is used
    - [ ] `Renderer::SetBlendState` should affect this shader; 
- [ ] *05pts*: Render two textured quads on screen
    - [ ] *04pts*: One using an invert color shader
    - [ ] *01pts*: One default
- [ ] *34pts*: Dev Console
    - [x] *02pts*: User can open the dev console using the **tilde** key '\~'.  
                   You may use a custom key if you make a note in your assignment readme.
        - [x] Upon opening console, be sure the input is cleared.
    - [x] *02pts*: Pressing tilde again closes the console.
    - [x] *09pts* If console is open, Escape should...
        - [x] *03pts*: Clear input if there is any text currently there
        - [x] *03pts*: Close the console if console is open.
        - [x] *03pts*: Normal game behaviour otherwise (for example, for now it will quit your application)
    - [ ] *14pts*: User can type commands **only** while console is open.
        - [x] *02pts*: Typing should insert characters at the carot
        - [x] *02pts*: The carot blinks (every quarter to half second usually looks okay)
        - [x] *02pts*; Left & Right arrow keys should move carot forward and back along string 
        - [x] *02pts*: Delete should delete the character after the carot
        - [x] *02pts*: Backspace should delete the character before the carot
        - [x] *02pts*: Pressing `Enter` submits the command
        - [ ] *02pts* If it is an invalid command (it has not handler), print an error message
    - [ ] *02pts* Game input is ignored while dev console is open.
    - [x] *03pts*: Support a `quit` command that quits the game.
    - [ ] *03pts*: Support a `help` command that lists all exposed commands
   
------

## Extras

- [ ] *X03.01 : 03pts*: Built-in error shader to use when a shader fails to compile.   
- [ ] *X03.02 : 04pts*: AutoComplete
- [ ] *X03.03 : 04pts*: Context Sensitive Auto Complete
- [ ] *X03.04 : 04pts*: Command History
- [ ] *X03.05 : 03pts*: Persistant Command History (requires X03.02)
- [ ] *X03.06 : 05pts*: Text Selection 
- [ ] *X03.10 : 04pts*: Cut, Copy & Paste from Window's Clipboard (Requires X03.04)
- [ ] *X03.11 : 02pts*: Text Scrolling
- [ ] *X03.12 : 02pts*: Vertical Scroll Bar (Requires X03.11)
- [ ] *X03.13 : 06pts*: Contextual Auto Complete (Requies X03.01)
- [ ] *X03.20 : 04pts*: Text Color Markup
- [ ] *X03.21 : 02pts*: Command Scripts
- [ ] *X03.22 : 02pts*: Mouse Input on Dev Console (scroll wheel to scroll, click to set carrot)
- [ ] *X03.23 : 04pts*: Mouse Selection (Requires X03.05 and X03.22)
- [ ] *X03.24 : 02pts*: Navigation Hotkeys 
- [ ] *X03.25 : 02pts*: Audio Cues
- [ ] *X03.26 : 02pts*: Open & Close Animations (Slides Open for Example)
- [ ] *X03.27 : 04pts*: Undo/Redo History (Ctrl+Z, Ctrl+Y)

------