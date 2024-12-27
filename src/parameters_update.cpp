#include "ofApp.h"

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

// this important parameters randomization work comes from GenerateMe (aka tsulej)
// change parameters of variation i
void ofApp::changeVariationParameters(int i)
{
    actionResetScale(i);
    randomizationSignal[i] = time;

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