param($conf)
$cmd = "& xmake f"
$cmd += " $conf"
if ($env:ARCH -eq "x86") {
    $cmd += " -a x86"
}
$cmd += " -c --file=.\3rd\xmake.lua"
echo $cmd
Invoke-Expression $cmd
xmake lua .\3rd\fetch.lua
xmake build --file=.\3rd\xmake.lua
xmake lua .\3rd\copy.lua
