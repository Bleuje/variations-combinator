#include "ofApp.h"

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

// reset scales/weights
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

// reset rotations
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

// reset scale/weight
void ofApp::actionResetScale(int i)
{
    weightCount[i] = 0;
    updateWeight(i);
    variationsParametersBuffer.updateData(variationsParameters);
    renderNewOne = true;
    doPrintState = false;
}

// change stereographic projection for 3D mode
void ofApp::actionChangeProjection()
{
    chosenProjectionDivisor = (chosenProjectionDivisor + 1)%4;
    renderNewOne = true;
    doPrintState = false;
}

// save image, and with a .txt log to know its variations
void ofApp::actionSaveImage()
{
    std::string s = ofGetTimestampString();
    ofPixels pixels;
    displayedImage.readToPixels(pixels);
    ofSaveImage(pixels,"images/image_"+s+"_large.png", OF_IMAGE_QUALITY_BEST);
    std::cout << "Saved image " << s << std::endl;
    doPrintState = false;
    saveLog(s);
}

// save screenshot of the entire screen
void ofApp::actionSaveScreenshot()
{
    std::string s = "screenshot_"+ofGetTimestampString();
    ofSaveScreen("images/"+s+".png");
    std::cout << "Saved screenshot " << s << std::endl;
    saveLog(s);
}