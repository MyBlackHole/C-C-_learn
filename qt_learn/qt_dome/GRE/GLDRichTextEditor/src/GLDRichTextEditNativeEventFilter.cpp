/*!
 * \file GLDRichTextEditNativeEventFilter.cpp
 *
 * \author xieg-a
 * \date 八月 2014
 *
 * 
 */
#include "GLDRichTextEditNativeEventFilter.h"
#include <QApplication>
#include <QDebug>

#include <Windows.h>
#include <Richedit.h>
#include <QEvent>

#include "GLDRichTextEdit.h"

GLDRichTextEditNativeEventFilter::GLDRichTextEditNativeEventFilter()
{
    qApp->installNativeEventFilter(this);
}

GLDRichTextEditNativeEventFilter::~GLDRichTextEditNativeEventFilter()
{

}

bool GLDRichTextEditNativeEventFilter::nativeEventFilter(const QByteArray &eventType, void *message, long *) 
{

    MSG *pMsgB = (MSG *)message;
    if (eventType == "windows_generic_MSG") 
    {
        MSG *pMsg = (MSG *)message;
        Q_ASSERT(NULL != pMsg);

        GLDRichTextEdit *pEdit = m_edits.value(pMsg->hwnd);
        if (NULL != pEdit)
        {
            onMsg(pMsg, pEdit);
        }

        if (pMsgB->message == WM_LBUTTONDOWN)
        {
            pEdit->enterKeyPress();
        }
    }
    // Rich Edit window 不是Qt widget 所以总是返回false
    return false;
}

void GLDRichTextEditNativeEventFilter::registerEdit(HWND edit, GLDRichTextEdit * editParent)
{
    Q_ASSERT(NULL != edit);
    Q_ASSERT(NULL != editParent);

    m_edits.insert(edit, editParent);
}

void GLDRichTextEditNativeEventFilter::removeEdit(HWND edit)
{
    Q_ASSERT(NULL != edit);
    m_edits.remove(edit);
}

bool GLDRichTextEditNativeEventFilter::onMsg(MSG * pMsg, GLDRichTextEdit * editParent)
{
    editParent->alwaysHideScrollBar();

    switch (pMsg->message)
    {
    case WM_KEYDOWN:
    {
        Qt::Key eKey = Qt::Key_No;
        switch (pMsg->wParam)
        {
        case VK_LEFT:
            eKey = Qt::Key_Left;
            break;
        case VK_RIGHT:
            eKey = Qt::Key_Right;
            editParent->catchWheelup();
            break;
        case VK_UP:
            eKey = Qt::Key_Up;
            editParent->catchWheelup();
            break;
        case VK_DOWN:
            eKey = Qt::Key_Down;
            editParent->catchWheelup();
            break;
        case VK_PRIOR:
            editParent->catchWheelup();
            break;
        case VK_NEXT:
            editParent->catchWheelup();
            break;
        default:
            editParent->updateUI(QEvent::KeyPress, Qt::NoButton, eKey);
            break;
        }
    }
        break;
    case WM_KEYUP:
        editParent->updateUI(QEvent::KeyPress, Qt::NoButton, Qt::Key_No);
    case WM_LBUTTONDOWN:
    {
        Qt::MouseButtons btns = Qt::NoButton;
        if (VK_LBUTTON & pMsg->message)
        {
            btns |= Qt::LeftButton;
        }
        if (VK_MBUTTON & pMsg->message)
        {
            btns |= Qt::MidButton;
        }
        if (VK_LBUTTON & pMsg->message)
        {
            btns |= Qt::RightButton;
        }
        editParent->updateUI(QEvent::MouseButtonPress, btns, Qt::Key_No);
    }
        break;
    case WM_RBUTTONUP:
    {
        editParent->emitRButtonUp();
    }
        break;
    case WM_RBUTTONDOWN:
    {
        editParent->updateUI(QEvent::MouseButtonPress, Qt::RightButton, Qt::Key_No);
    }
        break;

    case WM_MOUSEWHEEL: //处理滚轮
    {
        int oPos     = (short)HIWORD(pMsg->wParam);//鼠标滚轮的方向
        int oSomeKey = (short)LOWORD(pMsg->wParam);//有哪些键还按下
        switch (oSomeKey)
        {
        case MK_CONTROL:
        {
            if (oPos > 0)
            {
                editParent->setZoomFactor(true);
            }
            else
            {
                editParent->setZoomFactor(false);
            }
        }
            break;
        default:
        {
            editParent->catchWheelup();
        }
            break;
        }
    }
    case WM_MOUSEMOVE:
    {

    }
        break;
    default:
        break;
    }
    return false;
}
