# Description
This codebase for personal project development and includes a custom math library, app scheduler, and 
other custom infastructure.

## Setup
Here are instructions for to configure the development environment on windows. This is how I did it but 
feel free to set it up however you'd like.

1. Install VSCode (https://code.visualstudio.com/download)
2. Install MSYS terminal (https://www.msys2.org/) and required packages
	- After download run the following commands
		'''
		pacman -Syu
		pacman -S mingw-w64-x86_64-toolchain base-devel
		pacman -S git
		pacman -S cmake
		'''
	- Check successful download
		'''
		python --version
		cmake --version
		gcc --version
		g++ --version
		git --version
		'''
	- Add downloaded MSYS packages to system path so they can be run anywhere on windows
		- Most likely MSYS package location: "C:\msys64\mingw64\bin"
3. Set default terminal in VSCode to be MSYS
	- In VSCode use the shortcut 'Ctrl Shift P'
	- Search for 'Preferences: Open User Settings (JSON)'
	- Copy following configuration code into file
		'''
		{
		"terminal.integrated.profiles.windows": {
				"MSYS2": {
					"path": "C:\\msys64\\usr\\bin\\bash.exe",
					"args": ["--login", "-i"]
				}
		},

		"terminal.integrated.defaultProfile.windows": "MSYS2"
		}
		'''
	- Open a new terminal in VSCode and it should be configured to be MSYS

4. Clone repo using ssh or https
5. Navigate to the personal_project directory and run './execute.sh' to verify setup
