#include "ofApp.h"

//--------------------------------------------------------------
// initialization
void ofApp::setup() {
    ofSetFrameRate(MAX_FPS);

    myFont.load("fonts/Raleway-Regular.ttf",floor(40.0*1080/CANVAS_HEIGHT));
    myFontBold.load("fonts/Raleway-Bold.ttf",floor(40.0*1080/CANVAS_HEIGHT));

    pixelCount.resize(CANVAS_WIDTH*CANVAS_HEIGHT);
    pixelCountBuffer.allocate(pixelCount, GL_DYNAMIC_DRAW); // array with pixel counts

    displayedImage.allocate(CANVAS_WIDTH, CANVAS_HEIGHT, GL_RGBA32F);
    debugImage.allocate(CANVAS_WIDTH, CANVAS_HEIGHT, GL_RGBA32F); // just useful to debug, for example to show gaussian noise values with an image

    pixelCountValueSetterShader.setupShaderFromFile(GL_COMPUTE_SHADER, "shaders/computeshader_countsetter.glsl");
    pixelCountValueSetterShader.linkProgram();
    countIncrementerShader.setupShaderFromFile(GL_COMPUTE_SHADER, "shaders/computeshader_countincrementer.glsl");
    countIncrementerShader.linkProgram();
    finalColorShader.setupShaderFromFile(GL_COMPUTE_SHADER, "shaders/computeshader_uintstoimage.glsl");
    finalColorShader.linkProgram();

    variationsParameters.resize(MAX_NUMBER_OF_VARIATIONS);
    for(int i=0;i<MAX_NUMBER_OF_VARIATIONS;i++) weightCount[i] = 0;
    for(int i=0;i<MAX_NUMBER_OF_VARIATIONS;i++) randomizationSignal[i] = 0;
    setNewParameters();
    variationsParametersBuffer.allocate(variationsParameters,GL_DYNAMIC_DRAW);
    variationsParametersBuffer.bindBase(GL_SHADER_STORAGE_BUFFER, 4);

    pixelCountBuffer.bindBase(GL_SHADER_STORAGE_BUFFER, 0);
    displayedImage.getTexture().bindAsImage(3, GL_WRITE_ONLY);
    debugImage.getTexture().bindAsImage(1, GL_WRITE_ONLY);

    ////////////////////////////////////////
    // check if there is a gamepad connected
    numberOfGamepads = ofxGamepadHandler::get()->getNumPads();

	if(numberOfGamepads>0){
			ofxGamepad* pad = ofxGamepadHandler::get()->getGamepad(0);
			ofAddListener(pad->onAxisChanged, this, &ofApp::axisChanged);
			ofAddListener(pad->onButtonPressed, this, &ofApp::buttonPressed);
			ofAddListener(pad->onButtonReleased, this, &ofApp::buttonReleased);
	}
	std::cout << "Number of gamepads : " << numberOfGamepads << std::endl;
    ////////////////////////////////////////

    printState();
}

//--------------------------------------------------------------
void ofApp::update() {
    time = 1.0*ofGetFrameNum()/MAX_FPS;

    std::stringstream strm;
    strm << "fps: " << ofGetFrameRate();
    ofSetWindowTitle(strm.str());

    ofSetColor(255);

    if(colorMode!=0 && colorMode!=3) ofBackground(0);
    else ofBackground(255*GREY_BACKGROUND_FACTOR);

    // compute new drawing only if needed
    if(renderNewOne // using this variable that forces a new drawing
    || (threeD>=1 && (ofGetFrameNum()%2==0)) // always draw in 3D mode but half of FPS to avoid too much GPU heat :) 
    || (threeD==0 && (curTranslationAxis1!=0 || curTranslationAxis2!=0)) // if we translate a variation
    || (threeD==0 && (curScaleAxis1!=0)) // if we rotate a variation
    )
    {
        drawFold();
        renderNewOne = false;
    }
}

//--------------------------------------------------------------
void ofApp::draw() {
    ofTranslate(120*ofGetWidth()/CANVAS_HEIGHT,0);
    ofScale(1.0*ofGetHeight()/CANVAS_HEIGHT);

    displayedImage.draw(0,0);
    showState();
}

// shader calls to compute image
void ofApp::drawFold()
{
    bool updateParametersBuffer = false;
    if(curTranslationAxis1!=0 || curTranslationAxis2!=0)
    {
        variationsParameters[indexOfChanges].tx += 2.0*curTranslationAxis1*translationStep;
        variationsParameters[indexOfChanges].ty += 2.0*curTranslationAxis2*translationStep;
        updateParametersBuffer = true;
    }

    if(curScaleAxis1!=0)
    {
        weightCount[indexOfChanges] += 1.0*curScaleAxis1;
        updateWeight(indexOfChanges);
        updateParametersBuffer = true;
    }

    if(curRotationAxis1!=0)
    {
        variationsParameters[indexOfChanges].controlledRotationAngle += 0.08*curRotationAxis1;
        updateParametersBuffer = true;
    }

    if(updateParametersBuffer) variationsParametersBuffer.updateData(variationsParameters);
    updateParametersBuffer = false;

    pixelCountValueSetterShader.begin();
    pixelCountValueSetterShader.setUniform1f("width", displayedImage.getWidth());
    pixelCountValueSetterShader.setUniform1f("height", displayedImage.getHeight());
    pixelCountValueSetterShader.setUniform1i("value", 0);
    pixelCountValueSetterShader.dispatchCompute(CANVAS_WIDTH / 32, CANVAS_HEIGHT / 32, 1);
    pixelCountValueSetterShader.end();

    countIncrementerShader.begin();
    countIncrementerShader.setUniform1f("width", displayedImage.getWidth());
    countIncrementerShader.setUniform1f("height", displayedImage.getHeight());
    countIncrementerShader.setUniform1i("nx", nx);
    countIncrementerShader.setUniform1i("ny", ny);
    countIncrementerShader.setUniform1i("sequenceLength",curNumberOfSuccessiveVariations);
    countIncrementerShader.setUniform1i("doSinusoid",doSinusoid);
    float sinusoidStretch = 0.5*pow(1.02,sinusoidStretchCount);
    countIncrementerShader.setUniform1f("sinusoidStretch",sinusoidStretch);
    countIncrementerShader.setUniform1i("operationsMode",operationsMode);
    countIncrementerShader.setUniform1i("distortionMode",distortionMode);
    countIncrementerShader.setUniform1f("randomizer",ofRandom(0,1));
    countIncrementerShader.setUniform1i("aIndex",animationIndex);
    countIncrementerShader.setUniform1f("aAngle",animationAngle);
    animationRadius = 0.5*pow(1.02,radiusCount);
    countIncrementerShader.setUniform1f("aRadius",animationRadius);
    countIncrementerShader.setUniform1i("aState",renderAnimation);
    countIncrementerShader.setUniform1i("threeD",threeD);
    countIncrementerShader.setUniform1f("mouseX",mouseX);
    countIncrementerShader.setUniform1f("mouseY",mouseY);

    float amp = 0.07;
    float tp = 0.0004*ofGetFrameNum();
    if(time >= latest3DJSMoveTime + 3) // automatic 3D rotations if no requested joystick rotations for 3 seconds
    {
        float q = ofMap(time,latest3DJSMoveTime + 3,latest3DJSMoveTime + 6,0,1,true);
        q = ease(q,2.0);
        curRot1 += q*amp*(ofNoise(tp,0)-0.5);
        curRot2 += q*amp*(ofNoise(tp,100)-0.5);
        curRot3 += q*amp*(ofNoise(tp,200)-0.5);
    }
    else
    {
        curRot1 += 0.1*curRotAxis1;
        curRot2 += 0.1*curRotAxis2;
    }
    countIncrementerShader.setUniform1f("rot1",curRot1);
    countIncrementerShader.setUniform1f("rot2",curRot2);
    countIncrementerShader.setUniform1f("rot3",curRot3);

    countIncrementerShader.setUniform1i("projectionIndex",chosenProjectionDivisor);

    //float projDist = 1.1*pow(1.003,mouseX-ofGetWidth()/2.0); // projection distance with mouse control
    float projDist = 3.0-ofMap(curR2,-1,1,0,1.7);
    countIncrementerShader.setUniform1f("projDist",projDist);
    countIncrementerShader.dispatchCompute(nx / 32, ny / 32, 1);
    countIncrementerShader.end();

    finalColorShader.begin();
    finalColorShader.setUniform1f("width", displayedImage.getWidth());
    finalColorShader.setUniform1f("height", displayedImage.getHeight());
    float countPerColorChange = pow(1.02,contrastCount)*800.;
    finalColorShader.setUniform1f("countPerColorChange", countPerColorChange);
    finalColorShader.setUniform1i("colormode",colorMode);
    finalColorShader.setUniform1f("randomizer",ofRandom(0,1));
    finalColorShader.setUniform1f("time",time);
    finalColorShader.setUniform1i("threeD",threeD);
    finalColorShader.setUniform1f("greyBackgroundFactor",GREY_BACKGROUND_FACTOR);
    finalColorShader.dispatchCompute(displayedImage.getWidth() / 32, displayedImage.getWidth() / 32, 1);
    finalColorShader.end();
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) {

}
