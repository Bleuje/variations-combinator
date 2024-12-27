#include "ofApp.h"

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
    if(key=='w' || key==OF_KEY_DOWN) // change the cursor position to control single folds
    {
        actionMoveCursor(-1);
    }
    if(key=='x' || key==OF_KEY_UP) // change the cursor position to control single folds
    {
        actionMoveCursor(+1);
    }
    if(key=='q') // change random variationsParameters on single fold (chosen with cursor)
    {
        actionRandomizeParametersOfSingleVariation();
    }
    if(key=='c' || key==OF_KEY_LEFT) // change fold type on cursor
    {
        actionChangeFunctionAtCursor(-1);
    }
    if(key=='v' || key==OF_KEY_RIGHT) // change fold type on cursor
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
    if(key=='&') // functionality to save a perfectly looping gif's frames, using circular translation at cursor, seems to be removed in this version
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