# dsb
Deadly Sea Battle (DSB) - AI competition
DSB program is aimed to find the best algorithms available for humans
for placing and shooting boats in classic “sea battle” game.
AI algorithms compete against each others to find the most optimal way for defeating your enemy in this game. 

For detailed documentation, see doc/ directory.

#Install dependencies:
-SDL2
-GLEW

For ubuntu it looks like this:
sudo apt-get install libsdl2-dev libglew-dev

To compile the project, run 'make' in the base directory (or you can run 'compile.sh' from the shell).
To run AI match, run compiled 'dsb' binary. It runs in OpenGL visualization mode if lauched without parameters;
run 'dsb --help' to see all available options.

Run competition/run_ai_competition.sh to run matches with all available plament/algos pairs and see the stats/AI-winner.
