------

## Core Task List [85%]

**Reminder:  You can use previously uncompleted extras as well as extras in this assignment to fill out the points.**

- [x] `Camera::SetProjectionPerspective( float fovDegrees, float nearZClip, float farZClip )` implemented
    - [x] Set projection to `60 degrees`, and `-0.1` to `-100.0` for the clip planes.
- [x] Camera now has a `Transform`
    - [x] Create the `Transform` class
    - [x] `Transform::SetPosition` implemented
    - [x] `Transform::Translate` implemented
    - [x] `Transform::SetRotationFromPitchRollYawDegrees`
        - [x] When storing degrees, make sure they are stored in sane ranges...
            - [x] Roll & Yaw is `-180` to `180` 
            - [x] Pitch is `-90` to `90`
- [x] Camera now calculates `view` matrix from their transform.
    - [x] `Transform::GetAsMatrix` implemented to calculate the camera's model matrix
    - [x] `MatrixInvertOrthoNormal` implemented to invert the camera's model into a view matrix
        - [-] `MatrixIsOrthoNormal` check added
        - [x] `MatrixTranspose` added
- [x] Draw a Quad at `(0, 0, -10)`, or 10 units in front of the origin (should be visible when you start)
- [ ] Allow player to move the camera by change the camera transform position
   - [x] `W` & `S`: Forward & Back (movement is relative to where you're looking)
   - [x] `A` & `D`: Left and Right (movement is relative to where you're looking)
   - [x] `Space` & `C`: Up and Down Movement (movement is absolute (world up and world down)
   - [x] `Left-Shift`: Move faster while held.
   - *Note:  If you want different controls, just make a note in your readme*
- [-] Allow player to turn the camera using the mouse.
    - [x] `InputSystem::HideSystemCursor` implemented
    - [x] `InputSystem::ClipSystemCursor` implemented
    - [x] `InputSystem::SetCursorMode` implemented
        - [x] `ABSOLUTE` mode is what you currently have
        - [x] `RELATIVE` move implemented
            - [x] Move mouse to the center of the screen, and store off the cursor position
                - *Note:  Be sure to actually make the system call, not just assume where you moved it is where it went.  This can cause drifting.*
            - [x] Each frame, get the cursor position, and calculate frame delta.
            - [x] ...after which, reset to center of screen and reget the current position. 
    - [x] Game should be set to `RELATIVE` mode
        - [x] `DevConsole` should unlock the mouse and set to `ABSOLUTE` mode
    - [x] Associate `X` movement with `yaw`
    - [x] Associate `Y` movement with `pitch`
        - [-] Do not allow pitch above `85` degrees or below `95` degrees - no going upside down... yet...
        - *Note:  Up to you if you want inverted-y or not.*
- [x] Support `RenderContext::SetModelMatrix`
    - [x] Create a new uniform buffer for storing a model matrix (slot 2)
    - [x] `SetModelMatrix` should update this uniform buffer
    - [x] `BeginCamera` should `SetModelMatrix` to the `IDENTITY`, and be sure to bind the buffer.
- [x] Be able to draw a cube mesh at `(1, 0.5, -12.0)`
    - [x] Create a `GPUMesh` class
        - [x] Implement `IndexBuffer`
        - [x] Be able to construct a mesh from a vertex and index array
        - [x] Add `RenderContext::BindIndexBuffer`
        - [x] Add `RenderContext::DrawIndexed`
        - [x] Add `RenderContext::DrawMesh`
            - This should bind the vertex buffer, index buffer, and then `DrawIndexed`
    - [x] Game creates a `cube mesh` around the origin with 2 unit sides. 
    - [x] Game has a `Transform` for the cube set at `(1, 0.5, -12.0f)`, 
    - [x] Cube transform sets `yaw` rotation to current time each frame
    - [x] Game should `SetModelMatrix` to the cube transform matrix
- [x] Support a depth buffer
    - [x] `Texture::CreateDepthStencilBuffer` added
    - [x] `Camera::SetDepthStencilBuffer` added
    - [x] `RenderContext` now automatcially creates a depth buffer during init matching the swap chain's size
    - [x] `RenderContext::BeginCamera`, now binds the camera's back buffer as well.
        - [x] **IMPORANT:  Do not bind the default one automatically if the camera doesn't have one set.  There are reasons a camera may not want a depth buffer!**
    - [x] Camera's clear options should now store off the `depth` and `stencil` clear values.
    - [x] If camera has a depth buffer and says it should clear depth, also clear the depth buffer.
        - Use `ID3D11DeviceConext::ClearDepthStencilView` to clear if camera says to.
- [x] Generate a **UV Sphere** mesh during `Game::Startup`
    - [x] `MeshUtils.hpp` has a function 
          `AddUVSphereToIndexedVertexArray( std::vector<VertexPCU>& verts, std::vector<uint>& indices, vec3 center, float radius, uint horizintalCuts, uint verticalCuts, RGBA color )`
    - [x] Draw this UV sphere multiple times as a large moving ring in your game.  Each one rotating along a local axis as well as rotating along a global axis.  See demo.
   
------

## Extras
- [ ] *X04.10: 03%*: Mouse input, show, and clip options should use a stack to track state `InputSystem::PushMouseOptions`, see notes...
- [ ] *X04.11: 02%*: **Requires X04.10** - Mouse options are disabled when window loses focus, and re-applied when gaining focus.
- [ ] *X04.15: 04%*: CPU Mesh Subdivide (tesselation)
- [ ] *X04.20: 02%*: Cube Sphere Generation
- [ ] *X04.30: 04%*: IcoSphere Generation (No UV)
- [ ] *X04.31: 02%*: **Requiers X04.30** - IcoSphere UVs (Can use spherical projection - there's no clean seam so  will require a wrapping sampler to wo -k)
- [ ] *X04.40: 02%*: Plane Generation (with subdivision count)
- [ ] *X04.41: 04%*: **Requires X04.40** - Surface Generation for equations of the form "vec3 f(u, v)";
- [ ] *X04.42: 04%*: **Requires X04.40** - NURB Generation (can use previous)

------
