#ifndef MAINTOPWIDGET_H
#define MAINTOPWIDGET_H

#include <QWidget>

class UserWidget;
class QTableWidget;
class MusicListsWidget;
class BackGroundWidget;

class MainTopWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MainTopWidget(QWidget *parent = 0);
    ~MainTopWidget();

    void addListItem(const QString &item);
    void addMusicItem(const QString &name, const QString &artist, const QString &special);

    void setMusicItem(const int &index, const QString &name, const QString &artist, const QString &special);

    void setcurrentList(int index);
    void setCurrentFile(int index);

    /**
     * @brief 获取当前播发列表名
     * @return
     */
    QString getCurrentListName();
    /**
     * @brief 获取当前播放音乐标题
     * @return
     */
    QString currentMusicName();

    /**
     * @brief 用户信息设置
     * @param head
     * @param name
     */
    void setUserInfo(const QString &head, const QString &name);

signals:
    void logIn();

    void play(int index);

    void musicListItemClicked(const QString &text);

public slots:
    void rmCurrentListItem();
    void rmCurrentMusicItem();
    void rmAll();
    void rmAllList();

    void currentMusic(int index);

private:
    void initUi();
    void initConnect();

private:
    BackGroundWidget *m_backGroundWidget;

    /**
     * @brief 歌单界面
     */
    MusicListsWidget *m_musicListsWidget;
    /**
     * @brief 用户界面
     */
    UserWidget *m_userWidget;
    /**
     * @brief 当前播放列表
     */
    QTableWidget *m_currentList;
};

#endif // MAINTOPWIDGET_H
