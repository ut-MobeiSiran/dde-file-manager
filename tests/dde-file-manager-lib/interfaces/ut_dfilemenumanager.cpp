#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include "dfilemenumanager.h"
#include "dfilemenu.h"
#include "dfmstandardpaths.h"
#include "views/dfilemanagerwindow.h"
#include "controllers/vaultcontroller.h"
#include "dfmglobal.h"
#include "dfmeventdispatcher.h"
#include "shutil/fileutils.h"

#include "stub.h"
DFM_USE_NAMESPACE

namespace  {
    class TestDFileMenuManager : public testing::Test
    {
    public:
        DFileMenuManager *m_menuMgr = nullptr;
        DFileMenu *m_menu = nullptr;

        virtual void SetUp() override
        {
            m_menuMgr = new DFileMenuManager();
            std::cout << "start DFileMenuManager" << std::endl;
        }

        virtual void TearDown() override
        {
            releaseMenu();

            delete m_menuMgr;
            m_menuMgr = nullptr;

            std::cout << "end DFileMenuManager" << std::endl;
        }

        void releaseMenu()
        {
            if (m_menu) {
                delete m_menu;
                m_menu = nullptr;
            }

        }
    };
}

TEST_F(TestDFileMenuManager, createDefaultBookMarkMenu)
{
    m_menu = m_menuMgr->createDefaultBookMarkMenu();
    EXPECT_NE(m_menu, nullptr);
}

TEST_F(TestDFileMenuManager, createUserShareMarkMenu)
{
    m_menu = m_menuMgr->createUserShareMarkMenu();
    EXPECT_NE(m_menu, nullptr);
}

TEST_F(TestDFileMenuManager, createToolBarSettingsMenu)
{
    m_menu = m_menuMgr->createToolBarSettingsMenu();
    EXPECT_NE(m_menu, nullptr);
}

TEST_F(TestDFileMenuManager, createNormalMenu)
{
    m_menu = m_menuMgr->createNormalMenu(DUrl(), DUrlList(), QSet<MenuAction>(), QSet<MenuAction>(), 0, true);
    EXPECT_EQ(m_menu, nullptr);
    releaseMenu();

    QString musicPath = DFMStandardPaths::location(DFMStandardPaths::MusicPath);
    DUrl url = DUrl::fromLocalFile(musicPath);
    m_menu = m_menuMgr->createNormalMenu(url, DUrlList(), QSet<MenuAction>(), QSet<MenuAction>(), 0, true);
    EXPECT_NE(m_menu, nullptr);
    releaseMenu();

    DUrlList urls;
    urls << url;
    m_menu = m_menuMgr->createNormalMenu(url, urls, QSet<MenuAction>(), QSet<MenuAction>(), 0, true);
    EXPECT_NE(m_menu, nullptr);
    releaseMenu();

    urls << url;
    m_menu = m_menuMgr->createNormalMenu(url, urls, QSet<MenuAction>(), QSet<MenuAction>(), 0, true);
    EXPECT_NE(m_menu, nullptr);
    releaseMenu();
}

TEST_F(TestDFileMenuManager, createVaultMenu)
{
    DFileManagerWindow menuMgr;
    m_menu = m_menuMgr->createVaultMenu(&menuMgr);
    EXPECT_NE(m_menu, nullptr);

    VaultController::VaultState (*st_state_unlock)(QString) = [](QString)->VaultController::VaultState {
        return VaultController::Unlocked;
    };

    VaultController::VaultState (*st_state_encrypt)(QString) = [](QString)->VaultController::VaultState {
        return VaultController::Encrypted;
    };

    Stub stub;
    stub.set(ADDR(VaultController, state), st_state_unlock);
    m_menu = m_menuMgr->createVaultMenu(&menuMgr);
    EXPECT_NE(m_menu, nullptr);

    stub.set(ADDR(VaultController, state), st_state_encrypt);
    m_menu = m_menuMgr->createVaultMenu(&menuMgr);
    EXPECT_NE(m_menu, nullptr);
}

TEST_F(TestDFileMenuManager, loadNormalPluginMenu)
{
    m_menu = m_menuMgr->createDefaultBookMarkMenu();
    EXPECT_NE(m_menu, nullptr);

    QString musicPath = DFMStandardPaths::location(DFMStandardPaths::MusicPath);
    DUrl url = DUrl::fromLocalFile(musicPath);
    DUrlList urls;
    urls << url;
    QList<QAction *> actions = m_menuMgr->loadNormalPluginMenu(m_menu, urls, url, true);
    EXPECT_TRUE(actions.size() > 0);
}

TEST_F(TestDFileMenuManager, loadEmptyAreaPluginMenu)
{
    m_menu = m_menuMgr->createDefaultBookMarkMenu();
    EXPECT_NE(m_menu, nullptr);

    QString musicPath = DFMStandardPaths::location(DFMStandardPaths::MusicPath);
    DUrl url = DUrl::fromLocalFile(musicPath);
    DUrlList urls;
    urls << url;
    QList<QAction *> actions = m_menuMgr->loadEmptyAreaPluginMenu(m_menu, url, true);
    EXPECT_TRUE(actions.size() == 0);
}

TEST_F(TestDFileMenuManager, getAction)
{
    QAction *action = m_menuMgr->getAction(DFMGlobal::Copy);
    EXPECT_TRUE(action != nullptr);
}

TEST_F(TestDFileMenuManager, getActionText)
{
    QString text = m_menuMgr->getActionText(DFMGlobal::Copy);
    EXPECT_FALSE(text.isEmpty());
}

TEST_F(TestDFileMenuManager, getDisableActionList)
{
    QString musicPath = DFMStandardPaths::location(DFMStandardPaths::MusicPath);
    DUrl url = DUrl::fromLocalFile(musicPath);
    DUrlList urls;
    urls << url;

    QSet<MenuAction> actions = m_menuMgr->getDisableActionList(url);
    EXPECT_TRUE(actions.size() > 0);

    actions = m_menuMgr->getDisableActionList(urls);
    EXPECT_TRUE(actions.size() > 0);
}

TEST_F(TestDFileMenuManager, genereteMenuByKeys)
{
    // this function write later.
}

TEST_F(TestDFileMenuManager, actionString)
{
    QString str = m_menuMgr->getActionString(DFMGlobal::Copy);
    EXPECT_FALSE(str.isEmpty());

    m_menuMgr->setActionString(DFMGlobal::Copy, "test");
    str = m_menuMgr->getActionString(DFMGlobal::Copy);
    EXPECT_EQ(str, "test");
}

TEST_F(TestDFileMenuManager, extendCustomMenu)
{
    m_menu = m_menuMgr->createDefaultBookMarkMenu();
    EXPECT_NE(m_menu, nullptr);

    QString musicPath = DFMStandardPaths::location(DFMStandardPaths::MusicPath);
    DUrl url = DUrl::fromLocalFile(musicPath);
    DUrlList urls;
    urls << url;

    // this function will be test laster.
    m_menuMgr->extendCustomMenu(m_menu, true, url, url);
}

TEST_F(TestDFileMenuManager, isCustomMenuSupported)
{
    QString musicPath = DFMStandardPaths::location(DFMStandardPaths::MusicPath);
    DUrl url = DUrl::fromLocalFile(musicPath);

    // this function will be test laster.
    bool bSupported = m_menuMgr->isCustomMenuSupported(url);
    EXPECT_TRUE(bSupported);
}

TEST_F(TestDFileMenuManager, ActionWhitelist)
{
    m_menuMgr->addActionWhitelist(DFMGlobal::Copy);
    EXPECT_TRUE(m_menuMgr->actionWhitelist().contains(DFMGlobal::Copy));

    QSet<MenuAction> actions;
    actions.insert(DFMGlobal::Cut);
    m_menuMgr->setActionWhitelist(actions);
    EXPECT_TRUE(m_menuMgr->actionWhitelist().contains(DFMGlobal::Cut));
}

TEST_F(TestDFileMenuManager, ActionBlacklist)
{
    m_menuMgr->addActionBlacklist(DFMGlobal::Copy);
    EXPECT_TRUE(m_menuMgr->actionBlacklist().contains(DFMGlobal::Copy));

    QSet<MenuAction> actions;
    actions.insert(DFMGlobal::Cut);
    m_menuMgr->setActionBlacklist(actions);
    EXPECT_TRUE(m_menuMgr->actionBlacklist().contains(DFMGlobal::Cut));
}

TEST_F(TestDFileMenuManager, isAvailableAction)
{
    m_menuMgr->addActionWhitelist(DFMGlobal::Copy);
    EXPECT_TRUE(m_menuMgr->isAvailableAction(DFMGlobal::Copy));
}

TEST_F(TestDFileMenuManager, setActionID)
{
    EXPECT_NO_FATAL_FAILURE(m_menuMgr->setActionID(DFMGlobal::Copy, "id"));
}

TEST_F(TestDFileMenuManager, registerMenuActionType)
{
    QAction *action = m_menuMgr->getAction(DFMGlobal::Copy);
    MenuAction type = m_menuMgr->registerMenuActionType(action);
    EXPECT_EQ(type, DFMGlobal::UserMenuAction);
}

TEST_F(TestDFileMenuManager, whetherShowTagActions)
{
    QString musicPath = DFMStandardPaths::location(DFMStandardPaths::MusicPath);
    DUrl url = DUrl::fromLocalFile(musicPath);
    DUrlList urls;
    urls << url;
    bool bShow = m_menuMgr->whetherShowTagActions(urls);
    EXPECT_TRUE(bShow);
}

TEST_F(TestDFileMenuManager, actionTriggered)
{
#if 0  // do somthing later.
    typedef QVariant(*fptr)(const QSharedPointer<DFMEvent>&, DFMAbstractEventHandler*);

    QVariant (*st_processEvent)(const QSharedPointer<DFMEvent>&, DFMAbstractEventHandler *) =
            [](const QSharedPointer<DFMEvent>&, DFMAbstractEventHandler *)->QVariant {
        return QVariant();
    };
    Stub stub;
    stub.set(fptr(&DFMEventDispatcher::processEvent), st_processEvent);

    bool (*st_runCommand)(const QString &, const QStringList &, const QString &) =
            [](const QString &, const QStringList &, const QString &) {
        return true;
    };
    stub.set(ADDR(FileUtils, runCommand), st_runCommand);
    // do somthing later.
#endif
}




