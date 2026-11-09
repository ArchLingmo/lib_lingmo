#include <QQmlExtensionPlugin>
#include <QQmlEngine>

#include "wallpaper.h"
#include "sessioninfo.h"

class QmlPlugins : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")

public:
    void registerTypes(const char * uri) override {
        qmlRegisterType<Wallpaper>(uri, 1, 0, "Wallpaper");
        // Session type and the "available in the Wayland/X11 session" wording
        qmlRegisterSingletonType<SessionInfo>(uri, 1, 0, "Session", [](QQmlEngine *, QJSEngine *) -> QObject * {
            return new SessionInfo;
        });
    }
};

#include "plugin.moc"