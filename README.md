# 
CS430Project4

How to use:
-----
In terminal, run main to create a 1000 by 1000 pixel ppm file with the command "./main 1000 1000 test.json out.ppm".
The PPM file can be name whatever you like.

16-11-15
-----
+ Update
    - All of the bugs with reflection have been fixed as far as I can tell

16-11-15
-----
+ Update
    - Reflection now properly reflects surfaces by passing a reflected vector into the recursion call, rather than passing in the normal vector
    - previous relfection would create an infinite mirror effect, rather than proper reflection

16-11-13
-----
+ Project Requirements
    - Calulate refraction for each level
    
+ Added Support
    - Reflection support for scene building added

16-11-13
-----
+ Project Requirements
    - Recurse through each pixel up to 7 times to find all reflection
    - End recurse at level seven, or if reflection total drops to 0%, or if no object found
    - Multiply color added at each level by (current overall reflection)-(object reflection) for each level
    - Calulate refraction for each level
