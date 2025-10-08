This github is for personal project development and includes some cool things. 

Instructions to configure development environment on windows
(1) Install VSCode (https://code.visualstudio.com/download)

(2) Install MSYS terminal (https://www.msys2.org/) and packages
	- After download run the following commands
		pacman -Syu
		pacman -S mingw-w64-x86_64-toolchain base-devel
		pacman -S git
		pacman -S cmake
	Check successful download with the following commands
		python --version
		cmake --version
		gcc --version
		g++ --version
		git --version
	Add downloaded MSYS packages to system path so they can be run anywhere on windows
		package locations "C:\msys64\mingw64\bin"
	Set default terminal in VSCode to be MSYS
		Ctrl Shift P
		Search for "Preferences: Open User Settings (JSON)"
		Copy following configuration code into file
			{
			"terminal.integrated.profiles.windows": {
    				"MSYS2": {
        				"path": "C:\\msys64\\usr\\bin\\bash.exe",
        				"args": ["--login", "-i"]
    				}
			},

			"terminal.integrated.defaultProfile.windows": "MSYS2"
			
			}
        Restart terminal in VSCode to get MSYS terminal

(3) Clone repo using ssh or https
