//
//  AKVariableDelayDSPKernel.hpp
//  AudioKit
//
//  Autogenerated by scripts by Aurelius Prochazka. Do not edit directly.
//  Copyright (c) 2015 Aurelius Prochazka. All rights reserved.
//

#ifndef AKVariableDelayDSPKernel_hpp
#define AKVariableDelayDSPKernel_hpp

#import "AKDSPKernel.hpp"
#import "AKParameterRamper.hpp"

extern "C" {
#include "soundpipe.h"
}

enum {
    delayTimeAddress = 0
};

class AKVariableDelayDSPKernel : public AKDSPKernel {
public:
    // MARK: Member Functions

    AKVariableDelayDSPKernel() {}

    void init(int channelCount, double inSampleRate) {
        channels = channelCount;

        sampleRate = float(inSampleRate);

        sp_create(&sp);
        sp_vdelay_create(&vdelay);
        sp_vdelay_init(sp, vdelay, internalMaxDelay);
        vdelay->del = 1.0;
    }

    void destroy() {
        sp_vdelay_destroy(&vdelay);
        sp_destroy(&sp);
    }
    
    void reset() {
    }
    void setMaxDelayTime(float duration) {
        internalMaxDelay = duration;
    }

    void setParameter(AUParameterAddress address, AUValue value) {
        switch (address) {
            case delayTimeAddress:
                delayTimeRamper.set(clamp(value, (float)0.0, (float)10.0));
                break;

        }
    }

    AUValue getParameter(AUParameterAddress address) {
        switch (address) {
            case delayTimeAddress:
                return delayTimeRamper.goal();

            default: return 0.0f;
        }
    }

    void startRamp(AUParameterAddress address, AUValue value, AUAudioFrameCount duration) override {
        switch (address) {
            case delayTimeAddress:
                delayTimeRamper.startRamp(clamp(value, (float)0.0, (float)10.0), duration);
                break;
        }
    }

    void setBuffers(AudioBufferList *inBufferList, AudioBufferList *outBufferList) {
        inBufferListPtr = inBufferList;
        outBufferListPtr = outBufferList;
    }

    void process(AUAudioFrameCount frameCount, AUAudioFrameCount bufferOffset) override {
        // For each sample.
        for (int frameIndex = 0; frameIndex < frameCount; ++frameIndex) {
            double delayTime = double(delayTimeRamper.getStep());

            int frameOffset = int(frameIndex + bufferOffset);

            vdelay->del = (float)delayTime;

            for (int channel = 0; channel < channels; ++channel) {
                float *in  = (float *)inBufferListPtr->mBuffers[channel].mData  + frameOffset;
                float *out = (float *)outBufferListPtr->mBuffers[channel].mData + frameOffset;

                sp_vdelay_compute(sp, vdelay, in, out);
            }
        }
    }

    // MARK: Member Variables

private:

    int channels = 2;
    float sampleRate = 44100.0;

    AudioBufferList *inBufferListPtr = nullptr;
    AudioBufferList *outBufferListPtr = nullptr;

    sp_data *sp;
    sp_vdelay *vdelay;
    
    float internalMaxDelay = 5.0;

public:
    AKParameterRamper delayTimeRamper = 1.0;
};

#endif /* AKVariableDelayDSPKernel_hpp */
