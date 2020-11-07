param (
    [switch] $debug = $false, # Build for debug
    [switch] $force = $false  # Force build all files, even unchanged
)

# Runs the visual studio environment batch file, mirroring any environment changes it makes
# https://stackoverflow.com/questions/36400268/setting-up-visual-studio-environment-variables-from-powershell
function Invoke-Environment {
    param
    (
        [Parameter(Mandatory=$true)]
        [string] $Command
    )

    $Command = "`"" + $Command + "`""
    cmd /c "$Command > nul 2>&1 && set" | . { process {
        if ($_ -match '^([^=]+)=(.*)') {
            [System.Environment]::SetEnvironmentVariable($matches[1], $matches[2])
        }
    }}
}

function Get-HashFile {
    $dict = @{}
    if (!(Test-Path $hashesfile -PathType Leaf)) 
    {
        New-Item -Path . -Name $hashesfile -ItemType File | Out-Null
        # Do nothing and return the empty dict if the file doesn't exist
    } else {
        foreach ($line in Get-Content $hashesfile) {
            $filename, $hash = $line.split("|")
            $hashstr = ""
            foreach ($x in $hash) {
                $hashstr += "$x|"
            }
            $hashstr = $hashstr.TrimEnd("|")
            $dict.Add($filename, $hashstr)
        }
    }
    return $dict
}

function Set-HashFile {
    param (
        [Parameter(Mandatory=$true)]
        [hashtable] $hashes
    )
    # Wipe the file if it exists, create it if not
    if (!(Test-Path $hashesfile -PathType Leaf)) {
        New-Item -Path . -Name $hashesfile -ItemType File | Out-Null
    } else {
        Clear-Content -Path $hashesfile
    }
    
    foreach ($h in $hashes.GetEnumerator()) {
        Add-Content -Path $hashesfile -Value ("{0}|{1}" -f $h.Name,$h.Value)
    }
}

if ($debug) {
    $buildname = "DEBUG"
    $hashesfile = "BuildHashes-Debug"
    $folder = "Debug"
} else {
    $buildname = "RELEASE"
    $hashesfile = "BuildHashes-Release"
    $folder = "Release"
}

Write-Host " [Building for " -NoNewline
Write-Host $buildname -ForegroundColor Green -NoNewline
Write-Host "]"

# Create the build directory if it isn't there (-Force shouldn't overwrite)
New-Item -Path . -Name "Build" -ItemType Directory -Force | Out-Null
New-Item -Path "Build" -Name $folder -ItemType Directory -Force | Out-Null

# Clear the build folder of everything but *.obj
Get-ChildItem -Path "Build\$folder" | ForEach-Object {
    if ($_.Attributes -eq "Directory") {
        # Remove all directories
        Remove-Item -Path $_.FullName -Recurse -Force | Out-Null
    } elseif ($_.Extension -ne ".obj") {
        # Remove all non *.obj files
        Remove-Item -Path $_.FullName
    }
}

# Check if we are already in the VS developer environment
if (!(Test-Path "env:VisualStudioVersion")) {
    # If not, grab the environment variables from the visual studio installation
    Write-Output "Initialising VS build tools..."
    Invoke-Environment "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat"
}

$sourcefiles = @(
    ".\Code\Util.cpp",
    ".\Code\Shader.cpp",
    ".\Code\Texture.cpp",
    ".\Code\Mesh.cpp",
    ".\Code\ResourceManager.cpp",
    ".\Code\Model.cpp",
    "Game.cpp",
    "main.cpp"
)

$includedirs = @(
    "C:\Lib\include"
)

$libdirs = @(
    "C:\Lib\msvc\lib"
)

$libfiles = @(
    "opengl32.lib",
    "glew32.lib",
    "glfw3dll.lib",
    "assimp-vc142-mt.lib"
)

# Construct the include/lib arguments from the list of directories
$includepathargs = @($includedirs | ForEach-Object {"/I$_"})
$libpathargs = @($libdirs | ForEach-Object {"/libpath:$_"})

# Make an empty array to store the object filenames
$objectfiles = New-Object string[] $sourcefiles.Length;

# Grab the hashes
$oldHashes = Get-HashFile
$newHashes = $oldHashes

Write-Output $oldHashes

$canlink = $true
for ($i=0; $i -lt $sourcefiles.Length; $i++) {
    $sourcefile = $sourcefiles[$i]
    $item = Get-Item $sourcefile
    $basename = $item.BaseName
    $directory = $item.Directory

    $hash = "CPP" + (Get-FileHash -Algorithm SHA1 -Path $sourcefile).Hash

    # Check to see if a header file exists
    $headerfile = "$directory\$basename.h"
    if (Test-Path $headerfile) {
        # If there is a header file, add it's hash to the source file's hash
        $hash += "|H" + (Get-FileHash -Algorithm SHA1 -Path $headerfile).Hash
    }
    # Check to see if the obj needs to be rebuilt
    if ($force -or $oldHashes[$sourcefile] -ne $hash) {
        Write-Output "Building $sourcefile..."
        if ($debug) {
            # /Od: disable compiler optimisations
            # /Z7: embed debugging symbols in the obj files
            # /EHsc: some exception handler thing that is required
            # /c: don't link (yet)
            $out = & cl /nologo /Od /Z7 @includepathargs /Fo:"Build\$folder\$basename.obj" /EHsc /c $sourcefile 
        } else {
            # /O2: enable compiler optimisations
            # /EHsc: might not be needed for release?
            $out = & cl /nologo /O2 @includepathargs /Fo:"Build\$folder\$basename.obj" /EHsc /c $sourcefile 
        }
        $code = $LASTEXITCODE
        if ($code -gt 0) {
            Write-Host " CL.exe returned error code $code on file $sourcefile!" -ForegroundColor Red
            foreach ($line in ($out -split "`r`n")) {
                Write-Host " $line" -ForegroundColor red
            }
            # Hash is invalid, remove it
            $newHashes.Remove($sourcefile)
            $canlink = $false
        } else {
            # Build succeeded, set new hash
            $newHashes[$sourcefile] = $hash
        }
    } else {
        Write-Output "Skipping $sourcefile, unchanged."
    }
    $objectfiles[$i] = ($basename) + ".obj"
}

# Write the hashes, with any changes, back to the file
Set-HashFile $newHashes

if (!$canlink) {
    Write-Host " Not linking - errors during build." -ForegroundColor Red
    exit 1
}

Push-Location "Build\$folder"

Write-Output "Linking..."
if ($debug) {
    # /debug:full: generate debugging symbols (in a PDB)
    $out = & link /nologo /debug:full @libpathargs /out:build.exe @libfiles @objectfiles
} else {
    $out = & link /nologo @libpathargs /out:build.exe @libfiles @objectfiles
}

Pop-Location

$code = $LASTEXITCODE
if ($code -gt 0) {
    Write-Host " LINK.exe returned error code $code!" -ForegroundColor Red
    foreach ($line in ($out -split "`r`n")) {
        Write-Host " $line" -ForegroundColor red
    }
    exit 1
}

# Copy all the stuff to the build directory
Copy-Item -Path "Models" -Destination "Build\$folder" -Recurse | Out-Null
Copy-Item -Path "Shaders" -Destination "Build\$folder" -Recurse | Out-Null
Copy-Item -Path "DLLs\*" -Destination "Build\$folder" -Recurse | Out-Null

Write-Host " Success!" -ForegroundColor Green
exit 0