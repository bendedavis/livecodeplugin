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
    addAndMakeVisible(fileButton);
    fileButton->setButtonText("Load Script");
    fileButton->addListener(this);
    evalButton->setButtonText("Evaluate");
    evalButton->addListener(this);
    addAndMakeVisible(textEd);
    textEd->setWantsKeyboardFocus(true);
    textEd->setReturnKeyStartsNewLine(true);
    textEd->addListener(this);
    textEd->setMultiLine(true);
    errorBox->setReadOnly(true);
    addAndMakeVisible(errorBox);
    std::string buildDateString = "Build Date: ";
    buildDateString.append(__DATE__);
    buildDate->setText((buildDateString), NotificationType::dontSendNotification);
    buildDate->setJustificationType(Justification::right);
    addAndMakeVisible(buildDate);
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
#define errorHeight 40
#define verHeight 20
    
    evalButton->setBounds(0, newHeight-buttonHeight-verHeight, newWidth, buttonHeight);
    fileButton->setBounds(0, newHeight-(buttonHeight*2)-verHeight, newWidth, buttonHeight);
    textEd->setBounds(0, 0, newWidth, newHeight-(buttonHeight*2)-errorHeight-verHeight);
    errorBox->setBounds(0,newHeight-(buttonHeight*2)-errorHeight-verHeight,newWidth,errorHeight);
    buildDate->setBounds(0,newHeight-verHeight,newWidth,verHeight);
    textEd->setText(processor.codeString);
}

void LivecodelangAudioProcessorEditor::buttonClicked(Button *button)
{
    if(button==fileButton)
    {
        FileChooser myChooser ("Please select a Lua script...");
        
        if (myChooser.browseForFileToOpen())
        {
            processor.extLuaScript = myChooser.getResult();
        }
    }
    if(button==evalButton)
    {
        processor.codeString=textEd->getText().toStdString();
        processor.makeSeq();
        errorBox->setText(processor.errorString);
        processor.queueNewClip=1;
    }
}
