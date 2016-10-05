# Chris French
# CS430 Project2 - Raycaster
Oct 4 2016
----------
  + How to use
    - In terminal type "./main test.json out.ppm" without the quotes
  
  + All functionality now present
    - sphere intersection code fixed so that front of sphere renders rather than back
    - plane intersection added
    - full comments
    
  + Extra Stuff
    - I wanted to show off the colision between objects so I modified the sample json to show off a water molecule dropping into the plane.

Oct 3 2016
----------
  + code now supports the following functionality
    - object color support
    - addative colors (overlayed objects add color up to white)
    - P6 PPM output
    - Shpere intersection
  + Missing functionality still required:
    - plane intersection
    - complete comments

Oct 1 2016
----------
  + Code from lectures combined to provide the following functionality:
    - Read json file
    - parse json file
    - store types from json file into structs
    - interpret camera type to build scene
    - interperet cylinder type for intersection
  
  + Missing functionality still required:
    - interpret sphere type for intersection
    - interpret plane type for intersection
    - Output intersections to P3 or P6 file
    - Complete comments
