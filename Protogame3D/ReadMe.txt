Jonathan Sarasua
SDST-A1
Due: 6/3/2020

W = Move Forward
S = Move Backward
A = Move Left
D = Move Right
C = Move Up
Space = Move Down
Mouse = Rotate camera
F1 = Play Sound

Problems:
Currently there is a bunch of weird behavior occurring. 
	1. I get a D3D11 error that only occurs on my machine, but it didn't when I had Butler compile it.
	The error doesn't stop the game running, but it does show memory leaks that aren't part of my game.
	2. I sporadically got a write access error on my inputsystem when closing down. It seemed to stop happening
	after I changed the order of my shutdown, but taking a deeper dive would probably be a good idea. The error message is
	like the below except **this** changes each time. The value stays constant:
		Exception thrown: write access violation.
		**this** was 0x1A276D9D9D0.
	3. For a little while I started getting Visual Studio errors, but it may have been caused by having 2 visual studios open