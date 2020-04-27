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
class LivecodelangAudioProcessorEditor  : public AudioProcessorEditor, public Button::Listener, public TextEditor::Listener
{
public:
    LivecodelangAudioProcessorEditor (LivecodelangAudioProcessor&);
    ~LivecodelangAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;
    void buttonClicked(Button* button) override;
    
    int newHeight=600;
    int newWidth=800;
    
private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    LivecodelangAudioProcessor& processor;
    TextButton* evalButton = new TextButton;
    TextButton* fileButton = new TextButton;
    TextEditor* textEd = new TextEditor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LivecodelangAudioProcessorEditor)
};
