Computer Vision
Run A2 Application

make all

For giving images input please don’t include the folder path. Just provide the images name.

Example:

./a2 part1 <image-name.jpg> <image-name.jpg> ….

./a2 part1 bigben_18.jpg bigben_19.jpg

Part1 & Part2 Will pick the Images from the images/part1_images folder

Part3 Will pick the Images from the images/part3_images folder

The application itself will pick the images from the predefined folder path in constants.h file, which you can change and make the application again.

To run part1
./a2 part1 image1 image2
./a2 part1 images …

To run part2
./a2 HMpart2 image1 image2
./a2 part2 image1 image2

To run part3
./a2 part3 lincoln.png
./a2 part3 base_image images...

All results that are related to Part1 (SIFT matching by drawing lines goes in folder) 
/gdhody-wsbabcoc-ramrao-a2/part1-matches

All results that are related to Part2 (RANSAC speedup) 
/gdhody-wsbabcoc-ramrao-a2/part2-ransac

All results that are related to Part3 (Warped Images) 
/gdhody-wsbabcoc-ramrao-a2/part3-transformations

All previous Run results will be found in the 
/gdhody-wsbabcoc-ramrao-a2/results

HeaderFilesInformation.txt contains information regarding the code in differnt header files

---
To help you run some test we have created some shell scripts:

Script to run Image Matching with one random Image from part1 with respect to all images and output results in the results folder with Results_<random_image_name.jpg>.txt
./multi-run-part1-script.sh
./run-command.sh

Script to run Wraping Application
./transformation-application.sh To run on notredam images
./transformation-lincoln-script.sh To run on lincoln images



