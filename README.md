# Zeabus software
This repository start on August 10, 2018

## Table of contents
- [Git tutorial](#tutorial-for-git-beginner)
   * [Windows user](#windows-user)
   * [Linux or macOS user](#linux-or-macos-user)
- [Zeabus vision](#zeabus-vision)
   * [Camera](#how-to-open-camera)
      + [Front camera](#front-camera-only)
      + [Bottom camera](#bottom-camera-only)
      + [All camera](#open-all-camera)
    
    
## Tutorial for Git beginner
### Windows user

__First way (beginner's way: Not recommend)__
1. Click "Clone or download" (Green button)
2. Click "Download ZIP"
3. Unzip and enjoy!!!

__Second way__
1. Install [git bash](https://git-scm.com/downloads)
2. Right click on your destination folder
3. Click git bash here
4. paste this command `git clone https://github.com/zeabusTeam/zeabus_software.git` in git bash
5. Enjoy!!!

### Linux or macOS user
1. Open your terminal
2. `mkdir <your directory name>`
3. `cd <your directory name>`
4. `git clone https://github.com/zeabusTeam/zeabus_software.git`
5. Enjoy!!!

[Back to Top](#zeabus-software)

## Zeabus vision
### How to open camera
#### Front camera only
```
roslaunch zeabus_vision front.launch
```
#### Bottom camera only
```
roslaunch zeabus_vision bottom.launch
```
#### Open all camera
```
roslaunch zeabus_vision camera.launch
```

[Back to Top](#zeabus-software)

### How to run mission code
(temporary reconstruction)

[Back to Top](#zeabus-software)
