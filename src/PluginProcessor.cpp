/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
Egaliseur_Simple_v1AudioProcessor::Egaliseur_Simple_v1AudioProcessor()
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

Egaliseur_Simple_v1AudioProcessor::~Egaliseur_Simple_v1AudioProcessor()
{
}

//==============================================================================
const juce::String Egaliseur_Simple_v1AudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool Egaliseur_Simple_v1AudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool Egaliseur_Simple_v1AudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool Egaliseur_Simple_v1AudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double Egaliseur_Simple_v1AudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int Egaliseur_Simple_v1AudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int Egaliseur_Simple_v1AudioProcessor::getCurrentProgram()
{
    return 0;
}

void Egaliseur_Simple_v1AudioProcessor::setCurrentProgram (int index)
{
}

const juce::String Egaliseur_Simple_v1AudioProcessor::getProgramName (int index)
{
    return {};
}

void Egaliseur_Simple_v1AudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void Egaliseur_Simple_v1AudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void Egaliseur_Simple_v1AudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool Egaliseur_Simple_v1AudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void Egaliseur_Simple_v1AudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);

        // ..do something to the data...
    }
}

//==============================================================================
bool Egaliseur_Simple_v1AudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* Egaliseur_Simple_v1AudioProcessor::createEditor()
{
//    return new Egaliseur_Simple_v1AudioProcessorEditor (*this);
    return new juce::GenericAudioProcessorEditor(*this);
}

//==============================================================================
void Egaliseur_Simple_v1AudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void Egaliseur_Simple_v1AudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

/// <summary>
/// Nous utiliserons trois bandes d'egalisation :
/// - Low cut band
/// - High cut band
/// - Peak parametric band
/// Pour les deux premiers, nous pourrons controler la frequence de coupure et la pente de coupure.
/// Pour la peak, nous pourrons controler la frequence centrale, le gain et la qualite (si la peak est large ou etroite)
/// </summary>
/// <returns></returns>
juce::AudioProcessorValueTreeState::ParameterLayout Egaliseur_Simple_v1AudioProcessor::CreateParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    // Low cut frequency parameter
    // Params : Param ID, Param Name, Normalisable Range, Default Value (= the low cut frequency parameter)
    // Normalisable Range parametres : human hearing range [20 Hz ; 20000 Hz] ;
    // 1. = on incremente de 1 en 1 la valeur du slider ;
    // skew factor = le pourcentage de couverture des frequences de la barre du slider (par ex 75% couvrira les frequences
    // de 20 à 1000 Hz, et les 25% restants couvriront de 1001 Hz a 20000 Hz). Si on le met a 1, on ne skew pas.
    layout.add(std::make_unique<juce::AudioParameterFloat>( "LowCut Freq",
                                                            "LowCut Freq",
                                                            juce::NormalisableRange<float>(20.f, 20000.f, 1.f, 1.f),
                                                            20.f));
    
    // High cut frequency parameter
    // Meme stuff que pour le low cut frequency parameter sauf que la valeur par defaut est la valeur haute des frequences
    layout.add(std::make_unique<juce::AudioParameterFloat>( "HighCut Freq",
                                                            "HighCut Freq",
                                                            juce::NormalisableRange<float>(20.f, 20000.f, 1.f, 1.f),
                                                            20000.f));

    // Peak cut frequency parameter
    // Meme stuff que pour les deux autres sauf que la valeur par defaut est la valeur sera de 750 Hz
   layout.add(std::make_unique<juce::AudioParameterFloat>(  "Peak Freq",
                                                            "Peak Freq",
                                                            juce::NormalisableRange<float>(20.f, 20000.f, 1.f, 1.f),
                                                            750.f));

    // Le gain sera exprime en decibels, un bon range est de -24 dB a +24 dB
    // 0.5 = lorqu'on bougera le slider, on modifiera le gain de 0.5 dB par 0.5 dB
    // Etant donne que l'on veut une progression lineaire, le skew facteur sera de 1
    // Par defaut, on ne veut pas ajouter de gain ou de coupure. La valeur par defaut sera donc de 0
    layout.add(std::make_unique<juce::AudioParameterFloat>( "Peak Gain",
                                                            "Peak Gain",
                                                            juce::NormalisableRange<float>(-24.f, 24.f, 0.5f, 1.f),
                                                            0.0f));

    // La largeur du peak
    layout.add(std::make_unique<juce::AudioParameterFloat>( "Peak Quality",
                                                            "Peak Quality",
                                                            juce::NormalisableRange<float>(0.1f, 10.f, 0.05f, 1.f),
                                                            1.f));
                                                            
    // On veut la possibilite de changer la raideur du filtre de coupure : dB par octaves
    // Maths derriere l'equation du filtre : par multiple de 6 ou 12 dB par octaves.
    // Nous utiliserons dans ce projet 4 choix : 12, 24, 36 et 48 dB / octaves
    // Les valeurs seront les memes pour le low cut et le high cut
    // La valeur par defaut de 0 correspond a un slope de 12 dB par octave
    juce::StringArray stringArray;

    for (int i{ 0 }; i < 4; ++i) {
        juce::String str;
        str << (12 + i * 12);
        str << " dB/Oct";
        stringArray.add(str);
    }
    
    layout.add(std::make_unique<juce::AudioParameterChoice>("LowCut Slope", "LowCut Slope", stringArray, 0));
    layout.add(std::make_unique<juce::AudioParameterChoice>("HighCut Slope", "HighCut Slope", stringArray, 0));

    return layout;
} // CreateParameterLayout

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new Egaliseur_Simple_v1AudioProcessor();
}
