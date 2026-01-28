#ifndef AUTHMANAGER_H
#define AUTHMANAGER_H

#include <QObject>
#include <QString>

class AuthManager : public QObject {
  Q_OBJECT
public:
  explicit AuthManager(QObject *parent = nullptr);

  bool login(const QString &username);
  void logout();

  bool isDeveloper() const;
  void setDeveloperMode(bool enabled);
  QString getUsername() const;

signals:
  void userChanged();
  void developerModeChanged(bool isDev);

private:
  QString m_username;
  bool m_isDeveloper;
};

#endif // AUTHMANAGER_H
