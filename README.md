# ConeLineDrawing


In my OpenCV project, I employed a series of image processing techniques to enhance the visibility of bright orange cones in images. The process began with blurring the image to reduce noise and improve data clarity. Using an HSV threshold, I successfully isolated the bright orange cones from the background, making them more distinguishable.

Next, I utilized OpenCV's contour method to extract the contours from the thresholded image, revealing the distinct shapes of the cones. To filter out irrelevant data, I discarded contours that were too small to represent cones effectively.

To identify the central points of each cone's contour, I calculated their midpoints. These midpoints served as the basis for finding the lines of best fit. I implemented a method that generated lines connecting every midpoint of each contour, allowing me to determine potential trajectories.

To ascertain the most accurate paths, I approximated the cones as circles and checked how many cones each line passed through. Through careful analysis, I identified the two lines that intersected the greatest number of circles in total. These lines were considered the most suitable reference paths for the cones.

Finally, I visualized the two chosen lines, highlighting their trajectories for easy reference. By employing this approach, I aimed to provide a clearer and more reliable reference path for autonomous vehicles, ultimately enhancing their navigation capabilities and safety.

![AutonomousAI](https://github.com/charliebharlie/ConeLineDrawing/assets/84645358/3fb35e0e-bfc2-4bdd-8eb2-b3df5f1ba10e)
