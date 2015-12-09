//
//  ofxVibe.cpp
//  based on https://github.com/BelBES/VIBE
//  ViBE is patented, so ask for licensing before using
//
//  Created by nausea on 12/7/15.
//
//

#include "ofxVibe.h"

ofxVibe::ofxVibe() : random_buffer_size(65535){
    model_=NULL;
    rng_idx=0;
    srand(0);
    for(int i=0;i<random_buffer_size;i++){
        rng_[i]=rand();
    }
    b1st=true;
}

ofxVibe::~ofxVibe(){
    if(model_!=NULL){
        delete[] model_;
    }
}

void ofxVibe::setup(int channels, int samples, int pixel_neighbor, int distance_threshold, int matching_threshold, int update_factor){
    samples_=samples;
    channels_=1; //ONLY 1 CHAN FOR NOW
    pixel_neighbor_=pixel_neighbor;
    distance_threshold_=distance_threshold;
    matching_threshold_=matching_threshold;
    update_factor_=update_factor;
    b1st=true;
}

cv::Vec2i ofxVibe::getRndNeighbor(int i, int j){
    int neighbor_count = (pixel_neighbor_ * 2 + 1) * (pixel_neighbor_ * 2 + 1);
    int rnd = rng_[rng_idx = ( rng_idx + 1 ) % random_buffer_size] % neighbor_count;
    int start_i = i - pixel_neighbor_;
    int start_j = j - pixel_neighbor_;
    int area = pixel_neighbor_ * 2 + 1;
    int position_i = rnd / area;
    int position_j = rnd % area;
    int cur_i = std::max(std::min(start_i + position_i, size_.height - 1), 0);
    int cur_j = std::max(std::min(start_j + position_j, size_.width - 1), 0);
    return cv::Vec2i(cur_i, cur_j);
}

void ofxVibe::init_(const cv::Mat &img){
    CV_Assert(img.channels() == channels_);
    size_ = img.size();
    model_ = new unsigned char[channels_ * samples_ * size_.width * size_.height];
    mask_ = cv::Mat(size_, CV_8UC1, cv::Scalar::all(0));
    
    unsigned char* image = img.data;
    for (int i = 0; i < img.rows; i++)
    {
        for (int j = 0; j < img.cols; j++)
        {
            for (int c = 0; c < channels_; c++)
            {
                model_[channels_ * samples_ * size_.width * i + channels_ * samples_ * j + c] = image[channels_ * size_.width * i + channels_ * j + c];
            }
            for (int s = 1; s < samples_; s++)
            {
                cv::Vec2i rnd_pos = getRndNeighbor(i, j);
                int img_idx = channels_ * size_.width * rnd_pos[0] + channels_ * rnd_pos[1];
                int model_idx = channels_ * samples_ * size_.width * i + channels_ * samples_ * j + channels_ * s;
                for (int c = 0; c < channels_; c ++)
                {
                    model_[model_idx + c] = image[img_idx + c];
                }
            }
        }
    }
}

void ofxVibe::update_(const cv::Mat &img){
    CV_Assert(channels_ == img.channels() && size_ == img.size());
    unsigned char *img_ptr = img.data;
    for (int i = 0; i < img.rows; i++)
    {
        for (int j = 0; j < img.cols; j++)
        {
            bool flag = false;
            int matching_counter = 0;
            int img_idx = channels_ * size_.width * i + channels_ * j;
            for (int s = 0; s < samples_; s ++)
            {
                int model_idx = channels_ * samples_ * size_.width * i + channels_ * samples_ * j + channels_ * s;
                int channels_counter = 0;
                for (int c = 0; c < channels_; c ++)
                {
                    if (std::abs(img_ptr[img_idx + c] - model_[model_idx + c]) < distance_threshold_)
                    {
                        channels_counter++;
                        //                        matching_counter++;
                    }
                }
                if (channels_counter == channels_)
                {
                    matching_counter++;
                }
                if ( matching_counter > matching_threshold_)
                {
                    flag = true;
                    break;
                }
            }
            
            if (flag)
            {
                mask_.data[size_.width * i + j] = 0;
                if(rng_[ rng_idx = ( rng_idx + 1 ) % random_buffer_size] % update_factor_)
                {
                    int sample = rng_[ rng_idx = ( rng_idx + 1 ) % random_buffer_size] % samples_;
                    int model_idx = channels_ * samples_ * size_.width * i + channels_ * samples_ * j + channels_ * sample;
                    for (int c = 0; c < channels_; c ++)
                    {
                        model_[model_idx + c] = img_ptr[img_idx + c];
                    }
                    
                    cv::Vec2i rnd_pos = getRndNeighbor(i, j);
                    sample = rng_[rng_idx = ( rng_idx + 1) % random_buffer_size] % samples_;
                    model_idx = channels_ * samples_ * size_.width * rnd_pos[0] + channels_ * samples_ * rnd_pos[1] + channels_ * sample;
                    for (int c = 0; c < channels_; c ++)
                    {
                        model_[model_idx + c] = img_ptr[img_idx + c];
                    }
                    
                }
            }
            else
            {
                mask_.data[size_.width * i + j] = 255;
            }
        }
    }
}

cv::Mat& ofxVibe::getMask(){
    return mask_;
}

void ofxVibe::convertColorSpace(cv::Mat & inMat, cv::Mat & outMat){
    if(inMat.channels()>1){
        cv::cvtColor(inMat, outMat, CV_BGR2GRAY);
    }else{
        inMat=outMat.clone();
    }
}

void ofxVibe::update1st(const cv::Mat &img){
    frame = img;
    convertColorSpace(frame, frame);
    init_(frame);
    gray = frame.clone();
}

void ofxVibe::updateInited(const cv::Mat &img){
    update_(gray);
    cv::Mat fg;
    frame.copyTo(fg, getMask());
    frame = img;
    convertColorSpace(frame, gray);
    
    foreground = fg.clone();
}

void ofxVibe::update(const cv::Mat &img){
    if(!b1st){
        updateInited(img);
    }else{
        update1st(img);
        b1st=false;
    }
}