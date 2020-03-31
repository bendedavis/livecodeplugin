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
    addAndMakeVisible(openButton);
    openButton->setButtonText("open");
    openButton->addListener(this);
    addAndMakeVisible(resetButton);
    resetButton->setButtonText("reset");
    resetButton->addListener(this);
    setSize (200, 100);
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
    openButton->setBounds(0,0,200,20);
    resetButton->setBounds(0,50,200,20);
}

void LivecodelangAudioProcessorEditor::buttonClicked(Button *button)
{
    if(button==openButton)
    {
        FileChooser myChooser("choose file");
        
        if (myChooser.browseForFileToOpen())
        {
            processor.myFile = myChooser.getResult();
        }
    }
    if(button==resetButton)
    {
            processor.resetAll();
    }
}
