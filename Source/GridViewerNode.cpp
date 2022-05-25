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
	: GenericProcessor ("Grid Viewer"),
	  subprocessorToDraw(0)
{

	setProcessorType(PROCESSOR_TYPE_SINK);

}

GridViewerNode::~GridViewerNode()
{ }

AudioProcessorEditor* GridViewerNode::createEditor()
{
	editor = new GridViewerEditor(this, true);
    return editor;
}

void GridViewerNode::setParameter(int index, float value)
{
	if (index == 0)
	{

		std::cout << "Node updating subprocessor to " << (uint32)value << std::endl;

		subprocessorToDraw = (uint32)value;

		activityView.reset();

		activityView = std::make_unique<ActivityView>(subprocessorChanCount[subprocessorToDraw], 10);

		float sampleRate = inputSampleRates[subprocessorToDraw];
		auto editor = (GridViewerEditor*) getEditor();
		editor->updateSampleRateLabel(String(sampleRate));

		skip = (int)(sampleRate / targetSampleRate);

		if (skip < 1)
			skip = 1;
	}
	
}

void GridViewerNode::process(AudioSampleBuffer& buffer)
{

	const int nChannels = buffer.getNumChannels();


	for (int ch = 0, localIndex = 0; ch < nChannels; ++ch)
	{
		if(getChannelSourceId(getDataChannel(ch)) == subprocessorToDraw)
		{
			const int nSamples = getNumSamples(ch);
			const float *buffPtr = buffer.getReadPointer(ch);

			for (int n = 0; n < nSamples; n += skip)
			{
				
				activityView->addSample(*(buffPtr + n), localIndex);
			}

			localIndex++;
		}
	}

	/*
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
	*/

}


void GridViewerNode::updateSettings()
{
    std::cout << "Setting num inputs on GridViewer to " << getNumInputs() << std::endl;

	int totalSubprocessors = 0;
	juce::SortedSet<uint32> inputSubprocessorIndices;
	inputSampleRates.clear();
	subprocessorChanCount.clear();
	subprocessorNames.clear();

	for (int i = 0; i < getTotalDataChannels(); i++)
	{
		uint32 channelSubprocessor = getChannelSourceId(getDataChannel(i));

		if (!inputSubprocessorIndices.contains(channelSubprocessor))
		{
			std::cout << "Adding subprocessor:  " << channelSubprocessor << std::endl;
			inputSubprocessorIndices.add(channelSubprocessor);
			inputSampleRates.set(channelSubprocessor, getDataChannel(i)->getSampleRate());
			subprocessorChanCount.set(channelSubprocessor, 1);
			subprocessorNames.set(channelSubprocessor, getSubprocessorName(i));
			totalSubprocessors++;
		}
		else
		{
			int chanCount = subprocessorChanCount[channelSubprocessor] + 1;
			subprocessorChanCount.set(channelSubprocessor, chanCount);
		}
	}

	// update the editor's subprocessor selection display, only if there's atleast one subprocessor
	if (totalSubprocessors > 0)
	{
		GridViewerEditor * ed = (GridViewerEditor*) getEditor();
		ed->updateSubprocessorSelectorOptions(inputSubprocessorIndices);
	}

}


uint32 GridViewerNode::getChannelSourceId(const InfoObjectCommon* chan)
{
    return getProcessorFullId(chan->getSourceNodeID(), chan->getSubProcessorIdx());
}


String GridViewerNode::getSubprocessorName(int channel)
{

	const DataChannel* ch = getDataChannel(channel);

	int subprocessorNameId = ch->findMetaData(MetaDataDescriptor::CHAR, 64, "subprocessor-name");

	String name;

	if (subprocessorNameId > -1)
	{
		
		const MetaDataValue* val = ch->getMetaDataValue(subprocessorNameId);
		String stringValue;
		val->getValue(stringValue);

		name = ch->getSourceName() + " " + stringValue;

		// std::cout << "Sb name: " << name << std::endl;
	}
	else {

		uint16 sourceNodeId = ch->getSourceNodeID();
		uint16 subProcessorIdx = ch->getSubProcessorIdx();

		name = ch->getSourceName() + " " + String(sourceNodeId) + "/" + String(subProcessorIdx);
	}

	return name;
}


bool GridViewerNode::enable()
{

	activityView->reset();

    auto editor = (GridViewerEditor*) getEditor();

	editor->enable();

	return isEnabled;
}

bool GridViewerNode::disable()
{
    ((GridViewerEditor*) getEditor())->disable();
    return true;
}
