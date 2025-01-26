# SwordGame

![Tucked ninja hiding from bad guy.](133948354-a7b2ed11-5af1-4507-b7cd-0e1fad9fe816.png)

## Creating A Build

You must have a source build of the engine. This is for building the server target.

First, for each map under "Content/Maps", except for "Content/Maps/LightingScenarios", open them in the editor and "build all levels".

Open project launcher from the editor and start a build.
1. Create new custom launch profile.
2. Use the following argument values.
    - Build.
        - Ensure "Build Configuration" is set to "Development". Note: We don't use "Test" or "Shipping" because there are important on-screen debug messages for the player to see.
    - Cook.
        - Choose "By the book".
        - Enable "Windows" and "WindowsServer" for "Cooked Platforms".
        - Ensure "En" is enabled for "Cooked Cultures".
        - Select "All" for "Cooked Maps".
    - Package.
        - Choose "Package & store locally".
        - Enable "Include an installer for prerequisites of packaged games".
    - Archive.
        - Enable archiving.
        - Choose a directory to output a copy of the build to. This directory will be your clean build that you can zip and distribute.
    - Deploy.
        - Choose "Do not deploy".
3. Launch the profile.
