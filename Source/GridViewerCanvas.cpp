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

#ifdef _WIN32
#define _USE_MATH_DEFINES
#endif

#include "GridViewerCanvas.h"

#include "GridViewerNode.h"
#include "GridViewerEditor.h"

#include "ColourScheme.h"

using namespace GridViewer;

void Electrode::setColour(Colour c_)
{
    c = c_;
    //repaint();
}

void Electrode::paint(Graphics& g)
{
    g.fillAll(c);
}

#pragma mark - GridViewerCanvas -

GridViewerCanvas::GridViewerCanvas(GridViewerNode * node_)
    : node(node_), numChannels(0)
{
    refreshRate = 30;

    updateElectrodeGrid(64);
}

GridViewerCanvas::~GridViewerCanvas()
{
}

void GridViewerCanvas::refreshState()
{
}

void GridViewerCanvas::update()
{

}

void GridViewerCanvas::setParameter(int, float)
{

}

void GridViewerCanvas::setParameter(int, int, int, float)
{
    
}


void GridViewerCanvas::refresh()
{
    const float* peakToPeakValues = node->getLatestValues();

    //std::cout << "Refresh." << std::endl;

    for (int i = 0; i < numChannels; i++)
    {
        electrodes[i]->setColour(ColourScheme::getColourForNormalizedValue(peakToPeakValues[i] / 200));
    }

    repaint();
}

void GridViewerCanvas::beginAnimation()
{
    std::cout << "Beginning animation." << std::endl;

    startCallbacks();
}

void GridViewerCanvas::endAnimation()
{
    std::cout << "Ending animation." << std::endl;

    stopCallbacks();
}

void GridViewerCanvas::updateCanvasSubprocessor(uint32 subProcId)
{
    numChannels = node->getSubprocessorChanCount(subProcId);

    std::cout << "Canvas subprocessor: " << subProcId << ", num of channels: " << numChannels << std::endl;

    int numColumns = 0;

    if(numChannels <= 64)
        numColumns = 8;
    else if(numChannels > 64 && numChannels <= 256)
        numColumns = 16;
    else if(numChannels > 256 && numChannels <= 1024)
        numColumns = 32;
    else if(numChannels > 1024 && numChannels <= 4096)
        numColumns = 64;
    else
    {
        numColumns = 64;
        numChannels = 4096;
    }

    updateElectrodeGrid(numColumns);

    for (int i = 0; i < electrodes.size(); i++)
    {
        if (i < numChannels)
            electrodes[i]->setColour(Colours::grey);
        else
            electrodes[i]->setColour(Colours::black);
    }
        

    repaint();
}

void GridViewerCanvas::updateElectrodeGrid(int numCols)
{
    const int totalPixels = numCols * numCols; 

    const int LEFT_BOUND = 20;
    const int TOP_BOUND = 20;
    const int SPACING = 2;
    const int NUM_COLUMNS = numCols;
    const int HEIGHT = 8;
    const int WIDTH = 8;

    electrodes.clear();

    for (int i = 0; i < totalPixels; i++)
    {
        Electrode* e = new Electrode();

        int column = i % NUM_COLUMNS;
        int row = i / NUM_COLUMNS;
        int L = LEFT_BOUND + column * (WIDTH + SPACING);
        int T = TOP_BOUND + row * (HEIGHT + SPACING);

        e->setBounds(L,
            T,
            WIDTH,
            HEIGHT);

        addAndMakeVisible(e);
        electrodes.add(e);
    }
}

void GridViewerCanvas::paint(Graphics &g)
{

    g.fillAll(Colours::darkgrey);
}

void GridViewerCanvas::resized()
{
    //viewport->setBounds(0,
    //                    0,
     //                   getWidth(),
     //                   getHeight());

}

#pragma mark - GridViewerViewport -

GridViewerViewport::GridViewerViewport(GridViewerCanvas *canvas)
    : Viewport(), canvas(canvas)
{
}

GridViewerViewport::~GridViewerViewport()
{
}

void GridViewerViewport::visibleAreaChanged(const Rectangle<int> &newVisibleArea)
{
    canvas->repaint(getBoundsInParent());
}
