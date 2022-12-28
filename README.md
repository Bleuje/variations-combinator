# Variations Combinator

This is a kind of real-time generative art drawing tool coded in openFrameworks and shaders.

The following blog shows images generated with this tool : [https://math-art-variations.tumblr.com/](https://math-art-variations.tumblr.com/)

openFrameworks [ofxGamepad](https://github.com/Bleuje/ofxGamepad) addon is used (I think you should use my fork because of a fix).

Tested only on linux so far. Making sure this is useable by other people than me is a work in progress... :)

![examples of outputs](doc/outputs-examples.gif)

## Main algorithm

The main algorithm composes a list of functions called *variations* (which I often just call functions here). They take a 2D position as input and return another 2D position (hence they can be composed). By plotting the images of particles filling a 2D square with transparent dots, subtle images can be obtained. The variations used here are functions found in fractal flames algorithms, that have been translated into glsl. Most of them have parameters that can be randomized. This is inspired by work from [Genrateme](https://github.com/genmeblog) aka [tsulej](https://github.com/tsulej) who has also shown his outputs in a tumblr blog : [https://folds2d.tumblr.com/](https://folds2d.tumblr.com/).

He made a tutorial to explain the algorithm, using Processing : [Folds](https://generateme.wordpress.com/2016/04/11/folds/)

This tool is real-time thanks to a compute shaders.

## 3D algorithm

The tool contains a 3D mode : using two 2D -> 2D functions a 4D point cloud can be obtained from the input 2D square. This 4D point cloud is projected into 3D with stereographic projection. This 3D point cloud is then again projected with another stereographic projection to view it on the 2D screen. (Keyboard shortcut for 3D mode : exclamation mark **!**)

![3D mode overview](doc/3dmode-overview.png)

## Gamepad

The work on user interface is focused on the use of xbox/playstation gamepad, and the openFrameworks addon [ofxGamepad](https://github.com/Bleuje/ofxGamepad) is used. The gamepad interface is explained on screen when pressing L2.

## Keyboard shortcuts

But it's also controllable with keyboard with even more actions, but not in a friendly way and intended for AZERTY keyboard layout.

### Main keyboard shortcuts :

| Key | Description |
| ----------- | ----------- |
| **Escape** | **quit** the app |
| **W** and **X** | function selection navigation / change cursor position |
| **C** and **V** | change function at cursor |
| **S** | **save** picture (with timestamp in filename), also saves the list of functions in a text file | 
| **Q** | randomize parameters of function at cursor (not possible for all variations) |
| **Y** | randomize parameters of all functions |
| **Z** | change all functions randomly |
| **F** / **D** | add / remove function above cursor |
| **P** / **O** | add / remove function at the end of the list |
| **N** | change **color** mode |
| **!** | activate/deactivate the **3D mode** |
| **E**| change "bounding mode" of drawing |


Less important keyboard shortcuts :

| Key | Description |
| ----------- | ----------- |
| Numpad arrows (**4**,**8**,**6**,**2**) | translate function ar cursor |
| **5** | reset all translations |
| **0** | reset ranslation of function at cursor |
| **1** and **3** | change scale of function at cursor |
| **.** | reset scale of function at cursor |
| **A** | re-render |
| **)** | screenshot |
| **+** / **-** | more or less opacity per particle |

There are some more, see void ofApp::keyPressed(int key) in source code.

## Tricks to get good pictures

Getting interesting results is not really easy the more you become used to the tool. I think that the normal way to explore is to build the main shape from the first functions of the list and gradually work towards texture with the following functions (I mean that the first function of the list is the last function to be applied, and it's the one at the top of the screen). You can also often keep a same function but randomize its parameters. Some variations have very strong character so they become quickly boring as main shape and are better used at an intermediate or texture level.

## License

GNU General Public License v3.0
