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
    
    Colour defaultBGColor = Colour::fromRGB(35, 31, 66);
    
    void evalCode()
    {
        processor.codeString=textEd->getText().toStdString();
        processor.makeSeq();
        errorBox->setText(processor.errorString);
        processor.queueNewClip=1;
    }
    
    bool keyPressed(const KeyPress &key) override
    {
        if(key==KeyPress (KeyPress::returnKey, ModifierKeys::shiftModifier, 0)	)
        {
            evalCode();
        }
        return false;
    }
    
    int UIHeight=600;
    int UIWidth=800;
    
private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    
    int lastUIState=1;
    int currentUIState=1;
    
    int currentTransportState=1;
    int lastTransportState=1;
    
    LivecodelangAudioProcessor& processor;
        
    TextButton* evalButton = new TextButton;
    TextButton* fileButton = new TextButton;
    TextEditor* textEd = new TextEditor;
    TextEditor* errorBox = new TextEditor;
    Label* buildDate = new Label;
    
    TextButton* playButton = new TextButton;
    
    TextEditor* bpmInput = new TextEditor;
    
    void showTransport();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LivecodelangAudioProcessorEditor)
};
