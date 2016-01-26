/*  YUView - YUV player with advanced analytics toolset
*   Copyright (C) 2015  Institut f�r Nachrichtentechnik
*                       RWTH Aachen University, GERMANY
*
*   YUView is free software; you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation; either version 2 of the License, or
*   (at your option) any later version.
*
*   YUView is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with YUView.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef SPLITVIEWWIDGET_H
#define SPLITVIEWWIDGET_H

#include <QWidget>

enum ViewMode {SIDE_BY_SIDE, COMPARISON};

class PlaylistTreeWidget;
class PlaybackController;

class splitViewWidget : public QWidget
{
  Q_OBJECT

public:
  explicit splitViewWidget(QWidget *parent = 0);

  /// Activate/Deactivate the splitting view. Only use this function!
  void setSplitEnabled(bool splitting);

  /// The common settings have changed (background color, ...)
  void updateSettings();
  
  /// Reset everything so that the zoom factor is 1 and the display positions are centered
  void resetViews();

  // Set the widget to the given view mode
  void setViewMode(ViewMode v) { if (viewMode != v) { viewMode = v; resetViews(); } }

  //
  void setPlaylistTreeWidget( PlaylistTreeWidget *p ) { playlist = p; }
  void setPlaybackController( PlaybackController *p ) { playback = p; }

public slots:


protected:
  void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
  void mouseMoveEvent(QMouseEvent * event) Q_DECL_OVERRIDE;
  void mousePressEvent(QMouseEvent * event) Q_DECL_OVERRIDE;
  void mouseReleaseEvent(QMouseEvent * event) Q_DECL_OVERRIDE;

  bool   splitting;         //!< If true the view will be split into 2 parts
  bool   splittingDragging; //!< True if the user is currently dragging the splitter
  double splittingPoint;    //!< A value between 0 and 1 specifying the horizontal split point (0 left, 1 right)

  QPoint  centerOffset;     //!< The offset of the view to the center (0,0)
  bool    viewDragging;     //!< True if the user is currently moving the view
  QPoint  viewDraggingMousePosStart;
  QPoint  viewDraggingStartOffset;
  double  zoomFactor;

  ViewMode viewMode;

  // Pointers to the playlist tree widget and to the playback controller
  PlaylistTreeWidget *playlist;
  PlaybackController *playback;
};

#endif // SPLITVIEWWIDGET_H