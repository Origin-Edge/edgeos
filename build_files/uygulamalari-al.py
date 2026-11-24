#!/usr/bin/env python3
"""
Origin Edge uygulamalarını (ORBITAL, Nate Browser) Edge App Store kataloğundan alır.
Her paket katalogdaki SHA256 ile doğrulanır, AppImage açılıp /usr/lib/<kimlik> altına konur
(Atomic sistemde /opt imajın parçası olmadığı için).
Kullanım: uygulamalari-al.py <çıktı_kökü>
"""
import hashlib, json, os, pathlib, shutil, subprocess, sys, urllib.request

KATALOG = "https://app.oedge.xyz/katalog.json"
UYGULAMALAR = ["orbital", "nate-browser"]

cikti = pathlib.Path(sys.argv[1])
gecici = pathlib.Path("/tmp/edge-uygulama")
gecici.mkdir(parents=True, exist_ok=True)

def indir(url: str, hedef: pathlib.Path) -> None:
    with urllib.request.urlopen(url, timeout=120) as y, open(hedef, "wb") as f:
        shutil.copyfileobj(y, f)

katalog = json.load(urllib.request.urlopen(KATALOG, timeout=60))
bulunan = {u["id"]: u for u in katalog["uygulamalar"]}

for kimlik in UYGULAMALAR:
    u = bulunan.get(kimlik) or sys.exit(f"{kimlik} katalogda yok")
    p = u["paketler"]["linux"]
    if p["tur"] != "appimage":
        sys.exit(f"{kimlik}: beklenmeyen paket türü {p['tur']}")
    dosya = gecici / f"{kimlik}.AppImage"
    print(f"{kimlik} {u['surum']} indiriliyor…", flush=True)
    indir(p["url"], dosya)
    ozet = hashlib.sha256(dosya.read_bytes()).hexdigest()
    if ozet != p["sha256"]:
        sys.exit(f"{kimlik}: SHA256 tutmadı ({ozet} != {p['sha256']})")
    dosya.chmod(0o755)
    acik = gecici / kimlik
    shutil.rmtree(acik, ignore_errors=True)
    acik.mkdir()
    subprocess.run([str(dosya), "--appimage-extract"], cwd=acik, check=True, stdout=subprocess.DEVNULL)
    hedef = cikti / "usr/lib" / kimlik
    shutil.rmtree(hedef, ignore_errors=True)
    shutil.copytree(acik / "squashfs-root", hedef, symlinks=True)
    (cikti / "usr/share/edgeos").mkdir(parents=True, exist_ok=True)
    (cikti / f"usr/share/edgeos/{kimlik}.surum").write_text(u["surum"] + "\n")
    print(f"{kimlik} {u['surum']} hazır → /usr/lib/{kimlik}", flush=True)
