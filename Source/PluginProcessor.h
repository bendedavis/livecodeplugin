/*
 ==============================================================================
 
 This file was auto-generated!
 
 It contains the basic framework code for a JUCE plugin processor.
 
 ==============================================================================
 */

#pragma once

#include <JuceHeader.h>
#include <sstream>
#include <string>
#include <regex>
#include <iostream>
#include "lua-src/lua.hpp"
#include "luaCommands.h"

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
    
    int UIChanged=0;
    String codeString="";
    File extLuaScript;
    
    int UIHeight;
    int UIWidth;
    
    int transport=0;
    int64_t runningTime=0;
    double bpm = 120;
        
    std::string errorString = "";

    int clockInternal = 1;
    
    int queueNewClip=0;
        
    void makeSeq()
    {
        errorString="";
        myClip[!clipSelect].clear();
        lua_State *L = luaL_newstate();
        luaL_openlibs(L);
        std::string luaTxtString = "";
        int commandFound=0;
        luaL_dostring(L, luaCommandString.c_str());
        if(extLuaScript.exists())
        {
            String luaFileName = extLuaScript.getFullPathName();
            luaL_dofile(L, luaFileName.toStdString().c_str());
        }
        std::string masterLengthSend = "masterLength = " + std::to_string(masterLength);
        luaL_dostring(L, masterLengthSend.c_str());
        std::stringstream stream(codeString.toStdString());
        std::string fullLine;
        std::string channelCommand[32];
        int exists[32];
        int channelToWrite;
        std::string channelType[32];
        for(int i=0;i<32;i++)
        {
            paramStor[i].clear();
            channelCommand[i]="";
            exists[i]=0;
        }
        while(getline(stream,fullLine))
        {
            if(fullLine.find('$')<fullLine.length())
            {
                if(commandFound==0)
                {
                    luaL_dostring(L, luaTxtString.c_str());
                    commandFound=1;
                }
                channelToWrite = std::stoi(fullLine.substr(1,fullLine.find('$')))-1;
                exists[channelToWrite]=1;
                channelType[channelToWrite]=fullLine.at(0);
                channelCommand[channelToWrite]=fullLine.substr(fullLine.find('$')+1);
            }
            else
            {
                if(commandFound==0)
                {
                luaTxtString+=fullLine + "\n";
                }
                else
                {
                    channelCommand[channelToWrite]+=fullLine;
                }
            }
        }
        for(int i=0;i<32;i++)
        {
            if(exists[i])
            {
                std::regex re(":");
                std::sregex_token_iterator first{channelCommand[i].begin(), channelCommand[i].end(), re, -1}, last;
                std::vector<std::string> tokens{first, last};
                for (auto t : tokens) {
                    processCommands(t,i);
                }
                
                int gateStep = 0;
                int lastGateStep = 0;
                int noteStep = 0;
                if(paramStor[i].gateLength>0&&paramStor[i].noteLength>0)
                {
                    auto channelDivide = luaEval(paramStor[i].divide,L);
                    auto channelMultiply = luaEval(paramStor[i].multiply,L);
                    auto channelTiming = channelMultiply/channelDivide;
                    masterLength= luaEval("masterLength",L);
                    auto totalLength = (masterLength*16)*channelTiming;
                    for(int step=0;step<totalLength;step++)
                    {
                        auto currentTiming = step*1.0f/totalLength;
                        std::string timingString = "currentTime= " + std::to_string(currentTiming);
                        luaL_dostring(L, timingString.c_str());
                        lastGateStep=gateStep;
                        gateStep=step%paramStor[i].gateLength;
                        if(gateStep<lastGateStep)
                        {
                            noteStep=0;
                        }
                        if(luaEval(paramStor[i].prob[gateStep], L)>=randomRange(0, 1))
                        {
                            auto currentNoteLength = paramStor[i].noteLength;
                            
                            float finalVel=1.0;
                            float finalShift=0;
                            if(paramStor[i].velocity!=nullptr)
                            {
                                finalVel = luaEval(paramStor[i].velocity[gateStep],L);;
                            }
                            finalShift=luaEval(paramStor[i].shift,L);
                            auto firstNote = ((float)step+finalShift)/((16.0f)*channelTiming);
                            auto nextNote = (((float)step+finalShift)+1)/((16.0f)*channelTiming);
                            int finalRepeat = 1;
                            if(paramStor[i].repeat!=nullptr)
                            {
                                finalRepeat = luaEval(paramStor[i].repeat[gateStep],L);
                            }
                            auto eachTiming = (nextNote-firstNote)/(float)finalRepeat;
                            if(finalRepeat>=1)
                            {
                                for(int rep=0;rep<finalRepeat;rep++)
                                {
                                    int polyCount=0;
                                    auto currentStepString = paramStor[i].notes[noteStep%currentNoteLength];
                                    std::string noteOrder[128];
                                    for(int m=0;m<128;m++)
                                        noteOrder[m]="";
                                    std::regex re(",");
                                    std::sregex_token_iterator first{currentStepString.begin(), currentStepString.end(), re, -1}, last;
                                    std::vector<std::string> tokens{first, last};
                                    int paranStarted=0;
                                    for (auto t : tokens)
                                    {
                                        if(paranStarted==0)
                                        {
                                            polyCount++;
                                        }
                                        if(t.find("(") < t.length())
                                            paranStarted++;
                                        if(t.find(")")<t.length())
                                        {
                                            paranStarted--;
                                        }
                                        if(paranStarted>0)
                                        {
                                            noteOrder[polyCount-1].append(t + ",");
                                        }
                                        else
                                        {
                                        noteOrder[polyCount-1].append(t);
                                        }
                                    }
                                    
                                    if (polyCount==1)
                                    {
                                        auto finalNote=luaEval(paramStor[i].notes[noteStep%currentNoteLength],L);
                                        
                                        if(channelType[i]=="n"&&paramStor[i].scale!="")
                                        {
                                            
                                            std::string quantizeMessage = "quantize(" + std::to_string(finalNote) + "," + paramStor[i].scale +")";
                                            finalNote=luaEval(quantizeMessage.c_str(),L);
                                            
                                        }
                                        makeNote(finalNote, finalVel , firstNote + eachTiming*rep, (1/16.0f)/channelTiming, myClip[!clipSelect], masterLength);
                                    }
                                    else
                                    {
                                        for(int p=0;p<polyCount;p++)
                                        {
                                            auto finalNote = luaEval(noteOrder[p],L);
                                            
                                            if(channelType[i]=="n"&&paramStor[i].scale!="")
                                            {
                                                
                                                std::string quantizeMessage = "quantize(" + std::to_string(finalNote) + "," + paramStor[i].scale +")";
                                                finalNote=luaEval(quantizeMessage.c_str(),L);
                                                
                                            }
                                            makeNote(finalNote, finalVel , firstNote + eachTiming*rep, (1/16.0f)/channelTiming, myClip[!clipSelect], masterLength);
                                        }
                                    }
                                }
                            }
                            noteStep++;
                        }
                    }
                }
            }
        }
        myClip[!clipSelect].sort();
        lua_close(L);
    }
    
private:
    
    inline void processCommands(std::string input, int channelNum)
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
        std::string *lowBuffer = new std::string[length-1];
        int iter=0;
        for (auto t : tokens)
        {
            if(iter>0)
            {
                    lowBuffer[iter-1]= t;
            }
            iter++;
        }
        execCommand(commandName, length-1, lowBuffer, channelNum);
        delete []lowBuffer;
    }
    
    inline void makeNote(float note, float velocity, float start, float duration, MidiMessageSequence &mySeq, int maxLength)
    {
        if(note>=0&&note<128)
        {
            if((note-(unsigned long)note)>0.5f)
               note = std::ceil(note);
            else
               note = std::floor(note);
            auto onEvent = MidiMessage::noteOn(1, note, velocity);
            auto offEvent = MidiMessage::noteOff(1, note, velocity);
            mySeq.addEvent(onEvent,start);
            auto endPosition = start+duration;
            if(endPosition<maxLength)
            {
                mySeq.addEvent(offEvent,endPosition);
            }
            else
            {
                mySeq.addEvent(offEvent,maxLength-0.01f);
            }
        }
    }
    
    inline float luaEval(std::string inputMessage, lua_State * currentState)
    {
        std::string luaMessage = "result_x11245=" + inputMessage;
        if(luaL_dostring(currentState, luaMessage.c_str()) == LUA_OK)
        {
        lua_getglobal(currentState, "result_x11245");
        return(lua_tonumber(currentState, -1));
        }
        else
        {
            std::string errorMessageString = lua_tostring(currentState, -1);
            std::regex re(":");
            std::sregex_token_iterator first{errorMessageString.begin(), errorMessageString.end(), re, -1}, last;
            std::vector<std::string> tokens{first, last};
            int lineIter=0;
            std::string cleanMessage="";
            for (auto t : tokens)
            {
                if(lineIter==2)
                    cleanMessage=t;
                lineIter++;
            }
            cleanMessage.append("\n");
            errorString.append(cleanMessage);
        }
    }
    
    inline float randomRange(float in1, float in2)
    {
        float randomFloat = (rand()%10000)/9999.0f;
        float randomReturn = (in1*randomFloat)+(in2*(1.0f-randomFloat));
        return(randomReturn);
    }
    
    inline void execCommand(std::string commandName, int length, std::string* lowBuff, int channelNum)
    {
        if(commandName == "prob")
        {
            paramStor[channelNum].prob = new std::string[length];
            paramStor[channelNum].gateLength=length;
            for(int i=0;i<length;i++)
            {
                paramStor[channelNum].prob[i]=lowBuff[i];
            }
        }
        else if (commandName == "velocity")
        {
            int sequenceLength = masterLength*16;
            paramStor[channelNum].velocity = new std::string[sequenceLength];
            for(int i=0;i<sequenceLength;i++)
            {
                paramStor[channelNum].velocity[i]=lowBuff[i%length];
            }
        }
        else if (commandName == "repeat" || commandName == "repeats")
        {
            int sequenceLength = masterLength*16;
            paramStor[channelNum].repeat = new std::string[sequenceLength];
            for(int i=0;i<sequenceLength;i++)
            {
                paramStor[channelNum].repeat[i]=lowBuff[i%length];
            }
        }
        else if(commandName == "note" || commandName == "notes")
        {
            paramStor[channelNum].notes = new std::string[length];
            paramStor[channelNum].noteLength=length;
            for(int i=0;i<length;i++)
            {
                paramStor[channelNum].notes[i]=lowBuff[i];
            }
        }
        else if(commandName == "multiply")
        {
            paramStor[channelNum].multiply=lowBuff[0];
        }
        else if(commandName == "divide")
        {
            paramStor[channelNum].divide=lowBuff[0];
        }
        else if(commandName == "shift")
        {
            paramStor[channelNum].shift=lowBuff[0];
        }
        else if(commandName == "quantize")
        {
            paramStor[channelNum].scale=lowBuff[0];
        }
    }
    
    double inline samplesToCount (int64_t sampleCount, double sr, double bpm)
    {
        double countInSeconds=(double)sampleCount/sr;
        double countInMilliseconds = countInSeconds*1000.0f;
        double beatCount=countInMilliseconds/(240000.0f / bpm);
        
        return(beatCount);
    }
    
    double wrap(double in, int mod)
    {
        double output=0;
        int64_t wholeNum = in;
        wholeNum%=mod;
        double frac=in-(int64_t)in;
        output=wholeNum+frac;
        return (output);
    }
    
    class ParamStor
    {
    public:
        std::string *prob;
        int gateLength;
        std::string *repeat;
        std::string *notes;
        int noteLength;
        std::string *velocity;
        std::string multiply;
        std::string divide;
        std::string shift;
        std::string scale;
        void clear()
        {
            prob = nullptr;
            gateLength=0;
            repeat=nullptr;
            notes=nullptr;
            noteLength=0;
            velocity=nullptr;
            multiply="1";
            divide="1";
            shift="0";
            scale="";
        }
    };
    
    int queueReset=0;
    ParamStor paramStor[32];
    MidiMessageSequence myClip[2];
    uint8_t clipSelect=0;
    int64_t lastRunningTime;
    double lastCount;
    double currentCount;
    uint16_t numEvents;
    double loopedCount;
    double lastLoopedCount;
    uint16_t masterLength=1;
    uint16_t eventsPlayed;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LivecodelangAudioProcessor)
};
