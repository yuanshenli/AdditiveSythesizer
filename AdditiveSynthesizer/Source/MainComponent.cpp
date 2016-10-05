// Music 256a / CS 476a | fall 2016
// CCRMA, Stanford University
//
// Author: Shenli Yuan (shenliy@stanford.edu)
// Description: Simple JUCE sine wave synthesizer

#ifndef MAINCOMPONENT_H_INCLUDED
#define MAINCOMPONENT_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "Sine.h"

const int MAX_SINE_WAVE = 7;

class MainContentComponent :
    public AudioAppComponent,
    private Slider::Listener,
    //private ToggleButton::Listener,
    private Button::Listener
{
public:
    MainContentComponent() : onOff (0), samplingRate(0.0)
    {
        nSines = 0;
        
        
        for (int i = 0; i < MAX_SINE_WAVE; i++){
            // configuring frequency slider and adding it to the main window
            addAndMakeVisible(frequencySlider[i]);
            frequencySlider[i].setRange (50.0, 5000.0);
            frequencySlider[i].setSkewFactorFromMidPoint (500.0);
            frequencySlider[i].setValue(1000); // will also set the default frequency of the sine osc
            frequencySlider[i].addListener (this);
        
            // configuring frequency label box and adding it to the main window
            addAndMakeVisible(frequencyLabel[i]);
            frequencyLabel[i].setText ("Frequency", dontSendNotification);
            frequencyLabel[i].attachToComponent (&frequencySlider[i], true);
            
        
            // configuring gain slider and adding it to the main window
            addAndMakeVisible(gainSlider[i]);
            gainSlider[i].setRange (0.0, 1.0);
            gainSlider[i].setValue(0.5); // will alsi set the default gain of the sine osc
            gainSlider[i].addListener (this);
        
        
            // configuring gain label and adding it to the main window
            addAndMakeVisible(gainLabel[i]);
            gainLabel[i].setText ("Gain", dontSendNotification);
            gainLabel[i].attachToComponent (&gainSlider[i], true);
        
        }
        
        for (int i = 0; i < 8; i++) {
            addAndMakeVisible(noteButton[i]);
            noteButton[i].addListener(this);
            
        }
        noteButton[0].setButtonText("C4");
        noteButton[1].setButtonText("D4");
        noteButton[2].setButtonText("E4");
        noteButton[3].setButtonText("F4");
        noteButton[4].setButtonText("G4");
        noteButton[5].setButtonText("A4");
        noteButton[6].setButtonText("B4");
        noteButton[7].setButtonText("C5");
        
        
       
        
        // configuring on/off button and adding it to the main window
        addAndMakeVisible(onOffButton);
        onOffButton.addListener(this);
        
        
        // configuring on/off label and adding it to the main window
        addAndMakeVisible(onOffLabel);
        onOffLabel.setText ("On/Off", dontSendNotification);
        onOffLabel.attachToComponent (&onOffButton, true);
        
        // configure addWave button and add it to the main window
        addAndMakeVisible(addWaveButton);
        addWaveButton.addListener(this);
        addWaveButton.setButtonText("Add Wave");
        
        // configure deleteWave button and add it to the main window
        addAndMakeVisible(deleteWaveButton);
        deleteWaveButton.addListener(this);
        deleteWaveButton.setButtonText("Delete Wave");
        
        
        // configuring addWave label and adding it to the main window
        // addAndMakeVisible(addWaveLabel);
        // addWaveLabel.setText ("addWave", dontSendNotification);
        // addWaveLabel.attachToComponent (&addWaveButton, true);
        
        setSize (800, 800);
        nChans = 1;
        setAudioChannels (0, nChans); // no inputs, one output
        
        
    }
    
    ~MainContentComponent()
    {
        shutdownAudio();
    }
    
    void resized() override
    {
        // placing the UI elements in the main window
        // getWidth has to be used in case the window is resized by the user
        const int sliderLeft = 80;
        const int sliderRight = 650;
        for (int i = 0; i < MAX_SINE_WAVE; i++){
            frequencySlider[i].setBounds (sliderLeft, 10 + 60 * i, getWidth() - sliderLeft - 160, 20);
            gainSlider[i].setBounds (sliderLeft, 40 + 60 * i, getWidth() - sliderLeft - 160, 20);
            frequencySlider[i].setVisible(false);
            frequencyLabel[i].setVisible(false);
            gainSlider[i].setVisible(false);
            gainLabel[i].setVisible(false);
        }
        onOffButton.setBounds (sliderRight + 60, 70, 100, 20);
        addWaveButton.setBounds (sliderRight, 10, 100, 20);
        deleteWaveButton.setBounds (sliderRight, 40, 100, 20);
        deleteWaveButton.setVisible(false);
        for (int i = 0; i < 8; i++){
            noteButton[i].setBounds(sliderRight, 100 + 30 * i, 100, 20);
        }
        
    }
    
    void sliderValueChanged (Slider* slider) override
    {
        if (samplingRate > 0.0){
            for (int i = 0; i < MAX_SINE_WAVE; i++){
                if (slider == &frequencySlider[i]){
                    sine[i].setFrequency(frequencySlider[i].getValue());
                }
                else if (slider == &gainSlider[i]){
                    gain[i] = gainSlider[i].getValue();
                }
            }
        }
    }
    
    void buttonClicked (Button* button) override
    {
        // turns audio on or off
        if (button == &onOffButton){
            if(onOffButton.getToggleState()){
                onOff = 1;
            }
            else{
                onOff = 0;
            }
        }
        
        // adds an oscillator
        if (button == &addWaveButton && nSines < MAX_SINE_WAVE){
            // frequencySlider[0].setValue(500);
            nSines++;
            deleteWaveButton.setVisible(true);
            if (nSines == MAX_SINE_WAVE) addWaveButton.setVisible(false);
            frequencySlider[nSines - 1].setVisible(true);
            frequencyLabel[nSines - 1].setVisible(true);
            gainSlider[nSines - 1].setVisible(true);
            gainLabel[nSines - 1].setVisible(true);
            
        }
        
        // deletes an oscillator
        if (button == &deleteWaveButton && nSines > 0){
            
            frequencySlider[nSines - 1].setVisible(false);
            frequencyLabel[nSines - 1].setVisible(false);
            gainSlider[nSines - 1].setVisible(false);
            gainLabel[nSines - 1].setVisible(false);
            nSines--;
            addWaveButton.setVisible(true);
            if (nSines == 0) deleteWaveButton.setVisible(false);
        }
        // Set frequency of the last added oscillator according to the button input
        if (button == &noteButton[0] && nSines > 0){
            frequencySlider[nSines - 1].setValue(261.626);
        }
        if (button == &noteButton[1] && nSines > 0){
            frequencySlider[nSines - 1].setValue(293.665);
        }
        if (button == &noteButton[2] && nSines > 0){
            frequencySlider[nSines - 1].setValue(329.628);
        }
        if (button == &noteButton[3] && nSines > 0){
            frequencySlider[nSines - 1].setValue(349.228);
        }
        if (button == &noteButton[4] && nSines > 0){
            frequencySlider[nSines - 1].setValue(391.995);
        }
        if (button == &noteButton[5] && nSines > 0){
            frequencySlider[nSines - 1].setValue(440.00);
        }
        if (button == &noteButton[6] && nSines > 0){
            frequencySlider[nSines - 1].setValue(493.883);
        }
        if (button == &noteButton[7] && nSines > 0){
            frequencySlider[nSines - 1].setValue(523.251);
        }
        
    }
    
    void prepareToPlay (int /*samplesPerBlockExpected*/, double sampleRate) override
    {
        samplingRate = sampleRate;
        for (int i = 0; i < MAX_SINE_WAVE; i++){
            sine[i].setSamplingRate(sampleRate);
        }
    }
    
    void releaseResources() override
    {
    }
    
    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) override
    {
        // getting the audio output buffer to be filled
        float* const buffer = bufferToFill.buffer->getWritePointer (0, bufferToFill.startSample);
        
        // computing one block
        for (int sample = 0; sample < bufferToFill.numSamples; ++sample)
        {   
            if(onOff == 1) {
                for (int i = 0; i < nSines; i++){
                    buffer[sample] += sine[i].tick() * gain[i];
                }
                buffer[sample] /= nSines;
            }
            else buffer[sample] = 0.0;
        }
    }
    
    
private:
    // UI Elements
    Slider frequencySlider[MAX_SINE_WAVE];
    Slider gainSlider[MAX_SINE_WAVE];
    ToggleButton onOffButton;
    TextButton addWaveButton;
    TextButton deleteWaveButton;
    TextButton noteButton[8]; // the note button

    Label frequencyLabel[MAX_SINE_WAVE], gainLabel[MAX_SINE_WAVE], onOffLabel;
    
    Sine sine[MAX_SINE_WAVE]; // the sine wave oscillator
    
    // Global Variables
    float gain[MAX_SINE_WAVE] = {0.0};
    int onOff, samplingRate, nChans, nSines;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainContentComponent)
};

Component* createMainContentComponent()     { return new MainContentComponent(); }


#endif  // MAINCOMPONENT_H_INCLUDED
