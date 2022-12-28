# Variations Combinator

This is a kind of generative art drawing tool coded in openFrameworks and shaders.

The following blog shows images generated with this tool : [https://math-art-variations.tumblr.com/](https://math-art-variations.tumblr.com/)

Tested only on linux so far.

![examples of outputs](doc/outputs-examples.gif)

## Main algorithm

The main algorithm composes a list of functions called *variations* that have a 2D position as input and return another 2D position (hence they can be composed). By plotting the images of particles filling a 2D square with transparent dots, subtle images can be obtained. The variations used here are functions found in fractal flames algorithms, that have been translated into glsl. Most of them have parameters that can be randomized. This is inspired by work from [Genrateme](https://github.com/genmeblog) aka [tsulej](https://github.com/tsulej) who has also shown his outputs in a tumblr blog : [https://folds2d.tumblr.com/](https://folds2d.tumblr.com/).

He made a tutorial to explain the algorithm, using Processing : [Folds](https://generateme.wordpress.com/2016/04/11/folds/)

My project is real-time thanks to a compute shaders.

## 3D algorithm

The project contains a 3D mode : using two 2D -> 2D functions a 4D point cloud can be obtained from the input square. This 4D point cloud is projected to 3D with stereographic projection. This 3D point cloud is then again projected with another stereographic projection to view it on the 2D screen.

![3D mode overview](3dmode-overview.png)

## Gamepad

The work on user interface is focused on the use of xbox/playstation gamepad, and the openFrameworks addon [ofxGamepad](https://github.com/underdoeg/ofxGamepad) is used.

## Keyboard shortcuts

But it's also controllable with keyboard with even more actions, but not in a friendly way and intended for AZERTY keyboard layout.

