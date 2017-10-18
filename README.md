# TextTools

A simple tool for converting a text file into a constant string in a C file
for inclusion into a command-line tool. When incorporated into Xcode, allows
you to create text files and have them automatically turned into a C string
for use in your code.

## License

Licensed under the open-source BSD license:

Copyright 2017 William Woody

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
this list of conditions and the following disclaimer in the documentation 
and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

## Usage

Usage: texttool [-h] [-o filename] [-s string] inputfile

* -h  
    Prints this help file. This help file will also be printed if any illegal
    parameters are provided. 

* -o  
    Uses the file name as the base name for the output files. (By default 
    this uses the input file name as the base name for the output files.) 

* -s  
    Uses the specified string name for the generated class. (By default this 
    uses the input file name as the string name.)

This program takes an input file MyFile.txt, and will generate two output files
MyFile.c and MyFile.h, with the string constant MyFile containing the contents of the input file.

## Using with Xcode

To build and incorporate texttool into Xcode, you need to build the texttool tool as a command-line tool, copy into a known location (such as `/usr/local/bin`), add the path to your shell $PATH setting (if needed), and modify the settings for each Xcode project where you use the tools.

### Step 1: Build the tools

This can be done by opening the TextTools project, selecting "texttool" and running "Archive". This will produce an archive with the individual texttool command-line tool. 

(After the archive process is complete, the "Archives" window will open, showing the archive of the tool. Right-click and select "Show In Finder" to show the *xcarchive* bundle. Right-click on the bundle and select "Show Package Contents." The command line tool will be in the directory Products/usr/local/bin.)

### Step 2: Copy to a known path location.

I usually copy the tools to `/usr/local/bin`; you may choose somewhere else if you wish.

Open Terminal. Create `/usr/local/bin` if necessary; this can be done by executing commands like:

    cd /usr
    sudo mkdir local
    cd local
    sudo mkdir bin

(After each sudo command you may need to enter your password.)

Now with the current path of the Terminal at `/usr/local/bin`, execute:

    open .

This will open a finder window at the location `/usr/local/bin`. You can then drag and drop the tool located in the *xcarchive* bundle using Finder. (Finder may ask for a password before copying the tool into place.)

### Step 3: Update your $PATH settings. (Optional)

This step is strictly not necessary but comes in handy if you wish to run the texttool tool from the command line.

Open Terminal. Make sure your home directory is the current directory. (If not, enter the command `cd ~`.) Determine if the file .bash_profile exists; if not, create the file using the command `touch .bash_profile`. Edit the file (`open .bash_profile`).

In the file, add the following line:

    export PATH=/usr/local/bin:$PATH

Save and close the file. The next terminal window that is opened will have the $PATH variable set appropriately. You can test this by opening a new terminal window and entering the command `texttool`; this will print version and copyright information for the tool.

### Step 4: Include support into your project.

For Xcode to use the tools you must update the *Build Rules* setting under the target for the project you are building.

First, open the Xcode project, and select the project file in the Project navigator on the far left. This will show the project settings.

Second, select the target which you wish to modify. You will need to change the build rules for each target in your project.

Third, select "Build Rules" from the tab bar along the top.

Fourth, select the "Add" button, the plus sign in the upper-left of the build rules page. This will add a new build rule at the top. 

For **texttool**, the build rule uses a new rule for a new file extension. The settings should be as follows:

    Process      Source files with names matching: *.txc
    Using        Custom script:
    
        /usr/local/bin/texttool -o "$DERIVED_SOURCES_DIR/$INPUT_FILE_BASE" "$INPUT_FILE_PATH"
    
    Output Files:
        $(INPUT_FILE_BASE).c

After this is done, .txc files will be compiled to .h and .c files of the same name. (You will need to explicitly add .txc files to your build process in your project file settings.)

You may use any extension you wish; for my use I tend to use the .txc file extension.

