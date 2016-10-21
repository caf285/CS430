# CS430Project3
Illumination

Oct 20, 2016
------------
+ How to use
  - Format includes name of program, followed by X size, Y Size, json file name, ppm file name
  - For example "./main 500 500 test.json out.ppm"

+ Additions
  - Angular Attenuation is now functional
  - Point Lights are now supported ... I hope
  - Specular color is now a reasonable size
  - Memory allocation bug has been fixed

+ Extra Stuff
  - Shadows only remove 50% of color instead of all color.
  
Oct 19, 2016
------------

+ Problems leftover
  - F(ang) makes no sense. We have no spot light data to test, and 1.0 for not spot light in the current example make my colors explode. Like there is no fade at all.
  - specular color is HUGE. all of the math has been triple checked, and there is no reason for such a large specular field, and yet the example sphere is 90% pure white.
  - about 50% of the runs crashes the program, and I have no idea why. Everything is malloced, but there is still trouble.

Oct 15, 2016
------------

+ Additions
  - light primitives now accepted and added to objects list

+ Functionality Still Required
  - light primitives must still be handled in scene
  - functions for each light object contribution must be added for color manipulation
  - functions for illumination must be added to code in program
