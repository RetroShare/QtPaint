/*
Copyright 2017 SillyLossy.
This program is free software; you can redistribute it and/or
modify it under the terms of the Lesser GNU General Public License
as published by the Free Software Foundation; either version 3
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the Lesser GNU General Public License for more details.
You should have received a copy of the Lesser GNU General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include "brushtool.h"

#include <QPainter>

const QString BrushTool::toolName()
{
    static const QString brushName = QStringLiteral("Brush");
    return brushName;
}

const QString BrushTool::iconName()
{
    static const QString brushIcon = QStringLiteral("brush");
    return brushIcon;
}

void BrushTool::onMousePress(PaintEvent* event)
{  
    paintPoint(event);
}

void BrushTool::onMouseRelease(PaintEvent*)
{

}

void BrushTool::onMouseMove(PaintEvent* event)
{
    paintLine(event);
}

void BrushTool::paintLine(PaintEvent* event)
{
    QPainter painter(event->selectedLayer);
    if (event->antialiasingEnabled)
        painter.setRenderHint(QPainter::HighQualityAntialiasing);
    painter.setPen(event->pen);
    painter.drawLine(event->previousPoint, event->currentPoint);
}

void BrushTool::paintPoint(PaintEvent* event)
{
    QPainter painter(event->selectedLayer);
    if (event->antialiasingEnabled)
        painter.setRenderHint(QPainter::HighQualityAntialiasing);
    painter.setPen(event->pen);
    painter.drawPoint(event->currentPoint);
}
