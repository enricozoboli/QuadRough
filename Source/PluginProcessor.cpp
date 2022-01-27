/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
QuadRoughAudioProcessor::QuadRoughAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ), apvts(*this, nullptr, "Parameters", createParameters()),
    //PRE Filters
    PreLowPass(juce::dsp::IIR::Coefficients<float>::makeLowPass(44100, 20000.0f, 0.1)),
    PreHighPass(juce::dsp::IIR::Coefficients<float>::makeHighPass(44100, 20.0f, 0.1)),
    PreLowShelf(juce::dsp::IIR::Coefficients<float>::makeLowShelf(44100, 144.0f, 0.1, 1.0f)),
    PreHighShelf(juce::dsp::IIR::Coefficients<float>::makeHighShelf(44100, 2773.0f, 0.1, 1.0f)),
    PreMidBell(juce::dsp::IIR::Coefficients<float>::makePeakFilter(44100, 755.0f, 0.1, 1.0f)),

    //POST Filters
    PostLowPass(juce::dsp::IIR::Coefficients<float>::makeLowPass(44100, 20000.0f, 0.1)),
    PostHighPass(juce::dsp::IIR::Coefficients<float>::makeHighPass(44100, 20.0f, 0.1)),
    PostLowShelf(juce::dsp::IIR::Coefficients<float>::makeLowShelf(44100, 144.0f, 0.1, 1.0f)),
    PostHighShelf(juce::dsp::IIR::Coefficients<float>::makeHighShelf(44100, 2773.0f, 0.1, 1.0f)),
    PostMidBell(juce::dsp::IIR::Coefficients<float>::makePeakFilter(44100, 755.0f, 0.1, 1.0f))
#endif
{
}

QuadRoughAudioProcessor::~QuadRoughAudioProcessor()
{
}

//==============================================================================
const juce::String QuadRoughAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool QuadRoughAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool QuadRoughAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool QuadRoughAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double QuadRoughAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int QuadRoughAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int QuadRoughAudioProcessor::getCurrentProgram()
{
    return 0;
}

void QuadRoughAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String QuadRoughAudioProcessor::getProgramName (int index)
{
    return {};
}

void QuadRoughAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void QuadRoughAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    //Filters preparation

    lastSampleRate = sampleRate;
    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = samplesPerBlock;
    spec.sampleRate = sampleRate;
    spec.numChannels = getTotalNumOutputChannels();
    
    //PRE Filters
    PreLowPass.prepare(spec);
    PreHighPass.prepare(spec);
    PreLowShelf.prepare(spec);
    PreHighShelf.prepare(spec);
    PreMidBell.prepare(spec);

    //POST Filters
    PostLowPass.prepare(spec);
    PostHighPass.prepare(spec);
    PostLowShelf.prepare(spec);
    PostHighShelf.prepare(spec);
    PostMidBell.prepare(spec);

    //PRE Filters Reset
    PreLowPass.reset();
    PreHighPass.reset();
    PreLowShelf.reset();
    PreHighShelf.reset();
    PreMidBell.reset();

    //POST Filters Reset
    PostLowPass.reset();
    PostHighPass.reset();
    PostLowShelf.reset();
    PostHighShelf.reset();
    PostMidBell.reset();
}

void QuadRoughAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool QuadRoughAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
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

void QuadRoughAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    //Create a context to pass to filters
    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);

    //Retrive values from components
    float inputdb = *apvts.getRawParameterValue("IN");
    float outputdb = *apvts.getRawParameterValue("OUT");
    float input = juce::Decibels::decibelsToGain(inputdb);
    float output = juce::Decibels::decibelsToGain(outputdb);
    float combo = *apvts.getRawParameterValue("DISTTYPE");
    float midsidebtn = *apvts.getRawParameterValue("MIDSIDE");
    float clipbtn = *apvts.getRawParameterValue("CLIPPER");
    float tonedb = *apvts.getRawParameterValue("TONE");
    float postone = juce::Decibels::decibelsToGain(tonedb);
    float negtone = juce::Decibels::decibelsToGain(-tonedb);

    //Prefilters Init
    *PreLowPass.state = *juce::dsp::IIR::Coefficients<float>::makeLowPass(lastSampleRate, 22000.0f, 0.1);
    *PreHighPass.state = *juce::dsp::IIR::Coefficients<float>::makeHighPass(lastSampleRate, 18.0f, 0.1);
    *PreLowShelf.state = *juce::dsp::IIR::Coefficients<float>::makeLowShelf(lastSampleRate, 144.0f, 0.5, postone);
    *PreHighShelf.state = *juce::dsp::IIR::Coefficients<float>::makeHighShelf(lastSampleRate, 2773.0f, 0.5, postone);
    *PreMidBell.state = *juce::dsp::IIR::Coefficients<float>::makePeakFilter(lastSampleRate, 755.0f, 1, negtone);

    //Postfilters Init
    *PostLowPass.state = *juce::dsp::IIR::Coefficients<float>::makeLowPass(lastSampleRate, 22000.0f, 0.1);
    *PostHighPass.state = *juce::dsp::IIR::Coefficients<float>::makeHighPass(lastSampleRate, 18.0f, 0.1);
    *PostLowShelf.state = *juce::dsp::IIR::Coefficients<float>::makeLowShelf(lastSampleRate, 144.0f, 0.5, negtone);
    *PostHighShelf.state = *juce::dsp::IIR::Coefficients<float>::makeHighShelf(lastSampleRate, 2773.0f, 0.5, negtone);
    *PostMidBell.state = *juce::dsp::IIR::Coefficients<float>::makePeakFilter(lastSampleRate, 755.0f, 1, postone );

    buffer.applyGain(input);

    //SAFE FILTERS
    PreLowPass.process(context);
    PreHighPass.process(context);

    //PREFILTERING
    PreHighShelf.process(context);
    PreLowShelf.process(context);
    PreMidBell.process(context);

    //DISTORTION
    if (midsidebtn > 0 && totalNumInputChannels == 2) {
        processMidSide(buffer);
    }
    else {
        processJointChannels(buffer);
    }
    
    //POST FILTERING
    PostHighShelf.process(context);
    PostLowShelf.process(context);
    PostMidBell.process(context);

    //SAFE FILTERS
    PostLowPass.process(context);
    PostHighPass.process(context);

    //CEILING OUTPUT
    if (clipbtn > 0) {
        finalceiling(buffer);
    }
    else {
        buffer.applyGain(output);
    }
}

void QuadRoughAudioProcessor::processJointChannels(juce::AudioBuffer<float>& buffer)
{
    float combo = *apvts.getRawParameterValue("DISTTYPE");

    if (combo == 0) {
        //CLASSIC
        tanhDistortion(buffer);
    }
    else if (combo == 1) {
        //PRISTINE
        asymDistortion(buffer);
    }
    else if (combo == 2) {
        //HARD
        hardclippingDistortion(buffer);
    }
    else if (combo == 3) {
        //MAD
        foldSinDistortion(buffer);
    }
}

void QuadRoughAudioProcessor::processMidSide(juce::AudioBuffer<float>& buffer)
{

    int numSamples = buffer.getNumSamples();
    juce::AudioSampleBuffer midBuffer, sideBuffer;

    float combo = *apvts.getRawParameterValue("DISTTYPE");
    float drivedb = *apvts.getRawParameterValue("DRIVE");
    float drive = juce::Decibels::decibelsToGain(drivedb);

    if (midBuffer.getNumSamples() != numSamples)
    {
        midBuffer.setSize(2, numSamples, false, true, true);
        sideBuffer.setSize(2, numSamples, false, true, true);
    }

    //retrive Left and Right Buffer
    float* left = buffer.getWritePointer(0);
    float* right = buffer.getWritePointer(1);

    //Mid Buffer
    midBuffer.copyFrom(0, 0, left, numSamples);
    juce::FloatVectorOperations::add(midBuffer.getWritePointer(0), right, numSamples);
    juce::FloatVectorOperations::multiply(midBuffer.getWritePointer(0), 0.5f, numSamples);

    //Side Buffer
    sideBuffer.copyFrom(0, 0, left, numSamples);
    juce::FloatVectorOperations::subtract(sideBuffer.getWritePointer(0), right, numSamples);
    juce::FloatVectorOperations::multiply(sideBuffer.getWritePointer(0), 0.5f, numSamples);

    //Distortion to Mid
    if (combo == 0) {
        //CLASSIC
        tanhDistortion(midBuffer);
    }
    else if (combo == 1) {
        //PRISTINE
        asymDistortion(midBuffer);
    }
    else if (combo == 2) {
        //HARD
        hardclippingDistortion(midBuffer);
    }
    else if (combo == 3) {
        //MAD
        foldSinDistortion(midBuffer);
    }

    //Left after processing
    juce::FloatVectorOperations::clear(left, numSamples);
    juce::FloatVectorOperations::add(left, midBuffer.getWritePointer(0), numSamples);
    juce::FloatVectorOperations::add(left, sideBuffer.getWritePointer(0), numSamples);

    //Right After processing
    juce::FloatVectorOperations::clear(right, numSamples);
    juce::FloatVectorOperations::add(right, midBuffer.getWritePointer(0), numSamples);
    juce::FloatVectorOperations::subtract(right, sideBuffer.getWritePointer(0), numSamples);
}

void QuadRoughAudioProcessor::tanhDistortion(juce::AudioBuffer<float>& buffer)
{
    auto totalNumInputChannels = getTotalNumInputChannels();

    //Retrive parameter values
    float drivedb = *apvts.getRawParameterValue("DRIVE");
    float drywetprc = *apvts.getRawParameterValue("DRYWET");
    float drywet = drywetprc / 100.0f;
    float drive = juce::Decibels::decibelsToGain(drivedb);

    for (auto channel = 0; channel < totalNumInputChannels; channel++)
    {
        auto* channelData = buffer.getWritePointer(channel);

        for (auto i = 0; i < buffer.getNumSamples(); i++) {

            channelData[i] = channelData[i] * (1 - drywet) + std::tanh(channelData[i] * drive) * std::tanh(4 / drive) * drywet;
        }
    }
}

void QuadRoughAudioProcessor::hardclippingDistortion(juce::AudioBuffer<float>& buffer)
{
    auto totalNumInputChannels = getTotalNumInputChannels();

    //Retrive parameter values
    float drivedb = *apvts.getRawParameterValue("DRIVE");
    float drywetprc = *apvts.getRawParameterValue("DRYWET");
    float drywet = drywetprc / 100.0f;
    float drive = juce::Decibels::decibelsToGain(drivedb);
    
    //fixed threshold
    float threshold = 1.0f;

    for (auto channel = 0; channel < totalNumInputChannels; channel++)
    {

        auto* channelData = buffer.getWritePointer(channel);

        for (auto i = 0; i < buffer.getNumSamples(); i++) {

            if (channelData[i] * drive > threshold) {
                channelData[i] = channelData[i] * (1 - drywet) + threshold*drywet;
            }
            else if (channelData[i] * drive < -threshold) {
                channelData[i] = channelData[i] * (1 - drywet) + (-threshold*drywet);
            }
            else {
                channelData[i] = channelData[i] * (1 - drywet) + channelData[i] * drive * drywet;
            }
        }
    }
}

void QuadRoughAudioProcessor::asymDistortion(juce::AudioBuffer<float>& buffer)
{
    auto totalNumInputChannels = getTotalNumInputChannels();

    //Retrive parameter values
    float drivedb = *apvts.getRawParameterValue("DRIVE");
    float drywetprc = *apvts.getRawParameterValue("DRYWET");
    float drywet = drywetprc / 100.0f;
    float drive = juce::Decibels::decibelsToGain(drivedb);
    
    //Fixed parameters for distortion shapes
    float q = -0.05f;
    float d = 7.0f;

    for (auto channel = 0; channel < totalNumInputChannels; channel++)
    {
        auto* channelData = buffer.getWritePointer(channel);

        for (auto i = 0; i < buffer.getNumSamples(); i++) {

            channelData[i] = channelData[i] * (1 - drywet) +  ((channelData[i] * drive - q) / (1 - exp(-d * (channelData[i] * drive - q))) + q / (1 - exp(d * q)))*drywet;
        }
    }
}

void QuadRoughAudioProcessor::foldSinDistortion(juce::AudioBuffer<float>& buffer)
{
    auto totalNumInputChannels = getTotalNumInputChannels();

    //Retrive parameter values
    float drivedb = *apvts.getRawParameterValue("DRIVE");
    float drywetprc = *apvts.getRawParameterValue("DRYWET");
    float drywet = drywetprc / 100.0f;
    float drive = juce::Decibels::decibelsToGain(drivedb);
    
    //Factor to "speed" the distortion
    float factor = 4.0f;

    for (auto channel = 0; channel < totalNumInputChannels; channel++)
    {

        auto* channelData = buffer.getWritePointer(channel);

        for (auto i = 0; i < buffer.getNumSamples(); i++) {

            channelData[i] = channelData[i] * (1 - drywet) + (channelData[i] + std::sin(factor * channelData[i] * drive)) * 0.25 * drywet;
        }
    }
}

void QuadRoughAudioProcessor::finalceiling(juce::AudioBuffer<float>& buffer)
{
    auto totalNumInputChannels = getTotalNumInputChannels();

    //Retrive output value
    float outputdb = *apvts.getRawParameterValue("OUT");
    float output = juce::Decibels::decibelsToGain(outputdb);

    for (auto channel = 0; channel < totalNumInputChannels; channel++)
    {

        auto* channelData = buffer.getWritePointer(channel);

        for (auto i = 0; i < buffer.getNumSamples(); i++) {

            if (channelData[i] > 1.0f) {
                channelData[i] = output;
            }
            else if (channelData[i] < -1.0f) {
                channelData[i] = -output;
            }
            else {
                channelData[i] = channelData[i] * output;
            }
        }
    }
}

//==============================================================================
bool QuadRoughAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* QuadRoughAudioProcessor::createEditor()
{
    return new QuadRoughAudioProcessorEditor (*this);
}

//==============================================================================
void QuadRoughAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void QuadRoughAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new QuadRoughAudioProcessor();
}

juce::AudioProcessorValueTreeState::ParameterLayout QuadRoughAudioProcessor::createParameters()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> parameters;

    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("IN", "Input Knob", -12.0f, 12.0f, 0.f));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("OUT", "Output Knob", -12.0f, 12.0f, 0.f));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("DRIVE", "Drive Knob", 0.f, 20.0f, 0.f));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("DRYWET", "DryWet Knob", 0.f, 100.0f, 100.f));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("TONE", "Tone Knob", -20.0f, 20.0f, 0.f));
    parameters.push_back(std::make_unique<juce::AudioParameterBool>("MIDSIDE", "MidSide Button", false));
    parameters.push_back(std::make_unique<juce::AudioParameterBool>("CLIPPER", "Clipper Button", false));
    parameters.push_back(std::make_unique<juce::AudioParameterChoice>("DISTTYPE", "Distortion Type", juce::StringArray("CLASSIC", "PRISTINE", "HARD", "MAD"), 0));


    return { parameters.begin(), parameters.end() };
}