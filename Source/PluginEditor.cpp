/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
QuadRoughAudioProcessorEditor::QuadRoughAudioProcessorEditor (QuadRoughAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{

    //Scalable Interface
    double ratio = 5.0 / 3.0;
    setResizable(true, true);
    setResizeLimits(400, 400 / ratio, 1000, 1000 / ratio);
    getConstrainer()->setFixedAspectRatio(ratio);
    setSize(600, 600 / ratio);


    ///INPUT KNOB///
    inputknob.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    inputknob.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 100, 20);
    inputknob.setRange(-12.0, 12.0, 0.1);
    inputknob.setTextValueSuffix(" dB");
    addAndMakeVisible(inputknob);

    ///INPUT LABEL///
    inputlabel.setText("INPUT", juce::dontSendNotification);
    addAndMakeVisible(inputlabel);

    ///DRIVE KNOB///
    driveknob.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    driveknob.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 100, 20);
    //call repaint to modify the curve displaied
    driveknob.onValueChange = [this] {(repaint(), juce::dontSendNotification); };
    driveknob.setRange(0, 20.0, 0.1);
    driveknob.setTextValueSuffix(" dB");
    addAndMakeVisible(driveknob);

    ///DRIVE LABEL///
    drivelabel.setText("DRIVE", juce::dontSendNotification);
    addAndMakeVisible(drivelabel);

    ///DRYWET KNOB///
    drywetknob.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    drywetknob.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 100, 20);
    drywetknob.setRange(0, 100.0, 0.01);
    drywetknob.setTextValueSuffix("%");
    addAndMakeVisible(drywetknob);

    ///TONE KNOB///
    toneknob.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    toneknob.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 100, 20);
    toneknob.setRange(-20.0, 20.0, 0.1);
    addAndMakeVisible(toneknob);

    ///DRYWET LABEL///
    drywetlabel.setText("DRY/WET", juce::dontSendNotification);
    addAndMakeVisible(drywetlabel);

    /// OUTPUT KNOB///
    outputknob.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    outputknob.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 100, 20);
    outputknob.setRange(-12.0, 12.0, 0.1);
    outputknob.setTextValueSuffix(" dB");
    addAndMakeVisible(outputknob);

    ///OUTPUT LABEL///
    outputlabel.setText("OUTPUT", juce::dontSendNotification);
    addAndMakeVisible(outputlabel);


    ///MID SIDE BUTTON
    midsidebtn.setColour(juce::TextButton::buttonOnColourId, juce::Colours::yellow);
    midsidebtn.setColour(juce::TextButton::buttonColourId, juce::Colours::orange);
    //change the name of the label
    midsidebtn.onStateChange = [this] {
        if (midsidebtn.getToggleState()) {
            midsidelabel.setText("M/S ON", juce::dontSendNotification);
        }
        else {
            midsidelabel.setText("M/S OFF", juce::dontSendNotification);
        }
    };
    
    midsidebtn.setClickingTogglesState(true);
    addAndMakeVisible(midsidebtn);

    ///MIDSIDE LABEL///
    midsidelabel.setText("M/S OFF", juce::dontSendNotification);
    addAndMakeVisible(midsidelabel);

    ///TONE LABEL///
    tonelabel.setText("TONE", juce::dontSendNotification);
    addAndMakeVisible(tonelabel);

    ///CLIPPER BUTTON
    clipperbtn.setColour(juce::TextButton::buttonOnColourId, juce::Colours::red);
    clipperbtn.setColour(juce::TextButton::buttonColourId, juce::Colours::darkred);
    //change the name of the label
    clipperbtn.onStateChange = [this] {
        if (clipperbtn.getToggleState()) {
            ceilinglabel.setText("CEILING ON", juce::dontSendNotification);
        }
        else {
            ceilinglabel.setText("CEILING OFF", juce::dontSendNotification);
        }
    };
    clipperbtn.setClickingTogglesState(true);
    addAndMakeVisible(clipperbtn);

    ///CEILING LABEL///
    ceilinglabel.setText("CEILING OFF", juce::dontSendNotification);
    addAndMakeVisible(ceilinglabel);

    ///COMBO BOX
    distBox.addItem("CLASSIC", 1);
    distBox.addItem("PRISTINE", 2);
    distBox.addItem("HARD", 3);
    distBox.addItem("MAD", 4);
    //call repaint to modify the curve displaied
    distBox.onChange = [this] {(repaint(), juce::dontSendNotification); };
    distBox.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(distBox);


    //ATTACHMENTS///
    outputknobAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "OUT", outputknob);
    inputknobAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "IN", inputknob);
    driveknobAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "DRIVE", driveknob);
    drywetknobAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "DRYWET", drywetknob);
    midsideAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "MIDSIDE", midsidebtn);
    clipperAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "CLIPPER", clipperbtn);
    toneknobAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "TONE", toneknob);
    distboxAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(audioProcessor.apvts, "DISTTYPE", distBox);

    //LOOKANDFEEL///
    inputknob.setLookAndFeel(&customLookAndFeel);
    outputknob.setLookAndFeel(&customLookAndFeel);
    driveknob.setLookAndFeel(&customLookAndFeel);
    drywetknob.setLookAndFeel(&customLookAndFeel);
    toneknob.setLookAndFeel(&customLookAndFeel);
    distBox.setLookAndFeel(&customLookAndFeel);
}

QuadRoughAudioProcessorEditor::~QuadRoughAudioProcessorEditor()
{
    //free the object from the LookAndFeel when the interface is destructed
    inputknob.setLookAndFeel(nullptr);
    outputknob.setLookAndFeel(nullptr);
    driveknob.setLookAndFeel(nullptr);
    drywetknob.setLookAndFeel(nullptr);
    distBox.setLookAndFeel(nullptr);
    toneknob.setLookAndFeel(nullptr);
}

//==============================================================================
void QuadRoughAudioProcessorEditor::paint(juce::Graphics& g)
{
    //BackgroundColor
    g.fillAll(juce::Colour(32, 32, 32));

    //PLOT THE DISTORTION FUNCTION
    float linew = 3;
    juce::Rectangle<int> plot(plot_x, plot_y - linew / 2, plot_w, plot_h + linew);
    g.setColour(juce::Colours::white);
    g.fillRect(plot);
    g.setColour(juce::Colours::red);
    float drivedb = driveknob.getValue();
    float drive = juce::Decibels::decibelsToGain(drivedb);
    int f_type = distBox.getSelectedId();
    float step_func = 0.05;
    
    //Tanh
    if (f_type == 1) {
        for (float ind = -1.5; ind < 1.5 - step_func; ind = ind + step_func) {
            g.drawLine(plot_x + plot_w / 2 + plot_w * ind / 3,
                plot_y + plot_h / 2 - plot_w * std::tanh(drive * ind) / 3,
                plot_x + plot_w / 2 + plot_w * (ind + step_func) / 3,
                plot_y + plot_h / 2 - plot_w * std::tanh(drive * (ind + step_func)) / 3,
                linew);
        }
    }

    //Asymetric
    else if (f_type == 2) {
        float q = -0.05f;
        float d = 7.0f;
        for (float ind = -1.5; ind < 1.5 - step_func; ind = ind + step_func) {
            float y = ((ind)*drive - q) / (1 - exp(-d * ((ind)*drive - q))) + q / (1 - exp(d * q));
            float ystep = ((ind + step_func) * drive - q) / (1 - exp(-d * ((ind + step_func) * drive - q))) + q / (1 - exp(d * q));
            if (ystep < 1.0f) {
                g.drawLine(plot_x + plot_w / 2 + plot_w * ind / 3,
                    plot_y + plot_h / 2 - plot_w * (y) / 3,
                    plot_x + plot_w / 2 + plot_w * (ind + step_func) / 3,
                    plot_y + plot_h / 2 - plot_w * (ystep) / 3,
                    linew);
            }
            else if (y >= 1.0f - ystep && y < 1.0f) {
                g.drawLine(plot_x + plot_w / 2 + plot_w * ind / 3,
                    plot_y + plot_h / 2 - plot_w * (y) / 3,
                    plot_x + plot_w / 2 + plot_w * (ind + step_func) / 3,
                    plot_y + plot_h / 2 - plot_w * (1.05f) / 3,
                    linew);
            }
        }
    }

    //HardClipping
    else if (f_type == 3) {
        g.drawLine(plot_x + plot_w / 2 + plot_w * (-1.5f) / 3,
            plot_y + plot_h / 2 - plot_w * (-1.0f) / 3,
            plot_x + plot_w / 2 + plot_w * (-1.0f / drive + 0.02f) / 3,
            plot_y + plot_h / 2 - plot_w * (-1.0f) / 3,
            linew);
        g.drawLine(plot_x + plot_w / 2 + plot_w * (-1.0f / drive) / 3,
            plot_y + plot_h / 2 - plot_w * (-1.0f) / 3,
            plot_x + plot_w / 2 + plot_w * (1.0f / drive + 0.02f) / 3,
            plot_y + plot_h / 2 - plot_w * (1.0f) / 3,
            linew);
        g.drawLine(plot_x + plot_w / 2 + plot_w * (1.0f / drive) / 3,
            plot_y + plot_h / 2 - plot_w * (1.0f) / 3,
            plot_x + plot_w / 2 + plot_w * (1.5f) / 3,
            plot_y + plot_h / 2 - plot_w * (1.0f) / 3,
            linew);

    }

    //Sinusoidal FoldBack
    else if (f_type == 4) {
        for (float ind = -1.5; ind < 1.5 - step_func; ind = ind + step_func) {
            g.drawLine(plot_x + plot_w / 2 + plot_w * ind / 3,
                plot_y + plot_h / 2 - plot_w * (ind + std::sin(4 * ind * drive)) * 0.25 / 3,
                plot_x + plot_w / 2 + plot_w * (ind + step_func) / 3,
                plot_y + plot_h / 2 - plot_w * (ind + step_func + std::sin(4 * (ind + step_func) * drive)) * 0.25 / 3,
                linew);
        }
    }
}

void QuadRoughAudioProcessorEditor::resized()
{
    //Get the dimension of the plugin
    auto width = getWidth();
    auto height = getHeight();
    auto centerX = width / 2;

    //All Dimensions based on the mainwindow dimension
    const int bigDiam = width * 0.2;
    const int smallDiam = width * 0.15;
    const int btnDiam = smallDiam * 0.3;
    const int fontscaler = width / 35;

    ///INPUT
    inputknob.setBounds(centerX - smallDiam / 2 - width / 2.8, height * 0.6, smallDiam, smallDiam);
    inputlabel.setBounds(centerX - smallDiam / 2 - width / 2.8, height * 0.75, smallDiam, smallDiam);
    inputlabel.setFont(juce::Font(fontscaler, juce::Font::bold));
    inputlabel.setJustificationType(4);

    ///TONE
    toneknob.setBounds(centerX - smallDiam / 2 - width / 4, height * 0.27, smallDiam, smallDiam);
    tonelabel.setBounds(centerX - smallDiam / 2 - width / 4, height * 0.41, smallDiam, smallDiam);
    tonelabel.setFont(juce::Font(fontscaler, juce::Font::bold));
    tonelabel.setJustificationType(4);

    ///OUTPUT
    outputknob.setBounds(centerX - smallDiam / 2 + width / 2.8, height * 0.6, smallDiam, smallDiam);
    outputlabel.setBounds(centerX - smallDiam / 2 + width / 2.8, height * 0.75, smallDiam, smallDiam);
    outputlabel.setFont(juce::Font(fontscaler, juce::Font::bold));
    outputlabel.setJustificationType(4);

    ///DRIVE
    driveknob.setBounds(centerX - bigDiam / 2, height * 0.15, bigDiam, bigDiam);
    drivelabel.setBounds(centerX - smallDiam / 2, height * 0.38, smallDiam, smallDiam);
    drivelabel.setFont(juce::Font(fontscaler, juce::Font::bold));
    drivelabel.setJustificationType(4);

    ///DRYWET
    drywetknob.setBounds(centerX - smallDiam / 2 + width / 4, height * 0.27, smallDiam, smallDiam);
    drywetlabel.setBounds(centerX - smallDiam / 2 + width / 4, height * 0.41, smallDiam, smallDiam);
    drywetlabel.setFont(juce::Font(fontscaler, juce::Font::bold));
    drywetlabel.setJustificationType(4);

    //MIDSIDE BTN
    midsidebtn.setBounds(centerX - btnDiam / 2 - width / 4, height * 0.05, btnDiam, btnDiam);
    midsidelabel.setBounds(centerX - smallDiam / 4 - width / 4, height * 0.12, smallDiam/2, smallDiam/2);
    midsidelabel.setFont(juce::Font(fontscaler, juce::Font::bold));
    midsidelabel.setJustificationType(4);

    //CLIPPER BTN
    clipperbtn.setBounds(centerX - smallDiam / 2 + width / 4, height * 0.05, btnDiam, btnDiam);
    ceilinglabel.setBounds(centerX - smallDiam / 4 + width / 5.5, height * 0.12, smallDiam*0.8, smallDiam*0.6);
    ceilinglabel.setFont(juce::Font(fontscaler, juce::Font::bold));
    ceilinglabel.setJustificationType(4);

    ///COMBO BOX
    distBox.setBounds(centerX - width * 0.2 / 2, height * 0.82, width * 0.2, height * 0.143);
    distBox.setJustificationType(4);

    ///PLOT DISTORTION FUNCTION
    plot_w = width * 0.2;
    plot_h = width * 0.2 / 3 * 2;
    plot_x = centerX - plot_w / 2;
    plot_y = height * 0.58;
}
