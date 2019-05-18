# BombermanUE4

Developed by Misael Lara.


Programmer-art version of the old Dyna Blaster/Bomberman game in 3D 

Based on Top-Down UE4 template that requires a mouse to be played.


## Instructions

### Gameplay

- Move the mouse to locate the cursor to an empty square
- Press the left mouse button to select the empty square in the game board
- Press the right mouse button to spawn a bomb
- Wait after the bomb blast to spawn another bomb.
- The bomb blast could destroy wooden walls, pickups and players.
- Some wooden walls have power-ups
- The game ends after 5 minutes or if the player is reached by a bomb blast.

### Power-ups

- Yellow sphere gives 3 bombs that can be spawned simultaneously.
- Purple sphere gives the next 3 bombs the ability of longer blasts.
- Power-ups are not collectable.


## Development

Used Unreal Engine 4.22 with Visual Studio 2017

All the game was developed in C++ except from the camera position which is fixed with a blueprint.

The game development begin in the class WallsGenerator.

The game board is constructed randomly and dynamically based on structure FBoardGrid (which is a member variable named "Board" in WallsGenerator class)
FBoardGrid is a TArray of USTRUCTS to simulate a bidimensional array since UE4 doesn´t support bidimensional TArrays.
Each position in the array represents a square in the game board. 

Each one will hold:
- A value that represents the type of wall that is going to be spawned in that position in the game board
- A reference of the actor that is in that position.

Example: Board.Rows[0].Columns[0] = WE_Undestructable   represents that the top-left square in the game board will spawn a Undestructable wall
         Board.Rows[0].ActorsInGameBoard[0] = Wall      represents that in this position of the game board a Wall is spawned.

ActorsInGameBoard TArray is of type AActor since each square in the game board could have any kind of actor (walls, pickups, nothing) 

The method GenerateBoard creates the board in its initial state. It leaves the corners of the game board free to spawning the players in that places. Then it assigns the undestructable walls since they are in fixed positions (when X and Y are odd).
The leaving places it gives values randomly between a destructable wall or an empty space to walk. 
  
Destructable walls are counted and stored in a temporary array which only holds the position of each destructable wall.
To know how many destructable walls must have a power-up inside them, just requires to multiply the total of elements of this temporary TArray by the percentaje 0.3
Finally it takes randomly an element of this temporary TArray to get the position of the Destructable wall and change it randomly in the Board structure with the value of a pick-up.

Walls are spawned inside the BeginPlay method assigning dynamically their material (destructable or undestructable) with a call to the SpawnActorDeferred UE4 method.


## Future development

- AI and multiplayer modules
- Add music
- Improve gameplay and 3D models
- Add boss fights
- Add social media connectivity
- Add leaderboards
- Add 3d special unlock option for VR mode








  
  

  








