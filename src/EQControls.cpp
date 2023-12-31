#include "EQControls.h"

EQControls::EQControls(Parameters& params_) :
    params(params_),
    bassAttachment(*params.bassParam, [this](float f) { parameterUpdated(0, f); }),
    midsAttachment(*params.midsParam, [this](float f) { parameterUpdated(1, f); }),
    trebleAttachment(*params.trebleParam, [this](float f) { parameterUpdated(2, f); })
{
    bands[0].label = "BASS";
    bands[0].attachment = &bassAttachment;
    bands[0].parameter = params.bassParam;

    bands[1].label = "MIDS";
    bands[1].attachment = &midsAttachment;
    bands[1].parameter = params.midsParam;

    bands[2].label = "TREBLE";
    bands[2].attachment = &trebleAttachment;
    bands[2].parameter = params.trebleParam;

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

    juce::Path path;
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

        int y = getBandY(band);
        if (path.isEmpty()) {
            path.startNewSubPath(rect.getX(), y);
        } else {
            path.lineTo(rect.getX(), y);
        }
        path.lineTo(rect.getRight(), y);

        auto text = juce::String(band.value, 1) + " dB";
        y += (band.value >= 0.0f) ? -4 : 15;
        g.setColour(juce::Colours::white);
        g.drawSingleLineText(text, band.rect.getCentreX(), y, juce::Justification::horizontallyCentred);
    }

    g.setColour(juce::Colours::white);
    g.strokePath(path, juce::PathStrokeType(3.0f, juce::PathStrokeType::JointStyle::curved));

    g.setColour(juce::Colours::white);
    for (auto& band : bands) {
        g.drawSingleLineText(
            band.label, band.rect.getCentreX(), band.rect.getBottom() - 4,
            juce::Justification::horizontallyCentred);
    }
}

void EQControls::mouseDown(const juce::MouseEvent& event)
{
    int bandIndex = indexOfBandAtPoint(event.getMouseDownPosition());
    activeBand = &bands[size_t(bandIndex)];

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
        setBandValue(*activeBand, startValue, distance, true);
    }
}

void EQControls::mouseUp([[maybe_unused]] const juce::MouseEvent& event)
{
    if (activeBand != nullptr) {
        activeBand->attachment->endGesture();
        activeBand = nullptr;
    }
}

void EQControls::mouseWheelMove(const juce::MouseEvent& event, const juce::MouseWheelDetails& wheel)
{
    if (event.mods.isAnyMouseButtonDown()) { return; }

    int bandIndex;
    if (wheel.isInertial) {
        bandIndex = lastUsedBand;  // can't change band when inertia
    } else {
        bandIndex = indexOfBandAtPoint(event.getPosition());
        lastUsedBand = bandIndex;
    }

    Band& band = bands[size_t(bandIndex)];
    float distance = wheel.deltaY * band.innerRect.getHeight() * (wheel.isSmooth ? 0.5f : 0.1f);
    setBandValue(band, band.value, distance, false);
}

void EQControls::parameterUpdated(int index, float value)
{
    bands[size_t(index)].value = value;
    repaint();
}

int EQControls::getBandY(const Band& band) const
{
    const auto& range = band.parameter->getNormalisableRange();
    float extent = range.end - range.start;
    return band.innerRect.getCentreY() - int(std::round(band.value * band.innerRect.getHeight() / extent));
}

int EQControls::indexOfBandAtPoint(const juce::Point<int>& point) const
{
    if (point.x < bands[0].rect.getRight()) {
        return 0;
    } else if (point.x < bands[1].rect.getRight()) {
        return 1;
    } else {
        return 2;
    }
}

void EQControls::setBandValue(Band& band, float referenceValue, float pixelDistance, bool isDragging)
{
    const auto& range = band.parameter->getNormalisableRange();
    float minValue = range.start;
    float maxValue = range.end;

    float dbDistance = pixelDistance * (maxValue - minValue) / float(band.innerRect.getHeight());
    float newValue = std::clamp(referenceValue - dbDistance, minValue, maxValue);

    if (isDragging) {
        band.attachment->setValueAsPartOfGesture(newValue);
    } else {
        band.attachment->setValueAsCompleteGesture(newValue);
    }
}
