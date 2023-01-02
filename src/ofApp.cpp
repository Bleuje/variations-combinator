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

void ofApp::setNewParameters()
{
    for(int i=0;i<curNumberOfSuccessiveVariations;i++)
    {
        if(!addingNewFold||(i==curNumberOfSuccessiveVariations-1)){
        if(!keepFoldTypes) variationsParameters[i].variationType = floor(ofRandom(0,numberOfVariationTypes));
            changeVariationParameters(i);
        }
    }

    variationsParametersBuffer.updateData(variationsParameters);
}

// weight is a scale factor on variation i
void ofApp::updateWeight(int i)
{
    variationsParameters[i].weight = pow(1.007,uniformWeightCount)*pow(1.01,weightCount[i]);
}

// try other folds
void ofApp::actionNewParameters()
{
    setNewParameters();
    renderNewOne = true;
}

// use final sinusoid or not
void ofApp::actionSetBounder()
{
    doSinusoid  = (doSinusoid+1)%2;
    renderNewOne = true;
}

// randomize parameters but same function types
void ofApp::actionRandomizeParametersOfAllVariations()
{
    keepFoldTypes = true;
    setNewParameters();
    keepFoldTypes = false;
    renderNewOne = true;
}

// randomize parameters on single function without changing its type
void ofApp::actionRandomizeParametersOfSingleVariation()
{
        changeVariationParameters(indexOfChanges);
        variationsParametersBuffer.updateData(variationsParameters);
        renderNewOne = true;
        doPrintState = false;
}

// add function at cursor
void ofApp::actionAddFunctionAtCursor()
{
        curNumberOfSuccessiveVariations = min(curNumberOfSuccessiveVariations+1,MAX_NUMBER_OF_VARIATIONS);
        for(int j=curNumberOfSuccessiveVariations-1;j>indexOfChanges;j--) variationsParameters[j] = variationsParameters[j-1];
        for(int j=curNumberOfSuccessiveVariations-1;j>indexOfChanges;j--) weightCount[j] = weightCount[j-1];
        changeVariationParameters(indexOfChanges);
        variationsParametersBuffer.updateData(variationsParameters);
        renderNewOne = true;
}

// remove function at cursor
void ofApp::actionRemoveFunctionAtCursor()
{
        curNumberOfSuccessiveVariations = max(1,curNumberOfSuccessiveVariations-1);
        for(int j=indexOfChanges;j<curNumberOfSuccessiveVariations;j++) variationsParameters[j] = variationsParameters[j+1];
        for(int j=indexOfChanges;j<curNumberOfSuccessiveVariations;j++) weightCount[j] = weightCount[j+1];
        indexOfChanges = indexOfChanges%curNumberOfSuccessiveVariations;
        variationsParametersBuffer.updateData(variationsParameters);
        renderNewOne = true;
}

// move cursor
void ofApp::actionMoveCursor(int step)
{
    indexOfChanges = (curNumberOfSuccessiveVariations+indexOfChanges+step)%curNumberOfSuccessiveVariations;
}

// change function at cursor
void ofApp::actionChangeFunctionAtCursor(int step)
{
    variationsParameters[indexOfChanges].variationType = (variationsParameters[indexOfChanges].variationType+numberOfVariationTypes+step)%numberOfVariationTypes;
    variationsParametersBuffer.updateData(variationsParameters);
    renderNewOne = true;
}

// change colors
void ofApp::actionChangeColors()
{
    colorMode = (colorMode+1)%3; // with this modulo only the 3 first color modes are used
    renderNewOne = true;
}

// change 2D/3D modes
void ofApp::actionChange2D3D()
{
    threeD = (threeD+1)%2;
    renderNewOne = true;
    curTranslationAxis1 = 0;
    curTranslationAxis2 = 0;
    curScaleAxis1 = 0;
}

// reset translations
void ofApp::actionResetTranslations()
{
    for(int i=0;i<curNumberOfSuccessiveVariations;i++)
    {
        variationsParameters[i].tx = 0;
        variationsParameters[i].ty = 0;
    }
    variationsParametersBuffer.updateData(variationsParameters);
    renderNewOne = true;
    doPrintState = false;
}

// reset translations
void ofApp::actionResetScales()
{
    for(int i=0;i<curNumberOfSuccessiveVariations;i++)
    {
        weightCount[i] = 0;
        updateWeight(i);
    }
    variationsParametersBuffer.updateData(variationsParameters);
    renderNewOne = true;
    doPrintState = false;
}

// reset translations
void ofApp::actionResetRotations()
{
    for(int i=0;i<curNumberOfSuccessiveVariations;i++)
    {
        variationsParameters[i].controlledRotationAngle = 0;
    }
    variationsParametersBuffer.updateData(variationsParameters);
    renderNewOne = true;
    doPrintState = false;
}

// reset translations
void ofApp::actionResetScale(int i)
{
    weightCount[i] = 0;
    updateWeight(i);
    variationsParametersBuffer.updateData(variationsParameters);
    renderNewOne = true;
    doPrintState = false;
}

void ofApp::actionChangeProjection()
{
    chosenProjectionDivisor = (chosenProjectionDivisor + 1)%4;
    renderNewOne = true;
    doPrintState = false;
}

//--------------------------------------------------------------
// keyboard interaction
void ofApp::keyPressed(int key) {
    doPrintState = true;
    if(key=='a') // rerender the same thing
    {
        renderNewOne = true;
        doPrintState = false;
    }
    if(key=='z') // try other folds
    {
        actionNewParameters();
    }
    if(key=='s') // save image
    {
        std::string s = ofGetTimestampString();
        ofPixels pixels;
        displayedImage.readToPixels(pixels);
        ofSaveImage(pixels,"images/image_"+s+"_large.png", OF_IMAGE_QUALITY_BEST);
        std::cout << "Saved image " << s << std::endl;
        doPrintState = false;
        saveLog(s);
    }
    if(key=='+') // change the pixel value per count
    {
        contrastCount--;
        renderNewOne = true;
        doPrintState = false;
    }
    if(key=='-') // change the pixel value per count
    {
        contrastCount++;
        renderNewOne = true;
        doPrintState = false;
    }
    if(key=='/') // change the final sinusoid stretching
    {
        sinusoidStretchCount--;
        renderNewOne = true;
        doPrintState = false;
    }
    if(key=='*') // change the final sinusoid stretching
    {
        sinusoidStretchCount++;
        renderNewOne = true;
        doPrintState = false;
    }
    if(key=='e') // use final sinusoid or not
    {
        actionSetBounder();
    }
    if(key=='t') // change weight of all folds
    {
        uniformWeightCount++;
        for(int i=0;i<curNumberOfSuccessiveVariations;i++) updateWeight(i);
        variationsParametersBuffer.updateData(variationsParameters);
        renderNewOne = true;
        doPrintState = false;
    }
    if(key=='r') // change weight of all folds
    {
        uniformWeightCount--;
        for(int i=0;i<curNumberOfSuccessiveVariations;i++) updateWeight(i);
        variationsParametersBuffer.updateData(variationsParameters);
        renderNewOne = true;
        doPrintState = false;
    }
    if(key=='y') // change random variationsParameters for same fold types
    {
        actionRandomizeParametersOfAllVariations();
    }
    if(key=='f') // increase the number of folds
    {
        addingNewFold = true;
        curNumberOfSuccessiveVariations = min(curNumberOfSuccessiveVariations+1,MAX_NUMBER_OF_VARIATIONS);
        setNewParameters();
        addingNewFold = false;
        renderNewOne = true;
    }
    if(key=='d') // decrease the number of folds
    {
        curNumberOfSuccessiveVariations = max(1,curNumberOfSuccessiveVariations-1);
        indexOfChanges = indexOfChanges%curNumberOfSuccessiveVariations;
        renderNewOne = true;
    }
    if(key=='p') // insert before cursor
    {
        actionAddFunctionAtCursor();
    }
    if(key=='o') // delete on cursor
    {
        actionRemoveFunctionAtCursor();
    }
    if(key=='w') // change the cursor position to control single folds
    {
        actionMoveCursor(-1);
    }
    if(key=='x') // change the cursor position to control single folds
    {
        actionMoveCursor(+1);
    }
    if(key=='q') // change random variationsParameters on single fold (chosen with cursor)
    {
        actionRandomizeParametersOfSingleVariation();
    }
    if(key=='c') // change fold type on cursor
    {
        actionChangeFunctionAtCursor(-1);
    }
    if(key=='v') // change fold type on cursor
    {
        actionChangeFunctionAtCursor(+1);
    }
    if(key=='m') // change mode : Full composition / Compositions of added pairs / Composition of added triplets
    {
        operationsMode = (operationsMode+1)%4;
        renderNewOne = true;
    }
    if(key=='l') // distorition mode : Normal / Lerp in the center / simple sligth lerp from input position
    {
        distortionMode = (distortionMode+1)%3;
        renderNewOne = true;
    }
    if(key=='n') // invert colors
    {
        actionChangeColors();
    }
    int numberOfDotsStep = 512;
    if(key=='u') // less amount of dots
    {
        nx = max(numberOfDotsStep,nx-numberOfDotsStep);
        ny = max(numberOfDotsStep,ny-numberOfDotsStep);
        renderNewOne = true;
    }
    if(key=='i') // more amount of dots
    {
        nx = nx+numberOfDotsStep;
        ny = ny+numberOfDotsStep;
        renderNewOne = true;
    }
    if(key=='6')
    {
        variationsParameters[indexOfChanges].tx += translationStep;
        variationsParametersBuffer.updateData(variationsParameters);
        renderNewOne = true;
        doPrintState = false;
    }
    if(key=='4')
    {
        variationsParameters[indexOfChanges].tx -= translationStep;
        variationsParametersBuffer.updateData(variationsParameters);
        renderNewOne = true;
        doPrintState = false;
    }
    if(key=='2')
    {
        variationsParameters[indexOfChanges].ty += translationStep;
        variationsParametersBuffer.updateData(variationsParameters);
        renderNewOne = true;
        doPrintState = false;
    }
    if(key=='8')
    {
        variationsParameters[indexOfChanges].ty -= translationStep;
        variationsParametersBuffer.updateData(variationsParameters);
        renderNewOne = true;
        doPrintState = false;
    }
    if(key=='5')
    {
        actionResetTranslations();
    }
    if(key=='0')
    {
        variationsParameters[indexOfChanges].tx = 0;
        variationsParameters[indexOfChanges].ty = 0;
        variationsParametersBuffer.updateData(variationsParameters);
        renderNewOne = true;
        doPrintState = false;
    }
    if(key=='1')
    {
        weightCount[indexOfChanges]--;
        updateWeight(indexOfChanges);
        variationsParametersBuffer.updateData(variationsParameters);
        renderNewOne = true;
        doPrintState = false;
    }
    if(key=='3')
    {
        weightCount[indexOfChanges]++;
        updateWeight(indexOfChanges);
        variationsParametersBuffer.updateData(variationsParameters);
        renderNewOne = true;
        doPrintState = false;
    }
    if(key=='.')
    {
        weightCount[indexOfChanges]=0;
        updateWeight(indexOfChanges);
        variationsParametersBuffer.updateData(variationsParameters);
        renderNewOne = true;
        doPrintState = false;
    }
    if(key==',')
    {
        radiusCount--;
        doPrintState = false;
    }
    if(key==';')
    {
        radiusCount++;
        doPrintState = false;
    }
    if(key=='&')
    {
        renderAnimation = true;
    }
    if(key=='!')
    {
        actionChange2D3D();
    }
    if(key=='=')
    {
        actionChangeProjection();
    }
    if(key==')')
    {
        std::string s = "screenshot_"+ofGetTimestampString();
        ofSaveScreen("images/"+s+".png");
        std::cout << "Saved screenshot " << s << std::endl;
        saveLog(s);
    }
    if(doPrintState) printState();
    showState();
}

// print info in console
void ofApp::printState()
{
    std::cout << "----------" << std::endl;

    if(threeD) std::cout << "3D mode." << std::endl;

    if(doSinusoid) std::cout << "Constraining mode activated." << std::endl;

    if(operationsMode==0) std::cout << "Full composition";
    if(operationsMode==1) std::cout << "Sum with last 2 functions";
    if(operationsMode==2) std::cout << "Sum with last 3 functions";
    if(operationsMode==3) std::cout << "One fixed point iteration";

    std::cout << std::endl;

    if(distortionMode==1) std::cout << "distortion at center" << std::endl;
    if(distortionMode==2) std::cout << "lerp from input mode" << std::endl;

    std::cout << "Large resolution : " << CANVAS_WIDTH << " x " << CANVAS_HEIGHT << " ; Dots amount : " << nx << " x " << ny << std::endl;


    for(int i=0;i<curNumberOfSuccessiveVariations;i++)
    {
        std::cout << "--> " << getName(variationsParameters[i].variationType);
        if(indexOfChanges==i) std::cout << " <--";
        std::cout << std::endl;
    }
}

void ofApp::axisChanged(ofxGamepadAxisEvent& e)
{
	//cout << "AXIS " << e.axis << " VALUE " << ofToString(e.value) << endl;
    int axisType = e.axis;
    float value = e.value;
    if(axisType==6 && e.value>0.5)
    {
        actionChangeFunctionAtCursor(1);
    }
    if(axisType==6 && e.value<-0.5)
    {
        actionChangeFunctionAtCursor(-1);
    }
    if(axisType==7 && e.value>0.5)
    {
        actionMoveCursor(-1);
    }
    if(axisType==7 && e.value<-0.5)
    {
        actionMoveCursor(1);
    }
    if(axisType==0 || axisType==1)
    {
        if(axisType==0) curTranslationAxis1 = 0;
        if(axisType==1) curTranslationAxis2 = 0;
        if(abs(e.value)>0.09)
        {
            if(axisType==0) curTranslationAxis1 = e.value;
            if(axisType==1) curTranslationAxis2 = e.value;
            renderNewOne = true;
            doPrintState = false;
        }
    }
    if(axisType==3 || axisType==4)
    {
        if(axisType==3) curRotAxis1 = 0;
        if(axisType==4) curRotAxis2 = 0;
        latest3DJSMoveTime = time;
        if(abs(e.value)>0.09)
        {
            latest3DJSMoveTime = time;
            if(axisType==3) curRotAxis1 = e.value;
            if(axisType==4) curRotAxis2 = e.value;
        }
    }
    if(axisType==4 && threeD==0)
    {
        curScaleAxis1 = 0;
        if(abs(e.value)>0.09)
        {
            curScaleAxis1 = -e.value;
            renderNewOne = true;
            doPrintState = false;
        }
    }
    if(axisType==3 && threeD==0)
    {
        curRotationAxis1 = 0;
        if(abs(e.value)>0.09)
        {
            curRotationAxis1 = e.value;
            renderNewOne = true;
            doPrintState = false;
        }
    }
    if(axisType==2)
    {
        curL2 = e.value;
    }
    if(axisType==5)
    {
        curR2 = e.value;
    }
}

void ofApp::buttonPressed(ofxGamepadButtonEvent& e)
{
	//cout << "BUTTON " << e.button << " PRESSED" << endl;
    int buttonId = e.button;
    if(buttonId == 0)
    {
        actionNewParameters();
    }
    if(buttonId == 1)
    {
        actionRandomizeParametersOfAllVariations();
    }
    if(buttonId == 2)
    {
        actionRandomizeParametersOfSingleVariation();
    }
    if(buttonId == 3)
    {
        if(threeD==0) actionSetBounder();
        else actionChangeProjection();
    }
    if(buttonId == 4)
    {
        actionAddFunctionAtCursor();
    }
    if(buttonId == 5)
    {
        actionRemoveFunctionAtCursor();
    }
    if(buttonId == 6)
    {
        actionChange2D3D();
    }
    if(buttonId == 7)
    {
        actionChangeColors();
    }
    if(buttonId == 10)
    {
        actionResetScales();
        actionResetRotations();
    }
    if(buttonId == 9)
    {
        actionResetTranslations();
    }
}

void ofApp::buttonReleased(ofxGamepadButtonEvent& e)
{
	//cout << "BUTTON " << e.button << " RELEASED" << endl;
}

// save list of functions in text file
void ofApp::saveLog(std::string s)
{
    std::string filename = "images/functions_" + s + ".txt";

    myFunctionsTextFile.open(filename,ofFile::WriteOnly);

    int length = curNumberOfSuccessiveVariations;

    myFunctionsTextFile << " -> ";

    if(operationsMode==0)
    {
        for(int i=0;i<length;i++)
        {
            myFunctionsTextFile << getName(variationsParameters[i].variationType) << " -> ";
        }
    }
    else if(operationsMode==1)
    {
        for(int i=0;i<length-2;i++)
        {
            myFunctionsTextFile << getName(variationsParameters[i].variationType) << " -> ";
        }
        if(length>1)
        {
            myFunctionsTextFile << (getName(variationsParameters[length-2].variationType) + " + " + getName(variationsParameters[length-1].variationType)) << " -> ";
        }
        else
        {
            myFunctionsTextFile << getName(variationsParameters[length-1].variationType) << " -> ";
        }
    }
    else if(operationsMode==2)
    {
        for(int i=0;i<length-3;i++)
        {
            myFunctionsTextFile << getName(variationsParameters[i].variationType) << " -> ";
        }
        if(length>2)
        {
            myFunctionsTextFile << (getName(variationsParameters[length-3].variationType) + " + " + getName(variationsParameters[length-2].variationType) + " + " + getName(variationsParameters[length-1].variationType)) << " -> ";
        }
        else if(length>1)
        {
            myFunctionsTextFile << (getName(variationsParameters[length-2].variationType) + " + " + getName(variationsParameters[length-1].variationType)) << " -> ";
        }
        else
        {
            myFunctionsTextFile << getName(variationsParameters[length-1].variationType) << " -> ";
        }
    }
    else if(operationsMode==3)
    {
        for(int i=0;i<length;i++)
        {
            myFunctionsTextFile << getName(variationsParameters[i].variationType) << " -> ";
        }
        for(int i=0;i<length;i++)
        {
            myFunctionsTextFile << getName(variationsParameters[i].variationType) << " -> ";
        }
    }

    myFunctionsTextFile.close();

    std::cout << "Saved functions." << std::endl;
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

// show info on screen
void ofApp::showState()
{

    float u = 50.0/30.0*1080/CANVAS_HEIGHT;

    float dx = CANVAS_HEIGHT+35*u;
    float dy = 150*u;

    float infoParameter = ofMap(curL2,-0.8,0.85,0,1,true);

    ofPushMatrix();

    ofTranslate(dx,dy);

    float yTranslation = 90*40.0/30.0*1080/CANVAS_HEIGHT;

    int frameNum = ofGetFrameNum();

    ofPushMatrix();

    ofTranslate(1100*pow(infoParameter,2.8)*u,0);

    for(int i=0;i<curNumberOfSuccessiveVariations;i++)
    {
        ofPushMatrix();

        int reversedIndex = (curNumberOfSuccessiveVariations - i - 1);

        float col = (colorMode!=0 && colorMode!=3 ? 255 : 0);

        ofTranslate(0,yTranslation*reversedIndex);

        std::string cursor = (indexOfChanges==i ? "  <" : "");

        std::string functionString = std::to_string(reversedIndex+1) + " : " + getName(variationsParameters[i].variationType) + cursor;

        if(indexOfChanges==i)
        {
            float strLength = functionString.size();


            ofPushMatrix();
            ofSetColor(col);
            ofTranslate(-10*u,-28*u);
            ofDrawRectangle(0,0,20*u+myFont.stringWidth(functionString),37*u);
            ofPopMatrix();

            ofSetColor(255-col);
            ofPushMatrix();
            //ofScale(1+0.075*(0.5+0.5*cos(0.5*frameNum))*15/strLength,1);
            myFont.drawString(functionString,0,0);
            ofPopMatrix();
        }
        else
        {
            ofSetColor(col);
            myFont.drawString(functionString,0,0);
        }

        if(i>0 && (threeD==0||(reversedIndex!=curNumberOfSuccessiveVariations/2-1)))
        {
            float troffset = 7*cos(TWO_PI*ofClamp(2.3*fmod(123+0.027*frameNum - 0.15*i,2.3),0,1));

            ofPushMatrix();
            ofTranslate(1,35 + troffset);
            ofScale(1,0.6);
            ofSetColor(col);
            myFont.drawString("^",0,0);
            ofTranslate(5.5,15);
            myFont.drawString("|",0,0);
            ofPopMatrix();
        }

        ofPopMatrix();
    }

    ofPopMatrix();

    ofTranslate(0,0);

    ofScale(0.8);

    float infoAlpha = 255*pow(infoParameter,2.5);

    float alpha0 = 255-infoAlpha;

    float col = (colorMode!=0 && colorMode!=3 ? 255 : 0);

    if(numberOfGamepads>0)
    {
        ofPushMatrix();
        ofTranslate(500*u,-50-300*u*infoParameter);
        ofSetColor(col,alpha0*0.75);
        ofScale(0.9);
        myFont.drawString("L2 to show info and controls",0,0);
        ofPopMatrix();
    }

    if(numberOfGamepads>0 && infoParameter>0.0001)
    {
        ofPushMatrix();

        float einfoP = 1-pow(1-infoParameter,2.5);

        ofTranslate(0,100*u*(1-einfoP));

        yTranslation *= 0.8;

        std::string howItWorksString = "HOW IT WORKS :";
        ofPushMatrix();
        ofSetColor(col,infoAlpha);
        ofTranslate(-10*u,-30*u);
        ofDrawRectangle(0,0,20*u+myFont.stringWidth(howItWorksString),38*u);
        ofPopMatrix();
        ofSetColor(255-col,infoAlpha);
        myFont.drawString(howItWorksString,0,0);
        ofSetColor(col,infoAlpha);

        ofTranslate(0,yTranslation);
        myFont.drawString("Particles initially filling a square go through several",0,0);
        ofTranslate(0,yTranslation);
        myFont.drawString("functions by composition (following the arrows).",0,0);
        ofTranslate(0,yTranslation);
        myFont.drawString("Most of those functions have random parameters.",0,0);
        ofTranslate(0,yTranslation);
        ofTranslate(0,yTranslation);

        std::string controlsString = "CONTROLS :";
        ofPushMatrix();
        ofSetColor(col,infoAlpha);
        ofTranslate(-10*u,-30*u);
        ofDrawRectangle(0,0,20*u+myFont.stringWidth(controlsString),38*u);
        ofPopMatrix();
        ofSetColor(255-col,infoAlpha);
        myFont.drawString(controlsString,0,0);
        ofSetColor(col,infoAlpha);

        ofTranslate(0,yTranslation);
        myFont.drawString("                         : select and change functions",0,0);
        myFontBold.drawString("Pad",0,0);
        ofTranslate(0,yTranslation);
        myFont.drawString("                         : random new set of functions",0,0);
        myFontBold.drawString("A",0,0);
        ofTranslate(0,yTranslation);
        myFont.drawString("                         : randomize parameters of all functions",0,0);
        myFontBold.drawString("B",0,0);
        ofTranslate(0,yTranslation);
        myFont.drawString("                         : randomize parameters of pointed function",0,0);
        myFontBold.drawString("X",0,0);
        ofTranslate(0,yTranslation);
        myFont.drawString("                         : in 2D, keep particles in screen or not",0,0);
        myFontBold.drawString("Y",0,0);
        ofTranslate(0,yTranslation);
        myFont.drawString("                         : in 3D, change 3D projection",0,0);
        ofTranslate(0,yTranslation);
        myFont.drawString("                         : add/remove function",0,0);
        myFontBold.drawString("L1/R1",0,0);
        ofTranslate(0,yTranslation);
        myFont.drawString("                         : scale/rotation on function",0,0);
        myFontBold.drawString("Left joystick",0,0);
        ofTranslate(0,yTranslation);
        myFont.drawString("                         : translation on function (view rotation in 3D)",0,0);
        myFontBold.drawString("Right joystick",0,0);
        ofTranslate(0,yTranslation);
        myFont.drawString("                         : 2D/3D modes",0,0);
        myFontBold.drawString("Middle left",0,0);
        ofTranslate(0,yTranslation);
        myFont.drawString("                         : color modes",0,0);
        myFontBold.drawString("Middle right",0,0);
        ofTranslate(0,yTranslation);
        ofTranslate(0,yTranslation);

        std::string aboutString = "ABOUT :";
        ofPushMatrix();
        ofSetColor(col,infoAlpha);
        ofTranslate(-10*u,-30*u);
        ofDrawRectangle(0,0,20*u+myFont.stringWidth(aboutString),38*u);
        ofPopMatrix();
        ofSetColor(255-col,infoAlpha);
        myFont.drawString(aboutString,0,0);
        ofSetColor(col,infoAlpha);

        ofTranslate(0,yTranslation);
        myFont.drawString("By Etienne Jacob",0,0);
        ofTranslate(0,yTranslation);
        myFont.drawString("Inspired by work from GenerateMe (Twitter: @generateme_blog)",0,0);

        ofPopMatrix();
    }

    ofPopMatrix();
}

// long boring code at the end :)
// change parameters of variation i
void ofApp::changeVariationParameters(int i)
{
    actionResetScale(i);

    variationsParameters[i].tx = 0;
    variationsParameters[i].ty = 0;

    variationsParameters[i].controlledRotationAngle = 0;

    variationsParameters[i].lines_scale = ofRandom(0.03f,0.4f);

    variationsParameters[i].lines_squared = ofRandom(0,1) < 0.5f ? true : false;
    variationsParameters[i].twintrian_weight = ofRandom(0.4f,1);

    variationsParameters[i].rdu01_0 = ofRandom(0,1);

    variationsParameters[i].julian_power = ofRandom(0,1)<0.5f? (int)ofRandom(4,10) : -(int)ofRandom(4,10);
    variationsParameters[i].julian_dist = ofRandom(0.5f,3.0f);
    variationsParameters[i].juliascope_power = ofRandom(0,1)<0.5f? (int)ofRandom(4,10) : -(int)ofRandom(4,10);
    variationsParameters[i].juliascope_dist = ofRandom(0.5f,2.0f);

    variationsParameters[i].rectangles_x = ofRandom(0.1f,1);
    variationsParameters[i].rectangles_y = ofRandom(0.1f,1);

    variationsParameters[i].scry_weight = ofRandom(0.4f,1);

    variationsParameters[i].fan2_x = ofRandom(-1,1);
    variationsParameters[i].fan2_y = ofRandom(2,7);

    variationsParameters[i].pdj_a = ofRandom(-3.0f,3.0f);
    variationsParameters[i].pdj_b = ofRandom(-3.0f,3.0f);
    variationsParameters[i].pdj_c = ofRandom(-3.0f,3.0f);
    variationsParameters[i].pdj_d = ofRandom(-3.0f,3.0f);

    variationsParameters[i].wedgejulia_count = (int)ofRandom(2,7);
    variationsParameters[i].wedgejulia_angle = ofRandom(-3,3);
    variationsParameters[i].wedgejulia_power = ofRandom(0,1)<0.5f?(int)ofRandom(2,7):-(int)ofRandom(2,7);
    variationsParameters[i].wedgejulia_dist = ofRandom(1,4);

    variationsParameters[i].rotation_angle = ofRandom(0,TWO_PI);

    variationsParameters[i].curl_c1 = ofRandom(0.1f,0.7f);
    variationsParameters[i].curl_c2 = ofRandom(0.1f,0.7f);

    variationsParameters[i].butterfly_xscl = ofRandom(0.8,1.5);

    variationsParameters[i].rings2_val = ofRandom(0.1,1.2);

    variationsParameters[i].popcorn_c = ofRandom(-0.8f,0.8f);
    variationsParameters[i].popcorn_f = ofRandom(-0.8f,0.8f);
    variationsParameters[i].waves_b = ofRandom(-0.8f,0.8f);
    variationsParameters[i].waves_e = ofRandom(-0.8f,0.8f);
    variationsParameters[i].waves_c = ofRandom(-0.8f,0.8f);
    variationsParameters[i].waves_f = ofRandom(-0.8f,0.8f);

    variationsParameters[i].bipolar_shift = ofRandom(-1.5,1.5);

    variationsParameters[i].cell_size = ofRandom(-2.5,2.5);

    variationsParameters[i].supershape_rnd = ofRandom(0,1)<0.8?0:ofRandom(0,0.3);
    variationsParameters[i].supershape_m = ofRandom(1,20);
    variationsParameters[i].supershape_n1 = ofRandom(0.5,20);
    variationsParameters[i].supershape_n2 = ofRandom(0.5,20);
    variationsParameters[i].supershape_n3 = ofRandom(0.5,20);
    variationsParameters[i].supershape_holes = ofRandom(-1,1);

    variationsParameters[i].loonie_weight = ofRandom(0.8,2.1);

    variationsParameters[i].blade_weight = ofRandom(0.4,1);

    variationsParameters[i].rays_weight = ofRandom(0.4,1);

    variationsParameters[i].ngon_sides = (int)ofRandom(3,9);
    variationsParameters[i].ngon_power = ofRandom(1.01, 4.0);
    variationsParameters[i].ngon_circle = ofRandom(0.5, 1.5);
    variationsParameters[i].ngon_corners = ofRandom(0.5, 1.5);

    variationsParameters[i].oscilloscope_separation = ofRandom(0.01,2);
    variationsParameters[i].oscilloscope_frequency = ofRandom(-8,8);
    variationsParameters[i].oscilloscope_amplitude = ofRandom(0.4,3);
    variationsParameters[i].oscilloscope_damping = ofRandom(0,1);

    variationsParameters[i].oscilloscope2_separation = ofRandom(0.01,2);
    variationsParameters[i].oscilloscope2_frequencyx = ofRandom(-8,8);
    variationsParameters[i].oscilloscope2_frequencyy = ofRandom(-8,8);
    variationsParameters[i].oscilloscope2_amplitude = ofRandom(0.4,3);
    variationsParameters[i].oscilloscope2_damping = ofRandom(0,1);
    variationsParameters[i].oscilloscope2_perturbation = ofRandom(0,1);

    variationsParameters[i].waves2_freqx = ofRandom(0.01,4);
    variationsParameters[i].waves2_freqy = ofRandom(0.01,4);
    variationsParameters[i].waves2_scalex = ofRandom(0.5,1.5);
    variationsParameters[i].waves2_scaley = ofRandom(0.5,1.5);

    variationsParameters[i].blocky_x = (ofRandom(0,1)<0.5?1.0:-1.0) * ofRandom(0.9,1);
    variationsParameters[i].blocky_y = (ofRandom(0,1)<0.5?1.0:-1.0) * ofRandom(0.9,1);
    variationsParameters[i].blocky_mp = (ofRandom(0,1)<0.5?1.0:-1.0) * ofRandom(1.5,6);
    variationsParameters[i].blocky_weight = ofRandom(0.4,1);

    variationsParameters[i].blob_low = ofRandom(0.2,0.7);
    variationsParameters[i].blob_high = ofRandom(0.8,1.2);
    variationsParameters[i].blob_waves = ofRandom(2,7);

    variationsParameters[i].bsplit_shiftx = ofRandom(-2,2);
    variationsParameters[i].bsplit_shifty = ofRandom(-2,2);

    variationsParameters[i].bswirl_in = ofRandom(-2,2);
    variationsParameters[i].bswirl_out = ofRandom(-2,2);

    variationsParameters[i].bwraps7_cellsize = ofRandom(0.4,2);
    variationsParameters[i].bwraps7_space = ofRandom(0,0.8);
    variationsParameters[i].bwraps7_gain = ofRandom(0.5,2.5);
    variationsParameters[i].bwraps7_inner_twist = ofRandom(-PI,PI);
    variationsParameters[i].bwraps7_outer_twist = ofRandom(-PI,PI);

    variationsParameters[i].blurzoom_x = ofRandom(-1,1);
    variationsParameters[i].blurzoom_y = ofRandom(-1,1);
    variationsParameters[i].blurzoom_length = ofRandom(-1.0,1.5);

    variationsParameters[i].bmod_radius = ofRandom(0.01, TWO_PI);
    variationsParameters[i].bmod_distance = ofRandom(0.01, 2);

    variationsParameters[i].bcollide_num = ofRandom(1.1,8.1);
    variationsParameters[i].bcollide_a = ofRandom(-0.5,1);

    variationsParameters[i].boarders2_c = ofRandom(0.1,0.9);
    variationsParameters[i].boarders2_left = ofRandom(0.1,0.9);
    variationsParameters[i].boarders2_right = ofRandom(0.1,0.9);

    variationsParameters[i].lazysuzan_twist = ofRandom(-2,2);
    variationsParameters[i].lazysuzan_space = ofRandom(-1,2);
    variationsParameters[i].lazysuzan_spin = ofRandom(-3,3);
    variationsParameters[i].lazysuzan_x = ofRandom(-0.5,0.5);
    variationsParameters[i].lazysuzan_y = ofRandom(-0.5,0.5);
    variationsParameters[i].lazysuzan_weight = ofRandom(0.4,1);

    variationsParameters[i].glynnsim1_radius = ofRandom(1.0,2.0);
    variationsParameters[i].glynnsim1_radius1 = ofRandom(0.05,0.8);
    variationsParameters[i].glynnsim1_phi1 = ofRandom(0,360);
    variationsParameters[i].glynnsim1_thickness = ofRandom(0,1);
    variationsParameters[i].glynnsim1_pow = ofRandom(-3,3);
    variationsParameters[i].glynnsim1_contrast = ofRandom(0,1);

    variationsParameters[i].rings_c2 = sq(ofRandom(-0.9,0.9));

    variationsParameters[i].escher_beta = ofRandom(-3,3);

    variationsParameters[i].radialblur_angle = HALF_PI * ofRandom(-1,1);

    variationsParameters[i].split_xsize = ofRandom(-0.8,0.8);
    variationsParameters[i].split_ysize = ofRandom(-0.8,0.8);

    //////////////////////////////////////////////////////
    // precomputed parameters from previous parameters //
    float tmp,tmp2;

    variationsParameters[i].julian_cpower = variationsParameters[i].julian_dist/variationsParameters[i].julian_power/2.0f;
    variationsParameters[i].julian_abspower = abs(variationsParameters[i].julian_power);

    variationsParameters[i].juliascope_cpower = variationsParameters[i].juliascope_dist/variationsParameters[i].juliascope_power/2.0f;
    variationsParameters[i].juliascope_abspower = abs(variationsParameters[i].juliascope_power);

    variationsParameters[i].fan2_dx = PI * variationsParameters[i].fan2_x * variationsParameters[i].fan2_x + 1e-4f;
    variationsParameters[i].fan2_dx2 = 0.5f * variationsParameters[i].fan2_dx;

    variationsParameters[i].wedgejulia_cf = 1.0f - 0.5f / PI * variationsParameters[i].wedgejulia_angle * variationsParameters[i].wedgejulia_count;
    variationsParameters[i].wedgejulia_cn = variationsParameters[i].wedgejulia_dist / variationsParameters[i].wedgejulia_power / 2.0f;
    variationsParameters[i].wedgejulia_rN = abs(variationsParameters[i].wedgejulia_power);

    variationsParameters[i].rings2_val2 = variationsParameters[i].rings2_val * variationsParameters[i].rings2_val;

    variationsParameters[i].bipolar_ps = -HALF_PI * variationsParameters[i].bipolar_shift;

    variationsParameters[i].supershape_pm_4 = variationsParameters[i].supershape_m / 4.0;
    variationsParameters[i].supershape_pneg1_n1 = -1.0 / variationsParameters[i].supershape_n1;

    variationsParameters[i].ngon_pow = variationsParameters[i].ngon_power/2.0;
    variationsParameters[i].ngon_b = TWO_PI / variationsParameters[i].ngon_sides;

    variationsParameters[i].blob_bdiff = variationsParameters[i].blob_high-variationsParameters[i].blob_low;

    tmp = 0.5 * (variationsParameters[i].bwraps7_cellsize / (1.0 + sq(variationsParameters[i].bwraps7_space)));
    variationsParameters[i].bwraps7_g2 = sq(variationsParameters[i].bwraps7_gain) + 1.0e-10;
    tmp2 = variationsParameters[i].bwraps7_g2 * tmp;
    if(tmp2>2.0)
      tmp2 = 1.0;
     else
      tmp2 *= 1.0 / (( sq(tmp2) / 4.0 + 1.0));
    variationsParameters[i].bwraps7_r2 = sq(tmp);
    variationsParameters[i].bwraps7_rfactor = tmp / tmp2;

    variationsParameters[i].bcollide_bcn_pi = variationsParameters[i].bcollide_num / PI;
    variationsParameters[i].bcollide_pi_bcn = PI / variationsParameters[i].bcollide_num;
    variationsParameters[i].bcollide_bca_bcn = (PI * variationsParameters[i].bcollide_a) / variationsParameters[i].bcollide_num;

    variationsParameters[i].boarders2_cl = variationsParameters[i].boarders2_c * variationsParameters[i].boarders2_left;
    variationsParameters[i].boarders2_cr = variationsParameters[i].boarders2_c + (variationsParameters[i].boarders2_c * variationsParameters[i].boarders2_right);

    tmp = PI * variationsParameters[i].glynnsim1_phi1 / 180.0;
    variationsParameters[i].glynnsim1_x1 = variationsParameters[i].glynnsim1_radius * cos(tmp);
    variationsParameters[i].glynnsim1_y1 = variationsParameters[i].glynnsim1_radius * sin(tmp);
    variationsParameters[i].glynnsim1_abspow = abs(variationsParameters[i].glynnsim1_pow);

    variationsParameters[i].escher_vc = 0.5 * (1.0 + cos(variationsParameters[i].escher_beta));
    variationsParameters[i].escher_vd = 0.5 * sin(variationsParameters[i].escher_beta);

    variationsParameters[i].radialblur_spinvar = sin(variationsParameters[i].radialblur_angle);
    variationsParameters[i].radialblur_zoomvar = cos(variationsParameters[i].radialblur_angle);
}

// names of primitive variations
std::string ofApp::getName(int ind)
{
    switch(ind){
        case 0: return "lines"; break;
        case 1: return "twintrian"; break;
        case 2: return "julian"; break;
        case 3: return "juliascope"; break;
        case 4: return "rectangles"; break;
        case 5: return "sinusoidal"; break;
        case 6: return "scry"; break;
        case 7: return "fan2"; break;
        case 8: return "pdj"; break;
        case 9: return "butterfly"; break;
        case 10: return "wedgejulia"; break;
        case 11: return "rotate"; break;
        case 12: return "cross"; break;
        case 13: return "curl"; break;
        case 14: return "rings2"; break;
        case 15: return "heart"; break;
        case 16: return "ex"; break;
        case 17: return "popcorn"; break;
        case 18: return "waves"; break;
        case 19: return "horseshoe"; break;
        case 20: return "polar"; break;
        case 21: return "ennepers"; break;
        case 22: return "cell"; break;
        case 23: return "boarders"; break;
        case 24: return "supershape"; break;
        case 25: return "loonie"; break;
        case 26: return "foci"; break;
        case 27: return "blade"; break;
        case 28: return "ngon"; break;
        case 29: return "diamond"; break;
        case 30: return "oscilloscope"; break;
        case 31: return "oscilloscope2"; break;
        case 32: return "blocky"; break;
        case 33: return "waves2"; break;
        case 34: return "blob"; break;
        case 35: return "bsplit"; break;
        case 36: return "bswirl"; break;
        case 37: return "bwraps7"; break;
        case 38: return "exponential"; break;
        case 39: return "blurzoom"; break;
        case 40: return "bmod"; break;
        case 41: return "bcollide"; break;
        case 42: return "boarders2"; break;
        case 43: return "eyefish"; break;
        case 44: return "disc"; break;
        case 45: return "cot"; break;
        case 46: return "julia"; break;
        case 47: return "spiral"; break;
        case 48: return "tangent"; break;
        case 49: return "rings"; break;
        case 50: return "split"; break;
    default : return "undefined";
    }
    return "";
}
