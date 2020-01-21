Jonathan Sarasua
SD1-A3
Due: 9/30/2019

Known Issues:
	Pausing doesn't stop the xbox controls from updating the playerShip. Mouse and Keyboard still works.

	Debug mode only works for the assignment to entities.

	Beetle and Wasp enemies still chase spot of dead players

	Shooting with the space bar doesn't get the rapid fire power up.



How to Use:
Esc: Closes application
T: Slows the starship
P: Pauses the application
N: Respawns starship but only if dead
O: Creates a randomly generated asteroid but only if less than the max of 12
Space: Shoots a bullet from the nose of the player ship in the direction of the player ship
	Also starts the game if on attract screen
F1: Toggle debug mode.
F8: resets the game.

XBOX Controls:
A: shoots bullets
Start: restarts the game/respawns the player ship if dead/starts game if on attract screen
Left Joystick: Thrust and turn


Deep Learning:
On my previous deep learning I talked about being productive only in bursts and having huge commits. I have changed the way I program
by working consistently for at least an hour after class every day, and I have tried with moderate success to stop and commit changes
regularly. I will keep on working at it.

On each of the past three assignments I have started by looking at the rubric, getting a good idea, and then almost never checking 
the rubric again. This has caused problems where I miss minute details on each piece of the assignment. For example my
wasp and beetles had 1 hp instead of > 1, debris alpha started at 255 and died only after a few seconds instead of half alpha that 
died after 2 seconds, etc. Each of these items were small but added up and made the final day miserable as I had to go back and fix 
almost every piece of the assignment. Also, I completely forgot about variable time until after my bonus features were implemented.
By the time I finished everything, I didn't have time to refactor my code to fix some of the obvious problems. For example, each of
my entities never calls the Entity::Update because I never implemented it! If I went through the document and checked off pieces as
I went, my code would be better because I wouldn't be having to hack it together last minute and would have allowed me time to go back 
and clean up some of the mess I made. I'd also have a better idea of how long the assignment will take me, so I can plan better.