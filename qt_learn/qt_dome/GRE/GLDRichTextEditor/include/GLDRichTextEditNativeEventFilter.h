/*!
 * \file GLDRichTextEditNativeEventFilter.h
 *
 * \author xieg-a
 * \date °ËÔÂ 2014
 *
 * 
 */
#ifndef __GLDRichTextEditNativeEventFilter_H__
#define __GLDRichTextEditNativeEventFilter_H__

#include <QAbstractNativeEventFilter>
#include <QHash>
#include <Windows.h>

class GLDRichTextEdit;

class GLDRichTextEditNativeEventFilter : public QAbstractNativeEventFilter
{
public:
    GLDRichTextEditNativeEventFilter();
    ~GLDRichTextEditNativeEventFilter();

    virtual bool nativeEventFilter(const QByteArray &eventType, void *message, long *) Q_DECL_OVERRIDE;

    void registerEdit(HWND edit, GLDRichTextEdit * editParent);
    void removeEdit(HWND edit);

private:
    bool onMsg(MSG * pMsg, GLDRichTextEdit * editParent);

private:
    QHash<HWND, GLDRichTextEdit *> m_edits;
};

#endif/*__GLDRichTextEditNativeEventFilter_H__*/
