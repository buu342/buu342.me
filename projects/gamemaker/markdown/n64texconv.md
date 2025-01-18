When I first got into the N64 scene, I needed a way to convert images from common image formats (such as PNG and BMP) into something which could be added to a project. There weren't any good tools for the process, and Nintendo only provided a tool for modifying already existing textures that had been converted into Color Index format. So, I wrote my own, and I did it in Game Maker because GM was convenient at the time. Why spend a lot of time fighting a bunch of C++ libraries when I have access to a program that already allows me to dynamically import images in a variety of formats out of the box?

I originally wrote the tool in Game Maker 8 because GM:S sandboxes everything, so importing external files at runtime is *a pain*. I did eventually port it over to GameMaker: Studio to make use of performance improvements and to have access to shaders to allow for more accurate previewing, solving the external assets problem by writing a custom DLL in C++ . The tool has support for all of the texture formats used on the N64:
* 32-Bit RGBA
* 16-Bit RGBA
* 16-Bit YUV
* 16-Bit Intensity + Alpha
* 8-Bit Intensity + Alpha
* 8-Bit Intensity
* 4-Bit Intensity + Alpha
* 4-Bit Intensity
* 8-Bit Color Index
* 4-Bit Color Index

The tool will reduce the sprite's bitrate if it does not match the selected format, and fully supports image alpha and custom alpha masks. One can also do some minor manipulations on the image, like chunking or resizing it. The exported files can be in either header file format, as a compilable C file, or as a packed binary file. 

On top of all of the features, the tool is also fully documented, including a compiled HTML manual that covers everything about N64 image formats, different manipulations that can be done to the images on hardware, and how to use the tool. An online version of the manual is available [here](https://buu342.github.io/GML-N64TextureConverter/).
