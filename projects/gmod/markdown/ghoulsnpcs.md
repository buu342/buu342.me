[Ghoul's Forest 3](https://www.youtube.com/watch?v=rphe8-Kqz44) is a horror WAD for DOOM 2, made in 2006 by CutManMike. It has a really simple premise: you're in a dark spooky forest with only a bow and arrow, and you must kill the four ghouls before they kill you. Each ghoul has their own special ability and/or gimmick, so you also have to figure out how to best deal with them. I'm a big fan of the original WAD and its multiplayer "sequel" Ghouls vs Humans, but I don't quite remember why I suddenly had an intrest in porting these enemies over to GMod. I think it was mostly the fact that I wanted to do some work in AI, and Ghoul's Forest 3 had a relatively small roster with pretty varied abilities. 

I wrote an animation system to mimick how ZDoom's DECORATE works, so that those could be as faithful to the original as possible, and then I got to work on the AI. These NPCs use a mixture of custom movement code and Source Engine's Navigation Meshes to help them hunt down the player. The addon [was praised by CutManMike](https://www.youtube.com/watch?v=aFbd3M-arO0&lc=UgzYMqzS4_mkjiftcpV4AaABAg) for being "surprisingly faithful to the source material".

The abilities of the Ghouls are as follows:
* Jitterskull, instead of moving, teleports a few meters towards you in an erratic pattern every second. Upon getting close enough, he will charge directly at you.
* Sjas flies really quickly around the map. When hit, he teleports to a random place in the map.
* The Creeper slowly moves towards the player. When hurt, the creeper flees really quickly.
* The Yurei slowly teleports around the player, appearing only for a second before disappearing. Each time she pops into existence, she's closer. If she gets close enough, she kills you. 
