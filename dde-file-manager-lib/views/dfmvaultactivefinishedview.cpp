#include "dfmvaultactivefinishedview.h"
#include "operatorcenter.h"
#include "../../controllers/vaultcontroller.h"
#include "vault/vaultlockmanager.h"
#include "app/define.h"
#include "accessibility/ac-lib-file-manager.h"

#include <QLabel>
#include <QPushButton>
#include <QGridLayout>
#include <DIconButton>
#include <QDebug>
#include <DWaterProgress>
#include <QThread>
#include <QMessageBox>
#include <QTimer>
#include <DLabel>
#include <DDialog>

DFMVaultActiveFinishedView::DFMVaultActiveFinishedView(QWidget *parent)
    : QWidget(parent)
    , m_pTips(nullptr)
    , m_pEncryptFinishedImage(nullptr)
    , m_pTips4(nullptr)
    , m_pFinishedBtn(nullptr)
{
    AC_SET_ACCESSIBLE_NAME(this, AC_VAULT_ACTIVE_ENCRYPT_WIDGET);

    // 标题
    QLabel *pLabelTitle = new QLabel(tr("Encrypt File Vault"), this);
    AC_SET_ACCESSIBLE_NAME(pLabelTitle, AC_VAULT_ACTIVE_ENCRYPT_TITLE);
    QFont font = pLabelTitle->font();
    font.setPixelSize(18);
    pLabelTitle->setFont(font);
    pLabelTitle->setAlignment(Qt::AlignHCenter);

    // 加密提示
    m_pTips = new DLabel(tr("Click 'Encrypt' and input the user password."), this);
    AC_SET_ACCESSIBLE_NAME(m_pTips, AC_VAULT_ACTIVE_ENCRYPT_CONTENT);
    m_pTips->setAlignment(Qt::AlignHCenter);

    // 加密保险箱图片
    m_pEncryVaultImage = new DLabel(this);
    AC_SET_ACCESSIBLE_NAME(m_pEncryVaultImage, AC_VAULT_ACTIVE_ENCRYPT_IMAGE);
    m_pEncryVaultImage->setPixmap(QIcon::fromTheme("dfm_vault_active_encrypt").pixmap(98, 88));
    m_pEncryVaultImage->setAlignment(Qt::AlignHCenter);

    // 进度条
    m_pWaterProgress = new DWaterProgress(this);
    AC_SET_ACCESSIBLE_NAME(m_pWaterProgress, AC_VAULT_ACTIVE_ENCRYPT_PROGRESS);
    m_pWaterProgress->setValue(1);
    m_pWaterProgress->setFixedSize(98, 98);
    // 进度条提示
    m_pTips3 = new DLabel(tr("Encrypting..."), this);
    AC_SET_ACCESSIBLE_NAME(m_pTips3, AC_VAULT_ACTIVE_ENCRYPT_PROGRESS_HINT);
    m_pTips3->setAlignment(Qt::AlignHCenter);

    // 加密完成完成图片
    m_pEncryptFinishedImage = new DLabel(this);
    AC_SET_ACCESSIBLE_NAME(m_pEncryptFinishedImage, AC_VAULT_ACTIVE_ENCRYPT_FINISH_IMAGE);
    m_pEncryptFinishedImage->setPixmap(QIcon::fromTheme("dfm_vault_active_finish").pixmap(128, 128));
    m_pEncryptFinishedImage->setAlignment(Qt::AlignHCenter);
    // 加密完成提示
    m_pTips4 = new DLabel(tr("The setup is complete"), this);
    AC_SET_ACCESSIBLE_NAME(m_pTips4, AC_VAULT_ACTIVE_ENCRYPT_FINISH_HINT);
    m_pTips4->setAlignment(Qt::AlignHCenter);

    // 加密保险箱按钮
    m_pFinishedBtn = new QPushButton(tr("Encrypt") , this);
    AC_SET_ACCESSIBLE_NAME(m_pFinishedBtn, AC_VAULT_ACTIVE_ENCRYPT_BUTTON);
    m_pFinishedBtn->setFixedSize(452, 30);
    connect(m_pFinishedBtn, &QPushButton::clicked,
            this, &DFMVaultActiveFinishedView::slotEncryptVault);

    // 布局
    m_pWidget1 = new QWidget(this);
    QVBoxLayout *play1 = new QVBoxLayout(m_pWidget1);
    play1->setMargin(0);
    play1->addWidget(m_pTips);
    play1->addSpacing(22);
    play1->addWidget(m_pEncryVaultImage);

    m_pWidget2 = new QWidget(this);
    QVBoxLayout *play2 = new QVBoxLayout(m_pWidget2);
    play2->setContentsMargins(0, 22, 0, 0);
    play2->addWidget(m_pWaterProgress);
    play2->addSpacing(22);
    play2->addWidget(m_pTips3);

    m_pWidget3 = new QWidget(this);
    QVBoxLayout *play3 = new QVBoxLayout(m_pWidget3);
    play3->setContentsMargins(0, 15, 0, 0);
    play3->addWidget(m_pEncryptFinishedImage);
    play3->addSpacing(10);
    play3->addWidget(m_pTips4);

    QVBoxLayout *m_pLay = new QVBoxLayout(this);
    m_pLay->setMargin(0);
    m_pLay->addWidget(pLabelTitle);
    m_pLay->addSpacing(10);
    m_pLay->addWidget(m_pWidget1);
    m_pLay->addWidget(m_pWidget2, 0, Qt::AlignHCenter);
    m_pLay->addWidget(m_pWidget3);
    m_pLay->addStretch();
    m_pLay->addWidget(m_pFinishedBtn, 0, Qt::AlignCenter);

    m_pWidget2->setVisible(false);
    m_pWidget3->setVisible(false);

    // cryfs对象
    VaultController * pcryfs = VaultController::ins();
    connect(pcryfs, &VaultController::signalCreateVault,
            this, &DFMVaultActiveFinishedView::slotEncryptComplete);

    // 初始化定时器
    m_pTimer = new QTimer(this);
    connect(m_pTimer, &QTimer::timeout,
             this, &DFMVaultActiveFinishedView::slotTimeout);
}

void DFMVaultActiveFinishedView::setFinishedBtnEnabled(bool b)
{
    m_pFinishedBtn->setEnabled(b);
    m_pFinishedBtn->setText(tr("Encrypt"));
    m_pWidget1->setVisible(true);
    m_pWidget2->setVisible(false);
    m_pWidget3->setVisible(false);
}

void DFMVaultActiveFinishedView::slotEncryptComplete(int nState)
{
    if(nState == 0){    // 创建保险箱成功
        m_pWaterProgress->setValue(100);
        m_pWaterProgress->stop();
        repaint();
        m_pTimer->setSingleShot(true);
        m_pTimer->start(500);

        // Reset autolock time config.
        VaultLockManager::getInstance().resetConfig();
    }else{
        QMessageBox::warning(this, QString(), QString(tr("Failed to create file vault: %1").arg(nState)));
    }
}

void DFMVaultActiveFinishedView::slotEncryptVault()
{
    if (!VaultLockManager::getInstance().checkAuthentication(VAULT_CREATE)){
        m_pFinishedBtn->setEnabled(true);
        return;
    }

    if(m_pFinishedBtn->text() == tr("Encrypt")){
        // 完成按钮灰化
        m_pFinishedBtn->setEnabled(false);
        // 隐藏右上角关闭按钮
        if(parentWidget()){
            DDialog *pParent = static_cast<DDialog*>(parentWidget()->parentWidget());
            if(pParent){
                pParent->setCloseButtonVisible(false);
            }
        }

        // 进度条
        m_pWaterProgress->start();
        m_pWidget1->setVisible(false);
        m_pWidget2->setVisible(true);
        m_pWidget3->setVisible(false);

        std::thread t([](){
            // 调用创建保险箱接口
            // 拿到密码
            QString strPassword = OperatorCenter::getInstance().getSaltAndPasswordClipher();
            VaultController::ins()->createVault(strPassword);
        });
        t.detach();
    }else{
        // 切换到保险箱主页面
        emit sigAccepted();
    }
}

void DFMVaultActiveFinishedView::slotTimeout()
{
    m_pWidget1->setVisible(false);
    m_pWidget2->setVisible(false);
    m_pWidget3->setVisible(true);
    m_pFinishedBtn->setText(tr("OK"));
    m_pFinishedBtn->setEnabled(true);
    // 显示右上角关闭按钮
    if(parentWidget()){
        DDialog *pParent = static_cast<DDialog*>(parentWidget()->parentWidget());
        if(pParent){
            pParent->setCloseButtonVisible(true);
        }
    }
}
