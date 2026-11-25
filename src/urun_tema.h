// EdgeOS uygulamaları — Origin Edge "Ürün" tasarım dili (slate + blue-600, Inter).
// Kurulum yardımcısı ve EdgeFile aynı temayı kullanır.
#pragma once
#include <QString>
#include <QGuiApplication>
#include <QPalette>
#include <QStyleHints>

// Sistem (Plasma) koyu temada mı? Önce Qt'nin renk şeması ipucu, yoksa pencere renginin parlaklığı.
inline bool sistemKoyu()
{
    const auto cs = QGuiApplication::styleHints()->colorScheme();
    if (cs == Qt::ColorScheme::Dark) return true;
    if (cs == Qt::ColorScheme::Light) return false;
    return QGuiApplication::palette().window().color().lightness() < 128;
}

// Origin Edge işareti: açık zeminde lacivert, koyu zeminde beyaz.
inline QString logoYolu(bool koyu)
{
    return koyu ? QStringLiteral("/usr/share/pixmaps/origin-edge-mark.svg")
                : QStringLiteral("/usr/share/pixmaps/origin-edge-mark-koyu.svg");
}

inline QString urunTema(bool koyu = false)
{
    // Renk jetonları
    const QString zemin   = koyu ? "#0b1120" : "#f8fafc";   // slate-50 / koyu lacivert
    const QString yuzey   = koyu ? "#0f172a" : "#ffffff";   // kart, liste
    const QString kenar   = koyu ? "#0b1120" : "#ffffff";   // kenar çubuğu
    const QString cizgi   = koyu ? "#1e293b" : "#e2e8f0";   // slate-200
    const QString cizgi2  = koyu ? "#334155" : "#cbd5e1";   // slate-300
    const QString yazi    = koyu ? "#e2e8f0" : "#0f172a";   // slate-900
    const QString soluk   = koyu ? "#94a3b8" : "#64748b";   // slate-500
    const QString hafif   = koyu ? "#1e293b" : "#f1f5f9";   // slate-100 (üzerine gelme)
    const QString mavi    = "#2563eb";                       // blue-600
    const QString maviKoyu= "#1d4ed8";                       // blue-700
    const QString maviZem = koyu ? "#172554" : "#eff6ff";   // blue-50
    const QString maviYazi= koyu ? "#93c5fd" : "#1d4ed8";
    const QString maviCiz = koyu ? "#1e3a8a" : "#bfdbfe";   // blue-200
    const QString kirmizi = koyu ? "#f87171" : "#dc2626";
    const QString kirmiziZ= koyu ? "#450a0a" : "#fef2f2";
    const QString yesil   = koyu ? "#4ade80" : "#16a34a";

    QString q = R"(
        QMainWindow, QWidget#rootWidget { background-color: @zemin; color: @yazi;
            font-family: 'Inter', 'Noto Sans', sans-serif; font-size: 14px; }
        QFrame#sidebar { background-color: @kenar; border-right: 1px solid @cizgi; }
        QFrame#card { background-color: @yuzey; border: 1px solid @cizgi; border-radius: 14px; }
        QLabel { color: @yazi; background: transparent; }
        QLabel#wordmark { font-size: 16px; font-weight: 600; color: @yazi; }
        QLabel#pageTitle { font-size: 26px; font-weight: 600; color: @yazi; }
        QLabel#subtitle { font-size: 14px; color: @soluk; }
        QLabel#sectionLabel, QLabel#mutedCaption { font-size: 12px; font-weight: 500; color: @soluk; }
        QLabel#cardTitle { font-size: 15px; font-weight: 600; color: @yazi; }
        QLabel#statFigure { font-family: 'JetBrains Mono', monospace; font-size: 18px; font-weight: 600; color: @mavi; }
        QLabel#monoMeta, QLabel#monoCaption { font-family: 'JetBrains Mono', monospace; font-size: 12px; color: @soluk; }
        QLabel#codeLine { font-family: 'JetBrains Mono', monospace; font-size: 12px; color: @yazi;
            background-color: @hafif; border: 1px solid @cizgi; border-radius: 8px; padding: 6px 10px; }
        QLabel#keycap { font-family: 'JetBrains Mono', monospace; font-size: 11px; color: @maviYazi;
            background-color: @maviZem; border: 1px solid @maviCiz; border-radius: 6px; padding: 2px 7px; }
        QPushButton { background-color: @yuzey; color: @yazi; border: 1px solid @cizgi2; border-radius: 10px;
            padding: 0px 16px; min-height: 36px; font-size: 13px; font-weight: 500; }
        QPushButton:hover { background-color: @hafif; }
        QPushButton:disabled { color: @soluk; border-color: @cizgi; }
        QPushButton#primaryBtn { background-color: @mavi; color: #ffffff; border: 1px solid @mavi; font-weight: 600; }
        QPushButton#primaryBtn:hover { background-color: @maviKoyu; border-color: @maviKoyu; }
        QPushButton#quietBtn { background: transparent; color: @soluk; border: 1px solid @cizgi; border-radius: 8px;
            padding: 0px 10px; font-size: 12px; min-height: 28px; }
        QPushButton#quietBtn:hover { color: @yazi; background-color: @hafif; }
        QPushButton#dangerBtn { background-color: @yuzey; color: @kirmizi; border: 1px solid @cizgi2; }
        QPushButton#dangerBtn:hover { background-color: @kirmiziZ; }
        QPushButton#navBtn { text-align: left; padding: 0px 12px; min-height: 38px; max-height: 38px; border-radius: 10px;
            border: 1px solid transparent; background: transparent; color: @soluk; font-size: 13px; font-weight: 500; }
        QPushButton#navBtn:hover { background-color: @hafif; color: @yazi; }
        QPushButton#navBtn[active="true"] { background-color: @maviZem; border: 1px solid @maviCiz; color: @maviYazi; font-weight: 600; }
        QComboBox, QLineEdit { background-color: @yuzey; color: @yazi; border: 1px solid @cizgi2; border-radius: 10px;
            padding: 0px 12px; min-height: 36px; font-size: 13px; selection-background-color: @mavi; }
        QLineEdit#monoInput { font-family: 'JetBrains Mono', monospace; font-size: 12px; color: @soluk; }
        QComboBox:focus, QLineEdit:focus { border: 2px solid @mavi; }
        QComboBox QAbstractItemView { background-color: @yuzey; color: @yazi; border: 1px solid @cizgi;
            selection-background-color: @maviZem; selection-color: @maviYazi; }
        QCheckBox { color: @soluk; font-size: 13px; spacing: 8px; }
        QCheckBox::indicator { width: 16px; height: 16px; border: 1px solid @cizgi2; border-radius: 5px; background-color: @yuzey; }
        QCheckBox::indicator:hover { border-color: @mavi; }
        QCheckBox::indicator:checked { background-color: @mavi; border-color: @mavi; }
        QTreeView { background-color: @yuzey; color: @yazi; border: 1px solid @cizgi; border-radius: 14px;
            padding: 4px; font-size: 13px; outline: none; }
        QTreeView::item { height: 34px; padding: 0px 8px; border-radius: 8px; }
        QTreeView::item:hover { background-color: @hafif; }
        QTreeView::item:selected { background-color: @maviZem; color: @maviYazi; }
        QHeaderView::section { background-color: @yuzey; color: @soluk; border: none; border-bottom: 1px solid @cizgi;
            padding: 8px 10px; font-size: 12px; font-weight: 500; }
        QPlainTextEdit { background-color: @hafif; color: @yazi; border: 1px solid @cizgi; border-radius: 10px;
            font-family: 'JetBrains Mono', monospace; font-size: 12px; padding: 10px; }
        QProgressBar { background-color: @hafif; border: none; border-radius: 3px; }
        QProgressBar::chunk { background-color: @mavi; border-radius: 3px; }
        QScrollBar:vertical { background: transparent; width: 10px; margin: 2px; }
        QScrollBar::handle:vertical { background: @cizgi2; border-radius: 4px; min-height: 30px; }
        QScrollBar::add-line, QScrollBar::sub-line { height: 0px; }
        QSplitter::handle { background: transparent; }
        QLabel#heroTitle { font-size: 34px; font-weight: 600; color: @yazi; }
        QLabel#strongLabel { color: @yazi; font-weight: 500; }
        QLabel#okText { color: @yesil; font-family: 'JetBrains Mono', monospace; font-size: 12px; }
        QLabel#iconTile { background-color: @maviZem; border: 1px solid @maviCiz; border-radius: 10px; }
        QToolButton#themeCard { background-color: @yuzey; color: @yazi; border: 1px solid @cizgi; border-radius: 14px;
            padding: 10px; font-size: 13px; font-weight: 600; }
        QToolButton#themeCard:hover { border-color: @cizgi2; background-color: @hafif; }
        QToolButton#themeCard[secili="true"] { border: 2px solid @mavi; background-color: @maviZem; color: @maviYazi; }
        QMenu { background-color: @yuzey; color: @yazi; border: 1px solid @cizgi; border-radius: 8px; padding: 4px; }
        QMenu::item { padding: 6px 16px; border-radius: 5px; font-size: 13px; }
        QMenu::item:selected { background-color: @hafif; color: @yazi; }
    )";
    const QList<QPair<QString, QString>> jeton = {
        {"@maviKoyu", maviKoyu}, {"@maviZem", maviZem}, {"@maviYazi", maviYazi}, {"@maviCiz", maviCiz},
        {"@kirmiziZ", kirmiziZ}, {"@kirmizi", kirmizi}, {"@cizgi2", cizgi2}, {"@cizgi", cizgi},
        {"@zemin", zemin}, {"@yuzey", yuzey}, {"@kenar", kenar}, {"@yazi", yazi}, {"@soluk", soluk},
        {"@hafif", hafif}, {"@yesil", yesil}, {"@mavi", mavi}};
    for (const auto &j : jeton) q.replace(j.first, j.second);
    return q;
}
