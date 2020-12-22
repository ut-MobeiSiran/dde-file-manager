/*

 * Copyright (C) 2020 ~ 2020 Deepin Technology Co., Ltd.

 *

 * Author:     lixiang

 *

 * Maintainer: lixianga@uniontech.com

 *

 * brief:

 *

 * date:    2020-08-27

 */


#include <gtest/gtest.h>

#include <QIODevice>

#include "textpreview.h"
#include "durl.h"

#include "stub.h"

DFM_USE_NAMESPACE

class TestTextPreview :public testing::Test
{
public:
     void SetUp() override
     {
        m_testPreview = new TextPreview(nullptr);
        QFile fp("./1.txt");
        fp.open(QIODevice::ReadWrite|QIODevice::Text);
        fp.write("I am writing file");
        fp.close();
        m_url = DUrl("file:./1.txt");
     }

     void TearDown() override
     {
        delete m_testPreview;
        m_testPreview = nullptr;
        QFile fp("./1.txt");
        fp.remove();
     }

public:
     TextPreview * m_testPreview;
     DUrl m_url;
};

typedef bool (*fpCanReadLine)(QIODevice*);
TEST_F(TestTextPreview, set_file_url)
{
    EXPECT_FALSE(m_testPreview->setFileUrl(m_url));

    bool (*st_canReadLine)() = [](){
      return true;
    };

    Stub stub;
    fpCanReadLine IODeviceFp = (fpCanReadLine)(&QIODevice::canReadLine);
    stub.set(IODeviceFp, st_canReadLine);

    QFile fp("./2.txt");
    fp.open(QIODevice::ReadWrite|QIODevice::Text);
    fp.write("I am writing file");
    fp.close();
    EXPECT_TRUE(m_testPreview->setFileUrl(DUrl("file:./2.txt")));
    QFile fp1("./2.txt");
    fp1.remove();
}

TEST_F(TestTextPreview, get_file_url)
{
    EXPECT_FALSE(m_testPreview->setFileUrl(m_url));
    EXPECT_TRUE(m_testPreview->fileUrl() == m_url);
}

TEST_F(TestTextPreview, get_content_widget)
{
    EXPECT_FALSE(m_testPreview->setFileUrl(m_url));
    EXPECT_TRUE(m_testPreview->contentWidget() == nullptr);
}

TEST_F(TestTextPreview, get_title)
{
    EXPECT_FALSE(m_testPreview->setFileUrl(m_url));
    EXPECT_FALSE(!m_testPreview->title().isEmpty());
}

TEST_F(TestTextPreview, get_show_statusbar_separator)
{
    EXPECT_FALSE(m_testPreview->setFileUrl(m_url));
    EXPECT_TRUE(m_testPreview->showStatusBarSeparator());
}
