/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "fstream"
#include <string>
#include <regex>
#include <iostream>

//==============================================================================
/**
*/
class LivecodelangAudioProcessor  : public AudioProcessor
{
public:
    //==============================================================================
    LivecodelangAudioProcessor();
    ~LivecodelangAudioProcessor();

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (AudioBuffer<float>&, MidiBuffer&) override;

    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;

    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
     File myFile;
    inline void resetAll()
    {
        for(int i=0;i<32;i++)
        {
            seqChannel[i].reset();
        }
    }

private:
    
    inline void processCommands(std::string input)
    {
        std::string commandName;
        int length=0;
        if(input.at(0)==' ')
        {
            input=input.substr(1);
        }
        std::regex re(" ");
        std::sregex_token_iterator first{input.begin(), input.end(), re, -1}, last;
        std::vector<std::string> tokens{first, last};
        for (auto t : tokens) {
            if(length==0)
                commandName=t;
            length++;
        }
        std::string lowBuffer[length-1];
        std::string highBuffer[length-1];
        int iter=0;
        for (auto t : tokens)
        {
            if(iter>0)
            {
                if(t.find('~')<t.length())
                {
                    unsigned long randPos;
                    randPos = t.find('~');
                    lowBuffer[iter-1]=t.substr(0,randPos);
                    highBuffer[iter-1]=t.substr(randPos+1);
                }
                else
                {
                    lowBuffer[iter-1]= t;
                    highBuffer[iter-1]= t;
                }
            }
            iter++;
        }
        
        execCommand(commandName, length-1, lowBuffer, highBuffer);
    }
    
    inline void execCommand(std::string commandName, int length, std::string* lowBuff, std::string* highBuff)
    {
        if(commandName == "midi")
        {
            seqChannel[activeChannel].midiChan = std::stoi(lowBuff[0]);
        }
        else if(commandName == "prob")
        {
            for(int i=0;i<length;i++)
            {
                seqChannel[activeChannel].prob[i]=std::stof(lowBuff[i]);
            }
             seqChannel[activeChannel].seqLength=length;
        }
        else if (commandName == "velocity")
        {
            for(int i=0;i<seqChannel[activeChannel].seqLength;i++)
            {
                seqChannel[activeChannel].velocity.lowValue[i%length]=std::stoi(lowBuff[i%length]);
                seqChannel[activeChannel].velocity.highValue[i%length]=std::stoi(highBuff[i%length]);
            }
        }
        else if (commandName == "repeat")
        {
            for(int i=0;i<seqChannel[activeChannel].seqLength;i++)
            {
                seqChannel[activeChannel].repeat.lowValue[i%length]=std::stoi(lowBuff[i%length]);
                seqChannel[activeChannel].repeat.highValue[i%length]=std::stoi(highBuff[i%length]);
            }
        }
        else if(commandName == "note" || commandName == "notes")
        {
            for(int i=0;i<length;i++)
            {
                seqChannel[activeChannel].note.lowValue[i]=std::stoi(lowBuff[i]);
                seqChannel[activeChannel].note.highValue[i]=std::stoi(highBuff[i]);
            }
            seqChannel[activeChannel].noteLength=length;
        }
        else if(commandName == "scale")
        {
            uint16_t finalScale=0;
            int currentBit;
            for(int i=0;i<12;i++)
            {
            if(lowBuff[i]=="1")
            {
                currentBit=1;
            }
            else
            {
                currentBit=0;
            }
            finalScale+=currentBit<<i;
            }
            for(int i=0;i<32;i++)
            {
                seqChannel[i].scale=finalScale;
            }
        }
        else if(commandName == "multiply")
        {
            seqChannel[activeChannel].multiply=std::stof(lowBuff[0]);
        }
        else if(commandName == "divide")
        {
            seqChannel[activeChannel].divide=std::stof(lowBuff[0]);
        }
    }
    
    inline void parseChannelNum(std::string channelID)
    {
        char cType;
        int receivedType=0;
        cType = channelID.at(0);
        if(cType == 'd')
            receivedType=0;
        else
        {
            receivedType=1;
        }
        std::string activeChannelString;
        activeChannelString = channelID.substr(1);
        activeChannel = std::stoi(activeChannelString)-1;
        
        seqChannel[activeChannel].type=receivedType;
    }
    
    static inline float interp(float in1, float in2, float x)
    {
        return ((1.0f - x) * in1 + in2 * x);
    }
    
    static inline void sendNoteOn(int channel, int note, uint8_t velocity, MidiBuffer& midiBuff, int currentSample)
    {
        auto noteOnMess = MidiMessage::noteOn(channel, note, velocity);
        midiBuff.addEvent(noteOnMess,currentSample);
    }
    
    static inline void sendNoteOff(int channel, int note, uint8_t velocity, MidiBuffer& midiBuff, int currentSample)
    {
        auto noteOffMess = MidiMessage::noteOff(channel,note,velocity);
        midiBuff.addEvent(noteOffMess,currentSample);
    }
    
    class RandomValue
    {
    private:
        float finalValue[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
        unsigned long indexCount = 0;
        int numRandom=1;
        float evolve = 1.0f;
    public:
        uint8_t lowValue[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
        uint8_t highValue[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
        
        void updateValues(uint8_t index)
        {
            if (indexCount >= numRandom)
            {
                if (lowValue[index] == highValue[index])
                {
                    finalValue[index] = highValue[index];
                }
                else
                {
                    float randomIndex;
                    randomIndex = (rand() % 4096) / 4095.0f;
                    finalValue[index] += (interp(lowValue[index], highValue[index], randomIndex)   - finalValue[index]) * evolve;
                    if (finalValue[index] < lowValue[index])
                        finalValue[index] = lowValue[index];
                    else if (finalValue[index] > highValue[index])
                        finalValue[index] = highValue[index];
                }
            }
            if (index == 0)
            {
                if (indexCount >= numRandom)
                    indexCount = 0;
                indexCount++;
            }
        }
        
        uint8_t getValue(int index)
        {
            if ((finalValue[index] - (int)finalValue[index]) < 0.5)
            {
                return (floor(finalValue[index]));
            }
            else
                return (ceil(finalValue[index]));
        }
    };
    
    class SeqChannel
    {
    private:
        uint8_t noteStatus = 0;
        uint8_t lastStatus = 1;
        uint8_t currentStep = 0;
        uint8_t lastStep = 0;
        RandomValue compareVal;
        uint8_t thisNote = 0;
        float phase = 0;
        float currentCount = 0;
        int lastVelocity = 0;
        float lastCount = 0;
        int lastNote = 0;
        int currentNote = 0;
        uint8_t lastMidiChan = 18;
        float finalRepeats = 1;
        int noteStep = 0;
        int lastNoteStep = 0;
        
        int quantize(int inNote, uint16_t scaleIn)
        {
            int diffs[12] = {13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13};
            int smallestDiff = 50;
            
            for (int i = 0; i < 12; i++)
            {
                if (((scaleIn >> i) & 1) == 1)
                {
                    diffs[i] = (inNote % 12) - i;
                }
            }
            
            int whichOne = 20;
            
            for (int i = 0; i < 12; i++)
            {
                if (abs(diffs[i]) < smallestDiff)
                    whichOne = i;
                smallestDiff = std::min(abs(diffs[i]), smallestDiff);
            }
            
            if (scaleIn == 0)
                return (inNote);
            else
                return (inNote - diffs[whichOne]);
        }
    public:
        SeqChannel()
        {
            reset();
        }
        void reset()
        {
            for (int i = 0; i < 16; i++)
            {
                velocity.lowValue[i] = 127;
                velocity.highValue[i] = 127;
                
                repeat.lowValue[i] = 1;
                repeat.highValue[i] = 1;
            }
            for (int i = 0; i < 16; i++)
            {
                prob[i] = 0;
                pw[i] = 0.5f;
            }
            shift = 0;
            divide = 1;
            multiply = 1;
            follow = 0;
        }
        int transport=0;
        uint8_t type = 0;
        uint8_t midiChan = 1;
        uint16_t scale = 0;
        uint8_t follow = 0;
        uint8_t chanFollow = 0;
        float followPhase = 0;
        unsigned long hitCount = 0;
        float outPhase = 0;
        uint8_t active = 1;
        float shift = 0.0;
        float prob[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
        uint8_t seqLength = 16;
        uint8_t noteLength = 16;
        uint8_t vLength;
        RandomValue note;
        float pw[16] = {0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5};
        float multiply = 1.0f;
        float divide = 1.0f;
        RandomValue repeat;
        RandomValue velocity;
        void update(float rt, MidiBuffer& midiBuff, int currentSample)
        {
            float postMultiply;
            if (follow == 0)
            {
                rt += shift;
                lastCount = currentCount;
                currentCount = (rt * multiply / divide);
                lastStep = currentStep;
                currentStep = (unsigned long)currentCount % seqLength;
                if (repeat.lowValue[currentStep] < 1)
                    repeat.lowValue[currentStep] = 1;
                if ((unsigned long)currentCount != (unsigned long)lastCount)
                {
                    if (currentStep == 0)
                        hitCount = 0;
                    noteStep = hitCount % noteLength;
                    note.updateValues(noteStep);
                    compareVal.lowValue[currentStep] = 0;
                    compareVal.highValue[currentStep] = 255;
                    compareVal.updateValues(currentStep);
                    repeat.updateValues(currentStep);
                    velocity.updateValues(currentStep);
                    finalRepeats = repeat.getValue(currentStep);
                }
                postMultiply = float(multiply / divide) * finalRepeats;
            }
            else
            {
                lastCount = currentCount;
                currentCount = (int)followPhase;
                currentStep = (int)followPhase % seqLength;
                postMultiply = repeat.getValue(currentStep);
            }
            if (follow == 0)
            {
                phase = (rt * postMultiply) - (int)(rt * postMultiply);
                if (phase > 1.0)
                    phase -= 1.0f;
            }
            else
            {
                phase = (followPhase * postMultiply) - (int)(followPhase * postMultiply);
            }
            lastStatus = noteStatus;
            if (phase < pw[currentStep])
            {
                noteStatus = 1 * active;
            }
            else
                noteStatus = 0;
            if ((prob[currentStep] * 256.0f) > compareVal.getValue(currentStep))
            {
                outPhase = hitCount;
                if ((noteStatus > lastStatus)&&transport)
                {
                    thisNote = note.getValue(noteStep);
                    hitCount++;
                    if (type == 1)
                    {
                        thisNote = quantize(thisNote, scale);
                    }
                    sendNoteOn(midiChan, thisNote, velocity.getValue(currentStep), midiBuff, currentSample);
                    lastMidiChan = midiChan;
                    lastNote = thisNote;
                    lastVelocity = velocity.getValue(currentStep);
                }
                else if (noteStatus < lastStatus)
                {
                    sendNoteOn(lastMidiChan, lastNote, 0, midiBuff, currentSample);
                }
            }
        }
    };
    
    SeqChannel seqChannel[32];
    int activeChannel = 0;
    int64_t runningTime;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LivecodelangAudioProcessor)
};
