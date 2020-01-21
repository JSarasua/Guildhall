Jonathan Sarasua
SD1-A6: Incursion Alpha
Due: 11/4/2019

Known Issues:
	Y/T keys stop working when game changes state sometimes

	Floodfill doesn't work correctly on final map

	Lives are part of map instead of Player, but are still passed when the map is completed

	Tank AI handles corners poorly and still collides with walls when no player targeted

	Game is still being passed to each object below it (World, Map, entity, etc.) instead of using
	a global pointer.

	Enemies can spawn on top of each other

	Sometimes using debug camera, the game's victory screen will not appear



How to Use:
Starting Incursion opens a windowed application where you control a tank with an xbox controller.
	Xbox controls:
		Left Joystick: 	Moves the tank
		Right Joystick: Moves the tank gun
		Start: 		Starts/Pauses/Unpauses Game
		Back: 		Pauses/Return to attract screen/Exits game
		A: 		Shoots
		B: 		Respawns Player

	Keyboard controls:
		Esc: 	Pauses/Return to attract screen/Exits game
		T:	Time moves at 0.1 times normal speed
		Y: 	Time moves at 4 times normal speed
		P: 	Pauses/Unpauses the application
		F1:	Activates debug mode where entities have circles drawn around them where cyan is the physical 
			radius and magenta is cosmetic
		F3:	Activates no clipping mode where the tank can move through solid objects like Stone
		F4:	Activates debug camera where the entire map is visible
		F8:	Rebuilds the game


Deep Learning
During this assignment I didn't stall like the previous one but I didn't flow as well I hoped either. I had to deal with my 
unfinished project from the previous assignment. Almost every day I worked on something but I was very slow on a few of the tasks. 
I noticed I'm afraid to spend time to refactor code because I don't believe I'll be able to finish the assignment. This is a terrible 
practice because part of the goal of the assignment is to clean up our code base. I must try harder in the future to 
spend a day or two each assignment to cleaning up old work. The goal is not to write a fully functional assignment but to be
a better coder. 

Looking back at a previous deep learning, my comments in Perforce are much better. I still haven't had a time where I had to revert
to an older version, but I can more easily tell what I worked on in the past.