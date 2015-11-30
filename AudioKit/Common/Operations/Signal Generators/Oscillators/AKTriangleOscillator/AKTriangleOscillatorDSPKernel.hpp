//
//  AKTriangleOscillatorDSPKernel.hpp
//  AudioKit
//
//  Autogenerated by scripts by Aurelius Prochazka. Do not edit directly.
//  Copyright (c) 2015 Aurelius Prochazka. All rights reserved.
//

#ifndef AKTriangleOscillatorDSPKernel_hpp
#define AKTriangleOscillatorDSPKernel_hpp

#import "AKDSPKernel.hpp"
#import "AKParameterRamper.hpp"

extern "C" {
#include "soundpipe.h"
}

enum {
    frequencyAddress = 0,
    amplitudeAddress = 1
};

class AKTriangleOscillatorDSPKernel : public AKDSPKernel {
public:
    // MARK: Member Functions

    AKTriangleOscillatorDSPKernel() {}

    void init(int channelCount, double inSampleRate) {
        channels = channelCount;

        sampleRate = float(inSampleRate);

        sp_create(&sp);
        sp_triangle_create(&triangle);
        sp_triangle_init(sp, triangle);
        *triangle->freq = 440;
        *triangle->amp = 0.5;
    }

    void destroy() {
        sp_triangle_destroy(&triangle);
        sp_destroy(&sp);
    }
    
    void reset() {
    }

    void setParameter(AUParameterAddress address, AUValue value) {
        switch (address) {
            case frequencyAddress:
                frequencyRamper.set(clamp(value, (float)0.0, (float)20000.0));
                break;

            case amplitudeAddress:
                amplitudeRamper.set(clamp(value, (float)0.0, (float)1.0));
                break;

        }
    }

    AUValue getParameter(AUParameterAddress address) {
        switch (address) {
            case frequencyAddress:
                return frequencyRamper.goal();

            case amplitudeAddress:
                return amplitudeRamper.goal();

            default: return 0.0f;
        }
    }

    void startRamp(AUParameterAddress address, AUValue value, AUAudioFrameCount duration) override {
        switch (address) {
            case frequencyAddress:
                frequencyRamper.startRamp(clamp(value, (float)0.0, (float)20000.0), duration);
                break;

            case amplitudeAddress:
                amplitudeRamper.startRamp(clamp(value, (float)0.0, (float)1.0), duration);
                break;

        }
    }

    void setBuffer(AudioBufferList *outBufferList) {
        outBufferListPtr = outBufferList;
    }

    void process(AUAudioFrameCount frameCount, AUAudioFrameCount bufferOffset) override {
        // For each sample.
        for (int frameIndex = 0; frameIndex < frameCount; ++frameIndex) {
            double frequency = double(frequencyRamper.getStep());
            double amplitude = double(amplitudeRamper.getStep());

            int frameOffset = int(frameIndex + bufferOffset);

            *triangle->freq = (float)frequency;
            *triangle->amp = (float)amplitude;

            float temp = 0;
            for (int channel = 0; channel < channels; ++channel) {
                float *out = (float *)outBufferListPtr->mBuffers[channel].mData + frameOffset;
                if (channel == 0) {
                    sp_triangle_compute(sp, triangle, nil, &temp);
                }
                *out = temp;
            }
        }
    }

    // MARK: Member Variables

private:

    int channels = 2;
    float sampleRate = 44100.0;

    AudioBufferList *outBufferListPtr = nullptr;

    sp_data *sp;
    sp_triangle *triangle;

public:
    AKParameterRamper frequencyRamper = 440;
    AKParameterRamper amplitudeRamper = 0.5;
};

#endif /* AKTriangleOscillatorDSPKernel_hpp */
