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

GridViewerEditor::GridViewerEditor(GenericProcessor* parentNode)
					: VisualizerEditor(parentNode, "Grid", 180),
					  hasNoInputs(true)
{
	gridViewerNode = (GridViewerNode *)parentNode;
    
    streamSelectionLabel = std::make_unique<Label>("Stream Selection Label", "Display Stream:");
    streamSelectionLabel->setBounds(10, 30, 130, 24);
    addAndMakeVisible(streamSelectionLabel.get());

	streamSelection = std::make_unique<ComboBox>("Stream Selector");
    streamSelection->setBounds(15, 60, 120, 20);
    streamSelection->addListener(this);
    addAndMakeVisible(streamSelection.get());
    
    streamSampleRateLabel = std::make_unique<Label>("Stream Sample Rate Label", "Sample Rate:");
	streamSampleRateLabel->setFont(Font("Fira Code", "SemiBold", 16.0f));
	streamSampleRateLabel->setJustificationType(Justification::centred);
    streamSampleRateLabel->setBounds(10, 90, 160, 24);
    addAndMakeVisible(streamSampleRateLabel.get());
}

GridViewerEditor::~GridViewerEditor()
{ }


void GridViewerEditor::comboBoxChanged(ComboBox* cb)
{
    if (cb == streamSelection.get())
    {
		uint16 selectedStream = cb->getSelectedId();

		if (selectedStream > 0)
			setDrawableStream(selectedStream);
    }

}

Visualizer* GridViewerEditor::createNewCanvas()
{
    return new GridViewerCanvas(gridViewerNode);
}

void GridViewerEditor::updateSettings()
{
	updateStreamSelectorOptions();
}

void GridViewerEditor::updateStreamSelectorOptions()
{
    bool needsUpdate = false;
	uint16 currentStreamId = streamSelection->getSelectedId();

	streamSelection->clear(dontSendNotification);

	// Add all datastreams to combobox
	for (auto stream : gridViewerNode->getDataStreams())
	{
		uint16 streamID = stream->getStreamId();

		streamSelection->addItem("[" + String(stream->getSourceNodeId()) + "] " +
			stream->getName(), streamID);

	}

	if (streamSelection->indexOfItemId(currentStreamId) > -1)
	{
		streamSelection->setSelectedId(currentStreamId, sendNotification);
	}
	else
	{
		if (streamSelection->getNumItems() > 0)
			streamSelection->setSelectedItemIndex(0, sendNotification);
		else
			streamSampleRateLabel->setText("Sample Rate: <NULL>", dontSendNotification);
	}

}

void GridViewerEditor::setDrawableStream(uint16 streamId)
{

	std::cout << "Setting drawable stream to " << streamId << std::endl;

	gridViewerNode->setParameter(0, streamId);

	if (canvas == nullptr)
		checkForCanvas();

	DataStream* stream = gridViewerNode->getDataStream(streamId);

	if (stream != nullptr)
	{
		GridViewerCanvas* c = (GridViewerCanvas*)canvas.get();
		c->updateDataStream(stream);
		streamSampleRateLabel->setText("Sample Rate: " + String(stream->getSampleRate()), dontSendNotification);
	}
		
}

void GridViewerEditor::startAcquisition()
{
	streamSelection->setEnabled(false);
}

void GridViewerEditor::stopAcquisition()
{
	streamSelection->setEnabled(true);
}
