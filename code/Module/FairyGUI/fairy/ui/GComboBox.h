#ifndef __GCOMBOBOX_H__
#define __GCOMBOBOX_H__

#include "base/Base.h"
#include "FairyGUIMacros.h"
#include "GComponent.h"
#include "GList.h"

NS_FGUI_BEGIN

class GComboBox : public GComponent
{
    FUI_RTTI_DERIVED(GComboBox, GComponent)
public:
    GComboBox();
    virtual ~GComboBox();

    CREATE_FUNC(GComboBox);

    const AZStd::string& getTitle() const;
    void setTitle(const AZStd::string& value);

    virtual const AZStd::string& getText() const override { return getTitle(); }
    virtual void setText(const AZStd::string& value) override { setTitle(value); }

    virtual const AZStd::string& getIcon() const override;
    virtual void setIcon(const AZStd::string& value) override;

    const AZStd::string& getValue() const;
    void setValue(const AZStd::string& value);

    int getSelectedIndex() const { return _selectedIndex; }
    void setSelectedIndex(int value);

    GController* getSelectionController() const { return _selectionController; }
    void setSelectionController(GController* value) { _selectionController = value; }

    vector<AZStd::string>& getItems() { return _items; }
    vector<AZStd::string>& getIcons() { return _icons; }
    vector<AZStd::string>& getValues() { return _values; }

    void refresh();

    int visibleItemCount;
    PopupDirection popupDirection;

protected:
    virtual void constructFromXML(TXMLElement* xml) override;
    virtual void setup_AfterAdd(TXMLElement* xml) override;
    virtual void handleControllerChanged(GController* c) override;
    virtual void handleGrayedChanged() override;

    void setState(const AZStd::string& value);
    void setCurrentState();
    void updateSelectionController();
    void updateDropdownList();
    void showDropdown();
    void renderDropdownList();

    GComponent* _dropdown;
    GObject* _titleObject;
    GObject* _iconObject;
    GList* _list;
    GController* _selectionController;

    vector<AZStd::string> _items;
    vector<AZStd::string> _icons;
    vector<AZStd::string> _values;

private:

    void onClickItem(EventContext* context);
    void onRollover(EventContext* context);
    void onRollout(EventContext* context);
    void onTouchBegin(EventContext* context);
    void onTouchEnd(EventContext* context);
    void onPopupWinClosed(EventContext* context);

    bool _itemsUpdated;
    int _selectedIndex;
    GController* _buttonController;
    bool _down;
    bool _over;
};

NS_FGUI_END

#endif
