#ifndef IUSERINTERFACE_H
#define IUSERINTERFACE_H

#include "MO/modinfo.h"
#include <uibase/delayedfilewriter.h>
#include <uibase/ipluginmodpage.h>
#include <uibase/iplugintool.h>

class QSettings;

class IUserInterface {
  public:
    virtual void storeSettings(QSettings& settings) = 0;

    virtual void registerPluginTool(MOBase::IPluginTool* tool) = 0;
    virtual void registerModPage(MOBase::IPluginModPage* modPage) = 0;

    virtual void installTranslator(const QString& name) = 0;

    virtual void disconnectPlugins() = 0;

    virtual bool closeWindow() = 0;
    virtual void setWindowEnabled(bool enabled) = 0;

    virtual void displayModInformation(ModInfo::Ptr modInfo, unsigned int index, int tab) = 0;

    virtual void lock() = 0;
    virtual void unlock() = 0;
    virtual bool unlockClicked() = 0;
    virtual void setProcessName(QString const&) = 0;
};

#endif // IUSERINTERFACE_H
