#pragma once

#include "JuceHeader.h"

/**
 * A class that wraps a value tree and provides other functionality
 */
class SynthesiserData {
public:
    /**
     * This method constructs a new synthesiser data by a identifier
     * @param id Identifier to add to the empty value tree
     */
    explicit SynthesiserData(const juce::Identifier& id);

    /**
     * This method constructs a new synthesiser data based on a existed value tree
     * @param data the other value tree to copy from
     */
    explicit SynthesiserData(juce::ValueTree data);

    /**
     * Destructor of the value tree wrapper.
     */
    virtual ~SynthesiserData() = default;

    /**
     * Value tree listener is directly connected with the value tree itself.
     * Our class is a wrapper class. We add support to directly add listener to our value tree data
     * @param listener the listener class that we want to connect directly to the value tree.
     */
    virtual void addListener(juce::ValueTree::Listener* listener);

    /**
     * Value tree listener is directly connected with the value tree itself.
     * Our class is a wrapper class. We add support to directly remove listener to our value tree data
     * @param listener the listener class that we want to remove directly from the value tree.
     */
    virtual void removeListener(juce::ValueTree::Listener* listener);

    /**
     * This conversion operator allows this wrapper to be treated as a value tree
     * @return The value tree data in the class
     */
    explicit operator juce::ValueTree() const;

    /**
     * Check whether the synthesiser data (value tree) is valid or not
     * @return true if the value tree is valid
     */
    virtual bool isValid() const;

    /**
     * Clear all the properties in our synthesiser data (value tree)
     */
    virtual void clear();

    /**
     * The value tree we wrap
     */
    mutable juce::ValueTree valueTree;
};