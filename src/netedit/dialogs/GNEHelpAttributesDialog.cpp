/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2025 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    GNEHelpAttributesDialog.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jul 2025
///
// Help dialog used in netedit
/****************************************************************************/

#include <netedit/GNETagProperties.h>
#include <netedit/elements/GNEAttributeCarrier.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/windows/GUIAppEnum.h>

#include "GNEHelpAttributesDialog.h"

// ===========================================================================
// method definitions
// ===========================================================================

GNEHelpAttributesDialog::GNEHelpAttributesDialog(GNEApplicationWindow* applicationWindow,
        const GNEAttributeCarrier* AC) :
    GNEDialog(applicationWindow, TLF("Attributes of %", AC->getTagStr()).c_str(),
              GUIIcon::MODEINSPECT, DialogType::BASIC_HELP, GNEDialog::Buttons::OK,
              OpenType::MODAL, ResizeMode::RESIZABLE) {
    // Create FXTable
    FXTable* table = new FXTable(myContentFrame, this, MID_TABLE, GUIDesignTableNotEditable);
    // configure table
    int sizeColumnDescription = 0;
    int sizeColumnDefinitions = 0;
    table->setVisibleRows((FXint)(AC->getTagProperty()->getNumberOfAttributes()));
    table->setVisibleColumns(4);
    table->setTableSize((FXint)(AC->getTagProperty()->getNumberOfAttributes()), 4);
    table->setBackColor(GUIDesignBackgroundColorWhite);
    table->setColumnText(0, TL("Attribute"));
    table->setColumnText(1, TL("Category"));
    table->setColumnText(2, TL("Description"));
    table->setColumnText(3, TL("Definition"));
    table->getRowHeader()->setWidth(0);
    table->setColumnHeaderHeight(GUIDesignHeight);
    // Iterate over vector of additional parameters
    int itemIndex = 0;
    // add internal attributes
    addAttributes(AC, table, itemIndex, sizeColumnDescription, sizeColumnDefinitions, false);
    // add netedit attributes
    addAttributes(AC, table, itemIndex, sizeColumnDescription, sizeColumnDefinitions, true);
    table->fitRowsToContents(0, itemIndex);
    // set header
    FXHeader* header = table->getColumnHeader();
    header->setItemJustify(0, JUSTIFY_CENTER_X);
    header->setItemSize(0, 120);
    header->setItemJustify(0, JUSTIFY_CENTER_X);
    header->setItemSize(1, 100);
    header->setItemJustify(1, JUSTIFY_CENTER_X);
    header->setItemSize(2, sizeColumnDescription * 8);
    header->setItemJustify(2, JUSTIFY_CENTER_X);
    header->setItemSize(3, sizeColumnDefinitions * 6);
    // open modal dialog
    openDialog();
}


GNEHelpAttributesDialog::~GNEHelpAttributesDialog() {
}


void
GNEHelpAttributesDialog::runInternalTest(const InternalTestStep::DialogArgument* /*dialogArgument*/) {
    // nothing to do
}


void
GNEHelpAttributesDialog::addAttributes(const GNEAttributeCarrier* AC, FXTable* table, int& itemIndex,
                                       int& sizeColumnDescription, int& sizeColumnDefinitions, const bool neteditAttributes) {
    for (const auto& attrProperty : AC->getTagProperty()->getAttributeProperties()) {
        if (attrProperty->isNeteditEditor() == neteditAttributes) {
            // Set attribute
            FXTableItem* attributeItem = new FXTableItem(attrProperty->getAttrStr().c_str());
            attributeItem->setJustify(FXTableItem::CENTER_X);
            table->setItem(itemIndex, 0, attributeItem);
            // Set description of element
            FXTableItem* categoryItem = new FXTableItem("");
            categoryItem->setText(attrProperty->getCategory().c_str());
            categoryItem->setJustify(FXTableItem::CENTER_X);
            table->setItem(itemIndex, 1, categoryItem);
            // Set description of element
            FXTableItem* descriptionItem = new FXTableItem("");
            descriptionItem->setText(attrProperty->getDescription().c_str());
            sizeColumnDescription = MAX2(sizeColumnDescription, (int)attrProperty->getDescription().size());
            descriptionItem->setJustify(FXTableItem::CENTER_X);
            table->setItem(itemIndex, 2, descriptionItem);
            // Set definition
            FXTableItem* definitionItem = new FXTableItem(attrProperty->getDefinition().c_str());
            definitionItem->setJustify(FXTableItem::LEFT);
            table->setItem(itemIndex, 3, definitionItem);
            sizeColumnDefinitions = MAX2(sizeColumnDefinitions, (int)attrProperty->getDefinition().size());
            itemIndex++;
        }
    }
}

/****************************************************************************/
