# CGRA PROJECT (Vinh, Rhys, Clara)


## Set up

- Follow Cmake requirements in Cmake_Setup.md file
- Open the project with your perferred IDE
- Ensure 'base' is selected for the project build
- Run the program


## Notes

- For testing the combustion, use preset 0 to 1. (default is set to 0)
- For visualising the different trees, use preset 2 to 6.

- Performance of preset 0 and 1 is around ~20-30fps.

*Intial prolyis can take sometime (~50 sec) to settle in but then spread quickly in a positive feedback loop.


## Testing

- Press 'START' to start the combustion & fire
- Press 'END' to end the combustion & fire

*Ensure you END the combustion first before generating another tree preset


## ImGui control

For tree generation, select a preset and shape -> then press 'Generate'

For combustion, uses the slider to control the different parameters:

- 'Start Section'           controls which section to start the combustion at (via index; default is set to base)
- 'Mass Loss Rate'          controls how fast the combusting section(s) loses it mass
- 'Air Heat Transfer coeff' controls how much heat is being transfered to the air
- 'Surface Diffusion coeff' controls how much heat is being transfered on the surface to neighbouring section(s)
- 'Char Isulation coeff'    controls the effectivenss of the isulation effect for spreading heat
- 'Water Content'           controls the amount of water in the tree sections (quickest way to stop fire spreading)

*section = a tree branch
