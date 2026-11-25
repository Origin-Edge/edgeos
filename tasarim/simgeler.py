#!/usr/bin/env python3
"""EdgeOS simge temaları — Origin Edge "Ürün" tasarım dili.

Tek kaynaktan iki tema üretir:
  files/usr/share/icons/EdgeOS       (açık)
  files/usr/share/icons/EdgeOS-Koyu  (koyu)

Uygulama simgeleri: yumuşak renkli karo (açıkta {renk}-50 zemin, {renk}-100 halka, {renk}-600 çizgi;
koyuda {renk}-950 zemin, {renk}-900 halka, {renk}-400 çizgi) ve 24'lük ızgarada 1.8 kalınlıkta çizgi simge.
Klasörler: mavi (blue) dolgu, üstünde beyaz çizgi simge.
Marka simgeleri (ORBITAL, Nate Browser) olduğu gibi kopyalanır.

Çalıştırma: python3 tasarim/simgeler.py   (depo kökünden)
"""
import re
import shutil
from pathlib import Path

KOK = Path(__file__).resolve().parent.parent
HEDEF = KOK / "files/usr/share/icons"
MARKA = KOK / "tasarim/marka-simgeler"
LOGO = KOK / "files/usr/share/pixmaps/origin-edge-mark.svg"

# Tailwind renkleri: 50, 100, 400, 600, 900, 950
TON = {
    "blue":    ("#eff6ff", "#dbeafe", "#60a5fa", "#2563eb", "#1e3a8a", "#172554"),
    "slate":   ("#f8fafc", "#e2e8f0", "#94a3b8", "#475569", "#334155", "#1e293b"),
    "emerald": ("#ecfdf5", "#d1fae5", "#34d399", "#059669", "#064e3b", "#022c22"),
    "amber":   ("#fffbeb", "#fef3c7", "#fbbf24", "#d97706", "#78350f", "#451a03"),
    "orange":  ("#fff7ed", "#ffedd5", "#fb923c", "#ea580c", "#7c2d12", "#431407"),
    "teal":    ("#f0fdfa", "#ccfbf1", "#2dd4bf", "#0d9488", "#134e4a", "#042f2e"),
    "rose":    ("#fff1f2", "#ffe4e6", "#fb7185", "#e11d48", "#881337", "#4c0519"),
    "cyan":    ("#ecfeff", "#cffafe", "#22d3ee", "#0891b2", "#164e63", "#083344"),
    "sky":     ("#f0f9ff", "#e0f2fe", "#38bdf8", "#0284c7", "#0c4a6e", "#082f49"),
    "indigo":  ("#eef2ff", "#e0e7ff", "#818cf8", "#4f46e5", "#312e81", "#1e1b4b"),
    "red":     ("#fef2f2", "#fee2e2", "#f87171", "#dc2626", "#7f1d1d", "#450a0a"),
}

# 24'lük ızgarada çizgi simgeler (Origin Edge için çizildi)
CIZIM = {
    "terminal": '<path d="M5 7l5 5-5 5"/><path d="M12.5 17H19"/>',
    "klasor":   '<path d="M3 7.5A1.5 1.5 0 0 1 4.5 6H9l2 2h8.5A1.5 1.5 0 0 1 21 9.5v8a1.5 1.5 0 0 1-1.5 1.5h-15A1.5 1.5 0 0 1 3 17.5z"/>',
    "ayar":     '<path d="M4 7h8.5M17.5 7H20M4 17h2.5M11.5 17H20"/><circle cx="15" cy="7" r="2.5"/><circle cx="9" cy="17" r="2.5"/>',
    "canta":    '<path d="M5.5 8h13l-1 12h-11z"/><path d="M9 8a3 3 0 0 1 6 0"/>',
    "arsiv":    '<rect x="3" y="4" width="18" height="5" rx="1.2"/><path d="M5 9v9a2 2 0 0 0 2 2h10a2 2 0 0 0 2-2V9"/><path d="M10 13h4"/>',
    "pasta":    '<path d="M20 13.5A8 8 0 1 1 10.5 4"/><path d="M14 3.5a7 7 0 0 1 6.5 6.5H14z"/>',
    "harf":     '<path d="M6 19l6-14 6 14"/><path d="M8.6 13h6.8"/>',
    "bocek":    '<path d="M8 10h8v5a4 4 0 0 1-8 0z"/><path d="M10 10V8a2 2 0 0 1 4 0v2"/><path d="M4 14h4M16 14h4M5 9l3 2M19 9l-3 2M5.5 20l2.8-2M18.5 20l-2.8-2"/>',
    "telefon":  '<rect x="7" y="3" width="10" height="18" rx="2.2"/><path d="M11 17.5h2"/>',
    "ara":      '<circle cx="10.5" cy="10.5" r="6"/><path d="M20 20l-5.2-5.2"/>',
    "yardim":   '<circle cx="12" cy="12" r="9"/><path d="M9.6 9.4a2.5 2.5 0 1 1 3.4 2.4c-.6.3-1 .8-1 1.5v.4"/><path d="M12 17h.01"/>',
    "bilgi":    '<circle cx="12" cy="12" r="9"/><path d="M12 11v5.5"/><path d="M12 7.6h.01"/>',
    "gunluk":   '<path d="M6 3h8.5L18 6.5V21H6z"/><path d="M9 11h6M9 14.5h6M9 18h3.5"/>',
    "menu":     '<path d="M4 6h16M4 12h16M4 18h9"/><path d="M16.5 20.5l4-4"/>',
    "ekran":    '<rect x="3" y="4" width="18" height="12" rx="2"/><path d="M8.5 20h7M12 16v4"/>',
    "anahtar":  '<circle cx="8" cy="15" r="4"/><path d="M11 12l8.5-8.5M16 7l2.5 2.5M18.5 4.5L20 6"/>',
    "kalem":    '<path d="M6 3h8l4 4v14H6z"/><path d="M14 3v4h4"/><path d="M9 12.5h6M9 16h4"/>',
    "disk":     '<rect x="3" y="5" width="18" height="14" rx="2.2"/><path d="M3 12h18"/><path d="M7 15.5h.01M10.5 15.5h.01"/>',
    "nabiz":    '<path d="M3 12h4l2.5-6.5 5 13L17 12h4"/>',
    "gulumse":  '<circle cx="12" cy="12" r="9"/><path d="M8.8 14a3.8 3.8 0 0 0 6.4 0"/><path d="M9 9.5h.01M15 9.5h.01"/>',
    "kamera":   '<path d="M4 8.5h3l1.8-2.5h6.4L17 8.5h3V19H4z"/><circle cx="12" cy="13.2" r="3.3"/>',
    "dunya":    '<circle cx="12" cy="12" r="9"/><path d="M3.5 9h17M3.5 15h17"/><path d="M12 3c2.4 2.5 3.6 5.5 3.6 9s-1.2 6.5-3.6 9c-2.4-2.5-3.6-5.5-3.6-9S9.6 5.5 12 3z"/>',
    "kalkan":   '<path d="M12 3l7 3v5.5c0 4.5-3 7.8-7 9.5-4-1.7-7-5-7-9.5V6z"/><path d="M9 12l2 2 4-4"/>',
    "cop":      '<path d="M4 7h16M9.5 7V4.5h5V7"/><path d="M6 7l1 13h10l1-13"/>',
    "cop-dolu": '<path d="M4 7h16M9.5 7V4.5h5V7"/><path d="M6 7l1 13h10l1-13"/><path d="M10 11v5.5M14 11v5.5"/>',
    "yildiz":   '<path d="M12 3.5l1.7 5.1 5.3.1-4.2 3.2 1.5 5.1-4.3-3-4.3 3 1.5-5.1L5 8.7l5.3-.1z"/>',
    "resim":    '<rect x="3" y="4" width="18" height="16" rx="2"/><circle cx="9" cy="9.5" r="1.8"/><path d="M21 16l-5-5-9 9"/>',
    "hesap":    '<rect x="5" y="3" width="14" height="18" rx="2"/><path d="M8 7h8"/><path d="M8.5 11.5h.01M12 11.5h.01M15.5 11.5h.01M8.5 15h.01M12 15h.01M15.5 15h.01M8.5 18h.01M12 18h3.5"/>',
    "belge":    '<path d="M6 3h8.5L18 6.5V21H6z"/><path d="M14 3v4h4"/>',
    "ev":       '<path d="M4 10.5L12 4l8 6.5V20H4z"/><path d="M9.5 20v-5.5h5V20"/>',
    "indir":    '<path d="M12 4v11"/><path d="M7.5 10.5L12 15l4.5-4.5"/><path d="M5 19.5h14"/>',
    "nota":     '<path d="M9 18V6l10-2v12"/><circle cx="6.5" cy="18" r="2.5"/><circle cx="16.5" cy="16" r="2.5"/>',
    "oynat":    '<path d="M8 5.5v13l10.5-6.5z"/>',
    "paylas":   '<circle cx="17" cy="5.5" r="2.5"/><circle cx="6.5" cy="12" r="2.5"/><circle cx="17" cy="18.5" r="2.5"/><path d="M8.7 10.7l6.1-3.9M8.7 13.3l6.1 3.9"/>',
    "sablon":   '<rect x="4" y="4" width="16" height="16" rx="2"/><path d="M4 9.5h16M9.5 9.5V20"/>',
    "sihir":    '<path d="M12 3v3.5M12 17.5V21M3 12h3.5M17.5 12H21M5.6 5.6l2.5 2.5M15.9 15.9l2.5 2.5M5.6 18.4l2.5-2.5M15.9 8.1l2.5-2.5"/>',
}

# (simge adları, renk, çizim)
UYGULAMALAR = [
    (["utilities-terminal", "org.kde.konsole", "konsole"], "slate", "terminal"),
    (["system-file-manager", "org.kde.dolphin", "edgefile"], "blue", "klasor"),
    (["preferences-system", "systemsettings"], "slate", "ayar"),
    (["plasmadiscover", "org.kde.discover", "system-software-install"], "blue", "canta"),
    (["ark", "org.kde.ark", "utilities-file-archiver"], "amber", "arsiv"),
    (["filelight", "org.kde.filelight"], "orange", "pasta"),
    (["accessories-character-map", "org.kde.kcharselect"], "slate", "harf"),
    (["tools-report-bug", "debug-run"], "rose", "bocek"),
    (["kdeconnect", "org.kde.kdeconnect"], "teal", "telefon"),
    (["kfind", "org.kde.kfind", "system-search"], "slate", "ara"),
    (["help-browser", "org.kde.khelpcenter", "help-contents"], "cyan", "yardim"),
    (["hwinfo", "org.kde.kinfocenter", "kinfocenter"], "indigo", "bilgi"),
    (["org.kde.kjournaldbrowser"], "slate", "gunluk"),
    (["kmenuedit", "org.kde.kmenuedit"], "slate", "menu"),
    (["krfb", "org.kde.krfb", "preferences-desktop-remote-desktop"], "teal", "ekran"),
    (["kwalletmanager", "org.kde.kwalletmanager"], "indigo", "anahtar"),
    (["kwrite", "org.kde.kwrite", "kate", "org.kde.kate", "accessories-text-editor"], "sky", "kalem"),
    (["partitionmanager", "org.kde.partitionmanager", "drive-harddisk"], "slate", "disk"),
    (["utilities-system-monitor", "org.kde.plasma-systemmonitor", "ksysguard"], "emerald", "nabiz"),
    (["preferences-desktop-emoticons", "org.kde.plasma.emojier"], "amber", "gulumse"),
    (["spectacle", "org.kde.spectacle", "applets-screenshooter"], "rose", "kamera"),
    (["firefox", "org.mozilla.firefox", "web-browser"], "orange", "dunya"),
    (["firewall-config", "security-high"], "indigo", "kalkan"),
    (["gwenview", "org.kde.gwenview", "image-viewer"], "emerald", "resim"),
    (["okular", "org.kde.okular"], "red", "belge"),
    (["kcalc", "org.kde.kcalc", "accessories-calculator"], "slate", "hesap"),
    (["edgeos-setup-wizard"], "blue", "sihir"),
]

KLASORLER = [
    (["folder", "inode-directory", "folder-blue"], None),
    (["folder-documents"], "belge"),
    (["folder-download"], "indir"),
    (["folder-music"], "nota"),
    (["folder-pictures"], "resim"),
    (["folder-videos"], "oynat"),
    (["folder-publicshare"], "paylas"),
    (["folder-templates"], "sablon"),
    (["user-home", "folder-home"], "ev"),
    (["user-desktop", "folder-desktop"], "ekran"),
]


def karo(renk, cizim, koyu):
    c50, c100, c400, c600, c900, c950 = TON[renk]
    zemin, halka, cizgi = (c950, c900, c400) if koyu else (c50, c100, c600)
    return (
        '<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 64 64" width="64" height="64">\n'
        f'  <rect x="5" y="5" width="54" height="54" rx="15" fill="{zemin}" stroke="{halka}" stroke-width="2"/>\n'
        f'  <g transform="translate(14 14) scale(1.5)" fill="none" stroke="{cizgi}" stroke-width="1.8" '
        'stroke-linecap="round" stroke-linejoin="round">'
        f'{CIZIM[cizim]}</g>\n</svg>\n'
    )


def klasor(cizim, koyu):
    arka, on, cizgi = ("#1d4ed8", "#3b82f6", "#dbeafe") if koyu else ("#60a5fa", "#3b82f6", "#ffffff")
    ic = ""
    if cizim:
        ic = (f'  <g transform="translate(21.2 25.7) scale(0.9)" fill="none" stroke="{cizgi}" stroke-width="2" '
              f'stroke-linecap="round" stroke-linejoin="round">{CIZIM[cizim]}</g>\n')
    return (
        '<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 64 64" width="64" height="64">\n'
        f'  <path d="M6 15a4 4 0 0 1 4-4h13.5l5 5H54a4 4 0 0 1 4 4v4H6z" fill="{arka}"/>\n'
        f'  <rect x="6" y="19" width="52" height="35" rx="5" fill="{on}"/>\n'
        '  <rect x="6" y="19" width="52" height="1.5" fill="#ffffff" fill-opacity="0.25"/>\n'
        f'{ic}</svg>\n'
    )


def logo_karo():
    """EdgeOS logosu: slate-900 karo (Nate Browser mavi karodan ayrışsın), beyaz Origin Edge işareti (iki temada aynı)."""
    yol = re.search(r'\sd="([^"]+)"', LOGO.read_text()).group(1)
    return (
        '<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 64 64" width="64" height="64">\n'
        '  <rect x="5" y="5" width="54" height="54" rx="15" fill="#0f172a" stroke="#334155" stroke-width="1.5"/>\n'
        '  <g transform="translate(15.5 16) scale(0.3)">'
        f'<path fill="#ffffff" fill-rule="evenodd" d="{yol}"/></g>\n</svg>\n'
    )


def index_theme(ad, koyu):
    miras = "breeze-dark,breeze,hicolor" if koyu else "breeze,hicolor"
    return f"""[Icon Theme]
Name={ad}
Comment=EdgeOS — Origin Edge Ürün tasarım dili ({'koyu' if koyu else 'açık'})
Inherits={miras}
Example=folder
DisplayDepth=32
Directories=apps/scalable,places/scalable

[apps/scalable]
Size=64
Context=Applications
MinSize=16
MaxSize=512
Type=Scalable

[places/scalable]
Size=64
Context=Places
MinSize=16
MaxSize=512
Type=Scalable
"""


def uret(dizin, ad, koyu):
    hedef = HEDEF / dizin
    if hedef.exists():
        shutil.rmtree(hedef)
    apps = hedef / "apps/scalable"
    places = hedef / "places/scalable"
    apps.mkdir(parents=True)
    places.mkdir(parents=True)
    (hedef / "index.theme").write_text(index_theme(ad, koyu))
    sayi = 0
    for adlar, renk, cizim in UYGULAMALAR:
        svg = karo(renk, cizim, koyu)
        for a in adlar:
            (apps / f"{a}.svg").write_text(svg)
            sayi += 1
    logo = logo_karo()
    for a in ["edgeos-logo", "distributor-logo", "start-here", "start-here-kde", "start-here-kde-plasma"]:
        (apps / f"{a}.svg").write_text(logo)
        sayi += 1
    for f in sorted(MARKA.glob("*.svg")):
        shutil.copy(f, apps / f.name)
        sayi += 1
    for adlar, cizim in KLASORLER:
        svg = klasor(cizim, koyu)
        for a in adlar:
            (places / f"{a}.svg").write_text(svg)
            sayi += 1
    for a, cizim in [("user-trash", "cop"), ("user-trash-full", "cop-dolu")]:
        (places / f"{a}.svg").write_text(karo("slate", cizim, koyu))
        sayi += 1
    print(f"{dizin}: {sayi} simge")


if __name__ == "__main__":
    (KOK / "files/usr/share/pixmaps/edgeos-logo.svg").write_text(logo_karo())
    uret("EdgeOS", "EdgeOS", koyu=False)
    uret("EdgeOS-Koyu", "EdgeOS Koyu", koyu=True)
