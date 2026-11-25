#!/usr/bin/env python3
"""EdgeOS renk şemaları ve Plasma global temaları — Origin Edge "Ürün" tasarım dili.

Üretir:
  files/usr/share/color-schemes/EdgeOS.colors, EdgeOSKoyu.colors
  files/usr/share/plasma/look-and-feel/org.originedge.edgeos.desktop       (açık)
  files/usr/share/plasma/look-and-feel/org.originedge.edgeos.koyu.desktop  (koyu)

Çalıştırma: python3 tasarim/tema.py   (depo kökünden)
"""
import json
import shutil
import subprocess
from pathlib import Path

KOK = Path(__file__).resolve().parent.parent
PAYLASIM = KOK / "files/usr/share"
DUVAR = PAYLASIM / "wallpapers/EdgeOS/contents"


def rgb(h):
    h = h.lstrip("#")
    return ",".join(str(int(h[i:i + 2], 16)) for i in (0, 2, 4))


# Tailwind jetonları
ACIK = dict(
    pencere="#f8fafc", pencere2="#f1f5f9",       # slate-50 / slate-100
    yuzey="#ffffff", yuzey2="#f8fafc",           # liste, alan
    dugme="#ffffff", dugme2="#f1f5f9",
    baslik="#ffffff", baslik2="#f8fafc",         # başlık çubuğu, araç çubuğu
    yazi="#0f172a", soluk="#64748b",             # slate-900 / slate-500
    vurgu="#2563eb", vurgu2="#3b82f6",           # blue-600 / blue-500
    baglanti="#2563eb", ziyaret="#4f46e5",
    olumsuz="#dc2626", notr="#d97706", olumlu="#059669",
    secim="#2563eb", secimYazi="#ffffff",
    ipucu="#0f172a", ipucuYazi="#f8fafc",
    tamamlayici="#0f172a", tamamlayici2="#1e293b", tamamlayiciYazi="#f1f5f9", tamamlayiciSoluk="#94a3b8",
    devreDisi="#94a3b8",
)
KOYU = dict(
    pencere="#0b1120", pencere2="#0f172a",
    yuzey="#0f172a", yuzey2="#111c33",
    dugme="#1e293b", dugme2="#253349",
    baslik="#0f172a", baslik2="#0b1120",
    yazi="#e2e8f0", soluk="#94a3b8",
    vurgu="#3b82f6", vurgu2="#60a5fa",
    baglanti="#60a5fa", ziyaret="#818cf8",
    olumsuz="#f87171", notr="#fbbf24", olumlu="#34d399",
    secim="#2563eb", secimYazi="#ffffff",
    ipucu="#1e293b", ipucuYazi="#e2e8f0",
    tamamlayici="#020617", tamamlayici2="#0f172a", tamamlayiciYazi="#e2e8f0", tamamlayiciSoluk="#94a3b8",
    devreDisi="#475569",
)


def grup(ad, zemin, zemin2, yazi, soluk, t):
    return f"""[Colors:{ad}]
BackgroundAlternate={rgb(zemin2)}
BackgroundNormal={rgb(zemin)}
DecorationFocus={rgb(t['vurgu'])}
DecorationHover={rgb(t['vurgu2'])}
ForegroundActive={rgb(t['vurgu'])}
ForegroundInactive={rgb(soluk)}
ForegroundLink={rgb(t['baglanti'])}
ForegroundNegative={rgb(t['olumsuz'])}
ForegroundNeutral={rgb(t['notr'])}
ForegroundNormal={rgb(yazi)}
ForegroundPositive={rgb(t['olumlu'])}
ForegroundVisited={rgb(t['ziyaret'])}

"""


def renk_semasi(ad, t, koyu):
    s = f"""[ColorEffects:Disabled]
Color={rgb(t['devreDisi'])}
ColorAmount=0
ColorEffect=0
ContrastAmount=0.65
ContrastEffect=1
IntensityAmount=0.1
IntensityEffect=2

[ColorEffects:Inactive]
ChangeSelectionColor=true
Color={rgb(t['pencere'])}
ColorAmount=0.025
ColorEffect=2
ContrastAmount=0.1
ContrastEffect=2
Enable=false
IntensityAmount=0
IntensityEffect=0

"""
    s += grup("Button", t["dugme"], t["dugme2"], t["yazi"], t["soluk"], t)
    s += grup("Complementary", t["tamamlayici"], t["tamamlayici2"], t["tamamlayiciYazi"], t["tamamlayiciSoluk"], t)
    s += grup("Header", t["baslik"], t["baslik2"], t["yazi"], t["soluk"], t)
    s += grup("Header][Inactive", t["pencere"], t["pencere2"], t["yazi"], t["soluk"], t)
    s += grup("Selection", t["secim"], t["vurgu2"], t["secimYazi"], "#dbeafe", t)
    s += grup("Tooltip", t["ipucu"], t["tamamlayici2"], t["ipucuYazi"], t["tamamlayiciSoluk"], t)
    s += grup("View", t["yuzey"], t["yuzey2"], t["yazi"], t["soluk"], t)
    s += grup("Window", t["pencere"], t["pencere2"], t["yazi"], t["soluk"], t)
    s += f"""[General]
ColorScheme={ad}
Name={'EdgeOS Koyu' if koyu else 'EdgeOS'}
shadeSortColumn=true

[KDE]
contrast=4

[WM]
activeBackground={rgb(t['baslik'])}
activeBlend={rgb(t['yazi'])}
activeForeground={rgb(t['yazi'])}
inactiveBackground={rgb(t['pencere'])}
inactiveBlend={rgb(t['soluk'])}
inactiveForeground={rgb(t['soluk'])}
"""
    return s


def global_tema(kimlik, ad, sema, simge, koyu):
    kok = PAYLASIM / "plasma/look-and-feel" / kimlik
    if kok.exists():
        shutil.rmtree(kok)
    (kok / "contents/previews").mkdir(parents=True)
    (kok / "metadata.json").write_text(json.dumps({
        "KPackageStructure": "Plasma/LookAndFeel",
        "KPlugin": {
            "Authors": [{"Name": "Origin Edge", "Email": "destek@oedge.xyz"}],
            "Description": f"EdgeOS {'koyu' if koyu else 'açık'} görünüm — Origin Edge Ürün tasarım dili",
            "Id": kimlik,
            "License": "MIT",
            "Name": ad,
            "Website": "https://oedge.xyz",
        },
        "X-Plasma-APIVersion": "2",
    }, ensure_ascii=False, indent=4) + "\n")
    (kok / "contents/defaults").write_text(f"""[kdeglobals][General]
ColorScheme={sema}

[kdeglobals][KDE]
widgetStyle=Breeze

[kdeglobals][Icons]
Theme={simge}

[plasmarc][Theme]
name=default

[kwinrc][org.kde.kdecoration2]
library=org.kde.breeze
theme=Breeze

[kcminputrc][Mouse]
cursorTheme=breeze_cursors

[Wallpaper]
Image=EdgeOS
""")
    kaynak = DUVAR / ("images_dark" if koyu else "images") / "1920x1080.jpg"
    for ad_, en in [("preview.png", 800), ("fullscreenpreview.jpg", 1920)]:
        subprocess.run(["magick", str(kaynak), "-resize", f"{en}x", str(kok / "contents/previews" / ad_)], check=True)


if __name__ == "__main__":
    d = PAYLASIM / "color-schemes"
    d.mkdir(parents=True, exist_ok=True)
    (d / "EdgeOS.colors").write_text(renk_semasi("EdgeOS", ACIK, False))
    (d / "EdgeOSKoyu.colors").write_text(renk_semasi("EdgeOSKoyu", KOYU, True))
    global_tema("org.originedge.edgeos.desktop", "EdgeOS", "EdgeOS", "EdgeOS", False)
    global_tema("org.originedge.edgeos.koyu.desktop", "EdgeOS Koyu", "EdgeOSKoyu", "EdgeOS-Koyu", True)
    print("renk şemaları ve global temalar üretildi")
