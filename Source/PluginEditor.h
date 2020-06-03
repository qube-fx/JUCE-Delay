/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class QubeDelayAudioProcessorEditor  : public AudioProcessorEditor
{
public:
    QubeDelayAudioProcessorEditor (QubeDelayAudioProcessor&);
    ~QubeDelayAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

private:
    
    Slider mDrywetSlider;
    Slider mFeedbackSlider;
    Slider mDelayTimeSlider;
    
    Label dryWetLabel;
    Label feedbackLabel;
    Label timeLabel;

    QubeDelayAudioProcessor& processor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (QubeDelayAudioProcessorEditor)
};
