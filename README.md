OpenGL-Assignment2
==================

Assignment 2 for COMP308 - Computer Graphics

Troy Houston

Done:
Core 	- Draws the skeleton correctly from the .asf file.
		- Allows rotation of the skeleton using the W, A, S, D keys.

Extension	- Has a custom pose "hands'n knees". I.e crawling/dog pose.
			- Has a walking pose
			- Has a sitting pose
			- Change pose using 'p'

Challenge	- 'Motion capture player' created.
			- Reads from 'priman.amc' to display the walking animation.
			- Basic player controls 'Play','Pause','Stop','Rewind','FastForward'
			- Rewind = Change direction (opposite of current).
			- FastForward = Increase speed (frame rate)
			- Controls displayed in the RIGHT CLICK GLUT_MENU
			- Camera position changed automatically for animation accordingly.

AKA Completed Everything


How to compile:
Included is a zip of the src and other required files.
Use the included files to create a C++ project with OpenGL.


How to use:
The Majority of the controls are printed to the screen. However:
Use: W, A, S, D, to control the view. X & Y axis rotation by 5 degrees.
Use: Z, X, to zoom in and out.
Use: P, to toggle between the Poses (default -> sitting -> walking -> custom).
Use: 1, to toggle between the Floor Shown (None -> Yellow -> Disco!)
Use: 2, to toggle joints on and off.
Use: 3, to toggle joint axis on and off.
Use: 4, to toggle the scene axis on and off.
Use: 5, to toggle if the head is shown.
Use: 6, to toggle between characters (bone -> body).

RIGHTCLICK for GLUT MENU:
Play - Plays the AMC animation from "assets/priman.amc" and changes the camera accordingly.
Pause - Pauses the current animation in it's state.
Stop - Stops the animation and returns to the default pose.
Rewind - Makes the animation play backwards.
Fast Forward - Increases the move speed of the animation.

The program should take the argument of the .asf file:
in this case:
"assets/priman.asf assets/priman.amc"



