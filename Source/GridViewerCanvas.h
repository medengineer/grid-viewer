/*
 ------------------------------------------------------------------

 This file is part of the Open Ephys GUI
 Copyright (C) 2017 Open Ephys

 ------------------------------------------------------------------

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.

 */

#ifndef __GRIDVIEWERCANVAS_H__
#define __GRIDVIEWERCANVAS_H__

#include "VisualizerWindowHeaders.h"

namespace GridViewer {

class Electrode : public Component
{
public:
    Electrode() : c(Colours::grey) { }

    void setColour(Colour c);

    void paint(Graphics& g);

private:
    Colour c;
};

class GridViewerCanvas : public Visualizer
{
public:

    /** Constructor */
    GridViewerCanvas(class GridViewerNode*);

    /** Destructor */
    virtual ~GridViewerCanvas() override;

    /**
     *  Required overrides from Visualizer
     */

    /** Called when the component's tab becomes visible again.*/
    void refreshState() override;

    /** Called when parameters of underlying data processor are changed.*/
    void update() override;

    /** Called instead of "repaint" to avoid redrawing underlying components if not necessary.*/
    void refresh() override;

    /** Called when data acquisition is active.*/
    void beginAnimation() override;

    /** Called when data acquisition ends.*/
    void endAnimation() override;

    void setParameter(int, float) override;

    void setParameter(int, int, int, float) override;

    /**
     *  Overrides from juce::Component
     */

    void paint(Graphics& g) override;
    void resized() override;

    void updateDataStream();

private:
    class GridViewerNode* node;

    ScopedPointer<class GridViewerViewport> viewport;
    OwnedArray<Electrode> electrodes;

    int numChannels;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GridViewerCanvas);
};

class GridViewerViewport : public Viewport
{
public:
    GridViewerViewport(GridViewerCanvas*);
    virtual ~GridViewerViewport() override;
    void visibleAreaChanged(const Rectangle<int>& newVisibleArea);

private:
    GridViewerCanvas* canvas;
    class ChannelViewCanvas* channelsView;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GridViewerViewport);
};

}

#endif /* __GRIDVIEWERCANVAS_H__ */
