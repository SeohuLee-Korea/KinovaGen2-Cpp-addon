# Kinova addon for C++ console applications
This repository contains my customized addons for Kinova C++ console applications.

As a student, I faced difficulties in developing practical applications for my research, projects, and experiments.
Managing multiple APIs or libraries within a single main() function was especially challenging.

To address this issue, I decided to develop addons for simpler and easier integration.

These addons aim to facilitate simple, easy, and high-level implementation, prioritizing usability over deep and low-level utilization.
While the current functions may be limited, I intend to continuously add and upgrade them.

This addon is developed based on Windows Microsoft Visual Studio 2022.

I hope these addons prove helpful to you.


# How to use (For beginners)
You have to manually download API and library to use this addons. 

1. Project configuration  
     a) Create a new project or open your project where you want to add this addon. (Check "Place solution and project in the same directory.")  
     b) Copy and paste "kinovaGen2" folder into the solution directory.  

2. Project Properties setting  
   a) Project -> Properties -> C/C++ -> General -> Additional Include Directories -> Edit -> Add a new line ".\kinovaGen2\include"  
   b) Project -> Properties -> C/C++ -> General -> Additional Include Directories -> Edit -> Add a new line ".\kinovaGen2\src"  
   c) Project -> Properties -> Linker -> General -> Additional Library Directories -> Edit -> Add a new line ".\kinovaGen2\lib"  
   d) Project -> Properties -> Configuration Properties -> Debugging -> Environment -> Edit -> Add a new line "PATH=$(SolutionDir)\kinovaGen2\lib\x86;%PATH%" or "PATH=$(SolutionDir)\kinovaGen2\lib\x64;%PATH%"  

3. Kinova Gen2 SDK Download  
   https://www.kinovarobotics.com/resources?r=79301 -> Gen2 SDK v1.5.1  

4. Library location  
   a) Find "CommandLayer.h", "CommunicationLayer.h", and "KinovaTypes.h" in your_download_path/JACO_SDK/API.  
   b) Copy and paste these files into ".\kinovaGen2\include".  
   c) Find "CommandLayerEthernet.dll", "CommandLayerWindows.dll", "CommunicationLayerEthernet.dll", and "CommunicationLayerWindows.dll" in your_download_path/JACO_SDK/API/x86 (32-bit) or your_download_path/JACO_SDK/API/x64 (64-bit).  
   d) Copy and paste these files into ".\kinovaGen2\lib\x86" (32-bit) or ".\kinovaGen2\lib\x64" (64-bit).  

5. Add source codes into the solution, and use them for implementations.  


























