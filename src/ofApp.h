#pragma once

#include "ofMain.h"
#include "ofxGamepadHandler.h"
#include "utils.h"
#include <random>
#include <iomanip>
#include <ctime>
#include <chrono>


#define MAX_NUMBER_OF_VARIATIONS 15
#define CANVAS_WIDTH (1536)
#define CANVAS_HEIGHT (1536)
#define GREY_BACKGROUND_FACTOR 0.88

class ofApp : public ofBaseApp {

public:
    void setup();
    void update();
    void draw();
    void drawFold();

    std::vector<uint32_t> pixelCount;
    ofBufferObject pixelCountBuffer;
    ofShader finalColorShader, pixelCountValueSetterShader, countIncrementerShader;

    ofFbo displayedImage, debugImage;

    int nx = 2048*4; // number of dots (x-axis)
    int ny = 2048*4; // number of dots (y-axis)

    int MAX_FPS = 60;
    float time;

    int numberOfVariationTypes = 51; // warning : to update if necessary

    bool renderNewOne = true;
    bool doPrintState = true;
    int curNumberOfSuccessiveVariations = 6;
    int contrastCount = -8;
    int doSinusoid = 1;
    int uniformWeightCount = 0;
    bool keepFoldTypes = false;
    int indexOfChanges = curNumberOfSuccessiveVariations-1;
    bool addingNewFold = false;
    float sinusoidStretchCount = 0;
    int operationsMode = 0;
    int distortionMode = 0;
    int colorMode = 0;
    int animationIndex = 0;
    float animationAngle = 0;
    float animationRadius = 0.5;
    int afr = 0;
    bool renderAnimation = 0;
    int numFrames = 100;
    int radiusCount = 0;
    int threeD = 0;
    double curRot1 = 0;
    double curRot2 = 0;
    double curRot3 = 0;
    double curRotAxis1 = 0;
    double curRotAxis2 = 0;
    double curTranslationAxis1 = 0;
    double curTranslationAxis2 = 0;
    double curScaleAxis1 = 0;
    double curRotationAxis1 = 0;
    double curL2 = -1;
    double curR2 = -1;
    float latest3DJSMoveTime = -1111;
    float translationStep = 0.02;
    int chosenProjectionDivisor = 0;
    double weightCount[MAX_NUMBER_OF_VARIATIONS];
    int numberOfGamepads = 0;

    void setNewParameters();
    void printState();
    void showState();
    void changeVariationParameters(int i);
    void updateWeight(int i);
    void saveLog(std::string s);
    std::string getName(int ind);

    // gamepad actions
    void actionNewParameters();
    void actionSetBounder();
    void actionRandomizeParametersOfAllVariations();
    void actionRandomizeParametersOfSingleVariation();
    void actionAddFunctionAtCursor();
    void actionRemoveFunctionAtCursor();
    void actionMoveCursor(int step);
    void actionChangeFunctionAtCursor(int step);
    void actionChangeColors();
    void actionChange2D3D();
    void actionResetTranslations();
    void actionResetScales();
    void actionResetScale(int i);
    void actionResetRotations();
    void actionChangeProjection();

    void keyPressed(int key); // keyboard shortcuts here

    void keyReleased(int key);
    void mouseMoved(int x, int y);
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void mouseEntered(int x, int y);
    void mouseExited(int x, int y);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);

    // gamepad events
    void axisChanged(ofxGamepadAxisEvent &e);
    void buttonPressed(ofxGamepadButtonEvent &e);
    void buttonReleased(ofxGamepadButtonEvent &e);

    ofFile myFunctionsTextFile;
    ofTrueTypeFont myFont, myFontBold;

    struct RandomParameters{
        int variationType;
        float weight;
        float tx;
        float ty;
        float controlledRotationAngle;

        bool lines_squared;
        float  lines_scale;
        float twintrian_weight;
        float rdu01_0;
        int julian_power;
        float julian_dist;
        int juliascope_power;
        float juliascope_dist;
        float rectangles_x;
        float rectangles_y;
        float scry_weight;
        float fan2_x;
        float fan2_y;
        float pdj_a;
        float pdj_b;
        float pdj_c;
        float pdj_d;
        int wedgejulia_count;
        float wedgejulia_angle;
        int wedgejulia_power;
        float wedgejulia_dist;
        float rotation_angle;
        float curl_c1;
        float curl_c2;
        float butterfly_xscl;
        float rings2_val;
        float popcorn_c;
        float popcorn_f;
        float waves_b;
        float waves_e;
        float waves_c;
        float waves_f;
        float bipolar_shift;
        float cell_size;
        float supershape_rnd;
        float supershape_m;
        float supershape_n1;
        float supershape_n2;
        float supershape_n3;
        float supershape_holes;
        float loonie_weight;
        float blade_weight;
        float rays_weight;
        int ngon_sides;
        float ngon_power;
        float ngon_circle;
        float ngon_corners;
        float oscilloscope_separation;
        float oscilloscope_frequency;
        float oscilloscope_amplitude;
        float oscilloscope_damping;
        float oscilloscope2_separation;
        float oscilloscope2_frequencyx;
        float oscilloscope2_frequencyy;
        float oscilloscope2_amplitude;
        float oscilloscope2_damping;
        float oscilloscope2_perturbation;
        float blocky_x;
        float blocky_y;
        float blocky_mp;
        float blocky_weight;
        float waves2_freqx;
        float waves2_freqy;
        float waves2_scalex;
        float waves2_scaley;
        float blob_low;
        float blob_high;
        float blob_waves;
        float bsplit_shiftx;
        float bsplit_shifty;
        float bswirl_in;
        float bswirl_out;
        float bwraps7_cellsize, bwraps7_space, bwraps7_gain, bwraps7_inner_twist, bwraps7_outer_twist;
        float blurzoom_x, blurzoom_y, blurzoom_length;
        float bmod_radius, bmod_distance;
        float bcollide_num, bcollide_a;
        float boarders2_c, boarders2_left, boarders2_right;
        float lazysuzan_twist, lazysuzan_space, lazysuzan_spin, lazysuzan_x, lazysuzan_y, lazysuzan_weight;
        float glynnsim1_radius, glynnsim1_radius1, glynnsim1_phi1, glynnsim1_thickness, glynnsim1_pow, glynnsim1_contrast;
        float rings_c2;
        float escher_beta;
        float radialblur_angle;
        float split_xsize, split_ysize;


        float julian_cpower;
        float julian_abspower;
        float juliascope_cpower;
        float juliascope_abspower;
        float fan2_dx;
        float fan2_dx2;
        float wedgejulia_cf;
        float wedgejulia_cn;
        float wedgejulia_rN;
        float rings2_val2;
        float bipolar_ps;
        float supershape_pm_4;
        float supershape_pneg1_n1;
        float ngon_pow;
        float ngon_b;
        float blob_bdiff;
        float bwraps7_g2, bwraps7_r2, bwraps7_rfactor;
        float bcollide_bcn_pi, bcollide_pi_bcn, bcollide_bca_bcn;
        float boarders2_cl, boarders2_cr;
        float glynnsim1_x1, glynnsim1_y1, glynnsim1_abspow;
        float escher_vc, escher_vd;
        float radialblur_spinvar, radialblur_zoomvar;
    };

    std::vector<RandomParameters> variationsParameters;
    ofBufferObject variationsParametersBuffer;
};
