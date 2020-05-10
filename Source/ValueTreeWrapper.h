/*
  ==============================================================================

    ValueTreeWrapper.h
    Created: 10 May 2020 10:30:59am
    Author:  Hanzhi Yin

    Credit to Henrich Taube. Code retrieved from MUS 205 SP20 semester.
    For more info, please contact to taube@illinois.edu.

  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"

class ValueTreeWrapper {
public:
    explicit ValueTreeWrapper(const juce::Identifier& id) : valueTreeData(id) {}
    explicit ValueTreeWrapper(juce::ValueTree data) : valueTreeData(std::move(data)) {}
    virtual ~ValueTreeWrapper() = default;

    /**
     * Value tree listener is directly connected with the value tree itself.
     * Our class is a wrapper class. We add support to directly add listener to our value tree data
     * @param listener the listener class that we want to connect directly to the value tree.
     */
    virtual void addListener(juce::ValueTree::Listener* listener) {
        valueTreeData.addListener(listener);
    }

    /**
     * Value tree listener is directly connected with the value tree itself.
     * Our class is a wrapper class. We add support to directly remove listener to our value tree data
     * @param listener the listener class that we want to remove directly from the value tree.
     */
    virtual void removeListener(juce::ValueTree::Listener* listener) {
        valueTreeData.removeListener(listener);
    }

    /**
     * Our class wraps a value tree.
     * This conversion operator allows this wrapper class can also be treated as a value tree
     * @return The value tree data in the class
     */
    explicit operator juce::ValueTree() const {
        return valueTreeData;
    }

    /**
     * Check whether the value tree is valid or not
     * @return true if the value tree is valid
     */
    virtual bool isValid() const = 0;

    /**
     * Clear all the properties in the value tree
     */
    virtual void clear() {
        valueTreeData.removeAllProperties(nullptr);
        valueTreeData.removeAllChildren(nullptr);
    }
private:
    mutable juce::ValueTree valueTreeData;
};