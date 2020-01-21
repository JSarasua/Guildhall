Jonathan Sarasua
SD1-A2
Due: 9/18/2019

Known Issues:
	The rubric has asteroids taking 1 damage when running into any entity, but the demo game and video have it
	dying when the player collides with one. I followed the demo game.

	The rubric has the entity constructor taking a pointer and a starting position. This makes sense if I'm
	instantiating a new bullet/asteroid every time I need a new one, but I created an array of bullets and
	asteroids on the stack. They get their positions when I activate them, so I have the position default to
	Vec2(0.f,0.f) in entity.hpp



How to Use:
Starting Starship opens a windowed application where a triangle moves across the screen.
Esc: Closes application
T: Slows the starship
P: Pauses the application
N: Respawns starship but only if dead
O: Creates a randomly generated asteroid but only if less than the max of 12
Space: Shoots a bullet from the nose of the player ship in the direction of the player ship
F1: Toggle debug mode.


Deep Learning
Every time I started working on Starship playable, I worked in bursts. I would be extremely productive for a few hours
before stopping. This is great, but I noticed I have a bad habit of committing changes at the end of each of these bursts
instead of after completing a single task. My commits have multiple changes all together which is a poor strategy if
I ever need to go back because my changes are all tangled together. In the future I will commit changes more regularly,
so coming back to changes days, weeks, or years down the road I will be able to see clearly what changes were attached to
what.