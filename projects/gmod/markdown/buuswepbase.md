In Garry's Mod, there exists a concept known as "Weapon Bases", which is basically a parent weapon class for other weapons to inherit from. As I have spent a lot of time writing scripted weapons (SWEPs) for Garry's Mod, I wrote my own SWEP base called Buu342's Weapon Base. This weapon base was used in a lot of my earlier weapon packs, but it was ultimately quite flaky and I chose to rewrite it from scratch instead of accruing even more technical debt. I have been using the weapon base for personal projects for quite a few years, but I ended up publicly releasing it in 2021 for other people to use as I considered it to be in a satisfactory state. 

My weapon base, on top of being heavily documented, features:
* Full clientside prediction so that laggy players have the best experience
* Shotgun reloading
* Sniper scope with swaying
* Ironsights
* Dynamic LUA-based animations for viewmodel sprinting, near-walls, jumping, walking, idling, breathing, and swaying
* Barrel smoke trail
* Low ammo clicking
* Support for weapon attachments, such as silencers
* Multiple crosshairs, customizable too
* Firemode switching, including built-in burst-fire mode
* Run sliding
* Laser pointer that respects distance to walls (Also in thirdperson)
* Flashlight that follows the gun's barrel (Also in thirdperson)
* Clientside magazine dropping
* Holding breath while scoping to steady the aim
* Shotgun door destruction
* Lots of clientside and serverside settings
* Thirdperson animations

This SWEP base is used in both my [RAGE SWEPs](ragesweps.html) and my [Dead Space Plasma Cutter](plasmacutter.html) addons.
