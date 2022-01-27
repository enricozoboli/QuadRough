/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
*/
class QuadRoughAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    QuadRoughAudioProcessor();
    ~QuadRoughAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    //Processing equally LR channels
    void processJointChannels(juce::AudioBuffer<float>&);

    //Splitting Mid and Side
    void processMidSide(juce::AudioBuffer<float>&);

    //Ceiling the output level
    void finalceiling(juce::AudioBuffer<float>&);

    //Tanh Algorithm
    void tanhDistortion(juce::AudioBuffer<float>&);

    //FoldSin Distortion
    void foldSinDistortion(juce::AudioBuffer<float>&);

    //Triode algorithm
    void asymDistortion(juce::AudioBuffer<float>&);

    //Hard Clipping
    void hardclippingDistortion(juce::AudioBuffer<float>&);

    //Value tree state Paramters
    juce::AudioProcessorValueTreeState apvts;
    juce::AudioProcessorValueTreeState::ParameterLayout createParameters();

private:
    //IIR filters 
    using Filter = juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>>;
    Filter PreLowPass, PreHighPass, PreLowShelf, PreHighShelf, PreMidBell, PostLowPass, PostHighPass, PostLowShelf, PostHighShelf, PostMidBell;

    //Samplerate used for initilialize filters
    float lastSampleRate;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (QuadRoughAudioProcessor)
};
