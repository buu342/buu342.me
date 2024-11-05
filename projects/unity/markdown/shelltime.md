Shell Time is a run and gun platformer that I made in 2 weeks. In this game, you play a super heroine who gets a long-awaited break in her boredom when aliens attack her city. Equipped with her dual pistols and laser sword, you shoot your way through the alien infested rooftops, buildings, and onto their ship.

The demo features bullet time (which affects sound and music), a seamless music transition system, a scripted camera that can focus on specific parts of the level to guide the player, and a custom directional sound implementation. There's also some other neat tidbits, like how the boss syncronizes to the music.

Before this project, I had already done some experimentation with Unity throughout the years, but this was my first time putting "something" large together. I was familiar with how Unity's character controller and sound system is *lacking*, so I had to write a custom character controller (so that I could get some movement physics which actually felt good and supported things like [Coyote Time](https://en.wiktionary.org/wiki/coyote_time)) and sound system (so that sound effects and music can be affected by slow motion, as well as so that the music can transition at the right points of the track).

### Controls

The game has a built-in tutorial covering the controls, but they will be written here just in case:

* D to move forward, A to move backwards
* W to jump (you can double jump)
* Aim with the mouse
* Pressing left click performs a melee attack which reflects projectiles. Holding it shoots your guns
* Right click slows down time, which can be used to aid in combat and give you more time to think

You can also use the 1 to 4 number keys to skip to different sections of the demo. Sometimes pressing the button will not jump to the correct level, so you'll need to press it again. Jumping between levels can potentially break scripting... Honestly it's there as a bodge so you can check out more of the game without having to play through it all.

### Asset Credits

Almost everything in this game was made by me, except the following:

* Music was made by Hidenori Shoji, taken from the Yakuza franchise of games. I knew I wasn't going to have time to make music from scratch, so I borrowed...
* Some of the particle textures were not made by me, they were taken from a myriad of sources. Notably, Speed Brawl and Guilty Gear. I wanted to do these myself but ran out of time since I wanted this done in 2 weeks.
* Some sound effects were taken from some audio effect CDs I have (Most came from "1001 Sound Effects"), but I mixed and combined them myself.
* I use two shaders written by Ryan Ross. The [Toon](https://github.com/IronWarrior/UnityToonShader) shader and [Outline](https://github.com/IronWarrior/UnityOutlineShader) shader. I would have written them myself, but I didn't feel like wasting an entire day learning how to take my existing HLSL code and wrangle it to work with Unity's stuff (doesn't help that the shader documentation is a mess). I did have to make several tweaks to both shaders to get them to play nicely with each other, as well as adding in a cool video glitch effect which I learnt about by reading the source code in [this repository](https://github.com/keijiro/KinoGlitch).

Everything else, I made it. I have included the Raw Assets in the project's [Google Drive](https://drive.google.com/drive/u/1/folders/1LjgYdFYKpYOtT2UIXvhdY1lmiuzb3UDS).
