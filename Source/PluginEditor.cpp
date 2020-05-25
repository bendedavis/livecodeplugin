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
    bpmInput->setText("120");
    addAndMakeVisible(evalButton);
    addAndMakeVisible(fileButton);
    fileButton->setButtonText("Load Script");
    fileButton->addListener(this);
    evalButton->setButtonText("Evaluate");
    evalButton->addListener(this);
    playButton->addListener(this);
    addAndMakeVisible(textEd);
    textEd->setColour(0x1000200, defaultBGColor);
    textEd->setWantsKeyboardFocus(true);
    textEd->setReturnKeyStartsNewLine(true);
    textEd->setMultiLine(true);
    errorBox->setReadOnly(true);
    errorBox->setMultiLine(true);
    errorBox->setScrollbarsShown(true);
    errorBox->setScrollBarThickness(10);
    addAndMakeVisible(errorBox);
    std::string buildDateString = "Build Date: ";
    buildDateString.append(__DATE__);
    buildDate->setText((buildDateString), NotificationType::dontSendNotification);
    buildDate->setJustificationType(Justification::right);
    addAndMakeVisible(buildDate);
    playButton->setButtonText("Play");
    UIHeight=600;
    UIWidth=800;
    setSize (UIWidth, UIHeight);
}

LivecodelangAudioProcessorEditor::~LivecodelangAudioProcessorEditor()
{
}

//==============================================================================
void LivecodelangAudioProcessorEditor::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    
    lastTransportState=currentTransportState;
    currentTransportState=processor.clockInternal;
    
    if(currentTransportState<lastTransportState)
    {
        showTransport();
    }
    
    if(processor.UIChanged==1)
    {
        textEd->setText(processor.codeString);
        processor.UIChanged=0;
    }
    
    lastUIState=currentUIState;
    currentUIState=processor.queueNewClip;
    
    if(currentUIState>lastUIState)
    {
        textEd->setColour(0x1000200, Colour::fromRGB(66, 61, 102));
        textEd->repaint();
    }
    
    if(lastUIState>currentUIState)
    {
        textEd->setColour(0x1000200, defaultBGColor);
        textEd->repaint();
    }
    
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));

    g.setColour (Colours::white);
    g.setFont (15.0f);
}

void LivecodelangAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    
        setSize (UIWidth, UIHeight);
    
#define buttonHeight 30
#define errorHeight 60
#define verHeight 20
    bpmInput->setBounds(120, UIHeight-verHeight, 100, verHeight);
    playButton->setBounds(0,UIHeight-verHeight,100,verHeight);
    evalButton->setBounds(0, UIHeight-buttonHeight-verHeight, UIWidth, buttonHeight);
    fileButton->setBounds(0, UIHeight-(buttonHeight*2)-verHeight, UIWidth, buttonHeight);
    textEd->setBounds(0, 0, UIWidth, UIHeight-(buttonHeight*2)-errorHeight-verHeight);
    errorBox->setBounds(0,UIHeight-(buttonHeight*2)-errorHeight-verHeight,UIWidth,errorHeight);
    buildDate->setBounds(0,UIHeight-verHeight,UIWidth,verHeight);
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
        evalCode();
    }
    if(button==playButton)
    {
        if(processor.transport==0)
        {
            processor.bpm=std::stof(bpmInput->getText().toStdString());
            processor.runningTime=0;
            processor.transport=1;
            playButton->setButtonText("Stop");
        }
        else
        {
            processor.transport=0;
            playButton->setButtonText("Play");
        }
    }
}

void LivecodelangAudioProcessorEditor::showTransport()
{
    addAndMakeVisible(playButton);
    addAndMakeVisible(bpmInput);
}
