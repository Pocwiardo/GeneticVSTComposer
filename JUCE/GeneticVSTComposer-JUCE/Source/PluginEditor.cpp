/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginEditor.h"
#include "PluginProcessor.h"

void InitializeRotarySlider(juce::Slider *slider, std::pair<float, float> range,
                            float interval, juce::LookAndFeel *lookAndFeel) {
  slider->setSliderStyle(
      juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
  slider->setRange(range.first, range.second, interval);
  slider->setLookAndFeel(lookAndFeel);
  slider->setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100,
                          slider->getTextBoxHeight());
}

void InitializeAttatchedLabel(juce::Label *label, const juce::String &text,
                              juce::Component *owner, bool onLeft,
                              juce::Justification justification) {
  label->setText(text, juce::dontSendNotification);
  label->attachToComponent(owner, onLeft);
  label->setJustificationType(justification);
}

//==============================================================================
GeneticVSTComposerJUCEAudioProcessorEditor::
    GeneticVSTComposerJUCEAudioProcessorEditor(
        GeneticVSTComposerJUCEAudioProcessor &p)
    : AudioProcessorEditor(&p), audioProcessor(p) {
  // Make sure that before the constructor has finished, you've set the
  // editor's size to whatever you need it to be.
  setSize(400, 800);

  int currentHeight = 10;
  juce::LookAndFeel *mainLookAndFeel =
      new juce::LookAndFeel_V4(juce::LookAndFeel_V4::getMidnightColourScheme());
  float rotaryWidth = 80;
  float rotaryHeight = 100;

  //--- Scale snapping button (toggle)
  scaleSnapBtn.setBounds(getWidth() / 2 - 100, currentHeight, 200, 30);
  scaleSnapBtn.setButtonText("Quantize to scale");
  scaleSnapBtn.setLookAndFeel(mainLookAndFeel);
  addAndMakeVisible(scaleSnapBtn);

  currentHeight += 30;

  separationLabel.setBounds(getWidth() / 2 - 75, currentHeight, 150, 30);
  separationLabel.setText("GENETIC MIDI COMPOSER", juce::dontSendNotification);
  separationLabel.setJustificationType(juce::Justification::centred);
  addAndMakeVisible(separationLabel);

  currentHeight += 50;

  //--- Mode Switch
  int modeButtonWidth = 120;
  // 0 - Full melody
  mode0Btn.setButtonText("Full melody");
  mode0Btn.setBounds(getWidth() / 2 - modeButtonWidth * 1.5, currentHeight,
                     modeButtonWidth, 30);
  mode0Btn.setClickingTogglesState(true);
  mode0Btn.setRadioGroupId(modeRadioGroupID);
  mode0Btn.setConnectedEdges(juce::Button::ConnectedOnRight);
  mode0Btn.setToggleState(true, juce::dontSendNotification);
  mode0Btn.setLookAndFeel(mainLookAndFeel);
  addAndMakeVisible(mode0Btn);

  // 1 - Craft Rhythm
  mode1Btn.setButtonText("Craft Rhythm");
  mode1Btn.setBounds(getWidth() / 2 - modeButtonWidth * 0.5, currentHeight,
                     modeButtonWidth, 30);
  mode1Btn.setClickingTogglesState(true);
  mode1Btn.setRadioGroupId(modeRadioGroupID);
  mode1Btn.setConnectedEdges(juce::Button::ConnectedOnRight |
                             juce::Button::ConnectedOnLeft);
  mode1Btn.setLookAndFeel(mainLookAndFeel);
  addAndMakeVisible(mode1Btn);

  // 2 - Build melody on rhythm
  mode2Btn.setButtonText("Build melody on rhythm");
  mode2Btn.setBounds(getWidth() / 2 + modeButtonWidth * 0.5, currentHeight,
                     modeButtonWidth, 30);
  mode2Btn.setClickingTogglesState(true);
  mode2Btn.setRadioGroupId(modeRadioGroupID);
  mode2Btn.setConnectedEdges(juce::Button::ConnectedOnLeft);
  mode2Btn.setLookAndFeel(mainLookAndFeel);
  addAndMakeVisible(mode2Btn);

  modeLbl.setText("Mode", juce::dontSendNotification);
  modeLbl.attachToComponent(&mode1Btn, false);
  modeLbl.setJustificationType(juce::Justification::centred);
  addAndMakeVisible(modeLbl);

  currentHeight += 40;

  //--- scale
  scaleBox1.setBounds(100, currentHeight, 90, 30);
  scaleBox1.setLookAndFeel(mainLookAndFeel);
  scaleBox1.addItemList(
      {"C", "C#", "D", "D#", "E", "F", "F#", "G#", "A", "A#", "B"}, 1);
  scaleBox1.setSelectedId(1);
  addAndMakeVisible(scaleBox1);

  scaleBox2.setBounds(200, currentHeight, 160, 30);
  scaleBox2.setLookAndFeel(mainLookAndFeel);
  scaleBox2.addSectionHeading("The diatonic scales");
  scaleBox2.addItem("Diatonic", 1);
  scaleBox2.addSectionHeading("Ancient scales");
  scaleBox2.addItem("Ionian", 2);
  scaleBox2.addItem("Dorian", 3);
  scaleBox2.addItem("Phrygian", 4);
  scaleBox2.addItem("Lydian", 5);
  scaleBox2.addItem("Mixolydian", 6);
  scaleBox2.addItem("Aeolian", 7);
  scaleBox2.addItem("Locrian", 8);
  scaleBox2.addSectionHeading("The major scales");
  scaleBox2.addItem("Major", 9);
  scaleBox2.addItem("Harmonic", 10);
  scaleBox2.addSectionHeading("The minor scales");
  scaleBox2.addItem("Natural minor", 11);
  scaleBox2.addItem("Harmonic minor", 12);
  scaleBox2.addItem("Melodic minor", 13);
  scaleBox2.addItem("Bachian", 14);
  scaleBox2.addItem("Minor neapolitan", 15);
  scaleBox2.addSectionHeading("Other scales");
  scaleBox2.addItem("Chromatic", 16);
  scaleBox2.addItem("Whole tone", 17);
  scaleBox2.addItem("Octatonic", 18);
  scaleBox2.setSelectedId(9);
  addAndMakeVisible(scaleBox2);

  scaleLbl.setText("Scale", juce::dontSendNotification);
  scaleLbl.attachToComponent(&scaleBox1, true);
  addAndMakeVisible(scaleLbl);

  currentHeight += 40;

  //--- sequence length
  seqLenBox.setBounds(100, currentHeight, 90, 30);
  seqLenBox.setLookAndFeel(mainLookAndFeel);
  seqLenBox.addItemList({"1", "2", "3", "4"}, 1);
  seqLenBox.setSelectedId(1);
  addAndMakeVisible(seqLenBox);

  seqLenLbl.setText("Measures", juce::dontSendNotification);
  seqLenLbl.attachToComponent(&seqLenBox, true);
  addAndMakeVisible(seqLenLbl);

  //--- note duration
  noteDurationBox.setBounds(290, currentHeight, 70, 30);
  noteDurationBox.setLookAndFeel(mainLookAndFeel);
  noteDurationBox.addItemList(noteDurationStr, 1);
  noteDurationBox.setSelectedId(2);
  addAndMakeVisible(noteDurationBox);

  noteDurationLbl.setText("Shortest note", juce::dontSendNotification);
  noteDurationLbl.attachToComponent(&noteDurationBox, true);
  addAndMakeVisible(noteDurationLbl);

  currentHeight += 60;

  //--- diversity
  diversitySlid.setBounds(10, currentHeight, rotaryWidth, rotaryHeight);
  InitializeRotarySlider(&diversitySlid, {0, 1}, 0.01, mainLookAndFeel);
  diversitySlid.setValue(0.8);
  addAndMakeVisible(diversitySlid);

  InitializeAttatchedLabel(&diversityLbl, "Diversity", &diversitySlid, false,
                           juce::Justification::centred);
  addAndMakeVisible(diversityLbl);

  //--- dynamics
  dynamicsSlid.setBounds(110, currentHeight, rotaryWidth, rotaryHeight);
  InitializeRotarySlider(&dynamicsSlid, {0, 1}, 0.01, mainLookAndFeel);
  dynamicsSlid.setValue(0.8);
  addAndMakeVisible(dynamicsSlid);

  InitializeAttatchedLabel(&dynamicsLbl, "Dynamics", &dynamicsSlid, false,
                           juce::Justification::centred);
  addAndMakeVisible(dynamicsLbl);

  //--- jazziness
  jazzinessSlid.setBounds(210, currentHeight, rotaryWidth, rotaryHeight);
  InitializeRotarySlider(&jazzinessSlid, {0, 1}, 0.01, mainLookAndFeel);
  jazzinessSlid.setValue(0.0);
  addAndMakeVisible(jazzinessSlid);

  InitializeAttatchedLabel(&jazzinessLbl, "Jazziness", &jazzinessSlid, false,
                           juce::Justification::centred);
  addAndMakeVisible(jazzinessLbl);

  currentHeight += 140 - (rotaryHeight / 2);

  //--- Pause Amount
  pauseAmountSlid.setBounds(310, currentHeight, rotaryWidth, rotaryHeight);
  InitializeRotarySlider(&pauseAmountSlid, {0, 1}, 0.01, mainLookAndFeel);
  pauseAmountSlid.setValue(0.0);
  addAndMakeVisible(pauseAmountSlid);

  InitializeAttatchedLabel(&pauseAmountLbl, "Pause Amount", &pauseAmountSlid,
                           false, juce::Justification::centred);
  addAndMakeVisible(pauseAmountLbl);

  currentHeight += rotaryHeight / 2;

  //--- valence
  valenceSlid.setBounds(10, currentHeight, rotaryWidth, rotaryHeight);
  InitializeRotarySlider(&valenceSlid, {0, 1}, 0.01, mainLookAndFeel);
  valenceSlid.setValue(0.8);
  addAndMakeVisible(valenceSlid);

  InitializeAttatchedLabel(&valenceLbl, "Valence", &valenceSlid, false,
                           juce::Justification::centred);
  addAndMakeVisible(valenceLbl);

  //--- arousal
  arousalSlid.setBounds(110, currentHeight, rotaryWidth, rotaryHeight);
  InitializeRotarySlider(&arousalSlid, {0, 1}, 0.01, mainLookAndFeel);
  arousalSlid.setValue(0.8);
  addAndMakeVisible(arousalSlid);

  InitializeAttatchedLabel(&arousalLbl, "Arousal", &arousalSlid, false,
                           juce::Justification::centred);
  addAndMakeVisible(arousalLbl);

  //--- weirdness
  weirdnessSlid.setBounds(210, currentHeight, rotaryWidth, rotaryHeight);
  InitializeRotarySlider(&weirdnessSlid, {0, 1}, 0.01, mainLookAndFeel);
  weirdnessSlid.setValue(0.0);
  addAndMakeVisible(weirdnessSlid);

  InitializeAttatchedLabel(&weirdnessLbl, "Weirdness", &weirdnessSlid, false,
                           juce::Justification::centred);
  addAndMakeVisible(weirdnessLbl);

  currentHeight += 110;

  //--- note range
  noteRangeSlid.setBounds(90, currentHeight, 250, 20);
  noteRangeSlid.setSliderStyle(juce::Slider::TwoValueHorizontal);
  noteRangeSlid.setTextBoxStyle(juce::Slider::NoTextBox, false, 40,
                                speedQualitySlid.getTextBoxHeight());
  noteRangeSlid.setLookAndFeel(mainLookAndFeel);
  noteRangeSlid.setRange(1, 8, 1);
  noteRangeSlid.setMaxValue(5);
  noteRangeSlid.setMinValue(3);
  addAndMakeVisible(noteRangeSlid);

  InitializeAttatchedLabel(&noteRangeLbl, "Note range", &noteRangeSlid, true,
                           juce::Justification::right);
  addAndMakeVisible(noteRangeLbl);

  currentHeight += 30;

  //--- speed-quality
  speedQualitySlid.setBounds(90, currentHeight, 250, 20);
  speedQualitySlid.setTextBoxStyle(juce::Slider::NoTextBox, false, 30,
                                   speedQualitySlid.getTextBoxHeight());
  speedQualitySlid.setLookAndFeel(mainLookAndFeel);
  speedQualitySlid.setRange(0, 2, 1);
  speedQualitySlid.setValue(1);
  // speedQualitySlid.setNumDecimalPlacesToDisplay
  addAndMakeVisible(speedQualitySlid);

  InitializeAttatchedLabel(&speedLbl, "Speed", &speedQualitySlid, true,
                           juce::Justification::right);
  addAndMakeVisible(speedLbl);

  qualityLbl.setBounds(speedQualitySlid.getX() + speedQualitySlid.getWidth() +
                           5,
                       currentHeight, 50, 20);
  qualityLbl.setText("Quality", juce::dontSendNotification);
  qualityLbl.setJustificationType(juce::Justification::centredLeft);
  addAndMakeVisible(qualityLbl);

  currentHeight += 30;

  //--- Start Generating Button
  startGenBtn.setBounds(getWidth() / 2 - 75, currentHeight, 150, 30);
  startGenBtn.setButtonText("Generate!");
  addAndMakeVisible(&startGenBtn);

  currentHeight += 35;

  //--- DEBUG label
  int debugWidth = getWidth() - 50;
  debugTextBox.setBounds((getWidth() - debugWidth) / 2, currentHeight,
                         debugWidth, getHeight() - currentHeight - 20);
  debugTextBox.setMultiLine(true, false);
  debugTextBox.setReadOnly(true);
  debugTextBox.setText("DEBUG", false);
  addAndMakeVisible(&debugTextBox);

  // adding Listeners
  scaleSnapBtn.addListener(this);
  startGenBtn.addListener(this);
}

GeneticVSTComposerJUCEAudioProcessorEditor::
    ~GeneticVSTComposerJUCEAudioProcessorEditor() {
  // removing Listeners
  scaleSnapBtn.removeListener(this);
  startGenBtn.removeListener(this);
}

void GeneticVSTComposerJUCEAudioProcessorEditor::buttonClicked(
    juce::Button *button) {
  if (button == &startGenBtn) // Start Generation Button clicked
  {
    int speedQualityNO =
        speedQualitySlid.getValue(); // index for values of population size and
                                     // generation number
    int composeMode = -1;
    if (mode0Btn.getToggleState())
      composeMode = 0;
    else if (mode1Btn.getToggleState())
      composeMode = 1;
    else if (mode2Btn.getToggleState())
      composeMode = 2;

    int noteLower =
        scaleBox1.getSelectedItemIndex() + noteRangeSlid.getMinValue() * 12;
    int noteHigher = scaleBox1.getSelectedItemIndex() +
                     noteRangeSlid.getMaxValue() * 12 + 12;

    // TODO - set the debugInfo string that will be shown in the window (get it
    // from generator)
    audioProcessor.GenerateMelody(
        composeMode, // Compose Mode
        scaleBox1.getText().toStdString() + " " +
            scaleBox2.getText().toStdString(), // scale
        {noteLower, noteHigher},               // note range
        diversitySlid.getValue(),              // diversity
        dynamicsSlid.getValue(),               // dynamics
        arousalSlid.getValue(),                // arousal
        pauseAmountSlid.getValue(),            // pause amount
        valenceSlid.getValue(),                // valence
        jazzinessSlid.getValue(),              // jazziness
        weirdnessSlid.getValue(),              // weirdness
        noteDurationValues[noteDurationBox.getSelectedId() -
                           1],                     // note duration
        SpeedQualityValues[speedQualityNO].first,  // population size
        SpeedQualityValues[speedQualityNO].second, // generation number
        seqLenBox.getText().getDoubleValue());     // sequence length
  } else if (button = &scaleSnapBtn) // when he scale button gets toggled
  {
    audioProcessor.scaleSnapping = scaleSnapBtn.getToggleState();
  }
}

//==============================================================================
void GeneticVSTComposerJUCEAudioProcessorEditor::paint(juce::Graphics &g) {
  // (Our component is opaque, so we must completely fill the background with a
  // solid colour)
  g.fillAll(
      getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

  g.setColour(juce::Colours::white);
  g.setFont(15.0f);

  // updating the debug text (its update'ed in the real-time)
  debugTextBox.setText(audioProcessor.debugInfo, false);
}

void GeneticVSTComposerJUCEAudioProcessorEditor::resized() {
  // This is generally where you'll want to lay out the positions of any
  // subcomponents in your editor..
}
