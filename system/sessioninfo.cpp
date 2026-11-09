/*
 * Copyright (C) 2026 LingmoOS Team.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 */

#include "sessioninfo.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QGuiApplication>
#include <QLocale>

// Written by lingmo-core (lingmo-wayland-check, root, before the login screen)
static const QString s_runDir = QStringLiteral("/run/lingmo-session");

static bool portuguese()
{
    return QLocale().name().startsWith(QLatin1String("pt"));
}

SessionInfo::SessionInfo(QObject *parent)
    : QObject(parent)
    , m_type(qEnvironmentVariable("XDG_SESSION_TYPE").toLower())
{
    if (m_type != QLatin1String("wayland") && m_type != QLatin1String("x11"))
        m_type.clear();
}

QString SessionInfo::platform() const
{
    return QGuiApplication::platformName();
}

bool SessionInfo::waylandAvailable() const
{
    return QFileInfo::exists(s_runDir + QStringLiteral("/wayland-supported"));
}

QString SessionInfo::waylandUnavailableReason() const
{
    QFile file(s_runDir + QStringLiteral("/wayland-unsupported"));
    if (!file.open(QIODevice::ReadOnly))
        return QString();
    return QString::fromUtf8(file.read(64)).trimmed();
}

QString SessionInfo::waylandUnavailableText() const
{
    return reasonText(waylandUnavailableReason());
}

QVariantMap SessionInfo::lastWaylandFailure() const
{
    // Written by lingmo-session (X11) after a Wayland start that never came up
    QString state = qEnvironmentVariable("XDG_STATE_HOME");
    if (state.isEmpty())
        state = QDir::homePath() + QStringLiteral("/.local/state");
    QFile file(state + QStringLiteral("/lingmo-session/wayland-failed"));
    QVariantMap values;
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return values;
    for (const QByteArray &line : file.read(4096).split('\n')) {
        const int eq = line.indexOf('=');
        if (eq > 0)
            values.insert(QString::fromUtf8(line.left(eq)).trimmed(), QString::fromUtf8(line.mid(eq + 1)).trimmed());
    }
    if (!values.isEmpty())
        values.insert(QStringLiteral("text"), reasonText(values.value(QStringLiteral("reason")).toString()));
    return values;
}

bool SessionInfo::supports(const QString &session) const
{
    return m_type.isEmpty() || session.compare(m_type, Qt::CaseInsensitive) == 0;
}

QString SessionInfo::onlyInText(const QString &session) const
{
    const bool wayland = session.compare(QLatin1String("wayland"), Qt::CaseInsensitive) == 0;
    if (portuguese())
        return wayland ? QStringLiteral("Disponível na sessão Wayland") : QStringLiteral("Disponível na sessão X11");
    return wayland ? QStringLiteral("Available in the Wayland session") : QStringLiteral("Available in the X11 session");
}

QString SessionInfo::reasonText(const QString &reason) const
{
    const bool pt = portuguese();
    if (reason.isEmpty())
        return QString();
    if (reason == QLatin1String("nvidia-modeset"))
        return pt ? QStringLiteral("O driver NVIDIA está sem o modeset do kernel (nvidia_drm.modeset=1), que o Wayland precisa.")
                  : QStringLiteral("The NVIDIA driver runs without kernel modesetting (nvidia_drm.modeset=1), which Wayland needs.");
    if (reason == QLatin1String("nvidia-legacy"))
        return pt ? QStringLiteral("Este driver NVIDIA é antigo demais para o Wayland (precisa da versão 495 ou mais nova).")
                  : QStringLiteral("This NVIDIA driver is too old for Wayland (it needs version 495 or newer).");
    if (reason == QLatin1String("shell"))
        return pt ? QStringLiteral("A barra e a área de trabalho não abriram na sessão Wayland.")
                  : QStringLiteral("The bar and the desktop did not come up in the Wayland session.");
    if (reason == QLatin1String("compositor"))
        return pt ? QStringLiteral("O compositor (KWin) não iniciou na sessão Wayland.")
                  : QStringLiteral("The compositor (KWin) did not start in the Wayland session.");
    return pt ? QStringLiteral("A sessão Wayland não abriu.") : QStringLiteral("The Wayland session did not start.");
}
