#include "GameManager.h"
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStandardPaths>
#include <QUrl>

GameManager::GameManager(QObject *parent) : QObject(parent) {
  // Set install path to AppData/Timur_steam/games
  m_gamesInstallPath =
      QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) +
      "/games";
  QDir dir;
  if (!dir.exists(m_gamesInstallPath)) {
    dir.mkpath(m_gamesInstallPath);
  }
}

void GameManager::fetchCatalog() {
  // For now, load from local file for testing
  // In real app, this would be a network request
  // TODO: Implement network fetch from GitHub raw URL

  // Attempt to read data/store_manifest.json relative to binary or current dir
  QFile file("data/store_manifest.json");
  if (!file.open(QIODevice::ReadOnly)) {
    // Fallback or error
    qWarning() << "Could not open manifest file";
    return;
  }

  QByteArray data = file.readAll();
  QJsonDocument doc = QJsonDocument::fromJson(data);
  QJsonArray array = doc.array();

  m_games.clear();
  for (const QJsonValue &val : array) {
    QJsonObject obj = val.toObject();
    Game game;
    game.id = obj["id"].toString();
    game.title = obj["title"].toString();
    game.description = obj["description"].toString();
    game.repoUrl = obj["repoUrl"].toString();
    game.author = obj["author"].toString();
    game.version = obj["version"].toString();

    // Check if installed
    QString localPath = m_gamesInstallPath + "/" + game.id;
    if (QDir(localPath).exists()) {
      game.isInstalled = true;
      game.localPath = localPath;
    }

    m_games.append(game);
  }
  emit catalogUpdated();
}

void GameManager::installGame(const Game &game) {
  emit gameInstallationStarted(game.id);

  QString targetDir = m_gamesInstallPath + "/" + game.id;
  QDir dir(targetDir);

  if (dir.exists()) {
    // If it exists, we ideally want to update (git pull) or reinstall.
    // For simplicity and robustness, we'll remove it and clean install for now,
    // unless we implement full git pull support.
    // However, the prompt asked to fix "file moved" error which implies
    // existing folder issues. Let's just remove it to be safe and ensure a
    // fresh clone.
    dir.removeRecursively();
  }

  // Simple git clone using QProcess
  QProcess *process = new QProcess(this);
  QStringList args;
  args << "clone" << game.repoUrl << targetDir;

  connect(process,
          QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
          [this, game, process](int exitCode, QProcess::ExitStatus status) {
            bool success = (status == QProcess::NormalExit && exitCode == 0);

            // Update local state
            for (auto &g : m_games) {
              if (g.id == game.id) {
                g.isInstalled = success;
                if (success)
                  g.localPath = m_gamesInstallPath + "/" + game.id;
                break;
              }
            }

            emit gameInstallationFinished(game.id, success);
            emit catalogUpdated(); // Refresh UI
            process->deleteLater();
          });

  process->start("git", args);
}

void GameManager::launchGame(const Game &game) {
  if (!game.isInstalled)
    return;

  QDir gameDir(game.localPath);
  QString indexFile;

  // 1. Try index.html
  if (gameDir.exists("index.html")) {
    indexFile = gameDir.absoluteFilePath("index.html");
  } else {
    // 2. Search for ANY .html file
    QStringList filters;
    filters << "*.html";
    QStringList files = gameDir.entryList(filters, QDir::Files);
    if (!files.isEmpty()) {
      indexFile = gameDir.absoluteFilePath(files.first());
    }
  }

  if (!indexFile.isEmpty()) {
    // Use QUrl::fromLocalFile to handle special characters and paths correctly
    emit gameLaunchRequested(QUrl::fromLocalFile(indexFile).toString());
  } else {
    qWarning() << "No HTML file found for game" << game.id;
  }
}

QList<Game> GameManager::getAllGames() const { return m_games; }

QList<Game> GameManager::getInstalledGames() const {
  QList<Game> installed;
  for (const auto &g : m_games) {
    if (g.isInstalled)
      installed.append(g);
  }
  return installed;
}
