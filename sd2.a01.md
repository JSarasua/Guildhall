## Rubric

### Required Tasks
**Required Tasks** are tasks that the course relies on being complete, and will most likely be built upon on later assignments.  All required tasks must be attempted before a bonus task will be graded. 

- [x] *10pts*: `Window` class created in Engine to spec, with Protogame being updated; 
- [x] *20pts*: `RenderContext` changes
    - All broken `RenderContext` methods a stripped with an assert or `todo` added to the body.
    - [x] `RenderContext` can be setup by giving it a window
- [x] *05pts* `Camera::SetClearMode` added
- [x] *05pts* `RenderContext::BeginCamera` should now clears depending on clear mode 
    - [x] For  now, clear the default swapchain on `RenderContext`
- [x] *10pts* `SwapChain` implemented
    - [x] Default `SwapChain` is added to your `RenderContext`, created during setup
    - [x] Ability to get the backbuffer texture.
- [x] *10pts*: `Texture` class added/modified
- [x] *10pts*: `Texture::GetOrCreateView` will return a texture view;    
- [x] *10pts*: Game code should cycle clear color each frame to show everything working. 
- [x] *20pts* Ability to create a debug context by pre defining `RENDER_DEBUG`
    - [x] All builds should define `RENDER_DEBUG` for now
    - [x] Be sure there are no leaked resources on shutdown (check `Output`)