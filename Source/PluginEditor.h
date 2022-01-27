/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/

//Custom LookAndFeel Class, Override the classic appereace of Components
class CustomLookAndFeel : public juce::LookAndFeel_V4
{
public:

    //KNOBS
    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos, float rotaryStartAngle, float rotaryEndAngle, juce::Slider& slider) override
    {
        float diameter = juce::jmin(width, height);
        float radius = diameter / 2;
        float centerX = x + width / 2;
        float centerY = y + height / 2;
        float rx = centerX - radius;
        float ry = centerY - radius;
        float angle = rotaryStartAngle + (sliderPos * (rotaryEndAngle - rotaryStartAngle));

        //Updating Variable according to sliderPos
        float vardiamter = diameter * sliderPos;
        float varradius = vardiamter / 2;
        float varRx = centerX - varradius;
        float VarRy = centerY - varradius;

        //Inside Circle
        juce::Rectangle<float> knobArea(rx, ry, diameter, diameter);
        g.setColour(juce::Colours::aliceblue);
        g.fillEllipse(knobArea);

        //Outside Circle
        g.setColour(juce::Colours::darkred);
        juce::Rectangle<float> innerArea(varRx, VarRy, vardiamter, vardiamter);
        g.fillEllipse(innerArea);


        /*///PATH TICK
        juce::Path knobTick;
        knobTick.addRectangle(0, -radius, 2.0f, radius * 0.33);
        g.fillPath(knobTick, juce::AffineTransform::rotation(angle).translated(centerX, centerY));
        */
    }

    //LABELS
    void drawLabel(juce::Graphics& g, juce::Label& label) override
    {
        g.fillAll(label.findColour(juce::Label::backgroundColourId));

        if (!label.isBeingEdited())
        {
            auto alpha = label.isEnabled() ? 1.0f : 0.5f;
            const juce::Font font(getLabelFont(label));

            g.setColour(label.findColour(juce::Label::textColourId).withMultipliedAlpha(alpha));
            //g.setColour(juce::Colours::darkred);
            g.setFont(font);

            auto textArea = getLabelBorderSize(label).subtractedFrom(label.getLocalBounds());

            g.drawFittedText(label.getText(), textArea, label.getJustificationType(),
                juce::jmax(1, (int)((float)textArea.getHeight() / font.getHeight())),
                label.getMinimumHorizontalScale());

            g.setColour(label.findColour(juce::Label::backgroundColourId).withMultipliedAlpha(alpha));

        }

        else if (label.isEnabled())
        {
            g.setColour(label.findColour(juce::Label::backgroundColourId));
        }

        g.drawRect(label.getLocalBounds());
    }

    //COMBOBOX
    void drawComboBox(juce::Graphics& g, int width, int height, bool,
        int, int, int, int, juce::ComboBox& box) override
    {
        auto cornerSize = box.findParentComponentOfClass<juce::ChoicePropertyComponent>() != nullptr ? 0.0f : 3.0f;
        juce::Rectangle<int> boxBounds(0, 0, width, height);

        g.setColour(juce::Colour(32, 32, 32));
        g.fillRoundedRectangle(boxBounds.toFloat(), cornerSize);

        g.setColour(juce::Colour(0, 0, 0));
        g.drawRoundedRectangle(boxBounds.toFloat().reduced(0.5f, 0.5f), cornerSize, 1.0f);

        juce::Rectangle<int> arrowZone(width - 30, 0, 20, height);
        juce::Path path;
        path.startNewSubPath((float)arrowZone.getX() + 3.0f, (float)arrowZone.getCentreY() - 2.0f);
        path.lineTo((float)arrowZone.getCentreX(), (float)arrowZone.getCentreY() + 3.0f);
        path.lineTo((float)arrowZone.getRight() - 3.0f, (float)arrowZone.getCentreY() - 2.0f);

        g.setColour(box.findColour(juce::ComboBox::arrowColourId).withAlpha((box.isEnabled() ? 0.9f : 0.2f)));
        g.strokePath(path, juce::PathStrokeType(2.0f));
    }

    //COMBOBOX MENU
    void drawPopupMenuItem(juce::Graphics& g, const juce::Rectangle<int>& area,
        const bool isSeparator, const bool isActive,
        const bool isHighlighted, const bool isTicked,
        const bool hasSubMenu, const juce::String& text,
        const juce::String& shortcutKeyText,
        const juce::Drawable* icon, const juce::Colour* const textColourToUse) override
    {
        if (isSeparator)
        {
            auto r = area.reduced(5, 0);
            r.removeFromTop(juce::roundToInt(((float)r.getHeight() * 0.5f) - 0.5f));

            g.setColour(juce::Colour(32, 32, 32));
            g.fillRect(r.removeFromTop(1));
        }
        else
        {
            /*auto textColour = (textColourToUse == nullptr ? findColour(juce::PopupMenu::textColourId)
                : *textColourToUse);*/
            auto textColour = juce::Colour(32, 32, 32);

            auto r = area.reduced(1);

            if (isHighlighted && isActive)
            {
                g.setColour(juce::Colour(32, 32, 32));
                g.fillRect(r);

                g.setColour(findColour(juce::PopupMenu::highlightedTextColourId));
            }
            else
            {
                g.setColour(juce::Colour(32, 32, 32));
            }

            r.reduce(juce::jmin(5, area.getWidth() / 20), 0);

            auto font = getPopupMenuFont();

            auto maxFontHeight = (float)r.getHeight() / 1.3f;

            if (font.getHeight() > maxFontHeight)
                font.setHeight(maxFontHeight);

            g.setFont(font);

            auto iconArea = r.removeFromLeft(juce::roundToInt(maxFontHeight)).toFloat();

            if (icon != nullptr)
            {
                icon->drawWithin(g, iconArea, juce::RectanglePlacement::centred | juce::RectanglePlacement::onlyReduceInSize, 1.0f);
                r.removeFromLeft(juce::roundToInt(maxFontHeight * 0.5f));
            }

            if (hasSubMenu)
            {
                auto arrowH = 0.6f * getPopupMenuFont().getAscent();

                auto x = static_cast<float> (r.removeFromRight((int)arrowH).getX());
                auto halfH = static_cast<float> (r.getCentreY());

                juce::Path path;
                path.startNewSubPath(x, halfH - arrowH * 0.5f);
                path.lineTo(x + arrowH * 0.6f, halfH);
                path.lineTo(x, halfH + arrowH * 0.5f);

                g.strokePath(path, juce::PathStrokeType(2.0f));
            }

            r.removeFromRight(3);
            g.drawFittedText(text, r, juce::Justification::centredLeft, 1);

            if (shortcutKeyText.isNotEmpty())
            {
                auto f2 = font;
                f2.setHeight(f2.getHeight() * 0.75f);
                f2.setHorizontalScale(0.95f);
                g.setFont(f2);

                g.drawText(shortcutKeyText, r, juce::Justification::centredRight, true);
            }
        }
    }
};


class QuadRoughAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    QuadRoughAudioProcessorEditor (QuadRoughAudioProcessor&);
    ~QuadRoughAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    //Variables for the plot
    float plot_w, plot_h, plot_x, plot_y;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    QuadRoughAudioProcessor& audioProcessor;

    //Sliders
    juce::Slider inputknob;
    juce::Slider outputknob;
    juce::Slider driveknob;
    juce::Slider drywetknob;
    juce::Slider toneknob;

    //Buttons
    juce::TextButton midsidebtn;
    juce::TextButton clipperbtn;

    //ComboDistortion
    juce::ComboBox distBox;

    //Labels
    juce::Label inputlabel;
    juce::Label outputlabel;
    juce::Label drywetlabel;
    juce::Label drivelabel;
    juce::Label midsidelabel;
    juce::Label ceilinglabel;
    juce::Label tonelabel;

    //Attachments
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> inputknobAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> outputknobAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> driveknobAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> drywetknobAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> toneknobAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> midsideAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> clipperAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> distboxAttachment;

    CustomLookAndFeel customLookAndFeel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (QuadRoughAudioProcessorEditor)
};
