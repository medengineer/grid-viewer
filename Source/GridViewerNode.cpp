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

#include "GridViewerNode.h"

#include "GridViewerEditor.h"
#include "GridViewerCanvas.h"

using namespace GridViewer;

ActivityView::ActivityView(int numChannels, int updateInterval_)
{
	for (int i = 0; i < numChannels; i++)
		peakToPeakValues.add(0);

	updateInterval = updateInterval_;

	reset();
}

const float* ActivityView::getPeakToPeakValues() {

	return peakToPeakValues.getRawDataPointer();
}

void ActivityView::addSample(float sample, int channel)
{
	if (channel == 0)
	{
		if (counter == updateInterval)
			reset();

		counter++;
	}

	//if (counter % 10 == 0)
	//{
	if (sample < minChannelValues[channel])
	{
		minChannelValues.set(channel, sample);
		return;
	}

	if (sample > maxChannelValues[channel])
	{
		maxChannelValues.set(channel, sample);
	}
	//}

}

void ActivityView::reset()
{

	//std::cout << "Reset." << std::endl;

	for (int i = 0; i < peakToPeakValues.size(); i++)
	{

		peakToPeakValues.set(i, maxChannelValues[i] - minChannelValues[i]);

		minChannelValues.set(i, 999999.9f);
		maxChannelValues.set(i, -999999.9f);
	}

	counter = 0;

}

GridViewerNode::GridViewerNode()
: GenericProcessor ("Grid Viewer")
{

}

GridViewerNode::~GridViewerNode()
{ }

AudioProcessorEditor* GridViewerNode::createEditor()
{
    editor = std::make_unique<GridViewerEditor> (this);
    return editor.get();
}

void GridViewerNode::setParameter(int index, float value)
{

	if (value > 100)
	{

		std::cout << "Node updating stream to " << value << std::endl;

		currentStream = (uint16)value;

		activityView.reset();

		activityView = std::make_unique<ActivityView>(getDataStream(currentStream)->getChannelCount(), 10);

		float sampleRate = getDataStream(currentStream)->getSampleRate();

		skip = (int)(sampleRate / targetSampleRate);

		if (skip < 1)
			skip = 1;
	}
	
}

void GridViewerNode::process(AudioBuffer<float>& b)
{

	uint32 numSamples = getNumSamplesInBlock(currentStream);

	for (auto channel : continuousChannels)
	{
		if (channel->getStreamId() == currentStream)
		{

			int globalIndex = channel->getGlobalIndex();
			int localIndex = channel->getLocalIndex();

			for (int i = 0; i < numSamples; i += skip)
			{
				activityView->addSample(*b.getReadPointer(globalIndex, i), localIndex);
			}
		}
	}

}



bool GridViewerNode::startAcquisition()
{

	activityView->reset();

    auto editor = (GridViewerEditor*) getEditor();

	editor->enable();

	return true;
}

bool GridViewerNode::stopAcquisition()
{
    ((GridViewerEditor*) getEditor())->disable();
    return true;
}
