/*
 ==============================================================================
 
 This file was auto-generated!
 
 It contains the basic framework code for a JUCE plugin processor.
 
 ==============================================================================
 */

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
LivecodelangAudioProcessor::LivecodelangAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
: AudioProcessor (BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
                  .withInput  ("Input",  AudioChannelSet::stereo(), false)
#endif
                  .withOutput ("Output", AudioChannelSet::stereo(), true)
#endif
                  )
#endif
{
    UIWidth=800;
    UIHeight=600;
}

LivecodelangAudioProcessor::~LivecodelangAudioProcessor()
{
}

//==============================================================================
const String LivecodelangAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool LivecodelangAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool LivecodelangAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool LivecodelangAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double LivecodelangAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int LivecodelangAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
    // so this should be at least 1, even if you're not really implementing programs.
}

int LivecodelangAudioProcessor::getCurrentProgram()
{
    return 0;
}

void LivecodelangAudioProcessor::setCurrentProgram (int index)
{
}

const String LivecodelangAudioProcessor::getProgramName (int index)
{
    return {};
}

void LivecodelangAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void LivecodelangAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void LivecodelangAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool LivecodelangAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
#else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;
    
    // This checks if the input layout matches the output layout
#if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
#endif
    
    return true;
#endif
}
#endif

void LivecodelangAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    buffer.clear();
    double sampleRate = getSampleRate();
    int64_t startSample = 0;
    
    AudioPlayHead* playHead = getPlayHead();
    if(playHead)
    {
        AudioPlayHead::CurrentPositionInfo currentPositionInfo;
        playHead->getCurrentPosition(currentPositionInfo);
        bpm = currentPositionInfo.bpm;
        startSample = currentPositionInfo.timeInSamples;
        transport = currentPositionInfo.isPlaying;
    }
    for(int i=0;i<buffer.getNumSamples();i++)
    {
        lastRunningTime=runningTime;
        if(playHead)
        {
            runningTime=startSample+i;
        }
        else
        {
            clockInternal=0;
            runningTime+=1;
        }
        
        lastCount = currentCount;
        currentCount = samplesToCount(runningTime, sampleRate, bpm);
        lastLoopedCount=loopedCount;
        loopedCount=wrap(currentCount,masterLength);
        if(loopedCount<lastLoopedCount)
        {
            if(queueNewClip)
            {
                clipSelect=!clipSelect;
                queueNewClip=0;
            }
            eventsPlayed=0;
        }
        numEvents = myClip[clipSelect].getNumEvents();
        if(transport&&(numEvents>0))
        {
            if(eventsPlayed<numEvents)
            {
                auto eventTime = myClip[clipSelect].getEventPointer(eventsPlayed)->message.getTimeStamp();
                if(loopedCount>=eventTime)
                {
                    for (int numSame = 0 ; numSame < numEvents; numSame++)
                    {
                        auto scannedTime = myClip[clipSelect].getEventPointer(numSame)->message.getTimeStamp();
                        if(scannedTime==eventTime)
                        {
                            midiMessages.addEvent(myClip[clipSelect].getEventPointer(eventsPlayed)->message, i);
                            eventsPlayed++;
                        }
                    }
                }
            }
        }
    }
}

//==============================================================================
bool LivecodelangAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* LivecodelangAudioProcessor::createEditor()
{
    return new LivecodelangAudioProcessorEditor (*this);
}

//==============================================================================
void LivecodelangAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    ValueTree preset("Preset");
    preset.setProperty("CodeString", codeString, nullptr);
    std::unique_ptr<XmlElement> presetData = preset.createXml();
    copyXmlToBinary(*presetData, destData);
}

void LivecodelangAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    
    auto xml = getXmlFromBinary(data, sizeInBytes);
    if(xml!=nullptr)
    {
        auto tree = ValueTree::fromXml(*xml);
        codeString=tree.getProperty("CodeString");
        UIChanged=1;
    }
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new LivecodelangAudioProcessor();
}
