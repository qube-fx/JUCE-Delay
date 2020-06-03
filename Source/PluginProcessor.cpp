/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
QubeDelayAudioProcessor::QubeDelayAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    // set up parameters and ranges
    addParameter(mDryWetParameter = new AudioParameterFloat("drywet",
                                                            "Dry Wet",
                                                            0.0,
                                                            1.0,
                                                            0.5));
    
    addParameter(mFeedbackParameter = new AudioParameterFloat("feedback",
                                                              "Feedback",
                                                              0,
                                                              0.98,
                                                              0.1));
    
    addParameter(mDelayTimeParameter = new AudioParameterFloat("delaytime",
                                                               "Delay Time",
                                                               0.01,
                                                               MAX_DELAY_TIME,
                                                               0.2));
    
    // initialise variables
    mCircularBufferLeft = nullptr;
    mCircularBufferRight = nullptr;
    mCircularBufferWriteHead = 0;
    mCircularBufferLength = 0;
    mDelayTimeInSamples = 0;
    mDelayReadHead = 0;
    mFeedbackLeft = 0;
    mFeedbackRight = 0;
    mDelayTimeSmoothed = 0;
}

QubeDelayAudioProcessor::~QubeDelayAudioProcessor()
{
    if (mCircularBufferLeft != nullptr) {
        delete [] mCircularBufferLeft;
        mCircularBufferLeft = nullptr;
    }
    
    if (mCircularBufferRight != nullptr) {
        delete [] mCircularBufferRight;
        mCircularBufferRight = nullptr;
    }
}

//==============================================================================
const String QubeDelayAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool QubeDelayAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool QubeDelayAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool QubeDelayAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double QubeDelayAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int QubeDelayAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int QubeDelayAudioProcessor::getCurrentProgram()
{
    return 0;
}

void QubeDelayAudioProcessor::setCurrentProgram (int index)
{
}

const String QubeDelayAudioProcessor::getProgramName (int index)
{
    return {};
}

void QubeDelayAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void QubeDelayAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
     // calculate delay time in samples according to the parameter value and sample rate
     mDelayTimeInSamples = sampleRate * *mDelayTimeParameter;
     // set the buffer length according to maximum delay time and sample rate
     mCircularBufferLength = sampleRate * MAX_DELAY_TIME;
     
    // Buffer set up - left channel
    // if the buffer is not a nullptr, set it to be
     if (mCircularBufferLeft != nullptr ) {
       delete [] mCircularBufferLeft;
       mCircularBufferLeft = nullptr;
    }
    // if the buffer is a nullptr, create a new float for it
    if (mCircularBufferLeft == nullptr ) {
        mCircularBufferLeft = new float[(int)mCircularBufferLength];
    }
     // fill the buffer with zeros
     zeromem(mCircularBufferLeft, mCircularBufferLength * sizeof(float));
     
    // Buffer set up - right channel
    if (mCircularBufferRight != nullptr ) {
        delete [] mCircularBufferRight;
        mCircularBufferRight = nullptr;
     }
      
     if (mCircularBufferRight == nullptr ) {
         mCircularBufferRight = new float[(int)mCircularBufferLength];
     }
     zeromem(mCircularBufferRight, mCircularBufferLength * sizeof(float));
     
     // initialise the buffer write head to 0
     mCircularBufferWriteHead = 0;
     // initialise the delay time
     mDelayTimeSmoothed = *mDelayTimeParameter;
}

void QubeDelayAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool QubeDelayAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

float QubeDelayAudioProcessor::linInterp(float sample_x, float sample_x1, float inPhase)
{
    // calculate the interpolated sample at position 'inPhase' between two samples 'x' and 'x1'
    return (1 - inPhase) * sample_x + inPhase * sample_x1;
}

void QubeDelayAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
    
    // calculate delay time in samples according to the parameter value and sample rate
    mDelayTimeInSamples = getSampleRate() * *mDelayTimeParameter;
    
    float* leftChannel = buffer.getWritePointer(0);
    float* rightChannel = buffer.getWritePointer(1);
    
    for (int j = 0; j < buffer.getNumSamples(); j++)
    {
        // apply smoothing to the delay time parameter to avoid unwanted artefacts
        mDelayTimeSmoothed = mDelayTimeSmoothed - 0.0001 * (mDelayTimeSmoothed - *mDelayTimeParameter);
        // update delay time in samples
        mDelayTimeInSamples = getSampleRate() * mDelayTimeSmoothed;
        
        // populate the circular buffer with the current sample plus feedback
        mCircularBufferLeft[mCircularBufferWriteHead] = leftChannel[j] + mFeedbackLeft;
        mCircularBufferRight[mCircularBufferWriteHead] = rightChannel[j] + mFeedbackRight;
        // set up the delay read head
        mDelayReadHead = mCircularBufferWriteHead - mDelayTimeInSamples;
        // in case it's not within the buffer range
        if (mDelayReadHead < 0) mDelayReadHead += mCircularBufferLength;
        // set up variables for linear interpolation
        int readHead_x = (int)mDelayReadHead;
        int readHead_x1 = readHead_x + 1;
        if (readHead_x1 >= mCircularBufferLength) readHead_x1 -= mCircularBufferLength;
        float readHeadFloat = mDelayReadHead - readHead_x;
        // perform linear interpolation for delay sample
        float delay_sample_left = linInterp(mCircularBufferLeft[readHead_x], mCircularBufferLeft[readHead_x1], readHeadFloat);
        float delay_sample_right = linInterp(mCircularBufferRight[readHead_x], mCircularBufferRight[readHead_x1], readHeadFloat);
        
        // calculate the feedback according to the Feedback parameter
        mFeedbackLeft = delay_sample_left * *mFeedbackParameter;
        mFeedbackRight = delay_sample_right * *mFeedbackParameter;
        // increment the write head circular buffer iterator
        mCircularBufferWriteHead++;
        // in case it's out of range
        if (mCircularBufferWriteHead >= mCircularBufferLength) {
            mCircularBufferWriteHead = 0;
        }
        // sum the dry and delayed signalsaccording to the DryWet parameter
        buffer.setSample(0, j, buffer.getSample(0,j) * (1 - *mDryWetParameter) + delay_sample_left * *mDryWetParameter);
        buffer.setSample(1, j, buffer.getSample(1,j) * (1 - *mDryWetParameter) + delay_sample_right * *mDryWetParameter);
    }
}

//==============================================================================
bool QubeDelayAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* QubeDelayAudioProcessor::createEditor()
{
    return new QubeDelayAudioProcessorEditor (*this);
}

//==============================================================================
void QubeDelayAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    std::unique_ptr<XmlElement> xml(new XmlElement("Delay"));
    
    xml->setAttribute("DryWet", *mDryWetParameter);
    xml->setAttribute("Feedback", *mFeedbackParameter);
    xml->setAttribute("DelayTime", *mDelayTimeParameter);
    
    copyXmlToBinary(*xml, destData);
}

void QubeDelayAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
    
    if (xml.get() != nullptr && xml->hasTagName("Delay"))
    {
        *mDryWetParameter = xml->getDoubleAttribute("DryWet");
        *mDelayTimeParameter = xml->getDoubleAttribute("DelayTime");
        *mFeedbackParameter = xml->getDoubleAttribute("Feedback");

    }
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new QubeDelayAudioProcessor();
}
