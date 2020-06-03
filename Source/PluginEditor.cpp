/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
QubeDelayAudioProcessorEditor::QubeDelayAudioProcessorEditor (QubeDelayAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    
    setSize (300, 120);
    
    auto& params = processor.getParameters();
    
    //  0 - DRY / WET
    AudioParameterFloat* dryWetParameter = (AudioParameterFloat*)params.getUnchecked(0);
    // Slider set up
    mDrywetSlider.setBounds(0, 20, 100, 100);
    mDrywetSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
    mDrywetSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    mDrywetSlider.setRange(dryWetParameter->range.start, dryWetParameter->range.end);
    mDrywetSlider.setValue(*dryWetParameter);
    addAndMakeVisible(mDrywetSlider);
    // Label
    addAndMakeVisible (dryWetLabel);
    dryWetLabel.setText ("Dry/Wet", dontSendNotification);
    dryWetLabel.attachToComponent (&mDrywetSlider, false);
    // Update the parameter value
    mDrywetSlider.onValueChange = [this, dryWetParameter] { *dryWetParameter = mDrywetSlider.getValue();};
    mDrywetSlider.onDragStart = [dryWetParameter] { dryWetParameter->beginChangeGesture(); };
    mDrywetSlider.onDragEnd = [dryWetParameter] { dryWetParameter->endChangeGesture(); };
    
    // 1 - FEEDBACK
    AudioParameterFloat* feedbackParameter = (AudioParameterFloat*)params.getUnchecked(1);
    
    mFeedbackSlider.setBounds(100, 20, 100, 100);
    mFeedbackSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
    mFeedbackSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    mFeedbackSlider.setRange(feedbackParameter->range.start, feedbackParameter->range.end);
    mFeedbackSlider.setValue(*feedbackParameter);
    addAndMakeVisible(mFeedbackSlider);
    
    addAndMakeVisible (feedbackLabel);
    feedbackLabel.setText ("Feedback", dontSendNotification);
    feedbackLabel.attachToComponent (&mFeedbackSlider, false);
    
    mFeedbackSlider.onValueChange = [this, feedbackParameter] { *feedbackParameter = mFeedbackSlider.getValue(); };
    mFeedbackSlider.onDragStart = [feedbackParameter] { feedbackParameter->beginChangeGesture(); };
    mFeedbackSlider.onDragEnd = [feedbackParameter] { feedbackParameter->endChangeGesture(); };
    
    
    // 2 - DELAY TIME
    AudioParameterFloat* delayTimeParameter = (AudioParameterFloat*)params.getUnchecked(2);
    
    mDelayTimeSlider.setBounds(200, 20, 100, 100);
    mDelayTimeSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
    mDelayTimeSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    mDelayTimeSlider.setRange(delayTimeParameter->range.start, delayTimeParameter->range.end);
    mDelayTimeSlider.setValue(*delayTimeParameter);
    addAndMakeVisible(mDelayTimeSlider);
    
    addAndMakeVisible (timeLabel);
    timeLabel.setText ("Delay time", dontSendNotification);
    timeLabel.attachToComponent (&mDelayTimeSlider, false);
    
    mDelayTimeSlider.onValueChange = [this, delayTimeParameter] { *delayTimeParameter = mDelayTimeSlider.getValue(); };
    mDelayTimeSlider.onDragStart = [delayTimeParameter] { delayTimeParameter->beginChangeGesture(); };
    mDelayTimeSlider.onDragEnd = [delayTimeParameter] { delayTimeParameter->endChangeGesture(); };
}

QubeDelayAudioProcessorEditor::~QubeDelayAudioProcessorEditor()
{
}

//==============================================================================
void QubeDelayAudioProcessorEditor::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
}

void QubeDelayAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}
