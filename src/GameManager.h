#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include "models/Game.h"
#include <QList>
#include <QObject>
#include <QProcess>

class GameManager : public QObject {
  Q_OBJECT
public:
  explicit GameManager(QObject *parent = nullptr);

  void fetchCatalog();
  void installGame(const Game &game);
  void launchGame(const Game &game);
  QList<Game> getAllGames() const;
  QList<Game> getInstalledGames() const;

signals:
  void catalogUpdated();
  void gameInstallationStarted(const QString &gameId);
  void gameInstallationFinished(const QString &gameId, bool success);
  void gameLaunchRequested(const QString &localPath);

private:
  QList<Game> m_games;
  QString m_gamesInstallPath;
};

#endif // GAMEMANAGER_H
