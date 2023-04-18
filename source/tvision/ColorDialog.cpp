#include <tvision/ColorCommandCodes.h>
#include <tvision/ColorDialog.h>

const char* const TColorDialog::name = "TColorDialog";

TColorDialog::TColorDialog(TPalette* aPalette, TColorGroup* aGroups) noexcept
    : TWindowInit(&TColorDialog::initFrame)
    , TDialog(TRect(0, 0, 61, 18), colors)
{
    options |= ofCentered;
    if (aPalette != 0) {
        pal = new TPalette("", 0);
        *pal = *aPalette;
    } else
        pal = 0;

    TScrollBar* sb = new TScrollBar(TRect(18, 3, 19, 14));
    insert(sb);

    groups = new TColorGroupList(TRect(3, 3, 18, 14), sb, aGroups);
    insert(groups);
    insert(new TLabel(TRect(2, 2, 8, 3), groupText, groups));

    sb = new TScrollBar(TRect(41, 3, 42, 14));
    insert(sb);

    TView* p = new TColorItemList(TRect(21, 3, 41, 14), sb, aGroups->items);
    insert(p);
    insert(new TLabel(TRect(20, 2, 25, 3), itemText, p));

    forSel = new TColorSelector(TRect(45, 3, 57, 7),
        TColorSelector::csForeground);
    insert(forSel);
    forLabel = new TLabel(TRect(45, 2, 57, 3), forText, forSel);
    insert(forLabel);

    bakSel = new TColorSelector(TRect(45, 9, 57, 11),
        TColorSelector::csBackground);
    insert(bakSel);
    bakLabel = new TLabel(TRect(45, 8, 57, 9), bakText, bakSel);
    insert(bakLabel);

    display = new TColorDisplay(TRect(44, 12, 58, 14), textText);
    insert(display);

    monoSel = new TMonoSelector(TRect(44, 3, 59, 7));
    monoSel->hide();
    insert(monoSel);
    monoLabel = new TLabel(TRect(43, 2, 49, 3), colorText, monoSel);
    monoLabel->hide();
    insert(monoLabel);

    insert(new TButton(TRect(36, 15, 46, 17), okText, cmOK, bfDefault));
    insert(new TButton(TRect(48, 15, 58, 17), cancelText, cmCancel, bfNormal));
    selectNext (false);

    if (pal != 0)
        setData(pal);
}

TColorDialog::~TColorDialog()
{
    delete pal;
}

void TColorDialog::handleEvent(TEvent& event)
{
    if (event.what == evBroadcast && event.message.command == cmNewColorItem)
        groupIndex = groups->focused;
    TDialog::handleEvent(event);
    if (event.what == evBroadcast && event.message.command == cmNewColorIndex)
        display->setColor(&pal->data[event.message.infoByte]);
}

ushort TColorDialog::dataSize()
{
    return *pal->data + 1;
}

void TColorDialog::getData(void* rec)
{
    getIndexes(colorIndexes);
    *(TPalette*)rec = *pal;
}

void TColorDialog::setData(void* rec)
{
    if (!pal)
        pal = new TPalette("", 0);
    *pal = *(TPalette*)rec;

    setIndexes(colorIndexes);
    display->setColor(&pal->data[groups->getGroupIndex(groupIndex)]);
    groups->focusItem(groupIndex);
    if (showMarkers) {
        forLabel->hide();
        forSel->hide();
        bakLabel->hide();
        bakSel->hide();
        monoLabel->show();
        monoSel->show();
    }
    groups->select();
}

void TColorDialog::setIndexes(TColorIndex*& colIdx)
{
    uchar numGroups, index;

    numGroups = groups->getNumGroups();
    if (colIdx && (colIdx->colorSize != numGroups)) {
        delete[] colIdx;
        colIdx = 0;
    }
    if (!colIdx) {
        colIdx = (TColorIndex*)new uchar[numGroups + 2];
        colIdx->groupIndex = 0;
        memset(colIdx->colorIndex, 0, numGroups);
        colIdx->colorSize = numGroups;
    }
    for (index = 0; index < numGroups; index++)
        groups->setGroupIndex(index, colIdx->colorIndex[index]);

    groupIndex = colIdx->groupIndex;
}

void TColorDialog::getIndexes(TColorIndex*& colIdx)
{
    uchar n = groups->getNumGroups();
    if (!colIdx) {
        colIdx = (TColorIndex*)new uchar[n + 2];
        memset(colIdx->colorIndex, 0, n);
        colIdx->colorSize = n;
    }
    colIdx->groupIndex = groupIndex;
    for (uchar index = 0; index < n; index++)
        colIdx->colorIndex[index] = groups->getGroupIndex(index);
}

#if !defined(NO_STREAMABLE)

void TColorDialog::write(opstream& os)
{
    TDialog::write(os);
    os << display << groups << forLabel << forSel
       << bakLabel << bakSel << monoLabel << monoSel;
}

void* TColorDialog::read(ipstream& is)
{
    TDialog::read(is);
    is >> display >> groups >> forLabel >> forSel
        >> bakLabel >> bakSel >> monoLabel >> monoSel;
    pal = 0;
    return this;
}

TStreamable* TColorDialog::build()
{
    return new TColorDialog(streamableInit);
}

TColorDialog::TColorDialog(StreamableInit) noexcept
    : TWindowInit(0)
    , TDialog(streamableInit)
{
}

#endif
