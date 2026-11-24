# EdgeOS

Origin Edge'in işletim sistemi. Fedora Kinoite (KDE Plasma, atomik) üstüne Origin Edge katmanı.

## Neden bu temel
- **Atomik güncelleme:** sistem tek parça imaj olarak güncellenir; yarım kalan güncelleme sistemi bozmaz.
- **Geri alma:** sorun çıkarsa `epm geri-al` ile bir önceki sürüme dönülür.
- **Bakım yükü:** çekirdek, sürücüler, firmware ve güvenlik yamaları Fedora'dan gelir; biz yalnız kendi katmanımıza bakarız.
- **Uygulamalar:** Flathub (Flatpak) ve `epm`.

## İçerik
| Parça | Yer |
|---|---|
| Kurulum yardımcısı, EdgeFile (Qt 6) | `src/` |
| `epm` (Flathub + bootc) | `files/usr/bin/epm` |
| ORBITAL, Nate Browser | Derlemede Edge App Store kataloğundan, SHA256 doğrulamalı (`build_files/uygulamalari-al.py`) |
| Tema: renk şeması, simgeler, duvar kâğıdı, Plasma görünümü | `files/usr/share/` |
| Kimlik, Flathub, otomatik güncelleme | `build_files/build.sh` |

## Derleme
```sh
docker build -f Containerfile -t localhost/edgeos:deneme .
# ya da
podman build -t localhost/edgeos:deneme .
```

Kurulum ISO'su (root gerekir):
```sh
sudo podman run --rm --privileged -v ./disk_config/iso.toml:/config.toml:ro -v ./cikti:/output \
  -v /var/lib/containers/storage:/var/lib/containers/storage \
  quay.io/centos-bootc/bootc-image-builder:latest --type anaconda-iso --rootfs btrfs localhost/edgeos:deneme
```

GitHub Actions (`.github/workflows/derle.yml`) her gece ve her push'ta imajı derleyip `ghcr.io/origin-edge/edgeos` adresine gönderir; ISO elle tetiklenir.

## Kurulu sistemde
```sh
epm install vlc          # uygulama kur
epm update               # uygulamalar + sistem
epm sistem               # sürüm ve bekleyen güncelleme
epm geri-al              # önceki EdgeOS sürümüne dön
```
