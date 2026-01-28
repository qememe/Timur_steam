#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "AuthManager.h"
#include "GameManager.h"
#include <QAction>
#include <QMainWindow>
#include <QPushButton>
#include <QStackedWidget>
#include <QWebEngineView>

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

private slots:
  void onLoginClicked();
  void updateGameList();
  void showGame(const QString &url);
  void onUserChanged();
  void onDeveloperModeChanged(bool isDev);
  void onAddGameClicked();

private:
  void setupUi();
  void verifyGitAvailability();

  QWidget *createStorePage();

  GameManager *m_gameManager;
  AuthManager *m_authManager;

  QStackedWidget *m_centralStack;
  QWebEngineView *m_webView;
  QWidget *m_storeWidget;

  QAction *m_loginAction;
  QAction *m_devModeAction;
  QPushButton *m_addGameBtn;
};

#endif // MAINWINDOW_H
