#include <QApplication>
#include <QMainWindow>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QStackedWidget>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QCheckBox>
#include <QLineEdit>
#include <QFrame>
#include <QProcess>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QScreen>
#include <QGuiApplication>
#include <QSysInfo>
#include <QStyle>
#include <QPainter>
#include <QPixmap>
#include <QSvgRenderer>
#include <QToolButton>
#include <QPainterPath>
#include <QStyleHints>
#include <QTimer>
#include "urun_tema.h"

static QString runCmd(const QString &cmd) {
    QProcess p;
    p.start("/bin/bash", {"-c", cmd});
    p.waitForFinished(4000);
    return QString::fromUtf8(p.readAllStandardOutput()).trimmed();
}

static QIcon lucideIcon(const QString &svgBody, const QString &strokeColor = "#64748b") {
    QString svg = QString(
        "<svg xmlns='http://www.w3.org/2000/svg' width='48' height='48' viewBox='0 0 24 24' "
        "fill='none' stroke='%1' stroke-width='1.75' stroke-linecap='round' stroke-linejoin='round'>%2</svg>"
    ).arg(strokeColor, svgBody);
    QSvgRenderer renderer(svg.toUtf8());
    QPixmap pm(48, 48);
    pm.fill(Qt::transparent);
    QPainter p(&pm);
    renderer.render(&p);
    return QIcon(pm);
}

class EdgeOSSetupWizard : public QMainWindow {
public:
    EdgeOSSetupWizard() {
        setWindowTitle("Kurulum ve sistem rehberi");
        setMinimumSize(960, 640);
        resize(980, 660);

        koyu = sistemKoyu();
        setStyleSheet(urunTema(koyu));

        QWidget *root = new QWidget(this);
        root->setObjectName("rootWidget");
        setCentralWidget(root);

        QHBoxLayout *mainLayout = new QHBoxLayout(root);
        mainLayout->setContentsMargins(0, 0, 0, 0);
        mainLayout->setSpacing(0);

        // Left Sidebar (256px)
        QFrame *sidebar = new QFrame(root);
        sidebar->setObjectName("sidebar");
        sidebar->setFixedWidth(256);
        QVBoxLayout *sideLayout = new QVBoxLayout(sidebar);
        sideLayout->setContentsMargins(16, 20, 16, 16);
        sideLayout->setSpacing(2);

        QHBoxLayout *brandRow = new QHBoxLayout();
        brandRow->setContentsMargins(6, 0, 6, 0);
        brandRow->setSpacing(10);
        logoLbl = new QLabel();
        logoLbl->setFixedSize(26, 26);
        logoCiz();
        QLabel *bTitle = new QLabel("EdgeOS");
        bTitle->setObjectName("wordmark");
        QLabel *bSub = new QLabel("Origin Edge");
        bSub->setObjectName("sectionLabel");
        brandRow->addWidget(logoLbl);
        brandRow->addWidget(bTitle);
        brandRow->addWidget(bSub);
        brandRow->addStretch();
        sideLayout->addLayout(brandRow);

        sideLayout->addSpacing(24);
        QLabel *navHdr = new QLabel("Yapılandırma");
        navHdr->setObjectName("sectionLabel");
        navHdr->setContentsMargins(8, 0, 0, 6);
        sideLayout->addWidget(navHdr);

        struct StepItem { QString label; QString svg; };
        QList<StepItem> steps = {
            {"Hoş geldiniz", "<path d='M7 10v12'/><path d='M15 5.88 14 10h5.83a2 2 0 0 1 1.92 2.56l-2.33 8A2 2 0 0 1 17.5 22H4a2 2 0 0 1-2-2v-8a2 2 0 0 1 2-2h2.76a2 2 0 0 0 1.79-1.11L12 2a3.13 3.13 0 0 1 3 3.88Z'/>"},
            {"Sistem özeti", "<rect width='18' height='18' x='3' y='3' rx='2'/><path d='M3 9h18'/><path d='M9 21V9'/>"},
            {"Zaman, dil ve klavye", "<circle cx='12' cy='12' r='10'/><polyline points='12 6 12 12 16 14'/>"},
            {"Görünüm", "<circle cx='12' cy='12' r='4'/><path d='M12 2v2'/><path d='M12 20v2'/><path d='m4.93 4.93 1.41 1.41'/><path d='m17.66 17.66 1.41 1.41'/><path d='M2 12h2'/><path d='M20 12h2'/>"},
            {"Paket yöneticisi (epm)", "<path d='m7.5 4.27 9 5.15'/><path d='M21 8a2 2 0 0 0-1-1.73l-7-4a2 2 0 0 0-2 0l-7 4A2 2 0 0 0 3 8v8a2 2 0 0 0 1 1.73l7 4a2 2 0 0 0 2 0l7-4A2 2 0 0 0 21 16Z'/><path d='m3.3 7 8.7 5 8.7-5'/><path d='M12 22V12'/>"},
            {"Yerleşik uygulamalar", "<rect width='7' height='7' x='3' y='3' rx='1'/><rect width='7' height='7' x='14' y='3' rx='1'/><rect width='7' height='7' x='14' y='14' rx='1'/><rect width='7' height='7' x='3' y='14' rx='1'/>"},
            {"Ağ ve oturum", "<path d='M12 22s8-4 8-10V5l-8-3-8 3v7c0 6 8 10 8 10'/>"}
        };

        for (int i = 0; i < steps.size(); ++i) {
            QPushButton *btn = new QPushButton(lucideIcon(steps[i].svg), "  " + steps[i].label);
            btn->setObjectName("navBtn");
            btn->setCursor(Qt::PointingHandCursor);
            connect(btn, &QPushButton::clicked, this, [this, i]() { goToPage(i); });
            navButtons.append(btn);
            sideLayout->addWidget(btn);
        }

        sideLayout->addStretch();

        QFrame *sysBadge = new QFrame();
        sysBadge->setObjectName("card");
        QVBoxLayout *sbLay = new QVBoxLayout(sysBadge);
        sbLay->setContentsMargins(14, 12, 14, 12);
        sbLay->setSpacing(4);
        QLabel *sb0 = new QLabel("EdgeOS by Origin Edge");
        sb0->setObjectName("strongLabel");
        sb0->setStyleSheet("font-size: 12px;");
        QLabel *sb1 = new QLabel("Linux " + QSysInfo::kernelVersion().section('-', 0, 0) + " · Fedora");
        sb1->setObjectName("monoMeta");
        QLabel *sb2 = new QLabel("epm · Flathub");
        sb2->setObjectName("sectionLabel");
        sbLay->addWidget(sb0);
        sbLay->addWidget(sb1);
        sbLay->addWidget(sb2);
        sideLayout->addWidget(sysBadge);

        mainLayout->addWidget(sidebar);

        // Right Content Area
        QVBoxLayout *rightLayout = new QVBoxLayout();
        rightLayout->setContentsMargins(32, 28, 32, 24);
        rightLayout->setSpacing(20);

        pages = new QStackedWidget();
        pages->addWidget(createPage0Welcome());
        pages->addWidget(createPage1Overview());
        pages->addWidget(createPage2TimeLangKb());
        pages->addWidget(createPage3Appearance());
        pages->addWidget(createPage4Epm());
        pages->addWidget(createPage5Apps());
        pages->addWidget(createPage6Finish());

        rightLayout->addWidget(pages, 1);

        // Bottom Bar
        QHBoxLayout *bottomBar = new QHBoxLayout();
        autostartCheck = new QCheckBox("Her oturum açılışında göster");
        autostartCheck->setChecked(false);   // ilk açılış ekranı bir kez gösterilir

        prevBtn = new QPushButton("Geri");
        nextBtn = new QPushButton("Devam et");
        nextBtn->setObjectName("primaryBtn");

        connect(prevBtn, &QPushButton::clicked, this, [this]() {
            if (pages->currentIndex() > 0) goToPage(pages->currentIndex() - 1);
        });
        connect(nextBtn, &QPushButton::clicked, this, [this]() {
            if (pages->currentIndex() < pages->count() - 1) {
                goToPage(pages->currentIndex() + 1);
            } else {
                finishWizard();
            }
        });

        bottomBar->addWidget(autostartCheck);
        bottomBar->addStretch();
        bottomBar->addWidget(prevBtn);
        bottomBar->addSpacing(8);
        bottomBar->addWidget(nextBtn);

        rightLayout->addLayout(bottomBar);
        mainLayout->addLayout(rightLayout, 1);

        goToPage(0);

        // Plasma'da açık/koyu değişince kendimizi de güncelle
        connect(QGuiApplication::styleHints(), &QStyleHints::colorSchemeChanged, this, [this]() { temaYenile(); });
    }

protected:
    void changeEvent(QEvent *e) override {
        if (e->type() == QEvent::ApplicationPaletteChange || e->type() == QEvent::ThemeChange)
            QTimer::singleShot(0, this, [this]() { temaYenile(); });
        QMainWindow::changeEvent(e);
    }

private:
    bool koyu = false;
    QLabel *logoLbl = nullptr;
    QList<QToolButton*> temaKartlari;

    void logoCiz() {
        const QString yol = logoYolu(koyu);
        if (!logoLbl || !QFile::exists(yol)) return;
        QSvgRenderer r(yol);
        const qreal dpr = qApp->devicePixelRatio();
        QPixmap pm(int(26 * dpr), int(26 * dpr));
        pm.setDevicePixelRatio(dpr);
        pm.fill(Qt::transparent);
        QPainter p(&pm);
        r.render(&p, QRectF(0, 0, 26, 26));
        logoLbl->setPixmap(pm);
    }

    void temaYenile() {
        const bool k = sistemKoyu();
        if (k == koyu) return;
        koyu = k;
        setStyleSheet(urunTema(koyu));
        logoCiz();
    }

    // Şu an seçili EdgeOS görünümü: "acik", "koyu" ya da "otomatik"
    static QString seciliGorunum() {
        if (runCmd("kreadconfig6 --file kdeglobals --group KDE --key AutomaticLookAndFeel") == "true") return "otomatik";
        const QString lnf = runCmd("kreadconfig6 --file kdeglobals --group KDE --key LookAndFeelPackage");
        return lnf.endsWith(".koyu.desktop") ? "koyu" : "acik";
    }

    void temaKartiSec(const QString &id) {
        for (auto *k : temaKartlari) {
            k->setProperty("secili", k->property("gorunum").toString() == id ? "true" : "false");
            k->style()->unpolish(k);
            k->style()->polish(k);
        }
    }
    QStackedWidget *pages;
    QList<QPushButton*> navButtons;
    QPushButton *prevBtn;
    QPushButton *nextBtn;
    QCheckBox *autostartCheck;

    void goToPage(int idx) {
        pages->setCurrentIndex(idx);
        for (int i = 0; i < navButtons.size(); ++i) {
            navButtons[i]->setProperty("active", i == idx ? "true" : "false");
            navButtons[i]->style()->unpolish(navButtons[i]);
            navButtons[i]->style()->polish(navButtons[i]);
        }
        prevBtn->setEnabled(idx > 0);
        nextBtn->setText(idx == 0 ? "Başlayalım" : (idx == pages->count() - 1 ? "Kurulumu tamamla" : "Devam et"));
    }

    QWidget* createPage0Welcome() {
        QWidget *w = new QWidget();
        QVBoxLayout *lay = new QVBoxLayout(w);
        lay->setContentsMargins(0, 0, 0, 0);
        lay->setSpacing(0);
        lay->addStretch(2);

        QLabel *logo = new QLabel();
        logo->setFixedSize(96, 96);
        if (QFile::exists("/usr/share/pixmaps/edgeos-logo.svg")) {
            QSvgRenderer r(QStringLiteral("/usr/share/pixmaps/edgeos-logo.svg"));
            const qreal dpr = qApp->devicePixelRatio();
            QPixmap pm(int(96 * dpr), int(96 * dpr));
            pm.setDevicePixelRatio(dpr);
            pm.fill(Qt::transparent);
            QPainter p(&pm);
            r.render(&p, QRectF(0, 0, 96, 96));
            logo->setPixmap(pm);
        }
        lay->addWidget(logo, 0, Qt::AlignHCenter);
        lay->addSpacing(28);

        QLabel *title = new QLabel("EdgeOS'a hoş geldiniz");
        title->setAlignment(Qt::AlignCenter);
        title->setObjectName("heroTitle");
        title->setStyleSheet("letter-spacing: -0.6px;");
        lay->addWidget(title);
        lay->addSpacing(12);

        QLabel *sub = new QLabel("Birkaç adımda saat, dil, klavye ve görünümü ayarlayalım. "
                                 "Sonra ORBITAL, Nate Browser ve EdgeFile kullanıma hazır.");
        sub->setObjectName("subtitle");
        sub->setAlignment(Qt::AlignCenter);
        sub->setWordWrap(true);
        sub->setFixedWidth(560);
        sub->setStyleSheet("font-size: 15px;");
        sub->setMinimumHeight(52);   // iki satır (15 px) kesilmesin
        sub->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        lay->addWidget(sub, 0, Qt::AlignHCenter);
        lay->addSpacing(32);

        QHBoxLayout *ozellik = new QHBoxLayout();
        ozellik->setSpacing(12);
        auto kart = [&](const QString &svg, const QString &b, const QString &a) {
            QFrame *c = new QFrame();
            c->setObjectName("card");
            c->setFixedWidth(200);
            QVBoxLayout *v = new QVBoxLayout(c);
            v->setContentsMargins(16, 14, 16, 14);
            v->setSpacing(4);
            QLabel *tile = new QLabel();
            tile->setFixedSize(36, 36);
            tile->setAlignment(Qt::AlignCenter);
            tile->setObjectName("iconTile");
            tile->setPixmap(lucideIcon(svg, koyu ? "#93c5fd" : "#2563eb").pixmap(18, 18));
            v->addWidget(tile);
            v->addSpacing(6);
            QLabel *l1 = new QLabel(b);
            l1->setObjectName("strongLabel");
            l1->setStyleSheet("font-size: 14px; font-weight: 600;");
            QLabel *l2 = new QLabel(a);
            l2->setObjectName("subtitle");
            l2->setWordWrap(true);
            l1->setAlignment(Qt::AlignLeft | Qt::AlignTop);
            l2->setAlignment(Qt::AlignLeft | Qt::AlignTop);
            v->addWidget(l1);
            v->addWidget(l2);
            v->addStretch();
            c->setMinimumHeight(150);
            ozellik->addWidget(c);
        };
        ozellik->addStretch();
        kart("<path d='M21 12a9 9 0 1 1-9-9c2.52 0 4.93 1 6.74 2.74L21 8'/><path d='M21 3v5h-5'/>", "Kendini günceller", "Yeni sürüm arka planda iner, sonraki açılışta geçer.");
        kart("<path d='M9 14 4 9l5-5'/><path d='M4 9h10.5a5.5 5.5 0 0 1 0 11H11'/>", "Geri alınabilir", "Sorun çıkarsa tek komutla önceki sürüme dönülür.");
        kart("<rect width='7' height='7' x='3' y='3' rx='1'/><rect width='7' height='7' x='14' y='3' rx='1'/><rect width='7' height='7' x='14' y='14' rx='1'/><rect width='7' height='7' x='3' y='14' rx='1'/>", "Binlerce uygulama", "epm ile Flathub'daki uygulamaları kurun.");
        ozellik->addStretch();
        lay->addLayout(ozellik);
        lay->addStretch(3);
        return w;
    }

    QWidget* createPage1Overview() {
        QWidget *w = new QWidget();
        QVBoxLayout *lay = new QVBoxLayout(w);
        lay->setContentsMargins(0, 0, 0, 0);
        lay->setSpacing(20);

        QHBoxLayout *heroRow = new QHBoxLayout();
        heroRow->setSpacing(16);
        QLabel *bigLogo = new QLabel();
        bigLogo->setFixedSize(52, 52);
        if (QFile::exists("/usr/share/pixmaps/edgeos-logo.svg")) {
            QSvgRenderer r(QStringLiteral("/usr/share/pixmaps/edgeos-logo.svg"));
            QPixmap pm(52, 52);
            pm.fill(Qt::transparent);
            QPainter p(&pm);
            r.render(&p);
            bigLogo->setPixmap(pm);
        }
        QVBoxLayout *hdr = new QVBoxLayout();
        hdr->setSpacing(4);
        QLabel *title = new QLabel("EdgeOS by Origin Edge");
        title->setObjectName("pageTitle");
        QLabel *sub = new QLabel("Fedora Atomic temelli EdgeOS sisteminizin donanım, paket yöneticisi (epm) ve masaüstü durumu");
        sub->setObjectName("subtitle");
        sub->setWordWrap(true);
        hdr->addWidget(title);
        hdr->addWidget(sub);
        heroRow->addWidget(bigLogo);
        heroRow->addLayout(hdr, 1);
        lay->addLayout(heroRow);

        QHBoxLayout *statRow = new QHBoxLayout();
        statRow->setSpacing(12);

        QString ramTotal = runCmd("awk '/MemTotal/ {printf \"%.1f GB\", $2/1024/1024}' /proc/meminfo");
        QString diskRoot = runCmd("df -h / | awk 'NR==2 {print $3 \" / \" $2}'");
        QString ipAddr = runCmd("ip -4 -br addr show scope global | awk '{print $3}' | head -n1");
        if (ipAddr.isEmpty()) ipAddr = "10.0.2.15/24";

        auto makeStatCard = [](const QString &label, const QString &figure, const QString &meta) {
            QFrame *c = new QFrame();
            c->setObjectName("card");
            QVBoxLayout *vl = new QVBoxLayout(c);
            vl->setContentsMargins(20, 16, 20, 16);
            vl->setSpacing(6);
            QLabel *l1 = new QLabel(label);
            l1->setObjectName("sectionLabel");
            QLabel *l2 = new QLabel(figure);
            l2->setObjectName("statFigure");
            QLabel *l3 = new QLabel(meta);
            l3->setObjectName("monoMeta");
            vl->addWidget(l1);
            vl->addWidget(l2);
            vl->addWidget(l3);
            return c;
        };

        statRow->addWidget(makeStatCard("Çekirdek", "Linux " + QSysInfo::kernelVersion(), QSysInfo::prettyProductName()));
        statRow->addWidget(makeStatCard("Bellek ve depolama", ramTotal, "Kök dizin: " + diskRoot));
        statRow->addWidget(makeStatCard("Paket yöneticisi", "epm 2.0", "Flathub · bootc"));
        lay->addLayout(statRow);

        QFrame *tableCard = new QFrame();
        tableCard->setObjectName("card");
        QVBoxLayout *tLay = new QVBoxLayout(tableCard);
        tLay->setContentsMargins(20, 20, 20, 20);
        tLay->setSpacing(12);

        QLabel *tTitle = new QLabel("Sistem bileşenleri");
        tTitle->setObjectName("cardTitle");
        tLay->addWidget(tTitle);

        QGridLayout *grid = new QGridLayout();
        grid->setHorizontalSpacing(24);
        grid->setVerticalSpacing(10);

        auto addRow = [&](int r, const QString &k, const QString &v, const QString &tag) {
            QLabel *kl = new QLabel(k);
            kl->setObjectName("subtitle");
            QLabel *vl = new QLabel(v);
            vl->setObjectName("strongLabel");
            vl->setStyleSheet("font-size: 13px;");
            QLabel *tl = new QLabel(tag);
            tl->setObjectName("keycap");
            grid->addWidget(kl, r, 0);
            grid->addWidget(vl, r, 1);
            grid->addWidget(tl, r, 2, Qt::AlignRight);
        };

        addRow(0, "Dağıtım kimliği", "EdgeOS by Origin Edge", "Fedora Atomic");
        addRow(1, "Güncelleme", "Atomik imaj: otomatik güncelleme, tek adımda geri alma", "bootc");
        addRow(2, "Paket yöneticisi", "epm install (Flathub)", "epm");
        addRow(3, "Öntanımlı uygulamalar", "ORBITAL · Nate Browser · EdgeFile", "MIME");

        grid->setColumnStretch(1, 1);
        tLay->addLayout(grid);

        QHBoxLayout *actRow = new QHBoxLayout();
        QPushButton *openInfoBtn = new QPushButton("Sistem bilgisini aç");
        QPushButton *openEpmBtn = new QPushButton("epm paket yöneticisi rehberi");
        openEpmBtn->setObjectName("primaryBtn");
        connect(openInfoBtn, &QPushButton::clicked, this, []() {
            QProcess::startDetached("/usr/bin/kinfocenter", {});
        });
        connect(openEpmBtn, &QPushButton::clicked, this, [this]() {
            goToPage(3);
        });
        actRow->addWidget(openEpmBtn);
        actRow->addWidget(openInfoBtn);
        actRow->addStretch();
        tLay->addLayout(actRow);

        lay->addWidget(tableCard);
        lay->addStretch();
        return w;
    }

    QWidget* createPage2TimeLangKb() {
        QWidget *w = new QWidget();
        QVBoxLayout *lay = new QVBoxLayout(w);
        lay->setContentsMargins(0, 0, 0, 0);
        lay->setSpacing(16);

        QVBoxLayout *hdr = new QVBoxLayout();
        hdr->setSpacing(6);
        QLabel *title = new QLabel("Zaman, dil ve klavye");
        title->setObjectName("pageTitle");
        QLabel *sub = new QLabel("Değişiklikler anında çalışan oturuma ve sistem yapılandırmasına uygulanır");
        sub->setObjectName("subtitle");
        hdr->addWidget(title);
        hdr->addWidget(sub);
        lay->addLayout(hdr);

        QLabel *feedbackLbl = new QLabel("");
        feedbackLbl->setObjectName("okText");

        QFrame *tzCard = new QFrame();
        tzCard->setObjectName("card");
        QVBoxLayout *tzLay = new QVBoxLayout(tzCard);
        tzLay->setContentsMargins(20, 16, 20, 16);
        tzLay->setSpacing(10);

        QLabel *tzTitle = new QLabel("Saat dilimi");
        tzTitle->setObjectName("cardTitle");
        tzLay->addWidget(tzTitle);

        QHBoxLayout *tzRow = new QHBoxLayout();
        tzRow->setSpacing(8);
        QComboBox *tzCombo = new QComboBox();
        tzCombo->addItem("Europe/Istanbul (UTC+03:00)", "Europe/Istanbul");
        tzCombo->addItem("Europe/Berlin (CET)", "Europe/Berlin");
        tzCombo->addItem("Europe/London (GMT)", "Europe/London");
        tzCombo->addItem("UTC", "UTC");
        tzCombo->addItem("America/New_York (EST)", "America/New_York");

        QPushButton *tzBtn = new QPushButton("Uygula");
        tzBtn->setObjectName("primaryBtn");
        connect(tzBtn, &QPushButton::clicked, this, [tzCombo, feedbackLbl]() {
            QString tz = tzCombo->currentData().toString();
            QProcess::execute("/usr/bin/timedatectl", {"set-timezone", tz});   // yetkiyi polkit sorar
            QString js = QString(
                "var ps = panels(); for (var i=0;i<ps.length;i++){ var ws=ps[i].widgets(); for(var j=0;j<ws.length;j++){ if(ws[j].type==='org.kde.plasma.digitalclock'){ ws[j].currentConfigGroup=['Appearance']; ws[j].writeConfig('lastSelectedTimezone','%1'); ws[j].writeConfig('selectedTimeZones',['%1','Local']); }}}"
            ).arg(tz);
            runCmd(QString("dbus-send --session --dest=org.kde.plasmashell --type=method_call --print-reply /PlasmaShell org.kde.PlasmaShell.evaluateScript \"string:%1\" >/dev/null 2>&1 || true").arg(js));
            QString curTime = runCmd(QString("TZ='%1' date '+%H:%M:%S (%Z)'").arg(tz));
            feedbackLbl->setText("Saat dilimi güncellendi: " + tz + " — " + curTime);
        });
        tzRow->addWidget(tzCombo, 1);
        tzRow->addWidget(tzBtn);
        tzLay->addLayout(tzRow);
        lay->addWidget(tzCard);

        QFrame *langCard = new QFrame();
        langCard->setObjectName("card");
        QVBoxLayout *langLay = new QVBoxLayout(langCard);
        langLay->setContentsMargins(20, 16, 20, 16);
        langLay->setSpacing(10);

        QLabel *langTitle = new QLabel("Sistem ve arayüz dili");
        langTitle->setObjectName("cardTitle");
        langLay->addWidget(langTitle);

        QHBoxLayout *langRow = new QHBoxLayout();
        langRow->setSpacing(8);
        QComboBox *langCombo = new QComboBox();
        langCombo->addItem("Türkçe (tr_TR.UTF-8)", "tr_TR.UTF-8:tr");
        langCombo->addItem("English (en_US.UTF-8)", "en_US.UTF-8:en_US");

        QPushButton *langBtn = new QPushButton("Uygula");
        langBtn->setObjectName("primaryBtn");
        connect(langBtn, &QPushButton::clicked, this, [langCombo, feedbackLbl]() {
            QStringList parts = langCombo->currentData().toString().split(":");
            QString loc = parts[0];
            QString kdeLang = parts[1];
            QProcess::execute("/usr/bin/localectl", {"set-locale", "LANG=" + loc});   // yetkiyi polkit sorar
            runCmd(QString("kwriteconfig6 --file plasma-localerc --group Formats --key LANG '%1' && kwriteconfig6 --file plasma-localerc --group Translations --key LANGUAGE '%2'").arg(loc, kdeLang));
            runCmd(QString("kwriteconfig6 --file kdeglobals --group Locale --key Language '%1'").arg(kdeLang));
            feedbackLbl->setText("Sistem dili güncellendi: " + loc);
        });
        langRow->addWidget(langCombo, 1);
        langRow->addWidget(langBtn);
        langLay->addLayout(langRow);
        lay->addWidget(langCard);

        QFrame *kbCard = new QFrame();
        kbCard->setObjectName("card");
        QVBoxLayout *kbLay = new QVBoxLayout(kbCard);
        kbLay->setContentsMargins(20, 16, 20, 16);
        kbLay->setSpacing(10);

        QLabel *kbTitle = new QLabel("Klavye düzeni");
        kbTitle->setObjectName("cardTitle");
        kbLay->addWidget(kbTitle);

        QHBoxLayout *kbRow = new QHBoxLayout();
        kbRow->setSpacing(8);
        QComboBox *kbCombo = new QComboBox();
        kbCombo->addItem("Türkçe Q (tr)", "tr:");
        kbCombo->addItem("Türkçe F (tr - f)", "tr:f");
        kbCombo->addItem("English US (us)", "us:");

        QPushButton *kbBtn = new QPushButton("Uygula");
        kbBtn->setObjectName("primaryBtn");
        connect(kbBtn, &QPushButton::clicked, this, [kbCombo, feedbackLbl]() {
            QStringList parts = kbCombo->currentData().toString().split(":");
            QString layout = parts[0];
            QString variant = parts.size() > 1 ? parts[1] : "";
            runCmd(QString("kwriteconfig6 --file kxkbrc --group Layout --key LayoutList '%1' && kwriteconfig6 --file kxkbrc --group Layout --key VariantList '%2' && kwriteconfig6 --file kxkbrc --group Layout --key Use true && kwriteconfig6 --file kxkbrc --group Layout --key ResetOldOptions true").arg(layout, variant));
            QProcess::startDetached("/usr/bin/localectl", {"set-x11-keymap", layout, "pc105", variant});   // sistem geneli (giriş ekranı); yetkiyi polkit sorar
            runCmd("dbus-send --session --type=signal /Layouts org.kde.keyboard.reloadConfig 2>/dev/null || true");
            runCmd("dbus-send --session --type=signal --dest=org.kde.KWin /Layouts org.kde.keyboard.reloadConfig 2>/dev/null || true");
            feedbackLbl->setText("Klavye düzeni etkinleştirildi: " + kbCombo->currentText());
        });
        kbRow->addWidget(kbCombo, 1);
        kbRow->addWidget(kbBtn);
        kbLay->addLayout(kbRow);
        lay->addWidget(kbCard);

        lay->addWidget(feedbackLbl);
        lay->addStretch();
        return w;
    }

    QWidget* createPage3Appearance() {
        QWidget *w = new QWidget();
        QVBoxLayout *lay = new QVBoxLayout(w);
        lay->setContentsMargins(0, 0, 0, 0);
        lay->setSpacing(16);

        QVBoxLayout *hdr = new QVBoxLayout();
        hdr->setSpacing(6);
        QLabel *title = new QLabel("Görünüm");
        title->setObjectName("pageTitle");
        QLabel *sub = new QLabel("Açık ya da koyu görünümü seçin. Renkler, simgeler, panel ve duvar kâğıdı birlikte değişir.");
        sub->setObjectName("subtitle");
        sub->setWordWrap(true);
        hdr->addWidget(title);
        hdr->addWidget(sub);
        lay->addLayout(hdr);

        QLabel *durum = new QLabel("");
        durum->setObjectName("okText");

        struct Secenek { QString id, ad, onizleme; };
        const QList<Secenek> secenekler = {
            {"acik", "Açık", "/usr/share/plasma/look-and-feel/org.originedge.edgeos.desktop/contents/previews/preview.png"},
            {"koyu", "Koyu", "/usr/share/plasma/look-and-feel/org.originedge.edgeos.koyu.desktop/contents/previews/preview.png"},
            {"otomatik", "Otomatik", ""},
        };

        QHBoxLayout *kartlar = new QHBoxLayout();
        kartlar->setSpacing(12);
        for (const auto &sc : secenekler) {
            QToolButton *k = new QToolButton();
            k->setObjectName("themeCard");
            k->setProperty("gorunum", sc.id);
            k->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
            k->setCursor(Qt::PointingHandCursor);
            k->setText(sc.ad);
            k->setIconSize(QSize(200, 113));
            k->setMinimumWidth(224);

            // Önizleme: otomatik için yarı açık, yarı koyu
            const qreal dpr = qApp->devicePixelRatio();
            QPixmap pm(int(200 * dpr), int(113 * dpr));
            pm.setDevicePixelRatio(dpr);
            pm.fill(Qt::transparent);
            {
                QPainter p(&pm);
                p.setRenderHint(QPainter::Antialiasing);
                p.setRenderHint(QPainter::SmoothPixmapTransform);
                QPainterPath yol;
                yol.addRoundedRect(QRectF(0, 0, 200, 113), 9, 9);
                p.setClipPath(yol);
                const QRectF tam(0, 0, 200, 113);
                if (sc.id != "otomatik") {
                    p.drawPixmap(tam, QPixmap(sc.onizleme), QRectF());
                } else {
                    QPixmap a(secenekler[0].onizleme), b(secenekler[1].onizleme);
                    p.drawPixmap(tam, a, QRectF());
                    QPainterPath sag;
                    sag.moveTo(120, 0); sag.lineTo(200, 0); sag.lineTo(200, 113); sag.lineTo(80, 113); sag.closeSubpath();
                    p.setClipPath(sag, Qt::IntersectClip);
                    p.drawPixmap(tam, b, QRectF());
                }
            }
            k->setIcon(QIcon(pm));

            connect(k, &QToolButton::clicked, this, [this, id = sc.id, durum]() {
                if (id == "otomatik") {
                    runCmd("kwriteconfig6 --notify --file kdeglobals --group KDE --key AutomaticLookAndFeel true");
                    durum->setText("Görünüm gün ışığına göre kendiliğinden açık ya da koyu olacak");
                } else {
                    runCmd("kwriteconfig6 --notify --file kdeglobals --group KDE --key AutomaticLookAndFeel false");
                    QProcess::startDetached("/usr/bin/plasma-apply-lookandfeel",
                        {"-a", id == "koyu" ? "org.originedge.edgeos.koyu.desktop" : "org.originedge.edgeos.desktop"});
                    durum->setText(id == "koyu" ? "Koyu görünüm uygulandı" : "Açık görünüm uygulandı");
                }
                temaKartiSec(id);
            });
            temaKartlari.append(k);
            kartlar->addWidget(k);
        }
        kartlar->addStretch();
        lay->addLayout(kartlar);
        temaKartiSec(seciliGorunum());

        QFrame *ekCard = new QFrame();
        ekCard->setObjectName("card");
        QVBoxLayout *ekLay = new QVBoxLayout(ekCard);
        ekLay->setContentsMargins(20, 16, 20, 16);
        ekLay->setSpacing(10);
        QLabel *ekTitle = new QLabel("Daha fazla ayar");
        ekTitle->setObjectName("cardTitle");
        ekLay->addWidget(ekTitle);
        QHBoxLayout *ekBtns = new QHBoxLayout();
        ekBtns->setSpacing(8);
        QPushButton *duvarBtn = new QPushButton("Duvar kâğıdı");
        QPushButton *ekranBtn = new QPushButton("Ekran ve çözünürlük");
        QPushButton *geceBtn = new QPushButton("Gece ışığı");
        connect(duvarBtn, &QPushButton::clicked, this, []() {
            QProcess::startDetached("/usr/bin/systemsettings", {"kcm_wallpaper"});
        });
        connect(ekranBtn, &QPushButton::clicked, this, []() {
            QProcess::startDetached("/usr/bin/systemsettings", {"kcm_kscreen"});
        });
        connect(geceBtn, &QPushButton::clicked, this, []() {
            QProcess::startDetached("/usr/bin/systemsettings", {"kcm_nightlight"});
        });
        ekBtns->addWidget(duvarBtn);
        ekBtns->addWidget(ekranBtn);
        ekBtns->addWidget(geceBtn);
        ekBtns->addStretch();
        ekLay->addLayout(ekBtns);
        lay->addWidget(ekCard);

        lay->addWidget(durum);
        lay->addStretch();
        return w;
    }

    QWidget* createPage4Epm() {
        QWidget *w = new QWidget();
        QVBoxLayout *lay = new QVBoxLayout(w);
        lay->setContentsMargins(0, 0, 0, 0);
        lay->setSpacing(16);

        QVBoxLayout *hdr = new QVBoxLayout();
        hdr->setSpacing(6);
        QLabel *title = new QLabel("Edge Package Manager (epm)");
        title->setObjectName("pageTitle");
        QLabel *sub = new QLabel("EdgeOS'ta uygulamalar Flathub'dan gelir; sistem tek parça imaj olarak güncellenir. Hepsi epm komutuyla yönetilir");
        sub->setObjectName("subtitle");
        sub->setWordWrap(true);
        hdr->addWidget(title);
        hdr->addWidget(sub);
        lay->addLayout(hdr);

        // Quick Interactive Installer Card
        QFrame *instCard = new QFrame();
        instCard->setObjectName("card");
        QVBoxLayout *instLay = new QVBoxLayout(instCard);
        instLay->setContentsMargins(20, 16, 20, 16);
        instLay->setSpacing(10);

        QLabel *instTitle = new QLabel("Hızlı paket kurulumu");
        instTitle->setObjectName("cardTitle");
        instLay->addWidget(instTitle);

        QHBoxLayout *instRow = new QHBoxLayout();
        instRow->setSpacing(8);
        QLineEdit *pkgInput = new QLineEdit();
        pkgInput->setPlaceholderText("Paket adı girin (ör. vlc, firefox, code, spotify)");
        QPushButton *epmInstallBtn = new QPushButton("epm install ile kur");
        epmInstallBtn->setObjectName("primaryBtn");
        connect(epmInstallBtn, &QPushButton::clicked, this, [pkgInput]() {
            QString p = pkgInput->text().trimmed();
            if (p.isEmpty()) p = "vlc";
            QProcess::startDetached("/usr/bin/konsole", {"-e", "/bin/bash", "-c", QString("epm install %1; echo ''; read -p 'Kapatmak için Enter tuşuna basın...'").arg(p)});
        });
        instRow->addWidget(pkgInput, 1);
        instRow->addWidget(epmInstallBtn);
        instLay->addLayout(instRow);
        lay->addWidget(instCard);

        // Command Reference Card
        QFrame *refCard = new QFrame();
        refCard->setObjectName("card");
        QVBoxLayout *refLay = new QVBoxLayout(refCard);
        refLay->setContentsMargins(20, 18, 20, 18);
        refLay->setSpacing(10);

        QLabel *refTitle = new QLabel("epm komut rehberi");
        refTitle->setObjectName("cardTitle");
        refLay->addWidget(refTitle);

        QGridLayout *cmdGrid = new QGridLayout();
        cmdGrid->setHorizontalSpacing(16);
        cmdGrid->setVerticalSpacing(8);

        auto addCmd = [&](int r, const QString &code, const QString &desc) {
            QLabel *cl = new QLabel(code);
            cl->setObjectName("codeLine");
            QLabel *dl = new QLabel(desc);
            dl->setObjectName("subtitle");
            cmdGrid->addWidget(cl, r, 0);
            cmdGrid->addWidget(dl, r, 1);
        };

        addCmd(0, "epm install <uygulama>", "Uygulamayı Flathub'dan kurar (ör. epm install vlc)");
        addCmd(1, "epm search <sorgu>", "Flathub'da uygulama arar");
        addCmd(2, "epm list", "Kurulu uygulamaları listeler");
        addCmd(3, "epm remove <uygulama>", "Uygulamayı kaldırır");
        addCmd(4, "epm update", "Uygulamaları ve sistemi günceller");
        addCmd(5, "epm sistem-guncelle", "Yeni EdgeOS sürümünü indirir; yeniden başlatınca geçer");
        addCmd(6, "epm geri-al", "Bir önceki EdgeOS sürümüne döner");

        cmdGrid->setColumnStretch(1, 1);
        refLay->addLayout(cmdGrid);
        lay->addWidget(refCard);

        lay->addStretch();
        return w;
    }

    QWidget* createPage5Apps() {
        QWidget *w = new QWidget();
        QVBoxLayout *lay = new QVBoxLayout(w);
        lay->setContentsMargins(0, 0, 0, 0);
        lay->setSpacing(14);

        QVBoxLayout *hdr = new QVBoxLayout();
        hdr->setSpacing(6);
        QLabel *title = new QLabel("Yerleşik uygulamalar ve dosya eşleşmeleri");
        title->setObjectName("pageTitle");
        QLabel *sub = new QLabel("ORBITAL, Nate Browser ve EdgeFile masaüstünde ve görev çubuğunda hazır gelir");
        sub->setObjectName("subtitle");
        hdr->addWidget(title);
        hdr->addWidget(sub);
        lay->addLayout(hdr);

        auto makeAppCard = [this](const QString &name, const QString &ver, const QString &desc, const QString &btnText, const QString &execPath) {
            QFrame *c = new QFrame();
            c->setObjectName("card");
            QVBoxLayout *vl = new QVBoxLayout(c);
            vl->setContentsMargins(20, 16, 20, 16);
            vl->setSpacing(8);

            QHBoxLayout *top = new QHBoxLayout();
            QLabel *t = new QLabel(name);
            t->setObjectName("cardTitle");
            QLabel *k = new QLabel(ver);
            k->setObjectName("keycap");
            top->addWidget(t);
            top->addStretch();
            top->addWidget(k);
            vl->addLayout(top);

            QLabel *d = new QLabel(desc);
            d->setObjectName("subtitle");
            d->setWordWrap(true);
            vl->addWidget(d);

            QHBoxLayout *act = new QHBoxLayout();
            QPushButton *b = new QPushButton(btnText);
            b->setObjectName("primaryBtn");
            connect(b, &QPushButton::clicked, this, [execPath]() {
                QProcess::startDetached(execPath, {});
            });
            act->addWidget(b);
            act->addStretch();
            vl->addLayout(act);
            return c;
        };

        lay->addWidget(makeAppCard(
            "ORBITAL",
            "v0.6.1 · AI & Kod",
            "Proje klasörleriyle çalışan masaüstü AI kod asistanı ve dahili web sohbeti — Markdown, JSON, kaynak kod ve AI dosyaları için öntanımlı uygulama",
            "ORBITAL'i başlat",
            "/usr/bin/orbital"
        ));

        lay->addWidget(makeAppCard(
            "Nate Browser",
            "v0.2.1 · Web & PDF",
            "Nate Shields reklam engelleyici, NateWallet ve ORBITAL yan paneli içeren tarayıcı — Web bağlantıları ve PDF belgeleri için öntanımlı uygulama",
            "Nate Browser'ı başlat",
            "/usr/bin/nate-browser"
        ));

        lay->addWidget(makeAppCard(
            "EdgeFile",
            "v1.0 · Dosya gezgini",
            "Canlı kod önizleme paneli, uygulama kısayolu çözümleyici ve doğrudan ORBITAL / Nate Browser entegrasyonuna sahip yerel dosya yöneticisi",
            "EdgeFile'ı başlat",
            "/usr/bin/edgefile"
        ));

        lay->addStretch();
        return w;
    }

    QWidget* createPage6Finish() {
        QWidget *w = new QWidget();
        QVBoxLayout *lay = new QVBoxLayout(w);
        lay->setContentsMargins(0, 0, 0, 0);
        lay->setSpacing(16);

        QVBoxLayout *hdr = new QVBoxLayout();
        hdr->setSpacing(6);
        QLabel *title = new QLabel("Ağ, güvenlik ve oturum");
        title->setObjectName("pageTitle");
        QLabel *sub = new QLabel("Hesap ayarlarınızı açın veya sistem araçlarını başlatın");
        sub->setObjectName("subtitle");
        hdr->addWidget(title);
        hdr->addWidget(sub);
        lay->addLayout(hdr);

        QFrame *pwCard = new QFrame();
        pwCard->setObjectName("card");
        QVBoxLayout *pwLay = new QVBoxLayout(pwCard);
        pwLay->setContentsMargins(20, 18, 20, 18);
        pwLay->setSpacing(10);

        QLabel *pwTitle = new QLabel("Kullanıcı hesabı ve parola");
        pwTitle->setObjectName("cardTitle");
        pwLay->addWidget(pwTitle);

        QLabel *pwInfo = new QLabel("Parolanızı, adınızı ve hesap resminizi KDE kullanıcı ayarlarından değiştirebilirsiniz.");
        pwInfo->setObjectName("subtitle");
        pwInfo->setWordWrap(true);
        QPushButton *pwBtn = new QPushButton("Kullanıcı ayarlarını aç");
        pwBtn->setObjectName("primaryBtn");
        connect(pwBtn, &QPushButton::clicked, this, []() { QProcess::startDetached("/usr/bin/systemsettings", {"kcm_users"}); });
        pwLay->addWidget(pwInfo);
        pwLay->addWidget(pwBtn, 0, Qt::AlignLeft);
        lay->addWidget(pwCard);

        QFrame *srvCard = new QFrame();
        srvCard->setObjectName("card");
        QVBoxLayout *srvLay = new QVBoxLayout(srvCard);
        srvLay->setContentsMargins(20, 18, 20, 18);
        srvLay->setSpacing(12);

        QLabel *srvTitle = new QLabel("Hızlı araçlar");
        srvTitle->setObjectName("cardTitle");
        srvLay->addWidget(srvTitle);

        QHBoxLayout *quickLaunch = new QHBoxLayout();
        quickLaunch->setSpacing(8);
        QPushButton *btnOrb = new QPushButton("ORBITAL");
        QPushButton *btnNate = new QPushButton("Nate Browser");
        QPushButton *btnFile = new QPushButton("EdgeFile");
        QPushButton *btnSite = new QPushButton("Origin Edge sitesi");
        btnSite->setObjectName("primaryBtn");

        connect(btnOrb, &QPushButton::clicked, this, []() { QProcess::startDetached("/usr/bin/orbital", {}); });
        connect(btnNate, &QPushButton::clicked, this, []() { QProcess::startDetached("/usr/bin/nate-browser", {}); });
        connect(btnFile, &QPushButton::clicked, this, []() { QProcess::startDetached("/usr/bin/edgefile", {}); });
        connect(btnSite, &QPushButton::clicked, this, []() { QProcess::startDetached("/usr/bin/nate-browser", {"https://oedge.xyz"}); });

        quickLaunch->addWidget(btnSite);
        quickLaunch->addWidget(btnOrb);
        quickLaunch->addWidget(btnNate);
        quickLaunch->addWidget(btnFile);
        quickLaunch->addStretch();
        srvLay->addLayout(quickLaunch);
        lay->addWidget(srvCard);

        lay->addStretch();
        return w;
    }

    void finishWizard() {
        QString cfgDir = QDir::homePath() + "/.config";
        QDir().mkpath(cfgDir + "/autostart");
        QString disFile = cfgDir + "/edgeos-wizard-disabled";
        QString autoFile = cfgDir + "/autostart/edgeos-setup-wizard.desktop";

        if (!autostartCheck->isChecked()) {
            QFile f(disFile);
            if (f.open(QIODevice::WriteOnly)) f.close();
            QFile af(autoFile);
            if (af.open(QIODevice::WriteOnly | QIODevice::Text)) {
                QTextStream out(&af);
                out << "[Desktop Entry]\nType=Application\nName=EdgeOS Kurulum Yardımcısı\nHidden=true\n";
            }
        } else {
            QFile::remove(disFile);
            QFile::remove(autoFile);
        }
        close();
    }
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("edgeos-setup-wizard");
    app.setApplicationDisplayName("EdgeOS by Origin Edge");
    EdgeOSSetupWizard wiz;
    if (app.arguments().contains("--ilk-acilis")) {
        if (QFile::exists(QDir::homePath() + "/.config/edgeos-wizard-disabled")) return 0;   // bir kez gösterildi
        wiz.showMaximized();
    } else {
        wiz.show();
    }
    return app.exec();
}
