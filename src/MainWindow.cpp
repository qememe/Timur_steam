#include "MainWindow.h"
#include <QAction>
#include <QDialogButtonBox>
#include <QDir>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMenu>
#include <QMessageBox>
#include <QPushButton>
#include <QToolBar>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), m_gameManager(new GameManager(this)),
      m_authManager(new AuthManager(this)) {
  setupUi();

  connect(m_gameManager, &GameManager::catalogUpdated, this,
          &MainWindow::updateGameList);
  connect(m_gameManager, &GameManager::gameLaunchRequested, this,
          &MainWindow::showGame);
  connect(m_gameManager, &GameManager::gameInstallationFinished, this,
          [this](const QString &gameId, bool success) {
            if (!success) {
              QMessageBox::warning(this, "Ошибка установки",
                                   "Не удалось установить игру " + gameId);
            }
            updateGameList();
          });

  connect(m_authManager, &AuthManager::userChanged, this,
          &MainWindow::onUserChanged);
  connect(m_authManager, &AuthManager::developerModeChanged, this,
          &MainWindow::onDeveloperModeChanged);

  // Initial fetch
  m_gameManager->fetchCatalog();
}

MainWindow::~MainWindow() {}

void MainWindow::setupUi() {
  m_centralStack = new QStackedWidget(this);
  setCentralWidget(m_centralStack);

  // Page 1: Store/Library
  m_storeWidget = createStorePage();
  m_centralStack->addWidget(m_storeWidget);

  // Page 2: Game Player
  m_webView = new QWebEngineView(this);
  m_centralStack->addWidget(m_webView);

  // Toolbar
  QToolBar *toolbar = addToolBar("Навигация");
  QAction *homeAction = toolbar->addAction("Библиотека");
  connect(homeAction, &QAction::triggered, [this]() {
    m_centralStack->setCurrentWidget(m_storeWidget);
    m_webView->stop();
    m_webView->setUrl(QUrl("about:blank"));
  });

  toolbar->addSeparator();

  m_loginAction = toolbar->addAction("Вход");
  connect(m_loginAction, &QAction::triggered, this,
          &MainWindow::onLoginClicked);

  m_devModeAction = toolbar->addAction("Стать разработчиком");
  m_devModeAction->setVisible(false); // Only visible when logged in
  connect(m_devModeAction, &QAction::triggered, [this]() {
    // Toggle dev mode
    bool newMode = !m_authManager->isDeveloper();
    m_authManager->setDeveloperMode(newMode);
  });

  resize(1024, 768);
}

QWidget *MainWindow::createStorePage() {
  QWidget *page = new QWidget;
  QVBoxLayout *layout = new QVBoxLayout(page);

  // Header
  QHBoxLayout *header = new QHBoxLayout;
  QLabel *title = new QLabel("Тимур Steam Библиотека");
  QFont font = title->font();
  font.setPointSize(20);
  title->setFont(font);
  header->addWidget(title);
  header->addStretch();

  // Add Game Button (Dev only)
  m_addGameBtn = new QPushButton("Добавить игру (Dev)");
  m_addGameBtn->setVisible(false);
  connect(m_addGameBtn, &QPushButton::clicked, this,
          &MainWindow::onAddGameClicked);
  header->addWidget(m_addGameBtn);

  layout->addLayout(header);

  // Game List
  QListWidget *list = new QListWidget;
  list->setObjectName("gameList");
  list->setIconSize(QSize(64, 64)); // Placeholder for icons
  layout->addWidget(list);

  connect(list, &QListWidget::itemDoubleClicked,
          [this, list](QListWidgetItem *item) {
            QString gameId = item->data(Qt::UserRole).toString();
            auto games = m_gameManager->getAllGames();
            for (const auto &g : games) {
              if (g.id == gameId) {
                if (g.isInstalled) {
                  m_gameManager->launchGame(g);
                } else {
                  item->setText(g.title + " (Установка...)");
                  m_gameManager->installGame(g);
                }
                break;
              }
            }
          });

  return page;
}

void MainWindow::updateGameList() {
  QListWidget *list = findChild<QListWidget *>("gameList");
  if (!list)
    return;

  list->clear();
  auto games = m_gameManager->getAllGames();
  for (const auto &game : games) {
    QString status = game.isInstalled ? " [ИГРАТЬ]" : " [УСТАНОВИТЬ]";
    QString text = game.title + " - " + game.version + status;
    QListWidgetItem *item = new QListWidgetItem(text);
    item->setData(Qt::UserRole, game.id);

    // Simple styling
    QFont f = item->font();
    if (game.isInstalled)
      f.setBold(true);
    item->setFont(f);

    list->addItem(item);
  }
}

void MainWindow::showGame(const QString &url) {
  m_webView->setUrl(QUrl(url));
  m_centralStack->setCurrentWidget(m_webView);
}

void MainWindow::onLoginClicked() {
  if (!m_authManager->getUsername().isEmpty()) {
    // Logout
    m_authManager->logout();
    return;
  }

  bool ok;
  QString text = QInputDialog::getText(
      this, "Вход", "Имя пользователя:", QLineEdit::Normal, "", &ok);
  if (ok && !text.isEmpty()) {
    m_authManager->login(text);
  }
}

void MainWindow::onUserChanged() {
  QString user = m_authManager->getUsername();
  if (user.isEmpty()) {
    m_loginAction->setText("Вход");
    m_devModeAction->setVisible(false);
  } else {
    m_loginAction->setText("Выход (" + user + ")");
    m_devModeAction->setVisible(true);
    m_devModeAction->setText("Включить режим разработчика");
  }
}

void MainWindow::onDeveloperModeChanged(bool isDev) {
  m_addGameBtn->setVisible(isDev);
  if (isDev) {
    m_devModeAction->setText("Выключить режим разработчика");
    setWindowTitle("Тимур Steam (Режим разработчика)");
  } else {
    m_devModeAction->setText("Включить режим разработчика");
    setWindowTitle("Тимур Steam");
  }
}

void MainWindow::onAddGameClicked() {
  // Generate JSON snippet for NEW game
  QDialog dialog(this);
  dialog.setWindowTitle("Добавить новую игру");
  QFormLayout form(&dialog);

  QLineEdit *idEdit = new QLineEdit(&dialog);
  QLineEdit *titleEdit = new QLineEdit(&dialog);
  QLineEdit *descEdit = new QLineEdit(&dialog);
  QLineEdit *repoEdit = new QLineEdit(&dialog);
  QLineEdit *authorEdit = new QLineEdit(&dialog);

  form.addRow("ID (slug):", idEdit);
  form.addRow("Название:", titleEdit);
  form.addRow("Описание:", descEdit);
  form.addRow("URL репозитория:", repoEdit);
  form.addRow("Автор:", authorEdit);

  QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                             Qt::Horizontal, &dialog);
  form.addRow(&buttonBox);

  connect(&buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
  connect(&buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

  if (dialog.exec() == QDialog::Accepted) {
    // Create JSON object
    QJsonObject obj;
    obj["id"] = idEdit->text();
    obj["title"] = titleEdit->text();
    obj["description"] = descEdit->text();
    obj["repoUrl"] = repoEdit->text();
    obj["author"] = authorEdit->text();
    obj["version"] = "1.0";

    // In a real app we'd push to GitHub.
    // Here we just append to local manifest for testing.
    QFile file("data/store_manifest.json");
    if (file.open(QIODevice::ReadWrite)) {
      QByteArray data = file.readAll();
      QJsonDocument doc = QJsonDocument::fromJson(data);
      QJsonArray array = doc.array();
      array.append(obj);

      file.resize(0);
      file.write(QJsonDocument(array).toJson());
      file.close();

      m_gameManager->fetchCatalog(); // Refresh
      QMessageBox::information(this, "Успех",
                               "Игра добавлена в локальный манифест!\n(В "
                               "продакшене это создало бы PR на GitHub)");
    }
  }
}

void MainWindow::verifyGitAvailability() {
  // Check if git is in path
}
