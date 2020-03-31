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
                  .withInput  ("Input",  AudioChannelSet::stereo(), true)
#endif
                  .withOutput ("Output", AudioChannelSet::stereo(), true)
#endif
                  )
#endif
{
    
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
    if(myFile.exists())
    {
        std::ifstream f(myFile.getFullPathName().toStdString().c_str());
        std::string fullLine;
        while(getline(f,fullLine))
        {
            if(fullLine.find('>')<fullLine.length())
            {
                parseChannelNum(fullLine.substr(0,fullLine.find(">")));
                
                unsigned long posRet = fullLine.find(">");
                
                std::string commands = fullLine.substr(posRet+1);
                
                std::regex re(":");
                std::sregex_token_iterator first{commands.begin(), commands.end(), re, -1}, last;
                std::vector<std::string> tokens{first, last};
                for (auto t : tokens) {
                    processCommands(t);
                }
            }
            else
            {
                processCommands(fullLine);
            }
        }
    }
    AudioPlayHead* playHead = getPlayHead();
    if(playHead)
    {
    AudioPlayHead::CurrentPositionInfo currentPositionInfo;
    playHead->getCurrentPosition(currentPositionInfo);
    for(int i=0;i<buffer.getNumSamples();i++)
    {
        float sampleRate = getSampleRate();
        float bpm = currentPositionInfo.bpm;
        double countInSeconds=runningTime/sampleRate;
        double countInMilliseconds = countInSeconds*1000.0f;
        double currentCount=countInMilliseconds/(15000.0f / bpm);
        for(int x=0;x<32;x++)
        {
            seqChannel[x].transport=currentPositionInfo.isPlaying;
            seqChannel[x].update(currentCount, midiMessages, i);
        }
        runningTime=currentPositionInfo.timeInSamples+i;
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
}

void LivecodelangAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new LivecodelangAudioProcessor();
}
