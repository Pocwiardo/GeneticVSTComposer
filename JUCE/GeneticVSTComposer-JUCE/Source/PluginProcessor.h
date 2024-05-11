/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "genetic.hpp"

//==============================================================================
/**
*/
class GeneticVSTComposerJUCEAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    GeneticVSTComposerJUCEAudioProcessor();
    ~GeneticVSTComposerJUCEAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //          CUSTOM
    //Method to generate melody and save it in the processor using Genetic Algorithms
    void GenerateMelody(std::string scale, std::pair<int, int> noteRange,
        std::pair<int, int> meter, double noteDuration, int populationSize, int numGenerations);
    int calculateSixteenthNoteSamples();
    std::vector<int> melody;
    std::string debugInfo;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GeneticVSTComposerJUCEAudioProcessor)
    juce::AudioPlayHead::CurrentPositionInfo lastPosInfo;
    std::vector<std::pair<int, int>> noteOffTimes; // To track note offs outside the current block
    std::array<int, 16> notes {60, 61, 62, -2, -2, -2, 65, -1, 66, 68, -1, 61, 62,63,70, 80};
    int currentNoteIndex = 0;
    int nextNoteTime = 0;
    int samplesBetweenNotes = 48000;
    bool isSequencePlaying = false;
    int activeNotesCount = 0; // Track how many keys are pressed
    int lastNote = -1;
    

};
