/*
 ------------------------------------------------------------------
 
 This file is part of the Open Ephys GUI
 Copyright (C) 2013 Open Ephys
 
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

#include "GridViewerEditor.h"

#include "GridViewerNode.h"
#include "GridViewerCanvas.h"

using namespace GridViewer;

GridViewerEditor::GridViewerEditor(GenericProcessor* parentNode, bool useDefaultParameterEditors=true)
					: VisualizerEditor(parentNode, 180, useDefaultParameterEditors),
					  hasNoInputs(true)
{

	tabText = "Grid Viewer";
	desiredWidth = 180;

	gridViewerNode = (GridViewerNode *)parentNode;
    
    streamSelectionLabel = std::make_unique<Label>("Stream Selection Label", "Display Stream:");
    streamSelectionLabel->setBounds(10, 30, 130, 24);
    //addAndMakeVisible(streamSelectionLabel.get());

	streamSelection = std::make_unique<ComboBox>("Stream Selector");
    streamSelection->setBounds(15, 60, 120, 20);
    streamSelection->addListener(this);
    //addAndMakeVisible(streamSelection.get());
    
    streamSampleRateLabel = std::make_unique<Label>("Stream Sample Rate Label", "Sample Rate:");
	streamSampleRateLabel->setFont(Font("Fira Code", "SemiBold", 16.0f));
	streamSampleRateLabel->setJustificationType(Justification::centred);
    streamSampleRateLabel->setBounds(10, 90, 160, 24);
    addAndMakeVisible(streamSampleRateLabel.get());

	updateStreamSelectorOptions();
}

GridViewerEditor::~GridViewerEditor()
{ }


void GridViewerEditor::comboBoxChanged(ComboBox* cb)
{
	/*
    if (cb == streamSelection.get())
    {
		uint16 selectedStream = cb->getSelectedId();

		if (selectedStream > 0)
			setDrawableStream(selectedStream);
    }
	*/

}

Visualizer* GridViewerEditor::createNewCanvas()
{
    return new GridViewerCanvas(gridViewerNode);
}

void GridViewerEditor::updateSettings()
{
	//updateStreamSelectorOptions();
}

void GridViewerEditor::updateStreamSelectorOptions()
{
	setDrawableStream();
}

void GridViewerEditor::setDrawableStream()
{

	//std::cout << "Setting drawable stream to " << streamId << std::endl;

	//gridViewerNode->setParameter(0, streamId);

	if (canvas != nullptr)
	{
		GridViewerCanvas* c = (GridViewerCanvas*)canvas.get();
		c->updateDataStream();
	}
		
}

void GridViewerEditor::startAcquisition()
{
	streamSelection->setEnabled(false);

	if (canvas != nullptr)
        canvas->beginAnimation();
}

void GridViewerEditor::stopAcquisition()
{
	streamSelection->setEnabled(true);

	if (canvas != nullptr)
        canvas->endAnimation();
}
