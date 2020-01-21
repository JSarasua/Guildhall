Jonathan Sarasua
SD1-A8
Due: 12/11/2019

Known Issues:
	Not using GameConfig to set CLIENTASPECT.
	There are still a few locations that look at GameCommon.hpp instead of reading from xml
		example: Game and UI camera dimensions

	Debug drawing no longer works

	Tags can only be set from Mutate and are only really used by FromImage



How to Use:
Starting Testgame2D opens a windowed application where you control an Actor with an xbox controller
or ESDF.
	Xbox controls:
		Left Joystick: 	Moves the Player
			    Y:	Toggles DevConsole	

	Keyboard controls:
		Esc: 	Closes application
		T:	Time moves at 0.1 times normal speed
		Y: 	Time moves at 4 times normal speed
		P: 	Pauses the application
		F3:	Activates no clipping mode where the player can move through all objects
		F4:	Activates debug camera where the entire map is visible
		F5:	Rebuilds the world
		F8:	Rebuilds the game
		E,S,D,F	Moves the Player


Deep Learning
Adventure went smoother than any of my previous assignments. I iterated on it almost every day including
over thanksgiving break. I had almost all functionality working a week before the deadline. My biggest
challenge was moving into the bonus assignments like Tags. I realized I still have the problem where I 
don't spend enough time planning what I'm going to do for more complicated pieces. I often put code in
the incorrect spot because I'm getting it to "just work" instead of thinking about how other classes may
use the code.

Over the break I plan on rewriting adventure to take advantage of how I now want to use tags.