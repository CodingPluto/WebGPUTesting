Hello future Chris!

Once you've cloned this repo, you will need to run:
        git submodule update --init --recursive
in the terminal to initalise the dawn library


Also to stop git spam in vscode, add the following to settings.json:
"git.detectSubmodules": false,
"git.autoRepositoryDetection": "openEditors"



To remove a git submodule:
chris@Christophers-MacBook-Air-3 WebGPUTesting % rm -rf external/dawn
chris@Christophers-MacBook-Air-3 WebGPUTesting % rm -rf .git/modules/external/dawn

Helpful commands to make sure git has succesflly initallised (do in order):
git submodule foreach --recursive git reset --hard
git submodule update --init --recursive --force