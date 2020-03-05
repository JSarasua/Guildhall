## Checklist
- [x] Allow user to adjust mass when selected [can not be 0, so clamp at 0.001 on the low end.  No high end limitation]
    - [x] Suggested keys are `[` and `]`
- [x] Add `friction` to the physics material, defaulted to 0
- [x] Allow user to adjust friction when selected [0 to 1]
    - [x] Suggested keys are `<` and `>'`
- [x] Add `drag` to the `Rigidbody2D`, defaulted to 0
- [x] Allow user to adjust object drag when selected [minimum of 0]
    - [x] Suggested keys are `:` and `\'` ***Read below
- [x] Switch to use a fixed time step
    - [x] Add a `Clock` class
        - [x] Engine should have a `Master` clock that is creaetd, that all clocks use as their parent by default
        - [x] Be sure to update your clock system in `App::BeginFrame`
    - [x] Give the physics engine its own clock (either basded off a game clock or master)
    - [x] Add a `Timer` class
    - [x] Add `Physics2D::GetFixedDeltaTime()` && `Physics2D::SetFixedDeltaTime( float frameTimeSeconds )`
        - [x] This should default to `120hz` or `1.0f / 120.0f` seconds per frame. 
    - [x] Use timer to break up `Physics2D::Update` into equal steps
    - [x] Add a console command `set_physics_update hz=NUMBER` to be able to change this at runtime.
- [x] Allow user to adjust time scale of the physics clock
    - [x] `P`: Pause & Resume
    - [x] `8`: Half the time scale
    - [x] `9`: Double the time scale
    - [x] `0`: Reset time scale to 1 and resume
    - [x] Print current time scale and pause state to screen somewhere
- [x] Implement `Rigidbody2D::GetVerletVelocity` to return an objects actual velocity this frame. 
    - [x] Save off an objects position at the start of a fixed physics step.
    - [x] Calculate verlet by taking `(currentPosition - frameStartPosition) / fixedDeltaTimeSeconds`
- [x] Use `VerletVelocity` in impulse calculations. 
    - Suggest using a function `Rigidbody2D::GetImpactVelocityAtPoint( vec2 point )` where point is unused atm, which 
      in turn calls `GetVerletVelocity()`, as we'll be doing more with this when rotational velocity comes online. 
- [x] Have a tooltip near the cursor when hovering over an object to tell information about it
    - [x] Simulation Mode
    - [x] Mass
    - [x] Current Velocity
    - [x] Current Verlet Velocity
    - [x] Coefficient of Restitution (`bounce`)
    - [x] Coefficient of Friction (`friction`)
    - [x] Drag value
- [x] Implement tangental impulse using friction 
    - [x] Follows similar rules to normal impulse when it comes to `DYNAMIC` vs `STATIC` vs `KINEMATIC` relative masses and when you apply it
    - [x] Applied only to dynamic objects
- [x] Apply drag force to all dynamic objects


*** Keys to change drag are ' and \ insted of ; and \