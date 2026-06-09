# SD1-A04: Starship Gold

## 1 How to Use

### 1.1 Build and Run

Open the `Starship\Starship.sln` file in Visual Studio, then press `CTRL + SHIFT + B` to build the solution. `Starship_Release_x64.exe` should be under the `Starship\Run\` folder.

### 1.2 Game Control

The game assumes that the player uses a Xbox Controller for gameplay input. Keyboard is mainly used for cheat codes.

##### **<u>Xbox controller input:</u>**

- Use `LS` to move around and set the player's facing direction.
- Hold `RT` to fire continually. You can also press `A` button to shoot a single bullet. 
- When player has died, press `START` button to respawn the ship.

If you don't have a controller, use keyboard for basic movements (advanced features limited) :

- Use `'S'` and `'F'` on your keyboard to turn the player ship direction.
- Use `'W'` to speed up in the current forward direction.
- Use the `SPACEBAR` to shoot.
- When player has died, use key button `'N'` to respawn the player ship.

Notice that if a controller is connected, the movements input from keyboard will not be accepted.

##### **<u>Cheat codes:</u>**

- Use `F1` to toggle the debug drawing.
- Use `F8` to restart a new game.
- Use `'C'` to kill all the enemies.
- Use `'I'` to generate new asteroids.
- Holding `'T'` slows the rate of time to 10% normal.
- Pressing key button `'P'` toggles the game to pause / unpause.
- Pressing `'O'` to run 1 frame and then pause.



## 2 Known Issues 



## 3 Deep Learning

Compared to the small projects I wrote before, working on Starship involved a lot of "adding new features to existing code" situations. A04 in particular required my own decisions and thoughts on this kind of progress, which exposed me to these situations more directly. 

Now I've become more accustomed to starting from the side that directly uses the new thing. Firstly create the new method that will be called, then break it down based on what each part actually does and creates new methods again. I also tend to first build the simplest version possible before thinking about whether the functionality or code needs to be made more complex.

The benefit is that this is less mentally demanding, and the code is easier to read when I look back later because it was built in a way that feels intuitive. I’ve realized this is actually more efficient than trying to “solve everything perfectly in one go”.

