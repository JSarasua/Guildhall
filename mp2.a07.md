### Goal 

- [x] Remove screen wrap and bottom of screen bounce.
    - [x] Add some static world geometry (at least a floor) to catch falling objects.
    - [x] Destroy objects that fall out of world bounds
        - [ ] Option A: Define a world bounds and draw it so users can see where objects should destroy at.
        - [x] Option B: Destroy objects that leave screen either left/right/bottom, but NOT top. 
- [x] Polygon-v-Polygon Intersection Check (using GJK)
- [x] Polygon-v-Polygon Manifold (normal/penetration) - object push out correctly
- [x] Polygon-v-Polygon Contacts - Objects generate one or two contact points.  
    - [x] Manifold now returns a contact edge (two points) instead of just a single contact point.  For cases where only one contact exists, 
          return an edge with the start and end point being the same. 
- [x] Apply Impulse using contact points
    - [x] Calculate impulse using the center of the contact edge.
    - [-] When applying impulse, use the point on the edge closest to each body's center of mass.
- [-] To make friction/bounce more stable - calculate normal impulse first, apply it, and then generate tangent impulse, and apply it. 

**Normal can sometimes flip, but it doesn't cause any obvious problem This only happens when the normal is very small
**Jitters