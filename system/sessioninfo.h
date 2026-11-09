/*
 * Copyright (C) 2026 LingmoOS Team.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 */

#ifndef SESSIONINFO_H
#define SESSIONINFO_H

#include <QObject>
#include <QVariantMap>

// Lingmo.System › Session (QML singleton): which session this app runs in and the
// wording for features that only exist in the other one. Policy (docs/plans/wayland.md):
// new features are Wayland-only; in the X11 session they show "Disponível na sessão
// Wayland" instead of disappearing.
//
//   import Lingmo.System 1.0
//   Session.isWayland                    // XDG_SESSION_TYPE == "wayland"
//   Session.supports("wayland")          // true when the feature can run here
//   Session.onlyInText("wayland")        // "Disponível na sessão Wayland"
//   SessionHint { session: "wayland" }   // that text, shown only where it applies
class SessionInfo : public QObject
{
    Q_OBJECT
    // "wayland", "x11" or "" (tty, unknown)
    Q_PROPERTY(QString type READ type CONSTANT)
    Q_PROPERTY(bool isWayland READ isWayland CONSTANT)
    Q_PROPERTY(bool isX11 READ isX11 CONSTANT)
    // Qt platform of this process ("wayland", "xcb"...): an app in the Wayland
    // session can still run through Xwayland
    Q_PROPERTY(QString platform READ platform CONSTANT)
    // Whether the login screen offers Lingmo (Wayland) on this computer (the GPU
    // driver can run it), and if not, why ("nvidia-modeset", "nvidia-legacy")
    Q_PROPERTY(bool waylandAvailable READ waylandAvailable CONSTANT)
    Q_PROPERTY(QString waylandUnavailableReason READ waylandUnavailableReason CONSTANT)
    Q_PROPERTY(QString waylandUnavailableText READ waylandUnavailableText CONSTANT)
    // The last Wayland start that failed and sent the user back to X11:
    // {reason, started, detected, text}; empty when there was none
    Q_PROPERTY(QVariantMap lastWaylandFailure READ lastWaylandFailure CONSTANT)

public:
    explicit SessionInfo(QObject *parent = nullptr);

    QString type() const { return m_type; }
    bool isWayland() const { return m_type == QLatin1String("wayland"); }
    bool isX11() const { return m_type == QLatin1String("x11"); }
    QString platform() const;

    bool waylandAvailable() const;
    QString waylandUnavailableReason() const;
    QString waylandUnavailableText() const;
    QVariantMap lastWaylandFailure() const;

    // session: "wayland" or "x11". True when this session is that one (or unknown,
    // so nothing gets hidden outside a Lingmo session)
    Q_INVOKABLE bool supports(const QString &session) const;
    // "Disponível na sessão Wayland" / "Available in the Wayland session"
    Q_INVOKABLE QString onlyInText(const QString &session) const;
    // Text for a reason code (lastWaylandFailure.reason, waylandUnavailableReason)
    Q_INVOKABLE QString reasonText(const QString &reason) const;

private:
    QString m_type;
};

#endif // SESSIONINFO_H
