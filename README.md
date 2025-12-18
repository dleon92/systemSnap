# systemSnap
C++ software for windows and linux to fetch and save the specifications the of the running machine and save it for later consultation

This application came from the need i have for a tool that grabs the information of the running machine, stores it and allow later consultation.
Even tho i am never wanted to become a TI support guy, somehow i ended up becoming one for my family, friends e work collegues and now i have under my supervision around 20 machines and often people came to me asking "can i upgrade my ram?", "can i upgrade my gpu?".
With this application, i want to be able to respond these questions quicker, maybe even remotely.
I am a junior software developer, leading to a full-stack career.
This is my first code almost all made by myself.
I have some help from gpt and alikes to solve some problems, but the code is still rough.
The development of this code will mirror the development of my programming habilities. 

MAIN SOFTWARE TRACES
- The code must work on linux and windows machines;
- Must be lite, easy and no brainyu;
- Must be made of as few files as possible;
- Must be portable, run-ready;
- Run on terminal or other GUI as lite as terminal.

HOW THE PROGRAM WORKS
- It provides the user with 3 options (show files, scan pc and leave);
- At "scan pc", it will detect what OS the user is running (windows or linux) and run the appropriate code for that OS;
- Then, it will display the specfications on screen and ask if the user want to save it or not;
- If not, it returns to the main menu. If yes, it will ask for the filename and create a .json on the folder /PCs and then comeback to the main menu;
- At "show files", it will scan for the existence of "PCs" folder and for ".json" files on it. Then, it will create a list with the .json filenames it found and create an index and ask what file the user wants to open;
- The user will input the index he wants to open and the code will check if the index exist on the list of files created previously. If not, will display an error. If yes, will display the specifications of the refered machine;

MILESTONES
- Translate to english;
- Ad colors and markers to the other stages of the application;
- Shrink the code lenght;
- Add comments to it;
- Improve the code in general;