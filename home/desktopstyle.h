/***************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (directui@nokia.com)
**
** This file is part of duihome.
**
** If you have questions regarding the use of this file, please contact
** Nokia at directui@nokia.com.
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation
** and appearing in the file LICENSE.LGPL included in the packaging
** of this file.
**
****************************************************************************/


#ifndef DESKTOPSTYLE_H_
#define DESKTOPSTYLE_H_

#include <DuiWidgetStyle>

class DesktopStyle : public DuiWidgetStyle
{
    Q_OBJECT
    DUI_STYLE(DesktopStyle)

    //! The image to draw the desktop background with
    DUI_STYLE_PTR_ATTRIBUTE(QPixmap *, desktopBackgroundImage, DesktopBackgroundImage)

    //! The image to be drawn above the desktop
    DUI_STYLE_PTR_ATTRIBUTE(QPixmap *, desktopBackgroundTop, DesktopBackgroundTop)

    //! The image to be drawn below the desktop
    DUI_STYLE_PTR_ATTRIBUTE(QPixmap *, desktopBackgroundBottom, DesktopBackgroundBottom)
};

class DesktopStyleContainer : public DuiWidgetStyleContainer
{
    DUI_STYLE_CONTAINER(DesktopStyle)
};

#endif /* DESKTOPSTYLE_H_ */