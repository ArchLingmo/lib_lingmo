// "Disponível na sessão Wayland": put it where a feature of the other session would
// be. It hides itself where the feature can run. Plain QtQuick.Controls, so it
// takes the app's style (Lingmo's in every Lingmo app).
//
//   import Lingmo.System 1.0
//   SessionHint { session: "wayland" }
import QtQuick 2.15
import QtQuick.Controls 2.15
import Lingmo.System 1.0

Label {
    // The session the feature needs: "wayland" or "x11"
    property string session: "wayland"

    visible: !Session.supports(session)
    text: Session.onlyInText(session)
    opacity: 0.7
    wrapMode: Text.WordWrap
}
