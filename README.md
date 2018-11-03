### VEngine is a library to base your graphical application on, like, a game or a 3d spectrum visualizer, or many other.

##### What we got here
- Graphics system using Vulkan with help of GLFW3 (also provides input) https://github.com/glfw/glfw
- Image loading using stb_image https://github.com/nothings/stb

##### What we got here

Compiling under Windows:
- Go to **dependencies** folder and run the .bat file.
- Ensure you have Vulkan SDK installed in official way
- Open the solution and compile it

Compiling under Linux:
- Download, unpack, and build the Vulkan SDK
- Download glfw most recent release, make and then sudo make install it
- Clone https://github.com/nothings/stb somewhere
- Open env.sh and adjust **STB_INCLUDE_DIR** and **VULKAN_SDK** to point to correct directories
- In the terminal where you will be compiling execute **source env.sh**
- Run **make**

##### Urgent things to do
- Images mipmapping with handy api
- Images rebuffering
- Support for custom vertex attribs layouts
- Semaphores abstraction to handle synchronization automatically with "depending on" style rules
- Thorought logging support with various levels of logging
- Maybe some debugger

The software is licensed under MIT license.
