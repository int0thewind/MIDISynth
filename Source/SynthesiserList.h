/*
  ==============================================================================

    SynthesiserList.h
    Created: 10 May 2020 10:54:13am
    Author:  Hanzhi Yin

  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"

class SynthesiserList : public ListBox, public ListBoxModel {
public:
    SynthesiserList();

    ~SynthesiserList() override = default;

    int getNumRows() override;

    void paintListBoxItem(int rowNumber, Graphics &g, int width, int height, bool rowIsSelected) override;
};