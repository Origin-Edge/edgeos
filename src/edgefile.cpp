#include <QApplication>
#include <QMainWindow>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QTreeView>
#include <QFileSystemModel>
#include <QHeaderView>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QFrame>
#include <QPlainTextEdit>
#include <QProcess>
#include <QFileInfo>
#include <QDir>
#include <QFile>
#include <QInputDialog>
#include <QMessageBox>
#include <QClipboard>
#include <QMenu>
#include <QStorageInfo>
#include <QProgressBar>
#include <QDateTime>
#include <QDesktopServices>
#include <QUrl>
#include <QPainter>
#include <QPixmap>
#include <QSvgRenderer>
#include "urun_tema.h"
#include <QRegularExpression>

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

struct DesktopEntryInfo {
    bool valid = false;
    QString name;
    QString comment;
    QString icon;
    QString exec;
};

static bool isDesktopEntryFile(const QString &path) {
    if (path.endsWith(".desktop", Qt::CaseInsensitive)) return true;
    QFileInfo fi(path);
    if (!fi.isFile() || fi.size() > 32768 || fi.size() < 15) return false;
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) return false;
    QByteArray head = f.read(128);
    return head.contains("[Desktop Entry]");
}

static DesktopEntryInfo parseDesktopFile(const QString &path) {
    DesktopEntryInfo info;
    if (!isDesktopEntryFile(path)) return info;
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) return info;
    while (!f.atEnd()) {
        QString line = QString::fromUtf8(f.readLine()).trimmed();
        if (line.startsWith("Name=") && info.name.isEmpty()) {
            info.name = line.mid(5).trimmed();
        } else if (line.startsWith("Name[tr]=")) {
            info.name = line.mid(9).trimmed();
        } else if (line.startsWith("Comment=") && info.comment.isEmpty()) {
            info.comment = line.mid(8).trimmed();
        } else if (line.startsWith("Icon=")) {
            info.icon = line.mid(5).trimmed();
        } else if (line.startsWith("Exec=")) {
            info.exec = line.mid(5).trimmed();
            info.exec.remove(QRegularExpression("%[fFuUdDnNickvm]"));
            info.exec = info.exec.trimmed();
        }
    }
    info.valid = !info.name.isEmpty() && !info.exec.isEmpty();
    return info;
}

static QIcon resolveDesktopIcon(const QString &iconName) {
    if (iconName.isEmpty()) return QIcon::fromTheme("application-x-executable");
    if (QFile::exists(iconName)) return QIcon(iconName);
    QStringList paths = {
        "/usr/share/pixmaps/" + iconName + ".png",
        "/usr/share/pixmaps/" + iconName + ".svg",
        "/usr/share/icons/OriginEdge-Icons/apps/scalable/" + iconName + ".svg",
        "/usr/share/icons/hicolor/scalable/apps/" + iconName + ".svg",
        "/usr/share/icons/hicolor/512x512/apps/" + iconName + ".png"
    };
    for (const QString &p : paths) {
        if (QFile::exists(p)) return QIcon(p);
    }
    return QIcon::fromTheme(iconName, QIcon::fromTheme("application-x-executable"));
}

// Custom QFileSystemModel that hides ".desktop" extension, displays the real application Name & Icon, and reports "Uygulama" as type
class EdgeFileSystemModel : public QFileSystemModel {
public:
    using QFileSystemModel::QFileSystemModel;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override {
        if (!index.isValid()) return QFileSystemModel::data(index, role);
        QString path = filePath(index);
        if (isDesktopEntryFile(path)) {
            DesktopEntryInfo de = parseDesktopFile(path);
            if (de.valid) {
                if (role == Qt::DisplayRole) {
                    if (index.column() == 0) return de.name;
                    if (index.column() == 2) return QStringLiteral("Uygulama");
                } else if (role == Qt::DecorationRole && index.column() == 0) {
                    return resolveDesktopIcon(de.icon);
                } else if (role == Qt::ToolTipRole) {
                    return de.comment.isEmpty() ? de.name : (de.name + " — " + de.comment);
                }
            }
        }
        return QFileSystemModel::data(index, role);
    }
};

class EdgeFileWindow : public QMainWindow {
public:
    EdgeFileWindow(const QString &initialPath = QDir::homePath()) {
        setWindowTitle("EdgeFile — Edge OS by Origin Edge");
        setMinimumSize(1040, 640);
        resize(1160, 720);

        applyUnslopTheme(false);

        QWidget *root = new QWidget(this);
        root->setObjectName("rootWidget");
        setCentralWidget(root);

        QHBoxLayout *mainLayout = new QHBoxLayout(root);
        mainLayout->setContentsMargins(0, 0, 0, 0);
        mainLayout->setSpacing(0);

        // 1. Unslop Graphite Sidebar (248px, #ffffff, 16px padding)
        QFrame *sidebar = new QFrame(root);
        sidebar->setObjectName("sidebar");
        sidebar->setFixedWidth(248);
        QVBoxLayout *sideLay = new QVBoxLayout(sidebar);
        sideLay->setContentsMargins(16, 20, 16, 16);
        sideLay->setSpacing(2);

        QHBoxLayout *brandRow = new QHBoxLayout();
        brandRow->setContentsMargins(6, 0, 4, 0);
        brandRow->setSpacing(10);
        QLabel *logoLbl = new QLabel();
        logoLbl->setFixedSize(22, 22);
        if (QFile::exists("/usr/share/pixmaps/origin-edge-mark-koyu.svg")) {
            QSvgRenderer r(QStringLiteral("/usr/share/pixmaps/origin-edge-mark-koyu.svg"));
            QPixmap pm(22, 22);
            pm.fill(Qt::transparent);
            QPainter p(&pm);
            r.render(&p);
            logoLbl->setPixmap(pm);
        }
        QLabel *bName = new QLabel("EdgeFile");
        bName->setObjectName("wordmark");
        QLabel *bMeta = new QLabel("Origin Edge");
        bMeta->setObjectName("mutedCaption");

        QPushButton *themeBtn = new QPushButton("Görünüm");
        themeBtn->setObjectName("quietBtn");
        themeBtn->setFixedHeight(28);
        connect(themeBtn, &QPushButton::clicked, this, [this]() {
            isDark = !isDark;
            applyUnslopTheme(isDark);
        });

        brandRow->addWidget(logoLbl);
        brandRow->addWidget(bName);
        brandRow->addWidget(bMeta);
        brandRow->addStretch();
        brandRow->addWidget(themeBtn);
        sideLay->addLayout(brandRow);

        sideLay->addSpacing(24);
        QLabel *placesHdr = new QLabel("Konumlar");
        placesHdr->setObjectName("sectionLabel");
        placesHdr->setContentsMargins(8, 0, 0, 6);
        sideLay->addWidget(placesHdr);

        struct PlaceItem { QString title; QString path; QString svg; };
        QString home = QDir::homePath();
        QList<PlaceItem> places = {
            {"Başlangıç", home, "<path d='m3 9 9-7 9 7v11a2 2 0 0 1-2 2H5a2 2 0 0 1-2-2z'/><polyline points='9 22 9 12 15 12 15 22'/>"},
            {"Masaüstü", home + "/Desktop", "<rect width='20' height='14' x='2' y='3' rx='2'/><line x1='8' x2='16' y1='21' y2='21'/><line x1='12' x2='12' y1='17' y2='21'/>"},
            {"Belgeler", home + "/Documents", "<path d='M15 2H6a2 2 0 0 0-2 2v16a2 2 0 0 0 2 2h12a2 2 0 0 0 2-2V7Z'/><path d='M14 2v4a2 2 0 0 0 2 2h4'/>"},
            {"İndirilenler", home + "/Downloads", "<path d='M21 15v4a2 2 0 0 1-2 2H5a2 2 0 0 1-2-2v-4'/><polyline points='7 10 12 15 17 10'/><line x1='12' x2='12' y1='15' y2='3'/>"},
            {"Resimler", home + "/Pictures", "<rect width='18' height='18' x='3' y='3' rx='2' ry='2'/><circle cx='9' cy='9' r='2'/><path d='m21 15-3.086-3.086a2 2 0 0 0-2.828 0L6 21'/>"},
            {"Uygulamalar", "/usr/share/applications", "<rect width='7' height='7' x='3' y='3' rx='1'/><rect width='7' height='7' x='14' y='3' rx='1'/><rect width='7' height='7' x='14' y='14' rx='1'/><rect width='7' height='7' x='3' y='14' rx='1'/>"},
            {"Kök dizin (/)", "/", "<ellipse cx='12' cy='5' rx='9' ry='3'/><path d='M3 5V19A9 3 0 0 0 21 19V5'/><path d='M3 12A9 3 0 0 0 21 12'/>"}
        };

        for (int i = 0; i < places.size(); ++i) {
            const auto &pl = places[i];
            QPushButton *btn = new QPushButton(lucideIcon(pl.svg), "  " + pl.title);
            btn->setObjectName("navBtn");
            btn->setFixedHeight(34);
            btn->setCursor(Qt::PointingHandCursor);
            connect(btn, &QPushButton::clicked, this, [this, pl, i]() {
                QDir().mkpath(pl.path);
                setActiveNav(i);
                navigateTo(pl.path);
            });
            navButtons.append(btn);
            sideLay->addWidget(btn);
        }

        sideLay->addStretch();

        QFrame *diskCard = new QFrame();
        diskCard->setObjectName("card");
        QVBoxLayout *dLay = new QVBoxLayout(diskCard);
        dLay->setContentsMargins(14, 12, 14, 12);
        dLay->setSpacing(8);

        QStorageInfo storage("/");
        double totalGb = storage.bytesTotal() / (1024.0 * 1024.0 * 1024.0);
        double freeGb = storage.bytesAvailable() / (1024.0 * 1024.0 * 1024.0);
        double usedGb = totalGb - freeGb;
        int pct = totalGb > 0 ? int((usedGb / totalGb) * 100.0) : 0;

        QHBoxLayout *dTop = new QHBoxLayout();
        QLabel *dTitle = new QLabel("Disk kullanımı");
        dTitle->setObjectName("mutedCaption");
        QLabel *dVal = new QLabel(QString("%1 / %2 GB").arg(usedGb, 0, 'f', 1).arg(totalGb, 0, 'f', 1));
        dVal->setObjectName("monoCaption");
        dTop->addWidget(dTitle);
        dTop->addStretch();
        dTop->addWidget(dVal);

        QProgressBar *bar = new QProgressBar();
        bar->setRange(0, 100);
        bar->setValue(pct);
        bar->setTextVisible(false);
        bar->setFixedHeight(4);

        dLay->addLayout(dTop);
        dLay->addWidget(bar);
        sideLay->addWidget(diskCard);
        sideLay->addSpacing(8);

        QPushButton *orbitalDirBtn = new QPushButton("Klasörü ORBITAL ile aç");
        orbitalDirBtn->setObjectName("primaryBtn");
        orbitalDirBtn->setFixedHeight(36);
        orbitalDirBtn->setCursor(Qt::PointingHandCursor);
        connect(orbitalDirBtn, &QPushButton::clicked, this, [this]() {
            QProcess::startDetached("/usr/bin/orbital", {currentPath});
        });
        sideLay->addWidget(orbitalDirBtn);

        mainLayout->addWidget(sidebar);

        // 2. Main Content Shell
        QVBoxLayout *contentLay = new QVBoxLayout();
        contentLay->setContentsMargins(24, 16, 24, 16);
        contentLay->setSpacing(16);

        QHBoxLayout *topBar = new QHBoxLayout();
        topBar->setSpacing(8);

        QPushButton *backBtn = new QPushButton(lucideIcon("<path d='m15 18-6-6 6-6'/>"), "");
        backBtn->setFixedSize(36, 36);
        backBtn->setToolTip("Geri");
        QPushButton *upBtn = new QPushButton(lucideIcon("<path d='m18 15-6-6-6 6'/>"), "");
        upBtn->setFixedSize(36, 36);
        upBtn->setToolTip("Üst klasör");
        QPushButton *refreshBtn = new QPushButton(lucideIcon("<path d='M3 12a9 9 0 1 0 9-9 9.75 9.75 0 0 0-6.74 2.74L3 8'/><path d='M3 3v5h5'/>"), "");
        refreshBtn->setFixedSize(36, 36);
        refreshBtn->setToolTip("Yenile");

        pathEdit = new QLineEdit();
        pathEdit->setObjectName("monoInput");
        pathEdit->setFixedHeight(36);
        connect(pathEdit, &QLineEdit::returnPressed, this, [this]() {
            QString p = pathEdit->text().trimmed();
            if (QDir(p).exists()) navigateTo(p);
        });

        searchEdit = new QLineEdit();
        searchEdit->setPlaceholderText("Filtrele");
        searchEdit->setFixedSize(180, 36);

        QPushButton *newFolderBtn = new QPushButton("Yeni klasör");
        newFolderBtn->setFixedHeight(36);
        QPushButton *newFileBtn = new QPushButton("Yeni dosya");
        newFileBtn->setFixedHeight(36);
        QPushButton *termBtn = new QPushButton("Terminal");
        termBtn->setFixedHeight(36);

        connect(backBtn, &QPushButton::clicked, this, [this]() {
            if (historyIdx > 0) {
                historyIdx--;
                navigateTo(history[historyIdx], false);
            }
        });
        connect(upBtn, &QPushButton::clicked, this, [this]() {
            QDir d(currentPath);
            if (d.cdUp()) navigateTo(d.absolutePath());
        });
        connect(refreshBtn, &QPushButton::clicked, this, [this]() {
            navigateTo(currentPath, false);
        });
        connect(newFolderBtn, &QPushButton::clicked, this, [this]() { createNewFolder(); });
        connect(newFileBtn, &QPushButton::clicked, this, [this]() { createNewFile(); });
        connect(termBtn, &QPushButton::clicked, this, [this]() {
            QProcess::startDetached("/usr/bin/konsole", {"--workdir", currentPath});
        });

        topBar->addWidget(backBtn);
        topBar->addWidget(upBtn);
        topBar->addWidget(refreshBtn);
        topBar->addWidget(pathEdit, 1);
        topBar->addWidget(searchEdit);
        topBar->addWidget(newFolderBtn);
        topBar->addWidget(newFileBtn);
        topBar->addWidget(termBtn);
        contentLay->addLayout(topBar);

        QSplitter *splitter = new QSplitter(Qt::Horizontal);
        splitter->setHandleWidth(12);

        fsModel = new EdgeFileSystemModel(this);
        fsModel->setFilter(QDir::AllEntries | QDir::NoDotAndDotDot | QDir::AllDirs);
        fsModel->setRootPath("/");

        treeView = new QTreeView();
        treeView->setModel(fsModel);
        treeView->setRootIsDecorated(false);
        treeView->setAlternatingRowColors(false);
        treeView->setSortingEnabled(true);
        treeView->sortByColumn(0, Qt::AscendingOrder);
        treeView->setSelectionMode(QAbstractItemView::SingleSelection);
        treeView->setContextMenuPolicy(Qt::CustomContextMenu);
        treeView->setUniformRowHeights(true);
        treeView->header()->setStretchLastSection(true);
        treeView->header()->resizeSection(0, 340);
        treeView->header()->resizeSection(1, 100);
        treeView->header()->resizeSection(2, 110);

        connect(searchEdit, &QLineEdit::textChanged, this, [this](const QString &txt) {
            if (txt.trimmed().isEmpty()) {
                fsModel->setNameFilters({});
                fsModel->setNameFilterDisables(false);
            } else {
                fsModel->setNameFilters({"*" + txt.trimmed() + "*"});
                fsModel->setNameFilterDisables(false);
            }
        });

        connect(treeView, &QTreeView::doubleClicked, this, [this](const QModelIndex &idx) {
            QString fpath = fsModel->filePath(idx);
            QFileInfo fi(fpath);
            if (fi.isDir()) navigateTo(fpath);
            else openFileSmart(fpath);
        });

        connect(treeView->selectionModel(), &QItemSelectionModel::currentChanged, this, [this](const QModelIndex &curr, const QModelIndex &) {
            updateInspector(fsModel->filePath(curr));
        });

        connect(treeView, &QTreeView::customContextMenuRequested, this, [this](const QPoint &pos) {
            QModelIndex idx = treeView->indexAt(pos);
            showContextMenu(idx.isValid() ? fsModel->filePath(idx) : currentPath, treeView->viewport()->mapToGlobal(pos));
        });

        splitter->addWidget(treeView);

        // Right Inspector Card
        QFrame *inspector = new QFrame();
        inspector->setObjectName("card");
        inspector->setMinimumWidth(300);
        inspector->setMaximumWidth(360);
        QVBoxLayout *insLay = new QVBoxLayout(inspector);
        insLay->setContentsMargins(20, 20, 20, 20);
        insLay->setSpacing(12);

        QHBoxLayout *insTop = new QHBoxLayout();
        QLabel *insHdr = new QLabel("Dosya önizleme");
        insHdr->setObjectName("sectionLabel");
        extKeycap = new QLabel("DİZİN");
        extKeycap->setObjectName("keycap");
        insTop->addWidget(insHdr);
        insTop->addStretch();
        insTop->addWidget(extKeycap);

        insNameLbl = new QLabel("Seçim yok");
        insNameLbl->setObjectName("cardTitle");
        insNameLbl->setWordWrap(true);

        insMetaLbl = new QLabel("");
        insMetaLbl->setObjectName("monoCaption");
        insMetaLbl->setWordWrap(true);

        previewBox = new QPlainTextEdit();
        previewBox->setReadOnly(true);
        previewBox->setPlaceholderText("Dosya içeriği burada görüntülenir");

        QPushButton *openOrbitalBtn = new QPushButton("ORBITAL ile aç");
        openOrbitalBtn->setObjectName("primaryBtn");
        openOrbitalBtn->setFixedHeight(36);

        QPushButton *openNateBtn = new QPushButton("Nate Browser ile aç");
        openNateBtn->setFixedHeight(36);

        QPushButton *renameBtn = new QPushButton("Yeniden adlandır");
        renameBtn->setFixedHeight(34);
        QPushButton *deleteBtn = new QPushButton("Sil");
        deleteBtn->setObjectName("dangerBtn");
        deleteBtn->setFixedHeight(34);

        connect(openOrbitalBtn, &QPushButton::clicked, this, [this]() {
            QString target = selectedPath.isEmpty() ? currentPath : selectedPath;
            QProcess::startDetached("/usr/bin/orbital", {target});
        });
        connect(openNateBtn, &QPushButton::clicked, this, [this]() {
            QString target = selectedPath.isEmpty() ? currentPath : selectedPath;
            QProcess::startDetached("/usr/bin/nate-browser", {target});
        });
        connect(renameBtn, &QPushButton::clicked, this, [this]() { renameSelected(); });
        connect(deleteBtn, &QPushButton::clicked, this, [this]() { deleteSelected(); });

        insLay->addLayout(insTop);
        insLay->addWidget(insNameLbl);
        insLay->addWidget(insMetaLbl);
        insLay->addWidget(previewBox, 1);
        insLay->addWidget(openOrbitalBtn);
        insLay->addWidget(openNateBtn);

        QHBoxLayout *editRow = new QHBoxLayout();
        editRow->setSpacing(8);
        editRow->addWidget(renameBtn);
        editRow->addWidget(deleteBtn);
        insLay->addLayout(editRow);

        splitter->addWidget(inspector);
        splitter->setStretchFactor(0, 1);
        splitter->setStretchFactor(1, 0);

        contentLay->addWidget(splitter, 1);

        QHBoxLayout *footer = new QHBoxLayout();
        statusLbl = new QLabel("Öntanımlı eşleşmeler: Web ve PDF dosyaları Nate Browser ile, kod ve AI belgeleri ORBITAL ile açılır");
        statusLbl->setObjectName("mutedCaption");
        QLabel *creditLbl = new QLabel("Edge OS by Origin Edge");
        creditLbl->setObjectName("monoCaption");
        footer->addWidget(statusLbl);
        footer->addStretch();
        footer->addWidget(creditLbl);
        contentLay->addLayout(footer);

        mainLayout->addLayout(contentLay, 1);

        setActiveNav(0);
        navigateTo(initialPath);
    }

private:
    bool isDark = false;
    EdgeFileSystemModel *fsModel;
    QTreeView *treeView;
    QLineEdit *pathEdit;
    QLineEdit *searchEdit;
    QLabel *insNameLbl;
    QLabel *insMetaLbl;
    QLabel *extKeycap;
    QPlainTextEdit *previewBox;
    QLabel *statusLbl;
    QList<QPushButton*> navButtons;
    QString currentPath;
    QString selectedPath;
    QStringList history;
    int historyIdx = -1;

    void setActiveNav(int idx) {
        for (int i = 0; i < navButtons.size(); ++i) {
            navButtons[i]->setProperty("active", i == idx ? "true" : "false");
            navButtons[i]->style()->unpolish(navButtons[i]);
            navButtons[i]->style()->polish(navButtons[i]);
        }
    }

    void applyUnslopTheme(bool dark) {
        setStyleSheet(urunTema(dark));
    }

    void navigateTo(const QString &path, bool recordHistory = true) {
        QFileInfo fi(path);
        if (!fi.exists() || !fi.isDir()) return;
        currentPath = fi.absoluteFilePath();
        pathEdit->setText(currentPath);
        QModelIndex idx = fsModel->index(currentPath);
        treeView->setRootIndex(idx);
        if (recordHistory) {
            while (history.size() > historyIdx + 1) history.removeLast();
            history.append(currentPath);
            historyIdx = history.size() - 1;
        }
        selectedPath = currentPath;
        updateInspector(currentPath);
    }

    void updateInspector(const QString &path) {
        if (path.isEmpty()) return;
        selectedPath = path;
        QFileInfo fi(path);

        if (isDesktopEntryFile(path)) {
            DesktopEntryInfo de = parseDesktopFile(path);
            if (de.valid) {
                insNameLbl->setText(de.name);
                extKeycap->setText("UYGULAMA");
                insMetaLbl->setText("Çalıştırılabilir masaüstü uygulaması");
                previewBox->setPlainText((de.comment.isEmpty() ? de.name : de.comment) + "\n\nKomut: " + de.exec);
                return;
            }
        }

        insNameLbl->setText(fi.fileName().isEmpty() ? "/" : fi.fileName());
        QString badge = fi.isDir() ? "DİZİN" : (fi.suffix().isEmpty() ? "DOSYA" : fi.suffix().toUpper());
        extKeycap->setText(badge);

        QString sizeStr = fi.isDir()
            ? QString("%1 öğe").arg(QDir(path).entryList(QDir::NoDotAndDotDot | QDir::AllEntries).count())
            : QString("%1 KB").arg((fi.size() + 1023) / 1024);
        insMetaLbl->setText(QString("%1  ·  %2").arg(sizeStr, fi.lastModified().toString("yyyy-MM-dd HH:mm")));

        previewBox->clear();
        if (fi.isFile() && fi.size() < 256 * 1024) {
            QFile f(path);
            if (f.open(QIODevice::ReadOnly | QIODevice::Text)) {
                QByteArray data = f.read(8192);
                if (!data.contains('\0')) {
                    previewBox->setPlainText(QString::fromUtf8(data));
                } else {
                    previewBox->setPlainText("İkili dosya");
                }
            }
        } else if (fi.isDir()) {
            QStringList entries = QDir(path).entryList(QDir::NoDotAndDotDot | QDir::AllEntries, QDir::DirsFirst | QDir::Name);
            QStringList cleanEntries;
            for (const QString &e : entries.mid(0, 45)) {
                QString full = path + "/" + e;
                if (isDesktopEntryFile(full)) {
                    DesktopEntryInfo de = parseDesktopFile(full);
                    cleanEntries.append(de.valid ? de.name : e);
                } else {
                    cleanEntries.append(e);
                }
            }
            previewBox->setPlainText(cleanEntries.join("\n"));
        }
    }

    void openFileSmart(const QString &fpath) {
        if (isDesktopEntryFile(fpath)) {
            DesktopEntryInfo de = parseDesktopFile(fpath);
            if (de.valid) {
                QProcess::startDetached("/bin/bash", {"-c", de.exec});
                return;
            }
        }
        QFileInfo fi(fpath);
        QString ext = fi.suffix().toLower();
        if (ext == "pdf" || ext == "html" || ext == "htm" || ext == "webp" || ext == "svg" || ext == "xml") {
            QProcess::startDetached("/usr/bin/nate-browser", {fpath});
            return;
        }
        QStringList orbitalExts = {"md", "txt", "json", "yaml", "yml", "toml", "ini", "conf", "py", "cpp", "c", "h", "hpp", "js", "ts", "jsx", "tsx", "qml", "sh", "rs", "go", "css", "prompt", "ai", "orbital"};
        if (orbitalExts.contains(ext)) {
            QProcess::startDetached("/usr/bin/orbital", {fpath});
            return;
        }
        QDesktopServices::openUrl(QUrl::fromLocalFile(fpath));
    }

    void createNewFolder() {
        bool ok = false;
        QString name = QInputDialog::getText(this, "Yeni klasör", "Klasör adı", QLineEdit::Normal, "yeni-klasor", &ok);
        if (ok && !name.trimmed().isEmpty()) {
            QDir(currentPath).mkdir(name.trimmed());
        }
    }

    void createNewFile() {
        bool ok = false;
        QString name = QInputDialog::getText(this, "Yeni dosya", "Dosya adı", QLineEdit::Normal, "notlar.md", &ok);
        if (ok && !name.trimmed().isEmpty()) {
            QFile f(currentPath + "/" + name.trimmed());
            if (f.open(QIODevice::WriteOnly)) f.close();
        }
    }

    void renameSelected() {
        if (selectedPath.isEmpty() || selectedPath == currentPath) return;
        QFileInfo fi(selectedPath);
        bool ok = false;
        QString newName = QInputDialog::getText(this, "Yeniden adlandır", "Yeni ad", QLineEdit::Normal, fi.fileName(), &ok);
        if (ok && !newName.trimmed().isEmpty() && newName != fi.fileName()) {
            QFile::rename(selectedPath, fi.absolutePath() + "/" + newName.trimmed());
        }
    }

    void deleteSelected() {
        if (selectedPath.isEmpty() || selectedPath == currentPath || selectedPath == "/") return;
        QFileInfo fi(selectedPath);
        auto res = QMessageBox::question(this, "Sil", QString("'%1' silinsin mi").arg(fi.fileName()));
        if (res == QMessageBox::Yes) {
            if (fi.isDir()) QDir(selectedPath).removeRecursively();
            else QFile::remove(selectedPath);
            selectedPath = currentPath;
        }
    }

    void showContextMenu(const QString &targetPath, const QPoint &globalPos) {
        selectedPath = targetPath;
        QMenu menu(this);
        menu.setStyleSheet("QMenu { background-color: #ffffff; color: #0f172a; border: 1px solid #e2e8f0; border-radius: 8px; padding: 4px; } QMenu::item { padding: 6px 16px; border-radius: 5px; font-size: 13px; } QMenu::item:selected { background-color: #f1f5f9; color: #0f172a; }");
        if (isDesktopEntryFile(targetPath)) {
            menu.addAction("Uygulamayı çalıştır", [this, targetPath]() { openFileSmart(targetPath); });
            menu.addSeparator();
        }
        menu.addAction("ORBITAL ile aç", [targetPath]() { QProcess::startDetached("/usr/bin/orbital", {targetPath}); });
        menu.addAction("Nate Browser ile aç", [targetPath]() { QProcess::startDetached("/usr/bin/nate-browser", {targetPath}); });
        menu.addSeparator();
        menu.addAction("Burada terminal aç", [this]() { QProcess::startDetached("/usr/bin/konsole", {"--workdir", currentPath}); });
        menu.addAction("Yolu kopyala", [targetPath]() { QApplication::clipboard()->setText(targetPath); });
        menu.addSeparator();
        menu.addAction("Yeniden adlandır", [this]() { renameSelected(); });
        menu.addAction("Sil", [this]() { deleteSelected(); });
        menu.exec(globalPos);
    }
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("edgefile");
    app.setApplicationDisplayName("EdgeFile");
    QString startDir = argc > 1 ? QString::fromLocal8Bit(argv[1]) : QDir::homePath();
    if (startDir.startsWith("file://")) startDir = QUrl(startDir).toLocalFile();
    if (!QDir(startDir).exists()) startDir = QDir::homePath();
    EdgeFileWindow win(startDir);
    win.show();
    return app.exec();
}
