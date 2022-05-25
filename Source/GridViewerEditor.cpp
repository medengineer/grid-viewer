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
    
    subprocessorSelectionLabel = std::make_unique<Label>("Subprocessor Selection Label", "Display Subprocessor:");
    subprocessorSelectionLabel->setBounds(10, 30, 130, 24);
    addAndMakeVisible(subprocessorSelectionLabel.get());

	subprocessorSelection = std::make_unique<ComboBox>("Subprocessor Selector");
    subprocessorSelection->setBounds(15, 60, 120, 20);
    subprocessorSelection->addListener(this);
    addAndMakeVisible(subprocessorSelection.get());
    
    subprocessorSampleRateLabel = std::make_unique<Label>("Subprocessor Sample Rate Label", "Sample Rate:");
	// subprocessorSampleRateLabel->setFont(Font("Fira Code", "SemiBold", 16.0f));
	subprocessorSampleRateLabel->setJustificationType(Justification::centred);
    subprocessorSampleRateLabel->setBounds(10, 90, 160, 24);
    addAndMakeVisible(subprocessorSampleRateLabel.get());
}

GridViewerEditor::~GridViewerEditor()
{ }


void GridViewerEditor::comboBoxChanged(ComboBox* cb)
{
    if (cb == subprocessorSelection.get())
    {
		uint32 selectedSubproc = cb->getSelectedId();

		setDrawableSubprocessor(selectedSubproc);
    }

}

Visualizer* GridViewerEditor::createNewCanvas()
{
    return new GridViewerCanvas(gridViewerNode);
}

void GridViewerEditor::updateSubprocessorSelectorOptions(juce::SortedSet<uint32> subprocessorIndices)
{
	std::cout << "Updating subprocessor selector!!!" << std::endl;
	// clear out the old data
    subprocessorSelection->clear(dontSendNotification);

	int subprocessorToSet = -1;
	if (subprocessorIndices.size() > 0)
	{
		subprocessorToSet = 0;
	}

	for (int i = 0; i < subprocessorIndices.size(); ++i)
	{
		String name = gridViewerNode->getSubprocessorNameForId(subprocessorIndices.getUnchecked(i));
		subprocessorSelection->addItem(name, subprocessorIndices.getUnchecked(i));
	}

	if (subprocessorToSet >= 0)
	{
		subprocessorSelection->setSelectedId(subprocessorIndices.getUnchecked(subprocessorToSet), sendNotification);
	}
	else
	{
		subprocessorSelection->addItem("None", 1);
		subprocessorSelection->setSelectedId(1, dontSendNotification);

		String sampleRateLabelText = "Sample Rate: <not available>";
		subprocessorSampleRateLabel->setText(sampleRateLabelText, dontSendNotification);
	}
}

void GridViewerEditor::updateSampleRateLabel(String labelText)
{
	String sampleRateLabelText = "Sample Rate: " + labelText;
	subprocessorSampleRateLabel->setText(sampleRateLabelText, dontSendNotification);
}

void GridViewerEditor::setDrawableSubprocessor(uint32 subProcId)
{

	std::cout << "Setting drawable subprocessor to " << subProcId << std::endl;

	gridViewerNode->setParameter(0, subProcId);

	if (canvas != nullptr)
	{
		GridViewerCanvas* c = (GridViewerCanvas*)canvas.get();
		c->updateCanvasSubprocessor(subProcId);
	}
		
}

void GridViewerEditor::startAcquisition()
{
	subprocessorSelection->setEnabled(false);

	if (canvas != nullptr)
        canvas->beginAnimation();
}

void GridViewerEditor::stopAcquisition()
{
	subprocessorSelection->setEnabled(true);

	if (canvas != nullptr)
        canvas->endAnimation();
}
