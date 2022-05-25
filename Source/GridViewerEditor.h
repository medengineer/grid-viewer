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

#ifndef __GRIDVIEWEREDITOR_H__
#define __GRIDVIEWEREDITOR_H__

#include "VisualizerEditorHeaders.h"

namespace GridViewer {

class GridViewerEditor
    : public VisualizerEditor,
      public ComboBox::Listener
{
public:

    /** Constructor */
    GridViewerEditor(GenericProcessor* parentNode, bool useDefaultParameterEditors);

    /** Destructor */
    virtual ~GridViewerEditor();

    /** Sets the drawable stream */
    void comboBoxChanged(ComboBox *cb) override;

    /** Called by the base class VisualizerEditor to display the canvas
        when the user chooses to display one

        @see VisualizerEditor::buttonClicked
     */
    virtual Visualizer* createNewCanvas() override;

    /** Updates available streams*/
	void updateSubprocessorSelectorOptions(juce::SortedSet<uint32> subProcessors);

    /** Update sample rate label */
    void updateSampleRateLabel(String newText);

    /** Disables stream selection*/
	void startAcquisition() override;

    /** Enables stream selection */
	void stopAcquisition() override;


private:
    juce::SortedSet<uint32> inputSubprocessors;

    class GridViewerNode* gridViewerNode;

    std::unique_ptr<Label> subprocessorSelectionLabel;
    std::unique_ptr<ComboBox> subprocessorSelection;

    std::unique_ptr<Label> subprocessorSampleRateLabel;

    bool hasNoInputs;

    /** Set drawable subproccesor for canvas*/
    void setDrawableSubprocessor(uint32 subProcId);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GridViewerEditor);
};

}

#endif /* __GRIDVIEWEREDITOR_H__ */
