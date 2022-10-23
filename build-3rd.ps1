if ($env:ARCH -eq "x86") {
    xmake f -a x86 -c --file=.\3rd\xmake.lua
} else {
    xmake f -c --file=.\3rd\xmake.lua
}
xmake lua .\3rd\fetch.lua
xmake build --file=.\3rd\xmake.lua
xmake lua .\3rd\copy.lua
