# Game Agar.wp
Simpler version of Agar.io

# Specifications
Library requirements are specified in requirements.txt

# Game
Game allow us to play simpler version agar.io. Steer your ball using arrow keys, try to get as big as you can.

# Before running the game
User have to change the server ip addres in module network.py, the ip addres is displayed when the server starts - in line 15L self.server = "..."

# Running game
To run the game user have to type 'python3 client.py' type his name and he joins the server and the game starts.

# Mechanics
- Each game lasts 3 minutes
- Player loss mass proportional to his mass in constant time periods
- Playe with bigger mass became slower to a certain point

# Extras
Server host is able to change the game mechanics changing the game parameters in class Parameters() in module player.py.
The game can handle as many players as fit in the game board.

# Problems with concurrency and global variables
Update: Each player after eating a cell food is spawning a new one, which avoid the problem.
[There could be a race condition in which we will spawn much more food, then we expect cause each player is spawning it]

Update: Each client thread focus on his player updating, position and mass, the only change which player can do  on other player is adding mass while he is being eaten by him.
[Small problems with changing player status, there are also some races for example one thread is handling eating one player by other and another thread is updating massloss, it could happen that the player could be spawned with the bass before beaing eaten.]

[To reduce the chance of occurrence such situations for example massloss is performed for each thread just for his client or collision is checking only players with higher number to avoid two times adding mass to player which eat cause if we would check all players two thread could handle the situation and there could occur a race.]
