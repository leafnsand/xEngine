#ifndef __POPUPMENU_H__
#define __POPUPMENU_H__

#include "FairyGUIMacros.h"
#include "base/Base.h"
#include "event/UIEventDispatcher.h"
#include "FieldTypes.h"

NS_FGUI_BEGIN
class GComponent;
class GButton;
class GList;

class PopupMenu : public Ref
{
    FUI_RTTI_DERIVED(PopupMenu, Ref)
public:
    static PopupMenu* create(const AZStd::string& resourceURL);
    static PopupMenu* create() { return create(""); }

    PopupMenu();
    virtual ~PopupMenu();

    GButton* addItem(const AZStd::string& caption, EventCallback callback);
    GButton* addItemAt(const AZStd::string& caption, int index, EventCallback callback);
    void addSeperator();
    const AZStd::string& getItemName(int index) const;
    void setItemText(const AZStd::string& name, const AZStd::string& caption);
    void setItemVisible(const AZStd::string& name, bool visible);
    void setItemGrayed(const AZStd::string& name, bool grayed);
    void setItemCheckable(const AZStd::string& name, bool checkable);
    void setItemChecked(const AZStd::string& name, bool check);
    bool isItemChecked(const AZStd::string& name) const;
    bool removeItem(const AZStd::string& name);
    void clearItems();
    int getItemCount() const;
    GComponent* getContentPane() const { return _contentPane; }
    GList* getList() const { return _list; }
    void show() { show(nullptr, PopupDirection::AUTO); }
    void show(GObject* target, PopupDirection dir);

protected:
    bool init(const AZStd::string& resourceURL);

    GComponent* _contentPane;
    GList* _list;

private:
    void onClickItem(EventContext* context);
    void onEnter(EventContext* context);
};

NS_FGUI_END

#endif
