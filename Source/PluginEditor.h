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
class LivecodelangAudioProcessorEditor  : public AudioProcessorEditor, public Button::Listener
{
public:
    LivecodelangAudioProcessorEditor (LivecodelangAudioProcessor&);
    ~LivecodelangAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;
    void buttonClicked(Button* button) override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    LivecodelangAudioProcessor& processor;
    int shiftHeld=0;
    TextButton* evalButton = new TextButton;
    TextButton* resetButton = new TextButton;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LivecodelangAudioProcessorEditor)
};
