#include "EQControls.h"

EQControls::EQControls(Parameters& params_) :
    params(params_),
    bassAttachment(*params.bassParam, [this](float f) { parameterUpdated(0, f); }),
    midsAttachment(*params.midsParam, [this](float f) { parameterUpdated(1, f); }),
    trebleAttachment(*params.trebleParam, [this](float f) { parameterUpdated(2, f); })
{
    bands[0].label = "BASS";
    bands[1].label = "MIDS";
    bands[2].label = "TREBLE";

    bands[0].attachment = &bassAttachment;
    bands[1].attachment = &midsAttachment;
    bands[2].attachment = &trebleAttachment;

    for (auto& band : bands) {
        band.attachment->sendInitialUpdate();
    }
}

void EQControls::resized()
{
    auto bounds = getLocalBounds();
    bands[0].rect.setBounds(0, 0, bounds.getWidth() / 3, bounds.getHeight());
    bands[1].rect.setBounds(bands[0].rect.getRight(), 0, bounds.getWidth() / 3, bounds.getHeight());
    bands[2].rect.setBounds(bands[1].rect.getRight(), 0, bounds.getWidth() - bands[1].rect.getRight(), bounds.getHeight());

    for (auto& band : bands) {
        band.innerRect = band.rect.withTrimmedTop(20).withTrimmedBottom(40).reduced(10, 0);
    }
}

void EQControls::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(60, 60, 60));
    g.setFont(juce::Font("Arial", 16.0f, juce::Font::plain));

    for (auto& band : bands) {
        auto gradientRect = band.rect.withTrimmedBottom(40).reduced(2, 0);
        juce::ColourGradient gradient(
            juce::Colour(30, 30, 30), gradientRect.getTopLeft().toFloat(),
            juce::Colour(60, 60, 60), gradientRect.getBottomLeft().toFloat(),
            false);
        g.setGradientFill(gradient);
        g.fillRect(gradientRect);

        auto rect = band.innerRect;
        g.setColour(juce::Colour(90, 90, 90));
        g.fillRect(rect.getX(), rect.getCentreY(), rect.getWidth(), 1);

        int y = rect.getCentreY() - int(std::round(band.value * rect.getHeight() / 12.0f));
        g.setColour(juce::Colours::white);
        g.fillRect(rect.getX(), y - 1, rect.getWidth(), 3);

        auto text = juce::String(band.value, 1) + " dB";
        y += (band.value >= 0.0f) ? -4 : 15;
        g.drawSingleLineText(text, band.rect.getCentreX(), y, juce::Justification::horizontallyCentred);
    }

    g.setColour(juce::Colours::white);
    for (auto& band : bands) {
        g.drawSingleLineText(
            band.label, band.rect.getCentreX(), band.rect.getBottom() - 4,
            juce::Justification::horizontallyCentred);
    }
}

void EQControls::parameterUpdated(int index, float value)
{
    bands[size_t(index)].value = value;
    repaint();
}

void EQControls::mouseDown(const juce::MouseEvent& event)
{
    if (event.getMouseDownX() < bands[0].rect.getRight()) {
        activeBand = &bands[0];
    } else if (event.getMouseDownX() < bands[1].rect.getRight()) {
        activeBand = &bands[1];
    } else {
        activeBand = &bands[2];
    }

    if (event.mods.isAltDown()) {
        activeBand->attachment->setValueAsCompleteGesture(0.0f);
        activeBand = nullptr;
    } else {
        startPos = event.position.getY();
        startValue = activeBand->value;
        activeBand->attachment->beginGesture();
    }
}

void EQControls::mouseDrag(const juce::MouseEvent& event)
{
    if (activeBand != nullptr) {
        float distance = event.position.getY() - startPos;
        float db = distance * 12.0f / float(activeBand->innerRect.getHeight());
        float newValue = std::clamp(startValue - db, -6.0f, 6.0f);
        activeBand->attachment->setValueAsPartOfGesture(newValue);
    }
}

void EQControls::mouseUp([[maybe_unused]] const juce::MouseEvent& event)
{
    if (activeBand != nullptr) {
        activeBand->attachment->endGesture();
        activeBand = nullptr;
    }
}
