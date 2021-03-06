## Checklist

- [x] Stop coloring objects red that are intersecting (it becomes a strobe light in this assignment otherwise)
- [x] Make sure your game creates polygons as `STATIC` by default, and discs create as `DYNAMIC` by default
- [x] Add support for `disc Collider2D::GetWorldBounds` for existing colliders
	- [x] **Easier Option**: Implement this using `AABB2` bounding boxes
    - [ ] **Challenge Option**: Implement this using a bounding `Disc`
    - [x] Update this bounds whenever you update the world shape of the object.
- [x] Switch `Collider2D::Intersects` to be non-virtual, and instead use a matrix lookup
    - [x] Use a *mid-phase* check to early out of a collision by comparing the bounds first.
    - [x] If passes the bounds check, lookup and call the appropriate callback
- [x] Add `Collider2D::GetManifold` that also uses a collision matrix or half matrix
    - [x] Again do an early out using the bounds.
    - [x] Implement a disc versus disc manifold
    - [x] Implement a disc versus polygon manifold
    - **Note: We are not doing polygon -vs- polygon yet**
- [x] `Collider2D` gets gets a `PhysicsMaterial`
    - [x] `PhysicsMaterial` has a property for `restitution` (or `bounciness` if you prefer)
    - [x] `Collider2D::GetBounceWith(Collider2D const* other) const` implemented to compute restitution between two objects
        - There are multiple methods for computing the restitution.  Product, Min, Max, Spherical, Lookup, etc...   
    - [x] Allow user to adjust bounciness using `+` and `-` while the object is selected.
    - [x] Set the alpha for the fill color to the counciness of the object.
- [x] Add a `Collision2D` object that contains..
    - Two pointers to the `Collider2D` objects involved in the collision (`me` and `them`)
    - A `manifold2` struct containing...
      - `normal` describing the normal at the impact point
      - `penetration` How deeply interpenetrated are the two objects.
- [x] `Physics2D` during a `SimulateStep` should now do the following
    - [x] `DetectCollisions` to compute all collisions between all colliders.
    - [x] `ResolveCollisions` to resolve all detected collisions
    - [x] `ResolveCollision` to resolve a single collision
- [x] `ResolveCollision` should...
    - [x] `CorrectObjects`, pushing them out of their respective objects
        - [x] Push should depend on the ratio of the masses
        - [x] Static and Kinematic objects assume infinite mass vs dynamic objects
        - [x] Kinematic vs Kinematic resolves based on masses
        - [x] Static objects never move, and should fully push the other object if it is not static.
        - [x] Two static objects do not correct at all.
    - [x] Calculate **normal impulse** at point of collision. 
    - [x] `Rigidbody2D::ApplyImpulseAt` to both objects (`impulse` to A, `-impulse` to B)
- [x] `Rigidbody2D::ApplyImpulseAt( vec2 worldPos, vec2 impulse )` implemented
    - [x] Impulse uses force to apply an instant change in velocity
        - `delta_velocity = impulse * inverse_mass`
    - [x] Ignore `worldPos` for now, it is there for when we apply rotational forces

------