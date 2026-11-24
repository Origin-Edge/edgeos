# EdgeOS — Fedora Kinoite (KDE Plasma, atomik) üstüne Origin Edge katmanı.
# Derleme: docker build -t edgeos:deneme .
ARG FEDORA=45

# 1) Kendi Qt uygulamalarımız: kurulum yardımcısı ve EdgeFile
FROM quay.io/fedora/fedora:${FEDORA} AS derleme
RUN dnf -y install --setopt=install_weak_deps=False cmake ninja-build gcc-c++ qt6-qtbase-devel qt6-qtsvg-devel && dnf clean all
COPY src /src
RUN cmake -S /src -B /b -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr \
 && cmake --build /b \
 && DESTDIR=/out cmake --install /b

# 2) ORBITAL ve Nate Browser: Edge App Store kataloğundan, SHA256 doğrulamalı
FROM quay.io/fedora/fedora:${FEDORA} AS uygulamalar
RUN dnf -y install --setopt=install_weak_deps=False python3 && dnf clean all
COPY build_files/uygulamalari-al.py /uygulamalari-al.py
RUN python3 -I /uygulamalari-al.py /out

# 3) EdgeOS imajı
FROM quay.io/fedora/fedora-kinoite:${FEDORA}
ARG EDGEOS_SURUM=1.0
COPY --from=derleme /out/usr /usr
COPY --from=uygulamalar /out/usr /usr
COPY files/ /
COPY build_files/build.sh /tmp/build.sh
RUN EDGEOS_SURUM=${EDGEOS_SURUM} /tmp/build.sh && ostree container commit
RUN bootc container lint
