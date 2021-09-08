/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
*/
class Egaliseur_Simple_v1AudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    Egaliseur_Simple_v1AudioProcessor();
    ~Egaliseur_Simple_v1AudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

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

    /// <summary>
    /// Cree le layout des parametres requis pour l'audio_processor_value_tree_state
    /// </summary>
    /// <returns></returns>
    static juce::AudioProcessorValueTreeState::ParameterLayout CreateParameterLayout();

    /// <summary>
    /// Coordonne la synchronisation entre les parametres qui controlent le DSP (Processeur de signal numérique)
    /// et les potentiometres de la GUI avec les variables du DSP
    /// Ce composant est public afin qu'on puisse y attacher les boutons et curseurs de la GUI
    /// 
    /// Params :
    /// ---------
    /// AudioProcessor & processorToConnectTo
    /// UndoManager* undoManagerToUse --> non utilise
    /// const Identifier& valueTreeType
    /// ParameterLayout parameterLayout
    /// </summary>
    juce::AudioProcessorValueTreeState audio_processor_value_tree_state { *this, nullptr, "Parameters", CreateParameterLayout() };

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Egaliseur_Simple_v1AudioProcessor)
};
