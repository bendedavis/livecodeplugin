/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
LivecodelangAudioProcessorEditor::LivecodelangAudioProcessorEditor (LivecodelangAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    addAndMakeVisible(evalButton);
    addAndMakeVisible(resetButton);
    resetButton->setButtonText("reset");
    resetButton->addListener(this);
    evalButton->setButtonText("evaluate");
    evalButton->addListener(this);
    addAndMakeVisible(processor.textEd);
    processor.textEd->setWantsKeyboardFocus(true);
    processor.textEd->setMultiLine(true);
    processor.textEd->setReturnKeyStartsNewLine(true);
    setResizable(true, true);
    setSize (processor.UIWidth, processor.UIHeight);
}

LivecodelangAudioProcessorEditor::~LivecodelangAudioProcessorEditor()
{
}

//==============================================================================
void LivecodelangAudioProcessorEditor::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));

    g.setColour (Colours::white);
    g.setFont (15.0f);
}

void LivecodelangAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    processor.UIHeight=getHeight();
    processor.UIWidth=getWidth();
#define buttonHeight 30
    int newHeight = getHeight();
    int newWidth = getWidth();
    
    
    evalButton->setBounds(0, newHeight-buttonHeight, newWidth, buttonHeight);
    resetButton->setBounds(0, newHeight-(buttonHeight*2), newWidth, buttonHeight);
    processor.textEd->setBounds(0, 0, newWidth, newHeight-(buttonHeight*2));

}

void LivecodelangAudioProcessorEditor::buttonClicked(Button *button)
{
    if(button==resetButton)
    {
            processor.resetAll();
    }
    if(button==evalButton)
    {
        processor.codeString=processor.textEd->getText().toStdString();
    }
}
