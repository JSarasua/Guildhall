Jonathan Sarasua
SD1-A4
Due: 10/10/2019

Known Issues:
	DrawAABB2 is DrawAABB2Filled because I have a separate DrawAABB2 which is an outline of an AABB2

	If you use F3 to go through stone but move onto the boundary wall, the game will throw an error.

	Game is still being passed to each object below it (World, Map, entity, etc.) instead of using
	a global pointer.



How to Use:
Starting Protogame2D opens a windowed application where you control a tank with an xbox controller.
	Xbox controls:
		Left Joystick: 	Moves the tank
		Right Joystick: Moves the tank gun

	Keyboard controls:
		Esc: Closes application
		T:	Time moves at 0.1 times normal speed
		Y: 	Time moves at 4 times normal speed
		P: 	Pauses the application
		F1:	Activates debug mode where entities have circles drawn around them where cyan is the physical 
			radius and magenta is cosmetic
		F3:	Activates no clipping mode where the tank can move through solid objects like Stone
		F4:	Activates debug camera where the entire map is visible
		F8:	Rebuilds the game


Deep Learning
While coding during the game jam, I realized I have a habit of coding everything in one piece. For example, I would
code populating the tiles types in one huge double for loop with over 10 if else conditions for each of the cases.
While it did work, it became incredibly difficult to refactor and took awhile to read. This may have been more efficient,
but I need to focus on readability of code. After using Squirrel's method of having separate loops for building the tile
map, my code instantly became more understandable. Another situation was getting the AABB2 for the tiles around the player.
I pass the x,y coordinates to get the index of the tile to get the AABB2 all on one line. If I split these pieces up and
labelled them, the code would be much easier to read. In the future, I'll take the extra minute to think about if splitting
up a piece of code will make the code more readable.