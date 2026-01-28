#include "AuthManager.h"

AuthManager::AuthManager(QObject *parent)
    : QObject(parent), m_isDeveloper(false) {}

bool AuthManager::login(const QString &username) {
  if (username.isEmpty())
    return false;
  m_username = username;
  emit userChanged();
  return true;
}

void AuthManager::logout() {
  m_username.clear();
  m_isDeveloper = false;
  emit userChanged();
  emit developerModeChanged(false);
}

bool AuthManager::isDeveloper() const { return m_isDeveloper; }

void AuthManager::setDeveloperMode(bool enabled) {
  if (m_isDeveloper != enabled) {
    m_isDeveloper = enabled;
    emit developerModeChanged(enabled);
  }
}

QString AuthManager::getUsername() const { return m_username; }
