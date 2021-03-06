/* -*- Mode: C++; indent-tabs-mode: nil; tab-width: 4 -*-
 * -*- coding: utf-8 -*-
 *
 * Copyright (C) 2011 ~ 2017 Deepin, Inc.
 *               2011 ~ 2017 Wang Yong
 *
 * Author:     Wang Yong <wangyong@deepin.com>
 * Maintainer: Wang Yong <wangyong@deepin.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "start_tooltip.h"
#include "utils.h"
#include <QApplication>
#include <QDebug>
#include <QDesktopWidget>
#include <QEvent>
#include <QPainter>
#include <QStyle>
#include <QWidget>
#include <DHiDPIHelper>

DWIDGET_USE_NAMESPACE

StartTooltip::StartTooltip(QWidget *parent) : QWidget(parent)
{
    setWindowFlags(Qt::WindowDoesNotAcceptFocus | Qt::BypassWindowManagerHint);
    setAttribute(Qt::WA_TranslucentBackground, true);

    iconImg = DHiDPIHelper::loadNxPixmap(Utils::getQrcPath("kill.svg"));

    installEventFilter(this);

    text = tr("Click the application you want to end");
    QSize size = Utils::getRenderSize(RECTANGLE_FONT_SIZE, text);

    setFixedSize(size.width() + RECTANGLE_PADDING * 2,
                 size.height() + iconImg.height() + RECTANGLE_PADDING * 3);

    Utils::passInputEvent(this->winId());
}

StartTooltip::~StartTooltip()
{
}

void StartTooltip::setWindowManager(DWindowManager *wm)
{
    windowManager = wm;

    WindowRect rootWindowRect = windowManager->getRootWindowRect();
    setGeometry(
        QStyle::alignedRect(
            Qt::LeftToRight,
            Qt::AlignCenter,
            this->size(),
            QRect(rootWindowRect.x, rootWindowRect.y, rootWindowRect.width, rootWindowRect.height)
            )
        );
}

bool StartTooltip::eventFilter(QObject *, QEvent *event)
{
    if (event->type() == QEvent::ShowToParent) {
        Utils::blurRect(windowManager, this->winId(), rect());
    } else if (event->type() == QEvent::HideToParent) {
        Utils::clearBlur(windowManager, this->winId());
    }

    return false;
}

void StartTooltip::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    Utils::drawTooltipBackground(painter, rect());

    painter.setOpacity(1);
    qreal devicePixelRatio = qApp->devicePixelRatio();
    painter.drawPixmap(QPoint((rect().width() - iconImg.width() / devicePixelRatio) / 2, RECTANGLE_PADDING * devicePixelRatio), iconImg);

    Utils::drawTooltipText(painter, text, "#000000", RECTANGLE_FONT_SIZE,
                           QRectF(rect().x(),
                                  rect().y() + RECTANGLE_PADDING + iconImg.height(),
                                  rect().width(),
                                  rect().height() - RECTANGLE_PADDING - iconImg.height()
                               ));
}
