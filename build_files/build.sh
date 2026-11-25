#!/usr/bin/bash
# EdgeOS imajı: Fedora Kinoite üstüne Origin Edge katmanı.
set -xeuo pipefail

FEDORA_SURUM=$(rpm -E %fedora)
EDGEOS_SURUM="${EDGEOS_SURUM:-1.0}"

# ── Kimlik (ID=fedora kalır: Anaconda, dnf ve araçlar buna bakıyor) ──
sed -i \
  -e "s|^NAME=.*|NAME=\"EdgeOS\"|" \
  -e "s|^PRETTY_NAME=.*|PRETTY_NAME=\"EdgeOS ${EDGEOS_SURUM} (Fedora ${FEDORA_SURUM} tabanlı)\"|" \
  -e "s|^VARIANT=.*|VARIANT=\"EdgeOS\"|" \
  -e "s|^VARIANT_ID=.*|VARIANT_ID=edgeos|" \
  -e "s|^HOME_URL=.*|HOME_URL=\"https://oedge.xyz\"|" \
  -e "s|^SUPPORT_URL=.*|SUPPORT_URL=\"https://oedge.xyz\"|" \
  -e "s|^BUG_REPORT_URL=.*|BUG_REPORT_URL=\"mailto:destek@oedge.xyz\"|" \
  -e "s|^LOGO=.*|LOGO=edgeos-logo|" \
  /usr/lib/os-release
grep -q '^IMAGE_VERSION=' /usr/lib/os-release || echo "IMAGE_VERSION=\"${EDGEOS_SURUM}\"" >> /usr/lib/os-release

# ── Origin Edge uygulamaları (AppImage'dan açıldı, /usr/lib altında) ──
cat > /usr/bin/orbital <<'EOF'
#!/usr/bin/bash
exec /usr/lib/orbital/AppRun "$@"
EOF
cat > /usr/bin/nate-browser <<'EOF'
#!/usr/bin/bash
exec /usr/lib/nate-browser/AppRun "$@"
EOF
chmod 0755 /usr/bin/orbital /usr/bin/nate-browser /usr/bin/epm

# Electron korumalı alanı: --no-sandbox yerine setuid yardımcı
if [ -f /usr/lib/nate-browser/chrome-sandbox ]; then
  chown root:root /usr/lib/nate-browser/chrome-sandbox
  chmod 4755 /usr/lib/nate-browser/chrome-sandbox
fi

# Uygulama kısayolları (AppImage'daki .desktop ve simgeler)
for k in orbital nate-browser; do
  d=$(find /usr/lib/$k -maxdepth 1 -name '*.desktop' | head -1)
  if [ -n "$d" ]; then
    sed -e "s|^Exec=[^ ]*|Exec=/usr/bin/$k|" -e "s|^TryExec=.*||" "$d" > /usr/share/applications/$k.desktop
  fi
  ikon=$(find /usr/lib/$k -maxdepth 1 \( -name '*.png' -o -name '*.svg' \) ! -name '.DirIcon' | head -1)
  [ -n "$ikon" ] && [ ! -e /usr/share/pixmaps/$k.svg ] && install -m644 "$ikon" /usr/share/pixmaps/$k.${ikon##*.}
done
# Masaüstü dosyası adı her AppImage'da farklı olabilir; panelin kullandığı adları sabitle
sed -i 's|^Icon=.*|Icon=orbital|' /usr/share/applications/orbital.desktop
sed -i 's|^Icon=.*|Icon=nate-browser|' /usr/share/applications/nate-browser.desktop
sed -i 's|^Name=.*|Name=ORBITAL|' /usr/share/applications/orbital.desktop
# Korumalı alan açık kalsın (Fedora'da kullanıcı ad alanları var, chrome-sandbox setuid)
sed -i 's| --no-sandbox||g' /usr/share/applications/nate-browser.desktop

# ── Sistem varsayılanları ──
# Fedora'nın sınırlı flatpak deposu yerine Flathub (/etc/flatpak/remotes.d ile gelir)
systemctl disable flatpak-add-fedora-repos.service 2>/dev/null || true
# Otomatik imaj güncellemesi (bootc indirir, sonraki açılışta geçer)
systemctl enable bootc-fetch-apply-updates.timer 2>/dev/null || true
# Varsayılan birim indirdikten sonra hemen yeniden başlatır (--apply); EdgeOS yalnız indirir.
mkdir -p /usr/lib/systemd/system/bootc-fetch-apply-updates.service.d
cat > /usr/lib/systemd/system/bootc-fetch-apply-updates.service.d/10-edgeos.conf <<'EOF'
[Service]
ExecStart=
ExecStart=/usr/bin/bootc upgrade --quiet
EOF

# İlk açılış: KDE'nin kurulum ekranı ve Fedora'nın hoş geldin uygulaması yerine EdgeOS kurulum ekranı
dnf -y remove --noautoremove plasma-setup plasma-welcome plasma-welcome-fedora
touch /etc/plasma-setup-done

# Giriş ekranı (plasma-login-manager) duvar kâğıdı: Fedora yerine EdgeOS
sed -i "s#/usr/share/wallpapers/Fedora/#/usr/share/wallpapers/EdgeOS/#g" /usr/lib/plasmalogin/defaults.conf
grep -q "wallpapers/EdgeOS/" /usr/lib/plasmalogin/defaults.conf

# Saat dilimi İstanbul (dil, klavye, bilgisayar adı files/etc altından gelir)
ln -sf ../usr/share/zoneinfo/Europe/Istanbul /etc/localtime

# Türkçe varsayılanlar (kurulumda kullanıcı değiştirebilir)
dnf -y install langpacks-tr glibc-langpack-tr rsms-inter-fonts jetbrains-mono-fonts-all
dnf clean all

# Simge ve masaüstü önbellekleri
for t in EdgeOS EdgeOS-Koyu; do gtk-update-icon-cache -f /usr/share/icons/$t; done
update-desktop-database /usr/share/applications || true

rm -rf /tmp/* /var/tmp/* /var/cache/* /var/log/* /var/lib/dnf
