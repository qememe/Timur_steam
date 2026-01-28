#ifndef GAME_H
#define GAME_H

#include <QString>

struct Game {
    QString id;
    QString title;
    QString description;
    QString repoUrl;
    QString author;
    QString version;
    bool isInstalled = false;
    QString localPath; // Path into games/ folder if installed
};

#endif // GAME_H
