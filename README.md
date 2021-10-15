# obs-birddog-p200-controller
A dockable OBS Studio plugin for controlling the BirdDog P200 camera.  
It uses the visca over ip protocol, so it might work with other cameras.  

## Controls
The main widget works like a normal joystick, but it has a few additional actions:  
- Scrolling zooms in and out
- Pressing the middle mouse button stops zooming immediately
- Pressing the right mouse button stops movement and zooming immediately

## Screenshots
![Dock screenshot](https://i.imgur.com/n7ZcaIt.png)  
![Settings screenshot](https://i.imgur.com/Iq9iOI8.png)

## Build from source
- Build OBS Studio: https://obsproject.com/wiki/Install-Instructions
- Add the files in this repository to UI/frontend-plugins/obs-birddog-p200-controller/
- Add "add_subdirectory(obs-birddog-p200-controller)" to the end of UI/frontend-plugins/CMakeLists.txt
- Rebuild OBS Studio

[Pre-built binaries](https://github.com/jvkolyadich/obs-birddog-p200-controller/releases) are only available for Windows 64-bit.  
