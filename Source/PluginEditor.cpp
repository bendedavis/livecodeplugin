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
    addAndMakeVisible(textEd);
    textEd->setWantsKeyboardFocus(true);
    textEd->setReturnKeyStartsNewLine(true);
    textEd->addListener(this);
    textEd->setMultiLine(true);
    setSize (800, 600);
}

LivecodelangAudioProcessorEditor::~LivecodelangAudioProcessorEditor()
{
}

//==============================================================================
void LivecodelangAudioProcessorEditor::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    if(processor.UIChanged==1)
    {
        textEd->setText(processor.codeString);
        processor.UIChanged=0;
    }
    
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));

    g.setColour (Colours::white);
    g.setFont (15.0f);
}

void LivecodelangAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
#define buttonHeight 30
   
    
    evalButton->setBounds(0, newHeight-buttonHeight, newWidth, buttonHeight);
    resetButton->setBounds(0, newHeight-(buttonHeight*2), newWidth, buttonHeight);
    textEd->setBounds(0, 0, newWidth, newHeight-(buttonHeight*2));

}

void LivecodelangAudioProcessorEditor::buttonClicked(Button *button)
{
    if(button==resetButton)
    {
        processor.resetAll();
    }
    if(button==evalButton)
    {
        processor.codeString=textEd->getText().toStdString();
        processor.makeSeq();
    }
}
