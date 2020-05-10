#include "SynthesiserData.h"

const Identifier SynthesiserData::TAIL_ON {"Tail On"};
const Identifier SynthesiserData::TAIL_OFF {"Tail Off"};

SynthesiserData::SynthesiserData() : valueTree(TAIL_ON) {}

SynthesiserData::SynthesiserData(const Identifier &id) : valueTree(id) {}

SynthesiserData::SynthesiserData(juce::ValueTree data) : valueTree(std::move(data)) {}

void SynthesiserData::addListener(juce::ValueTree::Listener *listener) {
    this->valueTree.addListener(listener);
}

void SynthesiserData::removeListener(juce::ValueTree::Listener *listener) {
    this->valueTree.removeListener(listener);
}

SynthesiserData::operator juce::ValueTree() const {
    return this->valueTree;
}

bool SynthesiserData::isValid() const {
    return this->valueTree.isValid();
}

void SynthesiserData::clear() {
    this->valueTree.removeAllProperties(nullptr);
    this->valueTree.removeAllChildren(nullptr);
}
