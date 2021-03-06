/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic startup code for a Juce application.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "ulib_pluginhost.h"
#include "ulib_PluginPanel.h"

AudioProcessor * ChuckRacksAudioProcessor::processorInstance = NULL;

//==============================================================================
ChuckRacksAudioProcessor::ChuckRacksAudioProcessor()
{
    //ConsoleGlobal *ConsoleGlobal::instance = 0;
    
    processorInstance = this;
    
    fprintf(stderr, "ChuckRacksAudioProcessor::ChuckRacksAudioProcessor\n");
    chuck_options options;
    libchuck_options_reset(&options);
//    options.buffer_size = getBlockSize();
//    options.adaptive_buffer_size = 0;
//    options.num_channels = getNumOutputChannels();
//    options.sample_rate = getSampleRate();
    options.slave = true;
    // hardcode (HACK!)
    options.buffer_size = 512;
    options.adaptive_buffer_size = 0;
    options.num_channels = 2;
    options.sample_rate = 44100;

    //TODO: check if a valid instance of chuck is running (how do I do this?)
    ck = libchuck_create(&options);
    
    libchuck_add_module(ck, (void*)pluginhost_query);
    libchuck_add_module(ck, (void*)pluginPanel_query);
    
    input_buffer = new float[options.buffer_size*options.num_channels];
    output_buffer = new float[options.buffer_size*options.num_channels];
   
    
    libchuck_vm_start(ck);
    
    codeEditorDemo = new CodeEditorDemo();
    
    fileContainerManagerModel = new FileContainerManagerModel(ck);
    
    //fileContainerManagerModel->addFileContainer();
    
    g_hostInfo->midiInputBufferP = (&midiInputBuffer);
    g_hostInfo->midiOutputBufferP = (&midiOutputBuffer);
    
    g_pluginPanel->fileContainerManager = fileContainerManagerModel;

    //NormalisableRange<float> paramRange(0.0, 1.0, 0.1, 1.0);
    //addParameter (testParameter = new AudioParameterFloat("param1", "Parameter1", paramRange, 1.0));
    
}

ChuckRacksAudioProcessor::~ChuckRacksAudioProcessor()
{
    fprintf(stderr, "ChuckRacksAudioProcessor::~ChuckRacksAudioProcessor\n");

    libchuck_destroy(ck);
    ck = NULL;
    
    if(input_buffer) { delete[] input_buffer; input_buffer = NULL; }
    if(output_buffer) { delete[] output_buffer; output_buffer = NULL; }
}

//==============================================================================
const String ChuckRacksAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

int ChuckRacksAudioProcessor::getNumParameters()
{
    return 0;
}

float ChuckRacksAudioProcessor::getParameter (int index)
{
    return 0.0f;
}

void ChuckRacksAudioProcessor::setParameter (int index, float newValue)
{
}

const String ChuckRacksAudioProcessor::getParameterName (int index)
{
    return String::empty;
}

const String ChuckRacksAudioProcessor::getParameterText (int index)
{
    return String::empty;
}

const String ChuckRacksAudioProcessor::getInputChannelName (int channelIndex) const
{
    return String (channelIndex + 1);
}

const String ChuckRacksAudioProcessor::getOutputChannelName (int channelIndex) const
{
    return String (channelIndex + 1);
}

bool ChuckRacksAudioProcessor::isInputChannelStereoPair (int index) const
{
    return true;
}

bool ChuckRacksAudioProcessor::isOutputChannelStereoPair (int index) const
{
    return true;
}

bool ChuckRacksAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool ChuckRacksAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool ChuckRacksAudioProcessor::silenceInProducesSilenceOut() const
{
    return false;
}

double ChuckRacksAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int ChuckRacksAudioProcessor::getNumPrograms()
{
    return 1;
}

int ChuckRacksAudioProcessor::getCurrentProgram()
{
    return 0;
}

void ChuckRacksAudioProcessor::setCurrentProgram (int index)
{
}

const String ChuckRacksAudioProcessor::getProgramName (int index)
{
    return String::empty;
}

void ChuckRacksAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void ChuckRacksAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void ChuckRacksAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

void ChuckRacksAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{    
    // Get current position/time info from host, otherwise set to some default
    AudioPlayHead::CurrentPositionInfo pos;
    if (getPlayHead() != nullptr && getPlayHead()->getCurrentPosition(pos)) {
        lastPosInfo = pos;
    }else{
        lastPosInfo.resetToDefault();
    }
    
    if (lastPosInfo.bpm != g_hostInfo->previousTempo)
    {
        g_hostInfo->setTempo(lastPosInfo.bpm);
    }
    
    if(lastPosInfo.isPlaying&&!g_hostInfo->wasPlaying)
    {
        g_hostInfo->broadcastPlayEvent();
    }
    
    if(!lastPosInfo.isPlaying&&g_hostInfo->wasPlaying)
    {
        g_hostInfo->broadcastStopEvent();

    }
    
    g_hostInfo->wasPlaying = lastPosInfo.isPlaying;
    
    
    g_hostInfo->positionInBeat = fmod( pos.ppqPosition,1 );
    
    //DBG(positionInBeat);
    if (g_hostInfo->positionInBeat>0.749) //sixteenth
    {
        if (g_hostInfo->current16th!=3)
        {
            //DBG("16th3");
            g_hostInfo->broadcast16thHit();
            g_hostInfo->current16th=3;
        }
    }
    else if (g_hostInfo->positionInBeat>0.499)
    {
        if (g_hostInfo->current16th!=2)
        {
            //DBG("16th2");
            g_hostInfo->broadcast16thHit();
            g_hostInfo->current16th=2;
        }
    }
    else if (g_hostInfo->positionInBeat>0.249)
    {
        if (g_hostInfo->current16th!=1)
        {
            //DBG("16th1");
            g_hostInfo->broadcast16thHit();
            g_hostInfo->current16th=1;
        }
    }
    else if (g_hostInfo->positionInBeat>0.0)
    {
        if (g_hostInfo->current16th!=0)
        {
            //DBG("16th0");
            g_hostInfo->broadcast16thHit();
            g_hostInfo->broadcastBeatStartEvent();

            g_hostInfo->current16th=0;
        }
    }
        // DBG(pos.ppqPosition);
       // DBG("16th!");
    
    g_hostInfo->previousTempo=lastPosInfo.bpm;
    
    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // I've added this to avoid people getting screaming feedback
    // when they first compile the plugin, but obviously you don't need to
    // this code if your algorithm already fills all the output channels.
    for (int i = getTotalNumInputChannels(); i < getTotalNumOutputChannels(); ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // copy input
    for (int channel = 0; channel < getTotalNumInputChannels(); ++channel)
    {
        float* channelData = buffer.getWritePointer (channel);
        
        for (int i = 0; i < buffer.getNumSamples(); i++)
        {
            input_buffer[i*2+channel] = channelData[i];
        }
    }
    
    libchuck_slave_process(ck, input_buffer, output_buffer, buffer.getNumSamples());
    
    // copy output
    
    
    
    MidiBuffer::Iterator midiIterator(midiMessages); //iterator to loop through our midi buffer that gets passed into the process block
    MidiBuffer tempMidiBuffer; // temporary midi buffer where we do ou "work" and store transposed midi messages
    MidiMessage tempMessage; // temporary midi message to store each midi message from our incoming buffer
    int midiMessagePos; //temporary varirable to store the location of each midi message that we iterate through
    bool doMidiBroadcast = false;
    
    midiInputBuffer.clear();
    
    
    while(midiIterator.getNextEvent(tempMessage, midiMessagePos)){
     
         //if(tempMessage.isNoteOnOrOff()){
           // tempMessage.setNoteNumber(tempMessage.getNoteNumber() + 12); //transpose the message
           // tempMidiBuffer.addEvent(tempMessage, midiMessagePos);
        //}
     
        midiInputBuffer.addEvent(tempMessage, midiMessagePos);
        doMidiBroadcast = true;
        
        const uint8 *midiVals = tempMessage.getRawData();
        int datasize = tempMessage.getRawDataSize();
        //std::cout<<int(midiVals[0])<<" "<<int(midiVals[1])<<" "<<int(midiVals[2])<<std::endl;
    }
    
    if(doMidiBroadcast)
    {
        g_hostInfo->midiOutputBufferPos = 0;
        g_hostInfo->broadcastMidiEvent();
    }
    
    midiMessages = midiOutputBuffer;
    midiOutputBuffer.clear();

    
    
    
    
    
    for (int channel = 0; channel < getTotalNumOutputChannels(); ++channel)
    {
        float* channelData = buffer.getWritePointer (channel);

        for (int i = 0; i < buffer.getNumSamples(); i++)
        {
            channelData[i] = output_buffer[i*2+channel];
        }
    }

    
    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
//    for (int channel = 0; channel < getNumInputChannels(); ++channel)
//    {
//        float* channelData = buffer.getWritePointer (channel);
//
//        // ..do something to the data...
//    }
}

//==============================================================================
bool ChuckRacksAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* ChuckRacksAudioProcessor::createEditor()
{
    return new ChuckRacksAudioProcessorEditor (this);
}

//==============================================================================
void ChuckRacksAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void ChuckRacksAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ChuckRacksAudioProcessor();
}
