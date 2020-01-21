Jonathan Sarasua
SD1-A1
Due: 9/5/2019

Known Issues:
	The rubric has putting the declaration of the pointer to g_app in App.cpp. I put it in Main_Windows.cpp 
	because it owns App and (in my mind) should be the one to instantiate and delete it.

	The comments say:
		HGLRC g_openGLRenderingContext = nullptr; // ...becomes RenderContext::m_apiRenderingContext
	Since the above is used by CreateRenderContext() which is staying in Main_Windows.cpp I decided to leave it
	there until we have a better way of moving CreateRenderContext()

	App::EndFrame is empty because all of the gl



How to Use:
Starting Starship opens a windowed application where a triangle moves across the screen.
Esc: Closes application
T: Slows the starship
P: Pauses the application


Deep Learning
When I started working on Starship, I tried not to get assistance from anyone. I was honestly embarrassed that I may
need assistance even though I'm rusty at C++. It took getting stuck trying to move CreateOSWindow() and 
CreateRenderContext() into RenderContext() for me to realize my mistake. I spent hours trying to do something I 
didn't need to do. Even if I did, there is a limit to what you can gain from beating your head against
the wall. When I finally did ask another student, I realized my mistake. In the future, if a problem is giving me 
fits for over an hour, I'll talk to the TAs, teacher, or other students about it before wasting more time. Also, 
by just talking about a problem aloud I can gain insights into how I may address it even if I can't get a direct 
answer. In other projects or even in a job, there will be times where I will read an assignment wrong and getting
clarification can solve headaches my pride won't solve. 