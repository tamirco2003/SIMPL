:repeat
simpl.exe calculatePiOrE.simp
if %errorlevel% == 0 goto :repeat
echo Success!