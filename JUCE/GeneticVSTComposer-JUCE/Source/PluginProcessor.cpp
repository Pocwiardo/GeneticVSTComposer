/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
GeneticVSTComposerJUCEAudioProcessor::GeneticVSTComposerJUCEAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
}

GeneticVSTComposerJUCEAudioProcessor::~GeneticVSTComposerJUCEAudioProcessor()
{
}

//==============================================================================
const juce::String GeneticVSTComposerJUCEAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool GeneticVSTComposerJUCEAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool GeneticVSTComposerJUCEAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool GeneticVSTComposerJUCEAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double GeneticVSTComposerJUCEAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int GeneticVSTComposerJUCEAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int GeneticVSTComposerJUCEAudioProcessor::getCurrentProgram()
{
    return 0;
}

void GeneticVSTComposerJUCEAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String GeneticVSTComposerJUCEAudioProcessor::getProgramName (int index)
{
    return {};
}

void GeneticVSTComposerJUCEAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void GeneticVSTComposerJUCEAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void GeneticVSTComposerJUCEAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool GeneticVSTComposerJUCEAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
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

static juce::String getMidiMessageDescription(const juce::MidiMessage& m)
{
    if (m.isNoteOn())           return "Note on " + juce::MidiMessage::getMidiNoteName(m.getNoteNumber(), true, true, 3);
    if (m.isNoteOff())          return "Note off " + juce::MidiMessage::getMidiNoteName(m.getNoteNumber(), true, true, 3);
    if (m.isProgramChange())    return "Program change " + juce::String(m.getProgramChangeNumber());
    if (m.isPitchWheel())       return "Pitch wheel " + juce::String(m.getPitchWheelValue());
    if (m.isAftertouch())       return "After touch " + juce::MidiMessage::getMidiNoteName(m.getNoteNumber(), true, true, 3) + ": " + juce::String(m.getAfterTouchValue());
    if (m.isChannelPressure())  return "Channel pressure " + juce::String(m.getChannelPressureValue());
    if (m.isAllNotesOff())      return "All notes off";
    if (m.isAllSoundOff())      return "All sound off";
    if (m.isMetaEvent())        return "Meta event";

    if (m.isController())
    {
        juce::String name(juce::MidiMessage::getControllerName(m.getControllerNumber()));

        if (name.isEmpty())
            name = "[" + juce::String(m.getControllerNumber()) + "]";

        return "Controller " + name + ": " + juce::String(m.getControllerValue());
    }

    return juce::String::toHexString(m.getRawData(), m.getRawDataSize());
}

void GeneticVSTComposerJUCEAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    jassert(buffer.getNumChannels() == 0);  // It's a MIDI plugin, no audio data should be processed.

    const int numSamples = buffer.getNumSamples();
    juce::MidiBuffer processedMidi;

    // Obtain the playhead from the host to fetch current BPM
    juce::AudioPlayHead* playHead = getPlayHead();
    juce::AudioPlayHead::CurrentPositionInfo playHeadInfo;

    if (playHead && playHead->getCurrentPosition(playHeadInfo)) {
        // Calculate the duration of a sixteenth note based on the current BPM
        double beatsPerSecond = playHeadInfo.bpm / 60.0;
        double secondsPerBeat = 1.0 / beatsPerSecond;
        double secondsPerSixteenth = secondsPerBeat / 4.0;
        samplesBetweenNotes = static_cast<int>(secondsPerSixteenth * getSampleRate());
    }

    for (const auto metadata : midiMessages) {
        const auto message = metadata.getMessage();
        const auto time = metadata.samplePosition;

        if (message.isNoteOn() && message.getVelocity() > 0) {
            int noteNumber = message.getNoteNumber();
            if (noteNumber >= 48 && noteNumber < 60) {
                int melodyIndex = noteNumber - 48;
                if (melodyIndex < melodies.size()) {
                    melody = melodies[melodyIndex];
                    currentNoteIndex = 0;  // Restart the sequence
                    nextNoteTime = time;   // Start now
                    isSequencePlaying = true;
                }
            }
        }
        else if (message.isNoteOff()) {
            if (message.getNoteNumber() >= 48 && message.getNoteNumber() < 60) {
                int melodyIndex = message.getNoteNumber() - 48;
                if (melodyIndex < melodies.size() && melody == melodies[melodyIndex]) {
                    isSequencePlaying = false;
                    processedMidi.addEvent(juce::MidiMessage::allNotesOff(1), time);  // Stop all notes to avoid hanging notes
                }
            }
        }
    }

    if (isSequencePlaying && !melody.empty()) {
        while (nextNoteTime < numSamples && isSequencePlaying) {
            if (currentNoteIndex < melody.size()) {
                const int note = melody[currentNoteIndex];
                if (note >= 0) {
                    processedMidi.addEvent(juce::MidiMessage::noteOn(1, note, (juce::uint8)100), nextNoteTime);
                    processedMidi.addEvent(juce::MidiMessage::noteOff(1, note), nextNoteTime + samplesBetweenNotes - 1);
                }
                else if (note == -1) {
                    // Pause, do nothing
                }
                else if (note == -2 && lastNote != -1) {
                    // Extend the last note, adjust the note off time
                    processedMidi.addEvent(juce::MidiMessage::noteOff(1, lastNote), nextNoteTime + samplesBetweenNotes - 1);
                }

                nextNoteTime += samplesBetweenNotes;
                currentNoteIndex = (currentNoteIndex + 1) % melody.size(); // Safe way to loop index
            }
            else {
                break;  // Break the loop if index is out of range
            }
        }
    }

    // Adjust for the next block
    nextNoteTime -= numSamples;
    midiMessages.swapWith(processedMidi);
}


void GeneticVSTComposerJUCEAudioProcessor::GenerateMelody(std::string scale, std::pair<int, int> noteRange,
                                                          std::pair<int, int> meter, double noteDuration, int populationSize, int numGenerations)
{
    //run the genetic algorithm
    GeneticMelodyGenerator generator(scale, noteRange, meter, noteDuration, populationSize, numGenerations);

    //melody = generator.run(1);
    melodies = generator.run(1);

    //DEBUG - for now just print some results to string
    debugInfo = "Generated Melodies:\n";
    int melodyCount = 0;
    for (const auto& melody : melodies) {
        debugInfo += "Melody " + std::to_string(++melodyCount) + ": ";
        for (int note : melody) {
            debugInfo += std::to_string(note) + " ";
        }
        debugInfo += "\n";  // Append a newline after each melody for better readability
    }
}

//==============================================================================
bool GeneticVSTComposerJUCEAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* GeneticVSTComposerJUCEAudioProcessor::createEditor()
{
    return new GeneticVSTComposerJUCEAudioProcessorEditor (*this);
}

//==============================================================================
void GeneticVSTComposerJUCEAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void GeneticVSTComposerJUCEAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new GeneticVSTComposerJUCEAudioProcessor();
}
