//
//  ofxVibe.h
//  based on https://github.com/BelBES/VIBE
//  ViBE is patented, so ask for licensing before using
//
//  Created by nausea on 12/7/15.
//
//

#pragma once
#include "ofxCv.h"

class ofxVibe{
public:
    ofxVibe();
    ~ofxVibe();
    void setup(int channels = 1, int samples = 20, int pixel_neighbor = 1, int distance_threshold = 20, int matching_threshold = 3, int update_factor = 16);
    cv::Mat& getMask();
    
    void update(const cv::Mat& img);
    template<class T> inline void update(ofBaseHasPixels_<T>& img){update(ofxCv::toCv(img));}
    
    inline void drawCurrentFrame(float x, float y){ofxCv::drawMat(frame,x,y);}
    inline void drawForeground(float x, float y){ofxCv::drawMat(foreground,x,y);}
    
    inline cv::Mat getCvForeground(){return foreground;}
    inline ofPixels getOfForeground(){ofPixels pix; ofxCv::toOf(foreground, pix); return pix;}
    
    const int random_buffer_size;
    
protected:
    
    void init_(const cv::Mat& img);
    void update_(const cv::Mat& img);
    void update1st(const cv::Mat& img);
    void updateInited(const cv::Mat& img);
    
    int samples_;
    int channels_;
    int pixel_neighbor_;
    int distance_threshold_;
    int matching_threshold_;
    int update_factor_;
    
    cv::Size size_;
    unsigned char *model_;
    
    cv::Mat mask_;
    cv::Mat frame;
    cv::Mat gray;
    cv::Mat foreground;
    
    unsigned int rng_[65535];
    int rng_idx;
    
    cv::Vec2i getRndNeighbor(int i, int j);
    bool b1st;
};